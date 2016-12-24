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
#include "OBD_mq.h"
#include "OBD_struct.h"

static struct ev_io *libev_list[SOCKET_ALLOC_MAX_CLIENT] = {0};
typedef struct sockaddr_in sockaddr_in_t;

static MemoryPool_t *ev_pool;
static threadpool_t *thread_pool;

static int server_socket;
struct ev_io socket_accept;
struct ev_loop *loop;

static mq_t obd_mq;
boolean mq_flag = FALSE;
pthread_mutex_t client_lock;
pthread_mutex_t mem_lock;

void OBD_decode_thread(void *data);
static void free_libev(struct ev_loop *loop,int fd)
{
	if(fd > SOCKET_ALLOC_MAX_CLIENT)
	{
		printf("more than SOCKET_ALLOC_MAX_CLIENT [%d]\n",fd);
		return;
	}
	if(libev_list[fd] == NULL)
	{
		printf("the fd already freed[%d]\n",fd)	;
		return ;
	}
	close(fd);
	ev_io_stop(loop,libev_list[fd]);
	Free(ev_pool,libev_list[fd]);
	libev_list[fd] = NULL;
}

static void ev_signal_cb(struct ev_loop *loop,ev_signal *w,int revents)
{
	ev_break(loop,EVBREAK_ALL);	
}

void read_func_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	ssize_t read, len;
	OBD_t *obd = OBD_init();
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
	len = strlen(obd->data);
	if(read < 0)
	{
		OBD_release(obd);
		printf("%s:%d\n",__func__,__LINE__);
		return;
	}
	else if(read == 0)
	{
		OBD_release(obd);
		free_libev(loop,w->fd);
		return;
	}
//	obd->data[len] = '\0';

	if(len == 0)
	{
		OBD_release(obd);
		return ;
	}
	
	if(FALSE == OBD_checksum(obd->data))
	{
		printf("%s:%d\n",__func__,__LINE__);
		OBD_release(obd);
		send(w->fd,"ERROR",strlen("ERROR"),0);
		return ;
		
	}
	send(w->fd,"OK",strlen("OK"),0);
	obd->data_length = len;
	list_init(&obd->list);
//	mq_put(&obd_mq,&obd->list);
//		OBD_release(obd);
	threadpool_add(thread_pool,OBD_decode_thread,(void*)obd,0);
	return;
}

static void accept_func_cb(struct ev_loop *loop,struct ev_io *w, int revents)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket;
//	pthread_mutex_lock(&client_lock);
	struct ev_io *w_client = (struct ev_io*)Malloc(ev_pool);
//	pthread_mutex_unlock(&client_lock);
	if(!w_client)
	{
		printf("%s:%d\n",__func__,__LINE__);
		return ;
	}
	if(EV_ERROR & revents)
	{
		Free(ev_pool,w_client);
		printf("%s:%d\n",__func__,__LINE__);
		return ;	
	}
	client_socket= accept(w->fd,(struct sockaddr*)&client_addr,&client_len);
	if(client_socket < 0)
	{
		Free(ev_pool,w_client);
		return;
	}
	if(client_socket > SOCKET_ALLOC_MAX_CLIENT)
	{
		printf("%s:%d\n",__func__,__LINE__);
		Free(ev_pool,w_client);
		close(client_socket);
		return ;
	}
	if(libev_list[client_socket])
	{
		printf("%s:%d\n",__func__,__LINE__);
		Free(ev_pool,w_client);
		return ;	
	}

	ev_io_init(w_client,read_func_cb,client_socket,EV_READ);
	ev_io_start(loop,w_client);
	libev_list[client_socket] = w_client;
}

void OBD_decode_thread(void *data)
{
	OBD_t *o = (OBD_t*)data;
	OBD_filter_escape_and_transfer_to_hex(o);
//	printf(" %s\n",o->data);
	OBD_decode(o);
	OBD_release(o);
}

int main(int argv, char **argc)
{
	int ret = 0;
	loop = ev_default_loop(0);
	ev_signal signal_watcher;
	int i =0;
	pthread_t thread;
	sockaddr_in_t server_addr;
	pthread_mutex_init(&mq_mutex,NULL);
//	pthread_cond_init(&mq_cond,NULL);
	mq_init(&obd_mq);
	if((ev_pool = CreateMemoryPool(sizeof(struct ev_io))) == NULL)return 1;
	if((obd_pool = CreateMemoryPool(sizeof(OBD_t))) == NULL) return 1;

	thread_pool = threadpool_create(32,128,0);

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
	threadpool_destroy(thread_pool,0);
	DestroyMemoryPool(&obd_pool);
	DestroyMemoryPool(&ev_pool);
	pthread_mutex_destroy(&mq_mutex);
	ev_loop_destroy(loop);
	
	return 0;

}
