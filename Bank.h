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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



	typedef struct account{
		char* name;
		float balance;
		int session;
//		int created;
		pthread_mutex_t lock;
	} account_t;


void printAccounts(account_t *acc);

void Bankinit();

struct account create(account_t *acc,char* name);

struct account *init();

int serve(account_t *acc);

float withdraw(account_t *acc,float amt);

float deposit(account_t *acc,float amt);

float query(account_t *acc);



#endif /* BANK_H_ */
