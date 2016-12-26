#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "OBD_network.h"
//static network_t network;
#if 1
static char cmd00[88] = {0xbb,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,
			 0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,
			 0x2,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,
			 0x3,0x3,0x3,0x3,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,
			 0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x4,0x5,0x5,0x5,
			 0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x5,0x6,0x6,
			 0x7,0x7,0x8,0x1e,0xee};
#endif
static void thread_func(void *data)
{
	int ret = 0,i = 0,j = 1;
	network_t network;
	network_t *net = &network;
	int accept_fd;
	unsigned char sendbuf[512] = {0};
	char buffer[512] = {0};

	network_init(net);

	net->addr_init(net);

	if(net->socket_init(net)< 0)
	{
		printf("%s:%d\n",__func__,__LINE__);
		return -1;
//		printf("%s:%d fd = %d\n",__func__,__LINE__,net->socket);
	}
//	if(net->socket_bind(net))
//	{
//		printf("%s:%d\n",__func__,__LINE__);
//		return -1;
//	}
		memset(sendbuf,0,512);
		memset(buffer,0,512);
//		memcpy(buffer,cmd00,89);
		sendbuf[0] = 0xbb;
		sendbuf[1] = 0x00;
		for(i = 2; i < 89; i++)
		{
			sendbuf[i] = i;
		}
		printf("sendbuf[0] = 0x%x\n",sendbuf[0]);
	connect(net->socket,(struct sockaddr*)&net->address,sizeof(net->address));
		printf("%s:%d \n",__func__,__LINE__);
	for(i = 0; i < 10000;i++)
	{
//		printf("%d",i);
//		printf("\n");
//		sprintf(sendbuf,"AA111111BB8111F%d11EE",j);
//		sprintf(sendbuf,"BB02123456780102030400EE");
//		send(net->socket,sendbuf,89,0);
		send(net->socket,cmd00,89,0);
//		usleep(1);
		ret = recv(net->socket,buffer,512,0);
//		buffer[ret] = '\0';
//		printf("%s\n",buffer);
	}
//	printf("fd[%d] = %s\n",net->socket,buffer);
	close(net->socket);
	printf("%s\n",buffer);
	return ;

}
#define THREAD_NUM	1
int main(int argv, char **argc)
{
	pthread_t t[THREAD_NUM];
	int i = 0;
	for (i = 0; i < THREAD_NUM; i++)
		pthread_create(&t[i],NULL,thread_func,NULL);
//		thread_func(NULL);
	for(i = 0; i < THREAD_NUM; i++)
	pthread_join(t[i],NULL);
	return 0;
}
