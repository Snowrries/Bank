#ifndef server_h
#define server_h
#include <sys/types.h>
#include <sys/socket.h>

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