#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "server.h"

int socks()){
	int sd;
	struct sockaddr_in add;
	
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Socketing failed. errorno: %d", errorno);
		return -1;
	}
	if((bind(sd, (struct sockaddr *)&add, sizeof(struct sockaddr))) == -1){
		printf("Bind failed. errorno: %d", errorno);
		return -2;
	}
	return sd;
}