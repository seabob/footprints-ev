#ifndef __OBD_STRUCT_H__
#define __OBD_STRUCT_H__

#include <stdlib.h>
#include <string.h>
#include "OBD_mq.h"
#include "string_ops.h"
//#include "OBD_cmd_id.h"
//#include "./FULMemoryPool/CProjectDfn.h"
#include "./FULMemoryPool/MemoryPool.h"

typedef unsigned char	boolean;
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define BUF_ZIZE	512
typedef struct obd_struct{
	char	data[BUF_ZIZE];
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

#define DATA_POS_BEGIN	4
#define DATA_POS_END	4
static inline boolean OBD_check_sum(OBD_t* obd)
{
	char *begin = strstr(obd->data,"BB");
	char *end = strstr(obd->data,"EE")-2;
	char *ptr = begin;
	u8_t i = 0, j = 0, tmp = 0, flag = 0,len = 0,sum = 0;

	if(!begin || !end || (end < begin))
		return FALSE;
	
	for(i = 0; i+2 <= len; i+=2)
	{
		if(!flag)
		{
			tmp = str2hex(begin[i],begin[i+1]);
			if(tmp == 0xAA)
			{
				flag = 1;
				continue;
			}
			sum += tmp;
		}else
		{
			sum += str2hex(begin[i+1],begin[i+1]);
			flag = 0;
		}
	}
	sum %= 256;
	tmp = str2hex(end[0],end[1]);
	printf("sum = 0x%x tmp = 0x%x\n",sum,tmp);
	if(tmp == sum)
		return TRUE;
	return FALSE;
}

static inline void OBD_init(OBD_t *obd, void *buf)
{
	if(!buf)
		return;
	list_init(&obd->list);
//	obd->data = (void*)buf;
}


static inline void OBD_decode(OBD_t *obd)
{
}

#endif
