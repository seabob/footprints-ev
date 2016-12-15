#include <stdio.h>
#include <stdlib.h>
#include "OBD_network.h"
#include "OBD_ev_struct.h"

static network_t network;
static OBD_ev_t ev;

static void ev_signal_cb(ev_loop_t *loop,ev_signal *w,int revents)
{
	printf("[ev] into ev_signal_cb\n");
	ev_break(loop,EVBREAK_ALL);
}

static void free_libev(ev_loop_t *loop,s32_t fd)
{
	if(fd > SOCKET_ALLOC_MAX_CLIENT)
	{
		return ;	
	}
	if(libev_list[fd] == NULL)
	{
		return ;	
	}
	close(fd);
	ev_io_stop(loop,libev_list[fd]);
	free(libev_list[fd]);
	return ;
}

static void ev_read_cb(ev_loop_t *loop,ev_io_t *w, int revents)
{
	OBD_ev_t *pev = &ev;
	ssize_t read;
	char buffer[4096] = {0};
	if(EV_ERROR &revents)
	{
		printf("%s:%d event error \n",__func__,__LINE__);
		return ;
	}

	read = recv(w->fd,buffer,4096,0);
	if(read < 0)
	{
		printf("%s:%d read error\n",__func__,__LINE__,read);
		return ;
	}else if(read == 0)
	{
		printf("client disconnected\n")	;
		free_libev(pev->loop,w->fd);
		return ;
	}else
	{
		buffer[read] = '\0';
	}
	printf("buffer = %s\n",buffer);

}


static void ev_accept_cb(ev_loop_t *loop,ev_io_t *w, int revents)
{
	OBD_ev_t *pev = &ev;
	sockaddr_in_t client_addr;
	socklen_t client_len = sizeof(client_addr);
	ev_io_t *w_client = (ev_io_t*)malloc(sizeof(ev_io_t));
	u32_t client_socket;
	if(!w_client)
	{
		printf("%s:%d malloc err\n",__func__,__LINE__);
		return ;
	}
	if(EV_ERROR & revents)
	{
		printf("%s:%D event error\n",__func__,__LINE__);
		return ;
	}
	client_socket = accept(w->fd,(sockaddr_in_t*)&client_addr,&client_len);
	if(client_socket < 0)
	{
		printf("%s:%d accept fd error\n",__func__,__LINE__);
		return ;
	}
	if(client_socket > SOCKET_ALLOC_MAX_CLIENT)
	{
		printf("%s:%d fd too large [%d]\n",client_socket);
		close(client_socket);
		return ;
	}
	if(libev_list[client_socket])
	{
		printf("%s:%d libev_list[%d] is in use\n",__func__,__LINE__,client_socket);
		return ;
	}
	ev_io_init(w_client,ev_read_cb,client_socket,EV_READ)'
	ev_io_start(pev->loop,w_client);
	libev_list[client_socket] = w_client;
	printf("Socket connecting ... libev_list[fd]->fd = %s\n",libev_list[client_socket]->fd);
}

int main(int argv, char **argc)
{
	int ret = 0;
	int accept_fd;
	network_t *net = &network;
	OBD_ev_t *pev = &ev;
	ev_signal signal_watcher;
	char buffer[4096] = {0};

	network_init(net);
	net->addr_init(net);
	if(!net->socket_init(net))
	{
		printf("%s:%d  %d\n",__func__,__LINE__,net->socket);
		return -1;	
	}
	if(net->socket_bind(net))
	{
		printf("%s:%d\n",__func__,__LINE__);
		return -1;
	}
	if(net->socket_listen(net))
	{
		printf("%s:%d\n",__func__,__LINE__);
		return -1;
	}
	
	OBD_ev_init(pev);
	ev_signal_init(&signal_watcher,ev_signal_cb,SIGINT);
	ev_signal_start(loop,&signal_watcher);

	ev_io_init(&pev->socket_accept,ev_accept_cb,net->socket,EV_READ);

	while(1)
	{
		accept_fd = accept(net->socket,(sockaddr_in_t*)NULL,NULL);
		if(accept_fd < 0)
		{
		printf("%s:%d\n",__func__,__LINE__);
			return -1;	
		}
		memset(buffer,0,4096);
		ret = recv(accept_fd,buffer,4096,0);
		printf("recv %s\n",buffer);

		ret = send(accept_fd,"you are connected\n",17,0);
		if(ret < 0)
			return -1;
		close(accept_fd);
	}
	close(net->socket);
	return 0;

}
