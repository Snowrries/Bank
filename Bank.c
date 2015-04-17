
#include "Bank.h"



int serve(account_t *acc){
	int error;
	if((error = pthread_mutex_trylock(&(acc->lock))) == EBUSY){
		printf("Account is already in use");
		return -1;
	}
	if(error != 0){
		printf("Error at Line %s Account could not be accessed \n", __LINE__);

		return -2;
	}
	pthread_mutex_lock(&(acc->lock));
	acc->session = 1;

	return 1;
}

struct account create(account_t *acc,char* name){
	int error;
		if((error = pthread_mutex_trylock(&newAccount)) == EBUSY){
			printf("Account is already in use");
			return *acc;
		}
		if(error != 0){
			printf("Error at Line %s Account could not be accessed \n", __LINE__);

			return *acc;
		}
	pthread_mutex_lock(&newAccount);
	acc->name = name;
	acc->balance = 0;
	acc->session = 0;
	pthread_mutex_unlock(&newAccount);
	return *acc;
}

struct account *init(){
	int error;
			if((error = pthread_mutex_trylock(&newAccount)) == EBUSY){
				printf("Account is already in use");
				return NULL;
			}
			if(error != 0){
				printf("Error at Line %s Account could not be accessed \n", __LINE__);

				return NULL;
			}
		pthread_mutex_lock(&newAccount);
		struct account *new =(struct account *) malloc(sizeof(struct account));
		new->name = NULL;
		pthread_mutex_unlock(&newAccount);
		return new;
}

float withdraw(account_t *acc,float amt){
	if(amt < 0){
		return acc->balance;
	}
	else if(amt > (acc->balance)){
		return acc->balance;
	}

	return acc->balance -=amt;
}

float deposit(account_t *acc,float amt){
	if(amt < 0){
		return acc->balance;
	}

	return acc->balance +=amt;

}

void printAccounts(account_t *acc){
	pthread_mutex_lock(&newAccount);
	int totalacc;
	int i;
	account_t *iter = acc;
	for(i = 0; i < 20 ; i ++){
		if(iter == NULL){
			continue;
		}
		printf("Account: %s \t",acc->name);
		printf("Balance: %f \t",acc->balance);
		if(acc->session == 1){
		printf("IN SERVICE \n");
		}
		totalacc++;
		iter ++;
	}
	printf("Total Accounts: %d",totalacc);
	pthread_mutex_unlock(&newAccount);

}

float query(account_t *acc){
	return acc->balance;

}
