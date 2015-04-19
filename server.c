#define _GNU_SOURCE
#include "server.h"

int readers = 0;

account_t* p;
pid_t parent_pid;
int shmid;
int sd;
sig_atomic_t deathflag = 1;
sem_t actionCycleSemaphore;
//static pthread_attr_t	user_attr;
//static pthread_attr_t	kernel_attr;



void organized_cleaning(int signale){

	/*Note: shmid is a global var. Further, sigint is blocked while shared memory is being created, 
	so shmid is guaranteed to be a pointer to a valid block of shared memory*/

	deathflag = 0;
	munmap(p,sizeof(account_t)*20);
	close(sd);

}

void child_cleaning(int signale, siginfo_t *ignore, void *ignore2){

	_exit(0);
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
	char message[256];
	//Print account info every 20 seconds. Raise a signal in Server main function.
	for(i = 0;i < 20; i++){
		if((p[i].name)[0] == '\0'){
			continue;
		}
		write( 1, message, sprintf( message, "\x1b[2;33mAccount name: %s ...\x1b[0m\n", p[i].name ) );
		write( 1, message, sprintf( message, "\x1b[2;33mBalance: %f ...\x1b[0m\n", p[i].balance ) );
//		printf("Account name: %s \n", p[i].name);
//		printf("Balance: %f \n", p[i].balance);
		if(p[i].session){
			write( 1, message, sprintf( message, "\x1b[2;33m In session: Yes  \x1b[0m\n" ) );
			//printf("In session: Yes\n");
		}
		else{
			write( 1, message, sprintf( message, "\x1b[2;33m In session: No  \x1b[0m\n" ) );
			//printf("In session: No\n");
		}
	}
}
void *
periodic_action_cycle_thread( void * ignore )
{
	struct sigaction	action;
	struct itimerval	interval;
	struct sigaction	exit;
	sem_t *reado;
	sem_t *writeo;
	sem_t *welcome;

	reado = sem_open("reado",O_CREAT,0644,0);
	writeo= sem_open("writeo",O_CREAT,0644,0);
	welcome = sem_open("welcome",O_CREAT,0644,0);


	pthread_detach( pthread_self() );			// Don't wait for me, Argentina ...
	action.sa_flags = SA_SIGINFO | SA_RESTART;
	action.sa_sigaction = print_handler;
	sigemptyset( &action.sa_mask );
	sigaction( SIGALRM, &action, 0 );			// invoke periodic_action_handler() when timer expires


	exit.sa_flags = 0;
	exit.sa_sigaction = child_cleaning;
	sigemptyset (&exit.sa_mask);
	sigaddset (&exit.sa_mask, SIGQUIT);
	sigaction(SIGQUIT, &exit, NULL);

	interval.it_interval.tv_sec = 20;
	interval.it_interval.tv_usec = 0;
	interval.it_value.tv_sec = 20;
	interval.it_value.tv_usec = 0;
	setitimer( ITIMER_REAL, &interval, 0 );			// every 20 seconds
	for ( ;; )
	{
		sem_wait( &actionCycleSemaphore );		// Block until posted
		printf("yo\n");
		sem_wait(reado);
		printf("yo1\n");
		sem_wait(welcome);
		printf("yo3\n");
		readers++;
		printf("%d\n",readers);
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
	char command[9];
	char account[101];
	char request[256];
	char line[1024];
	char *temp;
	int i;
	int curr,size;
	int insesh;
	float munni;
	account_t *act;
	struct sigaction end;
	struct sigaction clientend;
	insesh  = 0;

	sem_t *reado;
	sem_t *writeo;
	sem_t *welcome;

	 reado = sem_open("reado",O_CREAT,0644,0);
	writeo= sem_open("writeo",O_CREAT,0644,0);
	welcome = sem_open("welcome",O_CREAT,0644,0);
	end.sa_flags = 0;
	end.sa_sigaction = child_cleaning;
	sigemptyset (&end.sa_mask);
	sigaddset (&end.sa_mask, SIGQUIT);
	sigaction(SIGQUIT, &end, NULL);

	clientend.sa_flags = 0;
	clientend.sa_sigaction = child_cleaning;
	sigemptyset (&clientend.sa_mask);
	sigaddset (&clientend.sa_mask, SIGPIPE);
	sigaction(SIGPIPE, &clientend, NULL);



	while(deathflag == 1){
		curr = 0;
		size = 0;
		line[0] = '\0';
		if(recv(sd,line, sizeof(line),0)<1){
			perror("recv failed");
			exit(0);
		}

		printf("%s\n",line);
			//send new balance, or error if broken
		if(sscanf(line,"%7s %101s", command, account) == 2){
			if(strcmp(command, "deposit") == 0 || strcmp(command, "withdraw") == 0){
				if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				if(send(sd, "You must be in a session to withdraw or deposit.", 48 , 0) == -1){
					perror("send");
				}	
				continue;
			}	
			temp = "strtod error";
			munni = strtod(account, &temp);
			printf("word %s, %f \n", command, munni);
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
			
			if(strcmp(command, "deposit") == 0){
				deposit(act, munni);
				if(send(sd, "Deposited funds.", 16 , 0) == -1){
					perror("send");
				}	
			}
			
			//Withdraw
			else if(strcmp(command, "withdraw") == 0){
				withdraw(act, munni);
				if(send(sd, "Withdrew funds. Thank you for your generous donation. ;)", 56 , 0) == -1){
					perror("send");
				}	
			}
			
			else{
				if(send(sd, "Unspecified error...", 20 , 0) == -1){
					perror("send");
				}	
			}
			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);
		}
		
		else if((strcmp(command, "create") == 0)||(strcmp(command, "serve") == 0)){
				
			printf("servecreate %s, %s\n", command, account);
			if(insesh == 1){
				//Send something like 'you're already being served.
				if(send(sd, "Active customer session: cannot create or serve new account.", 60 , 0) == -1){
					perror("send");
				}
				//Let's hope I counted right
				//Must end session to creat account'
				continue;
			}
			printf("reado sem\n");
			sem_wait(reado);
			printf("writeo sem\n");
			sem_wait(writeo);
			if(strcmp(command, "create") == 0){
				for(i = 0; i < 20; i++){
					if((p[i].name)[0] == '\0'){//We need to init all SHM to 0
						printf("Account Made: %s\n",account);
						create(&p[i],account);
						printf("%s\n", p[i].name);
						//sem_post(writeo);
						//sem_post(reado);
						break;
					}
					else if(strcmp(p[i].name, account) == 0){
						//account already exists.
						//Handle ... somehow.
						//Send error, already exists
						if(send(sd, "Account name already exists.", 28 , 0) == -1){
							perror("send");
						}
						//sem_post(writeo);
						//sem_post(reado);
						break;
					}
				}
			}

			else if(strcmp(command, "serve") == 0){
				for(i = 0; i < 20; i++){
					if(((p[i].name)[0] != '\0') && (strcmp(p[i].name, account) == 0)){
						serve(act = &p[i]);
						insesh = 1;
						break;//I hope this exits the loop
					}
				}
				if(i == 20){
					//Could not serve. Account not found. Return such?
					if(send(sd, "Could not find account.", 23 , 0) == -1){
						perror("send");
					}
				}
			}
			else{
				if(send(sd, "Unspecified error...", 20 , 0) == -1){
					perror("send");
				}	
			}
			sem_post(writeo);
			sem_post(reado);
			if(i == 20){
				//Send error, bank full
				if(send(sd, "Error, bank full.", 17,0) == -1){
					perror("send");
				}
			}
		}
	}

		//query, end, quit
		
		else if(sscanf(request,"%6s\n", command) == 1){
			printf("queryendquit %s\n", command);
			if(insesh == 0){
				if(strcmp(command,"query")==0){
				//Send something like 'you must be in a session to use this operation.'
					if(send(sd,"You must be in a session to use this operation.", 47, 0)==0){
						perror("send");
					}
				}
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
			
			if(strcmp(command, "query") == 0){
				
				sprintf(request,"%f",query(act));
				if(send(sd, request, strlen(request), 0) == -1){
					perror("send");
				}
			}
			//end
			else if(strcmp(command, "end") == 0){
				//Consider error checking this
				insesh = 0;
				act->session = 0;
				pthread_mutex_unlock(&(act->lock));
				if(send(sd,"Client session ended. You may now create another account, or be served.", 71, 0) == -1){
					perror("send");
				}
				
			}
			//quit
			else if(strcmp(command, "quit") == 0){
				if(insesh == 1){
					pthread_mutex_unlock(&(act->lock));
					if(send(sd,"Client session ended. Goodbye.", 30 , 0) == -1){
						perror("send");
					}
				}
				else{
					if(send(sd,"Goodbye.", 8 , 0) == -1){
						perror("send");
					}
				}
				sem_wait(welcome);
				readers--;
				if(readers == 0){//If last reader, unlock write.
					sem_post(writeo);
				}
				sem_post(welcome);
				break;
			}
			else{
				if(send(sd, "Unspecified error...", 20 , 0) == -1){
					perror("send");
				}	
			}
			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);
		}

		else{
			printf("Please enter a valid command, in all lowercase. ");
			break;
		}
	}

	sem_close(reado);
	sem_close(writeo);
	sem_close(welcome);
	close(sd);
	printf("Exiting Child\n");
}

