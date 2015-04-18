/*
 * client.c
 *
 *  Created on: Apr 10, 2015
 *      Author: Anthony
 */

#include	<sys/types.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<errno.h>
#include	<string.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<ctype.h>


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
				sleep( 1 );
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
void serverscout(){
	
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
	int			sd;
	char			message[256];
	char			buffer[512];
	char			account[101];
	char			*command;
	int			len;
	float 			munni;
	int 			i;


	if ( argc < 2 )
	{
		fprintf( stderr, "\x1b[1;31mNo host name specified.  File %s line %d.\x1b[0m\n", __FILE__, __LINE__ );
		exit( 1 );
	}
	else if( argc > 2){
		printf("Too many arguments. Please input just a host name.");
		exit(1);
	}
	else if ( (sd = connect_to_server( argv[1], "54261" )) == -1 )
	{
		write( 1, message, sprintf( message,  "\x1b[1;31mCould not connect to server %s errno %s\x1b[0m\n", argv[1], strerror( errno ) ) );
		return 1;
	}
	else
	{
		printf( "Connected to server %s\n", argv[1] );
		printf("Available commands:\n create accountname\n serve accountname\n");
		printf("deposit amount\n withdraw amount\n query\n end\n quit\n");
		printf("Account names may only be up to 100 characters long. We'll truncate for you if it's too long. ");
		printf("Please do not withdraw or deposit negative numbers. We do not deal in anti-currency.");
		while ( 1 )
		{
			printf("Enter command:\t");
			//Ordered in this fashion because there are expected to be more withdraws and deposits
			//Than send and receives... Just a guess though. Shouldn't affect efficiency terribly.
			if(scanf("%sm %d",command, &munni)==2){
				//withdraws and deposits
				len = strlen(command);
				if(len > 8){
					printf("Invalid command.");
					continue;
				}
				for(i = 0; i < len; i++){
					command[i] = (char)tolower(command[i]);
				}
				if((strcmp(command, "withdraw") == 0) || (strcmp(command, "deposit") == 0)){
					if(sprintf(buffer, "%s %f", command, &munni) < 0){
						printf("Invalid input...");
						continue;
					}
					if(reliablemail(sd, buffer, strlen(buffer)) == -1){
						printf("Failed to send to server. Try again?");
					};//Hopefully munni is a valid float.
				}
				else{
					printf("Invalid command.");
					continue;
				}
			}
			
			else if(scanf("%sm %100s", command, account) == 2){
				//Create, serve
				len = strlen(command);
				if(len > 6){
					printf("Invalid command.");
					continue;
				}
				for(i = 0; i < len; i++){
					command[i] = (char)tolower(command[i]);
				}
				if((strcmp(command, "create") == 0) || (strcmp(command, "serve") == 0)){
					if(sprintf(buffer, "%s %s", command, account) < 0){
						printf("Invalid input...");
						continue;
					}
					if(reliablemail(sd, buffer, strlen(buffer)) == -1){
						printf("Failed to send to server. Try again?");
					}//You can trust on this mail gettin there!
					//Just maybe not in one piece...
				}
				else{
					printf("Invalid command.");
					continue;
				}
			}
			
			else if(scanf("%sm", command) == 1){
				//query, end, quit
				len = strlen(command);
				if(len > 5){
					printf("Invalid command.");
					continue;
				}
				for(i = 0; i < len; i++){
					command[i] = (char)tolower(command[i]);
				}
				if((strcmp(command, "query") == 0) || (strcmp(command, "end") == 0)|| (strcmp(command, "quit") == 0)){
					if(reliablemail(sd, command, len) == -1){
						printf("Failed to send to server. Try again?");
					}//Hopefully munni is a valid float.
				}
				else{
					printf("Invalid command.");
					continue;
				}
			}
			
			else{
				printf("Invalid input. Please check that there are no kittens prancing on your keyboard before proceeding.");
				continue;
			}
			free(command);
			sleep(3);
		}
		close( sd );
		return 0;
	}
}
