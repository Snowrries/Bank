/*
 * Bank.h
 *
 *  Created on: Apr 11, 2015
 *      Author: Anthony
 */

#ifndef BANK_H_
#define BANK_H_

#include <pthread.h>
#include <errno.h>

	typedef struct account{
		char* name;
		float balance;
		int session;
		pthread_mutex_t lock;
	} account_t;


struct account *create(char* name);

int serve(struct account *acc);

float withdraw(struct account *acc,float amt);

float deposit(struct account *acc,float amt);

float query(struct account *acc);



#endif /* BANK_H_ */
