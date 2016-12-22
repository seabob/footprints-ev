#ifndef __OBD_MQ_H__
#define __OBD_MQ_H__

#include <pthread.h>
#include "types_define.h"
#include "./FULMemoryPool/MemoryPool.h"

typedef struct obd_list{
	struct obd_list *prev;
	struct obd_list *next;
}list_t;

typedef struct obd_mq{
	pthread_mutex_t mq_mutex;
//	pthread_cond_t mq_cond;
	u32_t		id;
	struct ev_io	*ev;
	u32_t		size;
	list_t		list;
}mq_t;

static mq_t *hash_mq[SOCKET_ALLOC_MAX_CLIENT] = {0};
static MemoryPool_t *hash_pool = NULL;
static MemoryPool_t *hash_list = NULL;

#ifndef container_of
#define container_of(ptr,type,member) \
	({ \
		const typeof(((type*)0)->member)* __mptr = (ptr); \
		(type*)((char*)__mptr-offsetof(type,member)); \
	})
#endif

#ifndef list_entry
#define list_entry(ptr,type,member) \
	container_of(ptr,type,member)
#endif

#ifndef list_for_each_entry
#define list_for_each_entry(pos,head,member) \
	for(pos = list_entry((head)->next,typeof(*pos),member); \
		&pos->member !=head; \
		pos = list_entry(pos->member.next,typeof(*pos),member))
#endif

static inline void list_init(list_t *list)
{
	list->next = list;
	list->prev = list;
}

static inline boolean mq_is_empty(mq_t *mq)
{
	pthread_mutex_lock(&mq_mutex);
	boolean flag;
	list_t *head = &mq->list;
	return head->prev==head?TRUE:FALSE;
	if(head->prev == head)
		flag = TRUE;
	else
		flag = FALSE;
	pthread_mutex_unlock(&mq_mutex);
	return flag;
}

static inline void list_del(list_t *head,list_t *node)
{
	node->next->prev = node->prev;
	head->next = node->next;
	list_init(node);
}

static inline list_t* list_del_back(list_t *head)
{
	if(head->prev == head)
		return NULL;
	list_t *current = head->prev;
	current->prev->next = head;
	head->prev = current->prev;
	list_init(current);
	return current;
}

static inline list_t* mq_get(mq_t *mq)
{
	pthread_mutex_lock(&mq_mutex);
	list_t *head = &mq->list;
	list_t *current = NULL;
	current =  list_del_back(&mq->list);
	if(current)
		mq->size--;
//	printf("GET size = %d\n",mq->size);
//	pthread_cond_signal(&mq_cond);
	pthread_mutex_unlock(&mq_mutex);
	return current;
}

static inline void list_add_front(list_t* head, list_t *current)
{
	current->next = head->next;
	head->next->prev = current;
	head->next = current;
	current->prev = head;
}

static inline void mq_put(mq_t *mq, list_t *current)
{
	pthread_mutex_lock(&mq_mutex);
	list_add_front(&mq->list,current);
	mq->size++;
//	pthread_cond_signal(&mq_cond);
//	printf("PUT size = %d\n",mq->size);
	pthread_mutex_unlock(&mq_mutex);
//	printf("aba\n");
}

static inline void mq_release(mq_t *mq)
{
	pthread_mutex_destroy(&mq->mq_mutex);
//	pthread_cond_destroy(&mq_cond);
}

static inline mq_t *get_mq(s32_t index)
{
	if(!hash_mq)
		return NULL;
	return hash_mq[index] == NULL?NULL:hash_mq[index];
}

static inline mq_t* mq_init(struct ev_io *ev, u32_t id)
{
	if(!hash_pool)
		hash_pool = CreateMemoryPool(sizeof(mq_t));
	if(!hash_pool)
		return NULL;
	mt_t *mq = Malloc(hash_pool);
	if(!mq)
		return NULL;
	if(hash_mq[ev->fd] != NULL)
	{
		Free(hash_pool,mq);
		return hash_mq[ev->fd];
	}

	pthread_mutex_init(&mq->mq_mutex,NULL);
//	pthread_cond_init(&mq_cond,NULL);
	mq->ev = ev;
	mq->id = id;
	list_init(&mq->list);
	mq->size = 0;
	hash_mq[ev->fd] = mq;
	return hash_mq[ev->fd];
}

#endif
