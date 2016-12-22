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
#include "OBD_connect_task.h"

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

//static mq_t obd_mq;
boolean mq_flag = FALSE;
pthread_mutex_t client_lock;

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
	char buffer[BUF_SIZE] = {0};
	if(EV_ERROR & revents)
	{
		printf("%s:%d\n",__func__,__LINE__);
		return;
	}
	read = recv(w->fd,buffer,BUF_SIZE,0);
	len = strlen(buffer);
	if(read < 0)
	{
		printf("%s:%d\n",__func__,__LINE__);
		return;
	}
	else if(read == 0)
	{
		printf("%s:%d\n",__func__,__LINE__);
		if(connect_tasks[w->fd])
		{
			connect_task_destroy(connect_tasks[w->fd]);
		}
		return;
	}
	buffer[len] = '\0';
	send(w->fd,"OK",strlen("OK"),0);

	if(len == 0)
		return ;
	{
		OBD_t *obd = OBD_init();
		memcpy(obd->data,buffer,len);
		obd->data_length = len;
		connect_task_put(connect_tasks[w->fd],obd);
	}
	
	return;
}

static void accept_func_cb(struct ev_loop *loop,struct ev_io *w, int revents)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket;

	if(EV_ERROR & revents)
	{
		printf("%s:%d\n",__func__,__LINE__);
		return ;	
	}
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
	if(connect_tasks[client_socket])
	{
		printf("%s:%d\n",__func__,__LINE__);
		connect_task_destroy(connect_tasks[client_socket]);
		return ;	
	}

	{
		connect_task_t *task = connect_task_init(loop,client_socket);
		if(!task)
		{
		printf("%s:%d\n",__func__,__LINE__);
			close(client_socket);
			return;
		}
		ev_io_init(&task->ev,read_func_cb,client_socket,EV_READ);
		ev_io_start(loop,&task->ev);
	}
//	libev_list[client_socket] = w_client;
}

void* thread_func_cb(void *data)
{
	connect_task_t *task = NULL;
	list_t *head = NULL;
	int i = 0;
	while(1)
	{
		if(mq_flag)
		{
			u32_t no_empty_count = 0;
			for(i = 0; i <SOCKET_ALLOC_MAX_CLIENT; i++)
			{
				task = connect_tasks[i];
				if(task)
					connect_task_destroy(task);
			}

			break;

		}else
		{
			for(i = 0; i <SOCKET_ALLOC_MAX_CLIENT; i++)
			{
				task = connect_tasks[i];
				if(task && !connect_task_is_empty(task))
				{
					connect_task_process((void*)task);
		//			threadpool_add(thread_pool,connect_task_process,(void*)task,0);
				}
			}
		}
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
	sockaddr_in_t server_addr
	;
//	if((ev_pool = CreateMemoryPool(sizeof(struct ev_io))) == NULL)return 1;
	if((obd_pool = CreateMemoryPool(sizeof(OBD_t))) == NULL) return 1;
	if((task_pool = CreateMemoryPool(sizeof(connect_task_t))) == NULL) return 1;

//	thread_pool = threadpool_create(4,16,0);
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
	
	mq_flag = TRUE;
	pthread_join(thread,NULL);
	close(server_socket);
//	threadpool_destroy(thread_pool,0);
	DestroyMemoryPool(&obd_pool);
	DestroyMemoryPool(&task_pool);
//	DestroyMemoryPool(&ev_pool);
	ev_loop_destroy(loop);
	
	return 0;

}
