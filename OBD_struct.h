#ifndef __OBD_STRUCT_H__
#define __OBD_STRUCT_H__

#include <stdlib.h>
#include <string.h>
#include "OBD_mq.h"
#include "string_ops.h"
#include "OBD_structs_new.h"
#include "OBD_cmd_id.h"
//#include "OBD_cmd_id.h"
//#include "./FULMemoryPool/CProjectDfn.h"
#include "./FULMemoryPool/MemoryPool.h"
//#include "./mysqlpool/mysqlpool.h"

typedef unsigned char	boolean;
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif
static int __cmd_count = 0;
typedef struct obd_struct{
	u8_t	data[BUF_SIZE];
	u8_t	data_length;
	u8_t	hex[BUF_SIZE];
	u8_t	id;
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
static inline boolean OBD_checksum(u8_t* data)
{
	char *begin = strstr(data,"BB");
	char *end = strstr(data,"EE")-2;
	char *ptr = begin;
	u8_t i = 0, j = 0, tmp = 0, flag = 0,len = 0;
	u32_t sum = 0;

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
//	printf("sum = 0x%x tmp = 0x%x\n",sum,tmp);
	if(tmp == sum)
		return TRUE;
	return FALSE;
}

static inline boolean OBD_filter_escape_and_transfer_to_hex(OBD_t* obd)
{
	char *begin = strstr(obd->data,"BB");
	char *end = strstr(obd->data,"EE");
	char *ptr = begin;
	u8_t i = 0, j = 0, flag = 0, tmp = 0;

	if(!begin || !end || (end < begin))
		return FALSE;

	for(i = 0; i < obd->data_length; i+=2)
	{
		if(!flag)
		{
			tmp = str2hex(begin[i],begin[i+1]);
			if(tmp == 0xAA)
			{
				flag = 1;
				continue;
			}
		}else
		{
			tmp = str2hex(begin[i+1],begin[i+1]);
			flag = 0;
		}
		obd->hex[j] = tmp;
//		printf("0x%x\n",obd->hex[j]);
		j++;
	}
	return TRUE;
}

static inline OBD_t* OBD_init(void)
{
	OBD_t *obd = Malloc(obd_pool);
	if(!obd)
		return NULL;
	list_init(&obd->list);
	obd->data_length = 0;
	memset(obd->data,0,BUF_SIZE);
	memset(obd->hex,0,BUF_SIZE);
	return obd;
}

static inline void OBD_release(OBD_t* obd)
{
	if(!obd)
		return;
	list_init(&obd->list);
	memset(obd->data,0,BUF_SIZE);
	memset(obd->hex,0,BUF_SIZE);
	obd->data_length = 0;
	Free(obd_pool,obd);
}

static inline void OBD_decode1(OBD_t *obd)
{
	cmd00_t *cmd = (cmd00_t*)obd->hex;
//	printf("obd->cmd = %d\n",cmd->head.cmd);
}

static inline void OBD_decode(OBD_t *obd)
{

	switch(obd->hex[1])
	{
		case CMD0x00:
			break;
		case CMD0x01:
			break;
		case CMD0x02:
		{
			cmd02_t *cmd = (cmd02_t*)obd->hex;
//			printf("client_id len =%d\n",sizeof(cmd->head.id));
//			u8_t len = sizeof(cmd->head.id);
//			printf("[%d]: %s\n",__cmd_count,obd->data);
//			for(i = 0; i < len; i++)
//				printf("client_id[%d] = 0x%2x\n",i,cmd->head.id[i]);
			break;
		case CMD0x03:
			break;
		case CMD0x07:
			break;
		case CMD0x08:
			break;
		case CMD0x10:
			break;
		case CMD0x13:
			break;
		case CMD0x14:
			break;
		case CMD0x15:
			break;
		case CMD0x16:
			break;
		case CMD0x17:
			break;
		case CMD0x18:
			break;
		case CMD0x19:
			break;
		case CMD0x1a:
			break;
		case CMD0x1b:
			break;
		case CMD0x21:
			break;
		case CMD0x22:
			break;
		case CMD0x30:
			break;
		case CMD0x32:
			break;
		case CMD0x33:
			break;
		case CMD0x34:
			break;
		case CMD0x42:
			break;
		case CMD0x43:
			break;
		case CMD0x50:
			break;
		default:
			break;
		}
	}
}

static inline void OBD_response(OBD_t *obd)
{
	
}

#endif
