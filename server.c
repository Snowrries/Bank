
#include "server.h"



void ChildSigHandler(int signal){

	while( (wait(0)) != -1){

	}
}


static void set_iaddr(struct sockaddr_in * sockaddr, long x, unsigned int port){
	memeset(sockaddr, 0, sizeof(*sockaddr));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(port);
	sockaddr->sin_addr.s_addr = htonl(x);

}


int socks(){
	int sd;
	int sporkd;
	int addrlen;
	pid_t pid;
	struct sockaddr_in add;
	struct addrinfo	addrinfo;
	struct addrinfo *result;
	char message[256];
	int on = 1;
	const char *port;
	port = "What's our port?";

	addrinfo.ai_flags = AI_PASSIVE;		// for bind()
	addrinfo.ai_family = AF_INET;		// IPv4 only
	addrinfo.ai_socktype = SOCK_STREAM;	// Want TCP/IP
	addrinfo.ai_protocol = 0;		// Any protocol
	addrinfo.ai_addrlen = 0;
	addrinfo.ai_addr = NULL;
	addrinfo.ai_canonname = NULL;
	addrinfo.ai_next = NULL;
	if ( getaddrinfo( 0, port, &addrinfo, &result ) != 0 ){
		fprintf( stderr, "\x1b[1;31mgetaddrinfo( %s ) failed errno is %s.  File %s line %d.\x1b[0m\n", port, strerror( errno ), __FILE__, __LINE__ );
		return -1;
	}
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Socketing failed. errno: %d", errno);
		return -1;
	}
	if((bind(sd, (struct sockaddr *)&add, sizeof(struct sockaddr))) == -1){
		printf("Bind failed. errno: %d", errno);
		return -2;
	}
	if(listen(sd,128) == -1){
		printf("Listen failed. Speak up or I have to put in my hearing aids. Errno: %d", errno);

	}
	
	while(1){
		addrlen = sizeof(struct sockaddr_storage);
		sporkd = accept(sd, )
	}
	
	
	pid = fork();
	if(pid == 0){
		signal(/*SIGSOMETHING*/,ChildSigHandler);
	}


	return sd;
}
