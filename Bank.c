/*
 * Bank.c
 *
 *  Created on: Apr 11, 2015
 *      Author: Anthony
 */

#include "Bank.h"



int serve(struct account *acc){
	int error;
	if((error = pthread_mutex_trylock(acc->lock)) == EBUSY){
		printf("Account is already in use");
		return -1;
	}
	if(error != 0){
		printf("Error at Line %s Account could not be accessed \n", __LINE__);

		return -2;
	}
	pthread_mutex_lock(acc->lock);
	acc->session = 1;

	return 1;
}

struct account *create(char* name){
	struct account *new =(struct account *) malloc(sizeof(struct account));
	new->name = name;
	new->balance = 0;
	new->session = 0;
	return new;
}

float withdraw(struct account *acc,float amt){
	if(amt < 0){
		return acc->balance;
	}
	else if(amt > (acc->balance)){
		return acc->balance;
	}

	return acc->balance -=amt;
}

float deposit(struct account *acc,float amt){
	if(amt < 0){
		return acc->balance;
	}

	return acc->balance +=amt;

}

float query(struct account *acc){
	return acc->balance;

}
