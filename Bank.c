/*
 * Bank.c
 *
 *  Created on: Apr 11, 2015
 *      Author: Anthony
 */

#include "Bank.h"

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
