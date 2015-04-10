
#include "server.h"



void ChildSigHandler(int signal){

	//while( (wait(0)) != -1){
	//}
	
}


static void set_iaddr(struct sockaddr_in * sockaddr, long x, unsigned int port){
	memeset(sockaddr, 0, sizeof(*sockaddr));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(port);
	sockaddr->sin_addr.s_addr = htonl(x);

}

void *client_session_thread(void* arg){

	int sd;
	char request[2048];
	char response[2048];
	char temp;
	int i;
	int limit,size;
	float ignore;
	long senderIPaddr;

	sd = *(int *) arg;
	free(arg);

	while(read(sd,request,sizeof(request)) >0){
		printf(("server receives input: %s \n"), request);
		size = strlen(request);

	}

}


int socks(){
	int sd;
	int sporkd;
	int pid;
	struct sockaddr_in add;
	struct addrinfo	addrinfo;
	struct addrinfo *result;
	socklen_t addrlen;
	struct sockaddr_storage them;
	char message[256];
	int on = 1;

	addrinfo.ai_flags = AI_PASSIVE;		// for bind()
	addrinfo.ai_family = AF_INET;		// IPv4 only
	addrinfo.ai_socktype = SOCK_STREAM;	// Want TCP/IP
	addrinfo.ai_protocol = 0;		// Any protocol
	addrinfo.ai_addrlen = 0;
	addrinfo.ai_addr = NULL;
	addrinfo.ai_canonname = NULL;
	addrinfo.ai_next = NULL;
	if ( getaddrinfo( 0, CLIENT_PORT, &addrinfo, &result ) != 0 ){
		fprintf( stderr, "\x1b[1;31mgetaddrinfo( %s ) failed errno is %s.  File %s line %d.\x1b[0m\n", CLIENT_PORT, strerror( errno ), __FILE__, __LINE__ );
		return -1;
	}
	/*if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Socketing failed. errno: %d", errno);
		return -1;
	}
	if((bind(sd, (struct sockaddr *)&add, sizeof(struct sockaddr))) == -1){
		printf("Bind failed. errno: %d", errno);
		return -2;
	}*/
	else if ( setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1 )
	{
		printf( "setsockopt() failed in %s()\n", func );
		return 0;
	}
	else if ( set_iaddr( &addr, INADDR_ANY, CLIENT_PORT ), errno = 0,
			bind( sd, (const struct sockaddr *)&addr, sizeof(addr) ) == -1 )
	{
		printf( "bind() failed in %s() line %d errno %d\n", func, __LINE__, errno );
		close( sd );
		return 0;
	}
	else if ( listen( sd, 100 ) == -1 )
	{
		printf( "Listen failed. Speak up or I have to put in my hearing aids.\nlisten() failed in %s() line %d errno %d\n", func, __LINE__, errno );
		close( sd );
		return 0;
	}
	
	else
	{
		ic = sizeof(senderAddr);
		while ( (sporkd = accept( sd, (struct sockaddr *)&senderAddr, &ic )) != -1 )
		{
			fdptr = (int *)malloc( sizeof(int) );
			*fdptr = sporkd;					// pointers are not the same size as ints any more.
		}
		close( sd );
		return 0;
	}
	
	while(1){
		addrlen = sizeof(struct sockaddr_storage);
		sporkd = accept(sd,(struct sockaddr *)&them, &addrlen);
		pid = fork();
		if(pid == -1)
		{
			perror("Forking error.");
			close(sporkd); // Make sure this is ok
		}
		else if(pid == 0)//Is Child process
		{
//			signal(SIGCHLD,ChildSigHandler); ?? 
		}
		else//Is parent process
		{
			printf("Created child process %d\n", pid);
			close(sporkd);
		}
	}
	return 0;
	//return sd;
}