int socks(const char* port){
	int sd;
//	int n;
	int sporkd;
	int pid;
//	struct sockaddr_in addr;
	struct addrinfo	addrinfo;
	struct addrinfo *result;
	socklen_t addrlen;
	struct sockaddr_in them;
//	char message[256];
	int on = 1;

	struct sigaction action;

	action.sa_handler = ChildSigHandler;
	action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
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
		perror("getaddrinfo");
		//fprintf( stderr, "\x1b[1;31mgetaddrinfo( %s ) failed errno is %s.  File %s request %d.\x1b[0m\n", CLIENT_PORT, strerror( errno ), __FILE__, __LINE__ );
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
			addrlen = sizeof(struct sockaddr_in);


			pid = fork();
			if(pid < 0)
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
				close(sporkd);
				printf("Created child process %d\n", pid);

			}
		}
	}
	return 0;

}

void sharingcaring(){
	/* Shared Memory Section*/

//	int shmid;
	int i;
	account_t* temp ;
//	key_t key;

	int size;
	size = 20 * sizeof(account_t); // 20 accounts
	p = (account_t*) mmap(NULL,sizeof(account_t) *20, -1 , MAP_SHARED | MAP_ANONYMOUS,0,0);
	/*
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
	*/
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

	pid_t pid;
	sem_t *reado;
	sem_t *writeo;
	sem_t *welcome;
	struct sigaction memclean;
	if((reado = sem_open("reado",O_CREAT,0644,0)) == SEM_FAILED){
			printf("Read semaphore init fail.");
			exit(1);
		}
		else if((writeo = sem_open("writeo",O_CREAT,0644,0)) == SEM_FAILED){
			printf("Write semaphore init fail.");
			exit(1);
		}
		else if((welcome = sem_open("welcome",O_CREAT,0644,0)) == SEM_FAILED){
			printf("Welcome semaphore init fail.");
			exit(1);
		}
	sem_post(reado);
	sem_post(writeo);
	sem_post(welcome);
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
	
	

	parent_pid = getpid();

	pid = fork();
	if(pid < 0){
		printf("Fork Failed");

	}
	else if(pid == 0){
		signal(SIGQUIT,SIG_DFL);
		periodic_action_cycle_thread(NULL);
		exit(0);
	}

	//Server-Client Service
	socks("54965");

	sem_close(reado);
	sem_close(writeo);
	sem_close(welcome);

	return 0;
	}

