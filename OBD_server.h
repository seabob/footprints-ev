#ifndef __OBD_SERVER_H__
#define __OBD_SERVER_H__


#include "OBD_mq.h"
#include "MemoryPool.h"
#include "threadpool.h"
#include "types_define.h"

typedef struct OBD_server_struct{
	mq_t			mq;
	mysql_t		 	mysqlpool;
	redis_t			redis;
	memory_t		obd_mempool;
	memory_t		data_mempool;
	tpool_t			thread_pool;
	boolean (*init)(server_t*);
	void (*release)(server_t*);
}server_t;

//static server_t Server;

static inline void OBD_server_release(server_t *server)
{
	threadpool_release(&server->thread_pool);
	Data_memory_pool_release(&server->data_mempool);
	OBD_memory_pool_release(&server->obd_mempool);
	redis_release(&server->redis);
	mysql_release(&server->mysqlpool);
	mq_release(&server->mq);
}

#define OBD_DATA_LENGTH		256
static inline boolean OBD_server_init(server_t *server,u16_t length)
{
	if(!server)
		return FALSE;
	if(mq_init(&server->mq))
		return FALSE;
	if(mysql_init(&server->mysqlpool))
		return FALSE;
	if(redis_init(&server->redis))
		return FALSE;
	if(memory_pool_init(&server->obd_mempool,length))
		return FALSE;
	if(memory_pool_init(&server->data_mempool,OBD_DATA_LENGTH))
		return FALSE;
	if(threadpool_init(&server->thread_pool))
		return FALSE;
	server->init = OBD_server_init;
	server->release = OBD_server_release;
	return TRUE;

}

#endif
