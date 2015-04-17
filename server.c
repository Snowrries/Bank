#include "server.h"

int readers = 0;

account_t* p;
int shmid;
int sd;
sem_t actionCycleSemaphore;
//static pthread_attr_t	user_attr;
static pthread_attr_t	kernel_attr;
sem_t *reado;
sem_t *writeo;
sem_t *welcome;


void organized_cleaning(int signale){

	/*Note: shmid is a global var. Further, sigint is blocked while shared memory is being created, 
	so shmid is guaranteed to be a pointer to a valid block of shared memory*/
	sem_destroy(reado);
	sem_destroy(writeo);
	sem_destroy(welcome);
	close(sd);
}

void ChildSigHandler(int signale){
	pid_t pid;
	int status;
	while( (pid = waitpid(-1,&status,WNOHANG)) == -1){
		sleep(1);
	}
	printf("Child process killed; PID: %d\n", (int)pid );
}
void print_handler(int signale, siginfo_t *ignore, void *ignore2){
	if ( signale == SIGALRM )
	{
		sem_post( &actionCycleSemaphore );
	}
}

void periodic_printing(){
	int i;
	//Print account info every 20 seconds. Raise a signal in Server main function.
	for(i = 0;i < 20; i++){
		printf("Account name: %s \n", p[i].name);
		printf("Balance: %f \n", p[i].balance);
		if(p[i].session){
			printf("In session: Yes");
		}
		else{
			printf("In session: No");
		}
	}
}
void *
periodic_action_cycle_thread( void * ignore )
{
	struct sigaction	action;
	struct itimerval	interval;


	pthread_detach( pthread_self() );			// Don't wait for me, Argentina ...
	action.sa_flags = SA_SIGINFO | SA_RESTART;
	action.sa_sigaction = print_handler;
	sigemptyset( &action.sa_mask );
	sigaction( SIGALRM, &action, 0 );			// invoke periodic_action_handler() when timer expires
	interval.it_interval.tv_sec = 20;
	interval.it_interval.tv_usec = 0;
	interval.it_value.tv_sec = 20;
	interval.it_value.tv_usec = 0;
	setitimer( ITIMER_REAL, &interval, 0 );			// every 20 seconds
	for ( ;; )
	{
		sem_wait( &actionCycleSemaphore );		// Block until posted

		sem_wait(reado);
		sem_wait(welcome);
		readers++;
		if(readers == 1){
			sem_wait(writeo);
		}
		sem_post(welcome);
		sem_post(reado);
		
		periodic_printing();
		
		sem_wait(welcome);
		readers--;
		if(readers == 0){
			sem_post(writeo);
		}
		sem_post(welcome);
		sched_yield();					// necessary?
	}
	return 0;//We'll never get here... haha
}

void client_session(int sd){
	char *buffer;
	char *command;
	char *arguments;
	char request[2048];
	char* storage;
	char temp;
	int i;
	int curr,size;
	float ignore;
	float money;
	long senderIPaddr;
	int insesh;
	account_t *act;

	insesh  = 0;

	while(1){
		curr = 0;
		size = 0;
		while((size = recv(sd,request,sizeof(request),0)) > 0){
			curr += size;
			if(curr > 2048){
				storage = "Overflow input. Please enter another command.";
				write( sd, storage, strlen(storage) + 1 );
				continue;
			}
			strncpy(storage, request, size);

		}
		//Here, we have a line of input from client. Let's decipher it.
		sscanf(storage, "%sm %sm", command, arguments);
		/*Check validity for command, switch, then check argument validity. */
		if(strlen(command)>9){
			printf("Invalid command.");
			continue;
		}
		buffer = malloc(sizeof(char)*9);
		memcpy(buffer, command, sizeof(char)*9);
		buffer[8] = '\0';
		//Maybe consider converting to lowercase instead of telling them to type in lower.
		if(strcmp(buffer, "create") == 0){
			if(insesh == 1){
				//Send something like 'you're already being served.
				if(send(sd, "Active customer session: cannot create new account.", 51 , 0) == -1){
					perror("send");
				}
				//Let's hope I counted right
				//Must end session to creat account'
				continue;
			}
			sem_wait(reado);
			sem_wait(writeo);
			buffer = realloc(buffer,sizeof(char)*101);
			sscanf(storage, "%s", buffer);
			buffer[100]='\0';
			for(i = 0; i < 20; i++){
				if(p[i].name != NULL){//We need to init all SHM to 0
					//Make account...
					p[i] = create(&p[i],buffer);
					break;
				}
				else if(strcmp(p[i].name, buffer) == 0){
					//account already exists.
					//Handle ... somehow.
					//Send error, already exists
					break;
				}
			}
			sem_post(reado);
			sem_post(writeo);
			if(i == 20){
				//Send error, bank full
				if(send(sd, "Error, bank full.", 17,0)==-1){
					perror("send");
				}
			}


		}
		else if(strcmp(buffer, "serve") == 0){
			if(insesh == 1){
				//Send something like 'you're already being served.'
				if(send(sd,"Already serving an account. Please close before attempting to serve another.",76,0)== -1){
					perror("send");
				}
			continue;
			}
			insesh = 1;
			sem_wait(reado);
			sem_wait(writeo);
			buffer = realloc(buffer,sizeof(char)*101);
			sscanf(storage, "%s", buffer);
			buffer[100]='\0';
			for(i = 0; i < 20; i++){
				if(((p[i].name) != NULL) && (strcmp(p[i].name, buffer) == 0)){
					serve(act = &p[i]);
					break;//I hope this exits the loop
				}
			}
			if(i == 20){
				//Could not serve. Account not found. Return such?

			}
			sem_post(reado);
			sem_post(writeo);
		}
		else if(strcmp(buffer, "deposit") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			sem_wait(reado);
			sem_wait(welcome);
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(writeo);
			}
			sem_post(welcome);
			sem_post(reado);
			//Try scanfing a number. truncate to the size of a float,
			//error check if string was greater
			//call the deposit,

			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);


			//send new balance, or error if broken
		}
		else if(strcmp(buffer, "withdraw") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			sem_wait(reado);
			sem_wait(welcome);
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(writeo);
			}
			sem_post(welcome);
			sem_post(reado);
			//Try scanfing the next entry as a float,
			//call the withdraw,

			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);

			//send new balance, or error if broken

		}
		else if(strcmp(buffer, "query") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			sem_wait(reado);
			sem_wait(welcome);
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(writeo);
			}
			sem_post(welcome);
			sem_post(reado);
			//Send account balance

			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);
		}
		else if(strcmp(buffer, "end") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			insesh = 0;


		}
		else if(strcmp(buffer, "quit") == 0){
			//Unlock the mutex in the act...
			return;
		}
		else{
			printf("Please enter a valid command, in all lowercase. ");
		}


	}

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

}

