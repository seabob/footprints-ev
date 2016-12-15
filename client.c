#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "OBD_network.h"
//static network_t network;
static void thread_func(void *data)
{
	int ret = 0,i = 0,j = 1;
	network_t network;
	network_t *net = &network;
	int accept_fd;
	char sendbuf[512] = {0};
	char buffer[512] = {0};

	network_init(net);

	net->addr_init(net);

	while(net->socket_init(net)< 0)
	{
		printf("%s:%d\n",__func__,__LINE__);
		usleep(10);
//		printf("%s:%d fd = %d\n",__func__,__LINE__,net->socket);
	}
//	if(net->socket_bind(net))
//	{
//		printf("%s:%d\n",__func__,__LINE__);
//		return -1;
//	}

	connect(net->socket,(struct sockaddr*)&net->address,sizeof(net->address));
		printf("%s:%d\n",__func__,__LINE__);
	for(i = 0; i < 10000;i++)
	{
	//	usleep(10);
		memset(sendbuf,0,512);
		memset(buffer,0,512);
		sprintf(sendbuf,"AA111111BB8111F11%dEE",j);
		j++;
		if(j == 10)
			j=1;
		send(net->socket,sendbuf,sizeof("AA000000BB8111F101EE"),0);
		ret = recv(net->socket,buffer,512,0);
		buffer[ret] = '\0';
//		printf("%s\n",buffer);
	}
//	printf("fd[%d] = %s\n",net->socket,buffer);
	close(net->socket);
	printf("%s\n",buffer);
	return ;

}
#define THREAD_NUM	5
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
