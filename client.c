/*
 * client.c
 */

#include "client.h"


sig_atomic_t clientend = 1;

static pthread_attr_t	kernel_attr;

void EndClient(int sig, siginfo_t *ignore, void *ignore2){
	clientend = 0;
}

int
connect_to_server( const char * server, const char * port )
{
	int			sd;
	struct addrinfo		addrinfo;
	struct addrinfo *	result;
	char			message[256];


	addrinfo.ai_flags = 0;
	addrinfo.ai_family = AF_INET;		// IPv4 only
	addrinfo.ai_socktype = SOCK_STREAM;	// Want TCP/IP
	addrinfo.ai_protocol = 0;		// Any protocol
	addrinfo.ai_addrlen = 0;
	addrinfo.ai_addr = NULL;
	addrinfo.ai_canonname = NULL;
	addrinfo.ai_next = NULL;


	if ( getaddrinfo( server, port, &addrinfo, &result ) != 0 )
	{
		fprintf( stderr, "\x1b[1;31mgetaddrinfo( %s ) failed.  File %s line %d.\x1b[0m\n", server, __FILE__, __LINE__ );
		return -1;
	}
	else if ( errno = 0, (sd = socket( result->ai_family, result->ai_socktype, result->ai_protocol )) == -1 )
	{
		freeaddrinfo( result );
		return -1;
	}
	else
	{
		do {
			if ( errno = 0, connect( sd, result->ai_addr, result->ai_addrlen ) == -1 )
			{
				sleep( 3 );
				write( 1, message, sprintf( message, "\x1b[2;33mConnecting to server %s ...\x1b[0m\n", server ) );
			}
			else
			{
				freeaddrinfo( result );
				return sd;		// connect() succeeded
			}
		} while ( errno == ECONNREFUSED );
		freeaddrinfo( result );
		return -1;
	}
}
void *serverscout(void *sdx){
	char buffer[124];
	int status;
	int sd = *((int *)(sdx));
	while(1){
		if((status = recv(sd, buffer, 100, 0)) > 0){
			//print buffer in some thread safe manner. 
			printf("%s\n", buffer);
		}

		else if(status == -1 || status == 0){
			printf("Lost connection.\n");
			exit(1);
		}
		else{
			perror("recv");
		}printf("Bytes received: %d", status);
	}
}
int reliablemail(int sd, char *buffer, int len){
	int	bsent;
	int	bleft;
	int	nom;
	bsent = 0;
	bleft = len;
	while(bleft > 0){
		if((nom = send(sd, buffer+bsent, bleft, 0)) == -1){
			perror("send");
			return -1;
		}
		bsent += nom;
		bleft -= nom;
	}
	return 0;
}


