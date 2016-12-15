#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "OBD_network.h"

void * thread_func(void *data)
{
//	while(1)
	{
	int ret = 0,i = 0,j = 0;
	network_t network;
	network_t *net = &network;
	int accept_fd;
	char sendbuf[512] = {0};
	char buffer[512] = {0};

	network_init(net);
	net->addr_init(net);
	if(!net->socket_init(net))
	{
		printf("%s:%d\n",__func__,__LINE__);
		return  NULL;	
	}

	connect(net->socket,(struct sockaddr*)&net->address,sizeof(net->address));
	while(1){
		if(j >= 50)
			break;
		j++;
		sleep(1);
		memset(sendbuf,0,512);
		memset(buffer,0,512);
		sprintf(sendbuf,"AA000000BB8111F10%dEE",i);
		i++;
		if(i == 10)
			i=0;
		send(net->socket,sendbuf,sizeof("AA000000BB8111F101EE"),0);
		recv(net->socket,buffer,512,0);
	}
		printf("buffer =  %s\n",buffer);
		close(net->socket);
	}
	return (void*)NULL;
}
int main(int argv, char **argc)
{
	pthread_t t[2];
	int i = 0;
	for(i = 0; i < 2 ;i++)
		pthread_create(&t[i],NULL,thread_func,NULL);
	for(i=0; i < 2; i++)
		pthread_join(t[i],NULL);
	return 0;
	
}
