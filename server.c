
#include "server.h"


account_t* p;
int readers;
sem_t read;
sem_t write;
sem_t welcome;


void organized_cleaning(int signal){
	shmctl(shmid, IPC_RMID, NULL);
	/*Note: shmid is a global var. Further, sigint is blocked while shared memory is being created, 
	so shmid is guaranteed to be a pointer to a valid block of shared memory*/
	raise(signal);
}

void ChildSigHandler(int signal){
	pid_t pid;
	int status;
	while( (pid = waitpid(-1,&status,WNOHANG)) == -1){
		sleep(1);
	}
	printf("Child process killed; PID: %d\n", (int)pid );
}

void periodic_printing(int signal, siginfo_t *ignore, void *ignore2){
	//Print account info every 20 seconds. Raise a signal in Server main function.
}


int socks(const char* port){
	int sd;
	int n;
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

	action.sa_handler = ChildSigHandler;
	action.sa_flags = 0;
	sigemptyset (&action.sa_mask);
	sigaddset(&action.sa_mask,SIGCHLD);

	addrinfo.ai_flags = AI_PASSIVE;		// for bind()
	addrinfo.ai_family = AF_INET;		// IPv4 only
	addrinfo.ai_socktype = SOCK_STREAM;	// Want TCP/IP
	addrinfo.ai_protocol = 0;		// Any protocol
	addrinfo.ai_addrlen = 0;
	addrinfo.ai_addr = NULL;
	addrinfo.ai_canonname = NULL;
	addrinfo.ai_next = NULL;
	if ( getaddrinfo( 0, port, &addrinfo, &result ) != 0 ){
		fprintf( stderr, "\x1b[1;31mgetaddrinfo( %s ) failed errno is %s.  File %s line %d.\x1b[0m\n", CLIENT_PORT, strerror( errno ), __FILE__, __LINE__ );
		return -1;
	}
	else if ( errno = 0, (sd = socket( result->ai_family, result->ai_socktype, result->ai_protocol )) == -1 ){
				printf("socket failed");
				return -1;
	}

	else if (setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1 )
	{
		printf( "setsockopt() failed");
		return 0;
	}
	else if (errno = 0, bind( sd, result->ai_addr, result->ai_addrlen ) == -1)
	{
		printf( "bind() failed in  line  errno \n" );
		close( sd );
		return 0;
	}
	else if ( listen( sd, 100 ) == -1 )
	{
		printf("listen failed");
	//	printf( "Listen failed. Speak up or I have to put in my hearing aids.\nlisten() failed in %s() line %d errno %d\n", func, __LINE__, errno );
		close( sd );
		return 0;
	}

	else
	{
		printf("%s\n","Server running...waiting for connections.");


		if(sigaction(SIGCHLD,&action,NULL)<0){
			perror("Sigaction failed");
			return 1;
		}
		while((sporkd = accept(sd,(struct sockaddr *)&them, &addrlen)) != -1){
			addrlen = sizeof(struct sockaddr_storage);


			pid = fork();
			if(pid < 0)
			{
				perror("Forking error.");
				close(sporkd); // Make sure this is ok
			}
			else if(pid == 0)//Is Child process
			{
				close(sd);

				//client_session(sporkd);
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

void sharingcaring(){
	/* Shared Memory Section*/

	int shmid;
	//	account_t* p; Is declared globally
	key_t key;

	int size;
	size = 20 * sizeof(account_t); // 20 accounts
	if(errno = 0, (key = ftok("testplan.txt",42)) == -1){
		printf("ftok failed; errno :  %s\n", strerror( errno ));
		exit( 1 );
	}
	else if(errno = 0, (shmid = shmget(key,size,IPC_CREAT | IPC_EXCL)) == -1){
		printf("shmget failed; errno :  %s\n", strerror( errno ));
		exit( 1 );
	}
	else if(errno = 0, (p = (account_t*) shmat(shmid,NULL,0)) == (void*) -1) {
		printf( "shmat() failed; errno :  %s\n", strerror( errno ) );
		exit( 1 );
	}
	
	//shared mem sucess.  Begin Server/Client Comunnications.
}

int main(){

	int sd;
	struct sigaction memclean;
	if(sem_init(read,1,21) == -1){
		printf("Read semaphore init fail.");
	}
	else if(sem_init(write,1,1) == -1){
		printf("Write semaphore init fail.");
	}
	else if(sem_init(welcome,1,1) == -1){
		print("Welcome semaphore init fail.");
	}
	readers = 0;
	//Semaphores at ready. No one reading. 
	memclean.sa_handler = organized_cleaning;
	sigemptyset (&memclean.sa_mask);
	sigaddset (&memclean.sa_mask, SIGINT);
	memclean.sa_flags = 0;
	sigaction(SIGINT, &memclean, NULL);
	sigprocmask(SIG_BLOCK, &memclean.sa_mask, NULL);
	//Note, there are forks in the server, but no threads... The forked processes are 2threaded. 
	
	//Shared Memory Setup
	//sharingcaring();
	//Shared Memory Init

	
	//account data[20] = p;//Not sure if this is ok?
	
	
	
	sigprocmask(SIG_UNBLOCK, &memclean.sa_mask, NULL);

	//Server-Client Service
	socks("54261");


	return 0;
}
