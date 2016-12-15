#ifndef __OBD_STRUCT_H__
#define __OBD_STRUCT_H__

#include <stdlib.h>
#include <string.h>
#include "OBD_mq.h"
#include "string_ops.h"
//#include "./FULMemoryPool/CProjectDfn.h"
#include "./FULMemoryPool/MemoryPool.h"

typedef unsigned char	boolean;
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

typedef struct obd_struct{
	char	*data;
	void	*response;
	char	*sql;
	char	*redis;
	int	fd;
	boolean (*init)(struct obd_struct*);
	u8_t (*get_cmd)(struct obd_struct*);
	void (*filter_escape)(struct obd_struct*);
	boolean	(*check_sum)(struct obd_struct*);
	char* (*pack_to_sql)(struct obd_struct*);
	char* (*pack_to_redis)(struct obd_struct*);
	list_t	list;
}OBD_t;

static inline u8_t OBD_get_cmd(OBD_t *obd)
{
	char *data = (char*)obd->data;
	return str2hex(data[2],data[3]);
}

static inline boolean OBD_check_sum(OBD_t* obd)
{
	return TRUE;	
}

static inline void OBD_init(OBD_t *obd, void *buf)
{
	if(!buf)
		return;
	list_init(&obd->list);
	obd->data = (void*)buf;
}


#endif
