#ifndef CLIENT_H
#define CLIENT_H

/*
• Account name (a string up to 100 characters long)
• Current balance (a floating-point number)
• In-session flag (a boolean flag indicating whether or not the account is currently
being serviced)
*/



void EndClient(int sig, siginfo_t *ignore, void *ignore2);

int reliablemail(int sd, char* buf, int len);

int connect_to_server( const char * server, const char * port );

int serverscout(void *arg);

#endif //client.h
