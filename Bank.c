
#include "Bank.h"

pthread_mutex_t newAccount;

int serve(account_t *acc){
	int error;
	if((error = pthread_mutex_trylock(&(acc->lock))) == EBUSY){
		printf("Account is already in use");
		return -1;
	}
	if(error != 0){
		printf("Error at Line %d Account could not be accessed \n", __LINE__);

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
			printf("Error at Line %d Account could not be accessed \n", __LINE__);

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

		if((error = pthread_mutex_lock(&newAccount)) == EBUSY){
			printf("Account is already in use");
			exit(1);
		}
		else if(error != 0){
			printf("Error at Line %d Account could not be accessed \n", __LINE__);
			exit(1);
		}
		
		struct account *new =(struct account *) malloc(sizeof(struct account));
		new->name = NULL;
		new->session = 0;
		new->balance = 0;
		if(pthread_mutex_init(&(new->lock),NULL) != 0){
					printf("Mutex init failed");
					return NULL;
				}
		pthread_mutex_unlock(&newAccount);
		return new;
}

void Bankinit(){

	if(pthread_mutex_init(&newAccount,NULL)!=0){
			printf("Mutex Init Failed");
			exit(1);
		}

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