void sharingcaring(){
	/* Shared Memory Section*/

	int shmid;
	int i;
	account_t* temp ;
	key_t key;

	int size;
	size = 20 * sizeof(account_t); // 20 accounts
	if(errno = 0, (key = ftok("testplan.txt",42)) == -1){
		printf("ftok failed; errno :  %s\n", strerror( errno ));
		exit( 1 );
	}
	else if(errno = 0, (shmid = shmget(key,size,0666 | IPC_CREAT | IPC_EXCL)) == -1){
		printf("shmget failed; errno :  %s\n", strerror( errno ));
		exit( 1 );
	}
	else if(errno = 0, (p = (account_t*) shmat(shmid,NULL,0)) == (void*) -1) {
		//No. p is an array of pointers to account_t's.
		//We need its size to be right. And we cast it as an account_t*,
		//so p[0] is the first account_t, p[1] is the second account_t, etc.
		printf( "shmat() failed; errno :  %s\n", strerror( errno ) );
		exit( 1 );
	}
	for(i = 0 ; i < 20 ; i++){
		if((temp = init()) == NULL){
			printf("Init failed");
			exit(1);
		}
				p[i] = *(init());

	}


	//shared mem sucess.  Begin Server/Client Comunnications.
}

int main(){

	int sd;
	char *func = "server main";
	pthread_t		tid;
	struct sigaction memclean;
	if((reado = sem_open("reado",O_CREAT,0640,20)) == SEM_FAILED){
			printf("Read semaphore init fail.");
			exit(1);
		}
		else if((writeo = sem_open("writeo",O_CREAT,0640,1)) == SEM_FAILED){
			printf("Write semaphore init fail.");
			exit(1);
		}
		else if((welcome = sem_open("welcome",O_CREAT,0640,1)) == SEM_FAILED){
			printf("Welcome semaphore init fail.");
			exit(1);
		}
	readers = 0;
	//Semaphores at ready. No one reading.

	memclean.sa_handler = organized_cleaning;
	sigemptyset (&memclean.sa_mask);
	sigaddset (&memclean.sa_mask, SIGINT);
	memclean.sa_flags = 0;
	sigaction(SIGINT, &memclean, NULL);
	sigprocmask(SIG_BLOCK, &memclean.sa_mask, NULL);

	sigprocmask(SIG_UNBLOCK, &memclean.sa_mask, NULL);
	//Note, there are forks in the server, but no threads... The forked processes are 2threaded. 
	Bankinit();
	//Shared Memory Setup
	sharingcaring();
	//Shared Memory Init

	
	//account data[20] = p;//Not sure if this is ok?
	
	



	if ( pthread_attr_init( &kernel_attr ) != 0 )
	{
		printf( "pthread_attr_init() failed in %s()\n", func );
		return 0;
	}
	else if ( pthread_attr_setscope( &kernel_attr, PTHREAD_SCOPE_SYSTEM ) != 0 )
	{
		printf( "pthread_attr_setscope() failed in %s() line %d\n", func, __LINE__ );
		return 0;
	}
	else if ( pthread_create( &tid, &kernel_attr, periodic_action_cycle_thread, 0 ) != 0 )
	{
		printf( "pthread_create() failed in %s()\n", func );
		return 0;
	}

	//Server-Client Service
	socks("54261");


	return 0;
}
