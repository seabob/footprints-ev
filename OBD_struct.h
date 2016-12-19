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

#define BUF_SIZE	512
typedef struct obd_struct{
	char	data[BUF_SIZE];
	list_t	list;
}OBD_t;

MemoryPool_t *obd_pool;

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

static inline OBD_t* OBD_init(void)
{
	OBD_t *obd = Malloc(obd_pool);
	if(!obd)
		return;
	list_init(&obd->list);
	memset(obd->data,0,BUF_SIZE);
	return obd;
}

static inline void OBD_release(OBD_t* obd)
{
	if(!obd)
		return;
	list_init(&obd->list);
	memset(obd->data,0,BUF_SIZE);
	Free(obd_pool,obd);
}

static inline void OBD_decode(OBD_t *obd)
{
}

#endif
