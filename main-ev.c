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

#define SOCKET_ALLOC_MAX_CLIENT	20480
static struct ev_io *libev_list[SOCKET_ALLOC_MAX_CLIENT] = {0};
typedef struct sockaddr_in sockaddr_in_t;

//static uv_tcp_t client;
static MemoryPool_t *ev_pool;
//static MemoryPool_t *w_pool;
//static MemoryPool_t *data_pool;
static threadpool_t *thread_pool;

static int server_socket;
struct ev_io socket_accept;
struct ev_loop *loop;

static mq_t obd_mq;
boolean mq_flag = FALSE;
pthread_mutex_t client_lock;
pthread_mutex_t mem_lock;

static long count = 0;
void OBD_decode_thread(void *);
void* thread_func_cb(void *);

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
	ssize_t read;
	OBD_t *obd = OBD_init();
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
		OBD_release(obd);
		printf("%s:%d\n",__func__,__LINE__);
		free_libev(loop,w->fd);
		return;
	}else
	{
		obd->data[read] = '\0';
	}
//	if(!OBD_check_sum(obd))
//	{
//		send(w->fd,"ERROR",strlen("ERROR"),0);
//		Free(data_pool,obd->data);
//		Free(obd_pool,obd);
//		return ;
//	}
	send(w->fd,"OK",strlen("OK"),0);
	list_init(&obd->list);
	mq_put(&obd_mq,&obd->list);
	
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
		Free(ev_pool,w_client);
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
		close(client_socket)	;
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
//	printf("data = %s\n",(char*)o->data);
//	printf("cmd = 0x%x\n",OBD_get_cmd(o));
//	Free(data_pool,(char*)o->data);
	OBD_release(o);
}

void* thread_func_cb(void *data)
{
	OBD_t *o = NULL;
	list_t *list = NULL;
	while(1)
	{
		if(mq_flag == TRUE && mq_is_empty(&obd_mq))
		{
			break;
		}
		list = mq_get(&obd_mq);
		if(list == NULL)
		{
			continue;
		}
		o = list_entry(list,OBD_t,list);
		if(!o)
		{
			printf("o is NULL\n");
			continue;
		}
		threadpool_add(thread_pool,OBD_decode_thread,(void*)o,0);
//		OBD_decode_thread((void*)o);
	}
	return (void*)NULL;
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
//	if((data_pool = CreateMemoryPool(BUF_SIZE)) == NULL) return 1;
	if((obd_pool = CreateMemoryPool(sizeof(OBD_t))) == NULL) return 1;

	thread_pool = threadpool_create(8,8,0);
	pthread_create(&thread,NULL,thread_func_cb,NULL);

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
	printf("aa\n");
	mq_flag = TRUE;
	pthread_join(thread,NULL);
	close(server_socket);
	threadpool_destroy(thread_pool,0);
//	DestroyMemoryPool(&data_pool);
	DestroyMemoryPool(&obd_pool);
	DestroyMemoryPool(&ev_pool);
	pthread_mutex_destroy(&mq_mutex);
	ev_loop_destroy(loop);
	
	return 0;

}
