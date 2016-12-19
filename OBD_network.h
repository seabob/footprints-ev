#ifndef __OBD_SERVER_H__
#define __OBD_SERVER_H__

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types_define.h"

typedef struct sockaddr_in	sockaddr_in_t;

typedef struct network_struct{
	s32_t		socket;
	sockaddr_in_t	address;
	s8_t (*init)(struct network_struct*);
	void (*addr_init)(struct network_struct*);
	s8_t (*socket_init)(struct network_struct*);
	s8_t (*socket_bind)(struct network_struct*);
	s8_t (*socket_listen)(struct network_struct*);
	void (*release)(struct network_struct*);
}network_t;

#define SERVER_PORT	11223
#define SERVER_ADDR	"192.168.0.115"
static inline void network_address_init(network_t *network)
{
	bzero(&network->address,sizeof(network->address));
	network->address.sin_family = AF_INET;
	network->address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	network->address.sin_port = htons(SERVER_PORT);
}

static inline s8_t network_socket_init(network_t *network)
{
	network->socket = socket(PF_INET,SOCK_STREAM,0);
	return network->socket;
}


static inline s8_t network_socket_bind(network_t *network)
{
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
//	setsockopt(network->socket,SOL_SOCKET,SO_REUSEADDR,&ops,sizeof(ops));
	setsockopt(network->socket,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
	return bind(network->socket,(struct sockaddr*)&network->address,sizeof(network->address));
}

static inline void network_socket_listen(network_t *network)
{
	listen(network->socket,32);
	
}

static inline void network_release(network_t *network)
{
	return;	
}

static inline s8_t network_init(network_t *network)
{
	network->init = network_init;
	network->socket_init = network_socket_init;
	network->addr_init = network_address_init;
	network->socket_bind = network_socket_bind;
//	network->socket_listen = network_socket_listen;
	network->release = network_release;
	return 0;
}

#endif
