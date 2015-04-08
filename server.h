#ifndef server_h
#define server_h
#include <sys/types.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>


#define CLIENT_PORT 514965

	//From BKR's server.c
	static void set_iaddr(struct sockaddr_in * sockaddr, long x, unsigned int port);

	/*
	Has a particular network socket descriptor to recv on and send to
	this is what we'll fork
	*/
	int service(int sd);
	
	/*
	socket, bind, listen, accept
	*/
	int socks();
	
	
	



#endif //Server.h
