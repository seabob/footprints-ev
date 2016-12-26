#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <ev.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include "./FULMemoryPool/MemoryPool.h"
#include "./threadpool/threadpool.h"
#include "types_define.h"
#include "db_types.h"
#include "OBD_mq.h"
#include "OBD_connect.h"
#include "OBD_struct.h"

typedef struct sockaddr_in sockaddr_in_t;

static threadpool_t *thread_pool;

static int server_socket;
struct ev_io socket_accept;

static mq_t obd_mq;

void OBD_decode_thread(void *data)
{
	OBD_t *o = (OBD_t*)data;
	OBD_decode(o,NULL);
	OBD_release(o);
}

static void ev_signal_cb(struct ev_loop *loop,ev_signal *w,int revents)
{
	ev_break(loop,EVBREAK_ALL);	
}

void read_func_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	ssize_t read, len;
	OBD_t *obd = OBD_init(w);
	if(!obd)
	{
		send(w->fd,"ERROR",strlen("ERROR"),0);
		printf("%s:%d\n",__func__,__LINE__);
		return ;
		
	}
	if(EV_ERROR & revents)
	{
		
		OBD_release(obd);
		printf("%s:%d\n",__func__,__LINE__);
		return;
	}
	read = recv(w->fd,obd->data,BUF_SIZE,0);
	if(read < 0)
	{
		OBD_release(obd);
		printf("%s:%d\n",__func__,__LINE__);
		return;
	}
	else if(read == 0)
	{
//		printf("%s:%d\n",__func__,__LINE__);
		OBD_release(obd);
		OBD_connect_destroy(ALL_CONNECTS[w->fd]);
		return;
	}

	if(obd->data[0] != 0xbb)
	{
		printf("%s:%d\n",__func__,__LINE__);
		OBD_release(obd);
		return ;
	}
	
	obd->data_length = read;
	obd->connect_id = w->fd;
	
	if(FALSE == OBD_checksum(obd))
	{
		printf("%s:%d\n",__func__,__LINE__);
		OBD_release(obd);
		send(w->fd,"ERROR",strlen("ERROR"),0);
		return ;
		
	}

	send(w->fd,"OK",strlen("OK"),0);
//	OBD_release(obd);
	OBD_decode_thread((void*)obd);
//	threadpool_add(thread_pool,OBD_decode_thread,(void*)obd,0);
	return;
}

static void accept_func_cb(struct ev_loop *loop,struct ev_io *w, int revents)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket;

	client_socket= accept(w->fd,(struct sockaddr*)&client_addr,&client_len);
	if(client_socket < 0)
	{
		return;
	}
	if(client_socket > SOCKET_ALLOC_MAX_CLIENT)
	{
		printf("%s:%d\n",__func__,__LINE__);
		close(client_socket);
		return ;
	}
	if(ALL_CONNECTS[client_socket])
	{
		printf("%s:%d\n",__func__,__LINE__);
		return ;	
	}

	if(OBD_connect_init(client_socket,read_func_cb) == NULL)
	{
		printf("%s:%d\n",__func__,__LINE__);
		close(client_socket);
		return;
	}
}

int main(int argv, char **argc)
{
	int ret = 0;
	loop = ev_default_loop(0);
	ev_signal signal_watcher;
	int i =0;
	sockaddr_in_t server_addr;
	pthread_mutex_init(&mq_mutex,NULL);

	if((obd_pool = CreateMemoryPool(sizeof(OBD_t))) == NULL) return 1;
	if((connect_pool = CreateMemoryPool(sizeof(connect_t))) == NULL) return 1;
	if((db_buffer_pool = CreateMemoryPool(BUF_SIZE)) == NULL) return 1;

//	thread_pool = threadpool_create(32,128,0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_addr.sin_port = htons(11223);

	server_socket = socket(PF_INET,SOCK_STREAM,0);
	{
		int ops = 1;
		setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&ops,sizeof(ops));
	}
	bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr));
	listen(server_socket,32);

	ev_signal_init(&signal_watcher,ev_signal_cb,SIGINT);
	ev_signal_start(loop,&signal_watcher);

	ev_io_init(&socket_accept,accept_func_cb,server_socket,EV_READ);
	ev_io_start(loop,&socket_accept);
	ev_run(loop,0);
	
	close(server_socket);
//	threadpool_destroy(thread_pool,0);
	DestroyMemoryPool(&obd_pool);
	DestroyMemoryPool(&db_buffer_pool);
	DestroyMemoryPool(&connect_pool);
	pthread_mutex_destroy(&mq_mutex);
	ev_loop_destroy(loop);
	
	return 0;

}
