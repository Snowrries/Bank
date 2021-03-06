#ifndef CLIENT_H
#define CLIENT_H

/*
• Account name (a string up to 100 characters long)
• Current balance (a floating-point number)
• In-session flag (a boolean flag indicating whether or not the account is currently
being serviced)
*/

struct account{
	char* name;
	float balance;
	int session;
};

int ping();

#endif //client.h