int
main( int argc, char ** argv )
{
	pthread_t		tid;
	int			sd;
	char			message[256];
	char			buffer[512];
	char			account[102];
	char			command[10];
	int			len;
	float 			munni;
	int 			i;
	char			line[1024];
	struct sigaction end;

	char *func = "client main";


	if ( argc < 2 )
	{
		fprintf( stderr, "\x1b[1;31mNo host name specified.  File %s line %d.\x1b[0m\n", __FILE__, __LINE__ );
		exit( 1 );
	}
	else if( argc > 2){
		printf("Too many arguments. Please input just a host name.\n");
		exit(1);
	}
	else if ( (sd = connect_to_server( argv[1], "54965" )) == -1 )
	{
		write( 1, message, sprintf( message,  "\x1b[1;31mCould not connect to server %s errno %s\x1b[0m\n", argv[1], strerror( errno ) ) );
		return 1;
	}

	else if ( pthread_attr_init( &kernel_attr ) != 0 )
	{
		printf( "pthread_attr_init() failed in %s()\n", func );
		return 0;
	}
	else if ( pthread_attr_setscope( &kernel_attr, PTHREAD_SCOPE_SYSTEM ) != 0 )
	{
		printf( "pthread_attr_setscope() failed in %s() line %d\n", func, __LINE__ );
		return 0;
	}
	else if ( pthread_create( &tid, &kernel_attr, &serverscout, (void *)(&sd) ) != 0 )
	{
		printf( "pthread_create() failed in %s()\n", func );
		return 0;
	}

	else
	{
		printf( "Connected to server %s\n", argv[1] );
		printf("Available commands:\n create accountname\n serve accountname\n");
		printf("deposit amount\n withdraw amount\n query\n end\n quit\n");
		printf("Account names may only be up to 100 characters long. We'll truncate for you if it's too long. \n");
		printf("Please do not withdraw or deposit negative numbers. We do not deal in anti-currency.\n");

		end.sa_flags = 0;
		end.sa_sigaction = EndClient;
		sigemptyset (&end.sa_mask);
		sigaddset (&end.sa_mask, SIGINT);
		sigaction(SIGINT, &end, NULL);

		while ( clientend )
		{
			command[0] = '\0';
			sleep(3);
			printf("Enter command:\t\n");
			//Ordered in this fashion because there are expected to be more withdraws and deposits
			//Than send and receives... Just a guess though. Shouldn't affect efficiency terribly.

			fgets(line, sizeof(line), stdin);

			if(sscanf(line,"%9s %f\n",command, &munni)==2){
				//withdraws and deposits

				len = strlen(command);

				if(len > 8){
					printf("Invalid command.\n");
					continue;
				}
				for(i = 0; i < len; i++){
					command[i] = (char)tolower(command[i]);
				}
				if((strcmp(command, "withdraw") == 0) || (strcmp(command, "deposit") == 0)){
					if(sprintf(buffer, "%s %g", command, munni) < 0){
						printf("Invalid input...\n");
						continue;
					}
					if(reliablemail(sd, buffer, strlen(buffer)+1) == -1){
						printf("Failed to send to server. Try again?\n");
					};//Hopefully munni is a valid float.
				}
				else{
					printf("Invalid command.\n");
					continue;
				}
			}

			else if(sscanf(line,"%7s %101s\n", command, account) == 2){
				//Create, serve
				if(strlen(account) == 101){
					printf("Account name too long.\n");
					continue;
				}
				len = strlen(command);
				if(len > 6){
					printf("Invalid command.\n");
					continue;
				}
				for(i = 0; i < len; i++){
					command[i] = (char)tolower(command[i]);
				}
				if((strcmp(command, "create") == 0) || (strcmp(command, "serve") == 0)){
					if(sprintf(buffer, "%s %s", command, account) < 0){
						printf("Invalid input...\n");
						continue;
					}
					if(reliablemail(sd, buffer, strlen(buffer)+1) == -1){
						printf("Failed to send to server. Try again?\n");
					}//You can trust on this mail gettin there!
					//Just maybe not in one piece...
				}
				else{
					printf("Invalid command.\n");
					continue;
				}
			}

			else if(sscanf(line,"%6s\n", command) == 1){
				//query, end, quit
				len = strlen(command);
				if(len > 5){
					printf("Invalid command.\n");
					continue;
				}
				for(i = 0; i < len; i++){
					command[i] = (char)tolower(command[i]);
				}
				if((strcmp(command, "query") == 0) || (strcmp(command, "end") == 0)|| (strcmp(command, "quit") == 0)){
					if(reliablemail(sd, command, len+1) == -1){
						printf("Failed to send to server. Try again?\n");
					}//Hopefully munni is a valid float.
				}
				else{
					printf("Invalid command.\n");
					continue;
				}
			}

			else{
				printf("Invalid input. Please check that there are no kittens prancing on your keyboard before proceeding.\n");
				continue;
			}

		}
		strcpy(command ,"quit");
		reliablemail(sd,command,5);
		close( sd );
		return 0;
	}
}
