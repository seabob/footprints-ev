#ifndef __OBD_CONNECT_TASK_H__
#define __OBD_CONNECT_TASK_H__

#include <ev.h>
#include <pthread.h>
#include "./FULMemoryPool/MemoryPool.h"
#include "OBD_struct.h"

typedef struct ev_io ev_io_t;
typedef struct ev_loop ev_loop_t;

static MemoryPool_t *task_pool = NULL;


typedef struct OBD_connect_task{
	u32_t		id;
	ev_loop_t	*loop;
	ev_io_t		ev;
	pthread_mutex_t	lock;
	u32_t		length;
	list_t		head;
}connect_task_t;

static connect_task_t *connect_tasks[SOCKET_ALLOC_MAX_CLIENT] = {0};

static inline connect_task_t *connect_task_init(ev_loop_t *loop,s32_t id)
{
	if(!task_pool)
		return NULL;
	connect_task_t * task = Malloc(task_pool);
	if(!task)
		return NULL;
	if(connect_tasks[id])
	{
		printf("%s:%d\n",__func__,__LINE__);
		Free(task_pool,task);
	}
	task->id = id;
	task->loop = loop;
	task->length = 0;
	pthread_mutex_init(&task->lock,NULL);
	list_init(&task->head);
	connect_tasks[id] = task;
	return connect_tasks[id];
}

static inline void connect_task_put(connect_task_t *task, OBD_t *node)
{
	pthread_mutex_lock(&task->lock);
	list_add_front(&task->head,&node->list);
	task->length++;
	pthread_mutex_unlock(&task->lock);
}

static inline OBD_t* connect_task_get(connect_task_t *task)
{
	pthread_mutex_lock(&task->lock);
	OBD_t *node = NULL;
	list_t *current = list_del_back(&task->head);
	if(current)
	{
		task->length--;
		node = list_entry(current,OBD_t,list);
	}
	pthread_mutex_unlock(&task->lock);
	return node;
}

static inline boolean connect_task_is_empty(connect_task_t *task)
{
	pthread_mutex_lock(&task->lock);
	boolean flag = FALSE;
	list_t *head = &task->head;
	if(head->prev == head)
		flag = TRUE;
	pthread_mutex_unlock(&task->lock);
	return flag;
}

static inline void connect_task_process(void *data)
{
	connect_task_t *task = (connect_task_t *)data;
	OBD_t *o = NULL;
	while((o = connect_task_get(task)) != NULL)
	{
		OBD_decode_thread((void*)o);
	}
}

static inline void connect_task_destroy(connect_task_t *task)
{
	if(!task)
		return;
	if(!connect_task_is_empty(task))
	{
		connect_task_process((void*)task);
	}
	list_init(&task->head);
	pthread_mutex_destroy(&task->lock);
	task->length = 0;
	connect_tasks[task->id] = NULL;
	close(task->id);
	ev_io_stop(task->loop,&task->ev);
	task->id = -1;
	Free(task_pool,task);
}


#endif
