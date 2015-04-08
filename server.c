
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
	pid_t pid;
	struct sockaddr_in add;
	
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
	pid = fork();
	if(pid == 0){
		signal(/*SIGSOMETHING*/,ChildSigHandler);
	}


	return sd;
}
