#ifndef server_h
#define server_h
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include "session.h"
#include "Bank.h"

#define CLIENT_PORT 54261


	void organized_cleaning(int signale);
	
	void ChildSigHandler(int signale);
	
	void client_session(int sd);
	
	void *periodic_action_cycle_thread( void * ignore );

	void periodic_printing();

	void print_handler(int signale, siginfo_t *ignore, void *ignore2);

	int socks();

	void sharingcaring();
	/*??
	From BKR's server.c
	static void set_iaddr(struct sockaddr_in * sockaddr, long x, unsigned int port);
	*/
#endif //Server.h
