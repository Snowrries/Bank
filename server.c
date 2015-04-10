
#include "server.h"

void organized_cleaning(int signal){
	shmctl(shmid, IPC_RMID, NULL);
}

void ChildSigHandler(int signal){
	pid_t pid;
	int status;
	while( (waitpid(-1,&status,WNOHANG)) == -1){

		//Do something with child return status if needed.

	}
	
}



void client_session(int sd){

	char request[128];
	char response[2048];
	char temp;
	int i;
	int limit,size;
	float ignore;
	long senderIPaddr;
	
	while(1){
		while(read(sd,request,sizeof(request)) > 0){ //Not sure this is right
			printf(("server receives input: %s \n"), request);
			size += strlen(request);
		}
		size = 0;
		

	}

}


int socks(){
	int sd;
	int sporkd;
	int pid;
	struct sockaddr_in addr;
	struct addrinfo	addrinfo;
	struct addrinfo *result;
	socklen_t addrlen;
	struct sockaddr_storage them;
	char message[256];
	int on = 1;

	struct sigaction action;

	action.sa_handler = &ChildSigHandler;
	action.sa_flags = 0;
	sigemptyset (&action.sa_mask);
	sigaddset(action.sa_mask,SIGCHLD);

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
		//printf( "setsockopt() failed in %s()\n", func );
		return 0;
	}
	else if ( set_iaddr( &addr, INADDR_ANY, CLIENT_PORT ), errno = 0,
			bind( sd, (const struct sockaddr *)&addr, sizeof(addr) ) == -1 )
	{
		//printf( "bind() failed in %s() line %d errno %d\n", func, __LINE__, errno );
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
	/*	ic = sizeof(senderAddr);
		while ( (sporkd = accept( sd, (struct sockaddr *)&senderAddr, &ic )) != -1 )
		{
			fdptr = (int *)malloc( sizeof(int) );
			*fdptr = sporkd;					// pointers are not the same size as ints any more.
		}
		close( sd );
		return 0;
	}*/
		if(sigaction(SIGCHLD,&act,NULL)<0){
			perror("Sigaction failed");
			return 1;
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
				close(sd);

				client_session(sporkd);
				exit(0);//This'll send a sigchld signal.
			}
			else//Is parent process
			{
				printf("Created child process %d\n", pid);
				close(sporkd);
			}
		}
	}
	return 0;
	//return sd;
}


int main(){

	int sd;
	struct sigaction memclean;
	memclean.sa_handler = organized_cleaning;
	sigemptyset (&memclean.sa_mask);
	sigaddset (&memclean.sa_mask, SIGINT);
	memclean.sa_flags = 0;
	sigaction(SIGINT, &memclean, NULL);
	sigprocmask(SIG_BLOCK, &memclean.sa_mask, NULL);
	//Note, there are forks in the server, but no threads... The forked processes are 2threaded. 
	

	/* Shared Memory Section
	key_t key;
	int shmid;
	char* p;
	int size;
	size = 4096; // 4kB
	if(errno = 0, (key = ftok("testplan.txt",42)) == -1){

	}
	else if(errno = 0, (shmid = shmget(key,size,IPC_CREAT | IPC_EXCL)) != -1){
		p = (char*) shmat(shmid,0,0);
		if( p == (void*) -1){
			printf( "shmat() failed  errno :  %s\n", strerror( errno ) );
			exit( 1 );
		}
		else{
			//shared mem sucess.  Begin Server/Client Comunnications.
		}
	}
	else if(errno = 0, (shmid = shmget(key,0 , 0666)) != -1){
		errno = 0;
		p = (char *)shmat(shmid, 0 ,0 );
		if(p == (void * )-1){
			printf("failed");

		}
		else
		{
			//shared mem
		}
	}
	else
	{
	printf("shmget failed");
	}*/
	
	sigprocmask(SIG_UNBLOCK, &memclean.sa_mask, NULL);

	//Server-Client Service
	socks();

}
