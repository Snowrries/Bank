#include "session.h"
static int readers;

void client_session(int sd){
	char *buffer;
	char *command;
	char *arguments;
	char request[2048];
	char* storage;
	char temp;
	int i;
	int curr,size;
	float ignore;
	float money;
	long senderIPaddr;
	int insesh;
	account_t *act;

	insesh  = 0;

	while(1){
		curr = 0;
		size = 0;
		while((size = recv(sd,request,sizeof(request),0)) > 0){
			curr += size;
			if(curr > 2048){
				storage = "Overflow input. Please enter another command.";
				write( sd, storage, strlen(storage) + 1 );
				continue;
			}
			strncpy(storage, request, size);

		}
		//Here, we have a line of input from client. Let's decipher it.
		sscanf(storage, "%sm %sm", &command, &arguments);
		/*Check validity for command, switch, then check argument validity. */
		if(strlen(command)>9){
			printf("Invalid command.");
			continue;
		}
		buffer = malloc(sizeof(char)*9);
		memcpy(buffer, command, sizeof(char)*9);
		buffer[8] = '\0';
		//Maybe consider converting to lowercase instead of telling them to type in lower.
		if(strcmp(buffer, "create") == 0){
			if(insesh == 1){
				//Send something like 'you're already being served.
				if(send(sd, "Active customer session: cannot create new account.", 51 , 0) == -1){
					perror("send");
				}
				//Let's hope I counted right
				//Must end session to creat account'
				continue;
			}
			sem_wait(&reado);
			sem_wait(&writeo);
			buffer = realloc(buffer,sizeof(char)*101);
			sscanf(storage, "%s", &buffer);
			buffer[100]='\0';
			for(i = 0; i < 20; i++){
				if(p[i].name != NULL){//We need to init all SHM to 0
					//Make account...
					p[i] = create(&p[i],buffer);
					break;
				}
				else if(strcmp(p[i].name, buffer) == 0){
					//account already exists.
					//Handle ... somehow.
					//Send error, already exists
					break;
				}
			}
			sem_post(reado);
			sem_post(writeo);
			if(i == 20){
				//Send error, bank full
				if(send(sd, "Error, bank full.", 17,0)==-1){
					perror("send");
				}
			}


		}
		else if(strcmp(buffer, "serve") == 0){
			if(insesh == 1){
				//Send something like 'you're already being served.'
				if(send(sd,"Already serving an account. Please close before attempting to serve another.",76,0)== -1){
					perror("send");
				}
			continue;
			}
			insesh = 1;
			sem_wait(reado);
			sem_wait(writeo);
			buffer = realloc(buffer,sizeof(char)*101);
			sscanf(storage, "%s", &buffer);
			buffer[100]='\0';
			for(i = 0; i < 20; i++){
				if(((p[i].name) != NULL) && (strcmp(p[i].name, buffer) == 0)){
					serve(act = *p[i]);
					break;//I hope this exits the loop
				}
			}
			if(i == 20){
				//Could not serve. Account not found. Return such?
				
			}
			sem_post(reado);
			sem_post(writeo);
		}
		else if(strcmp(buffer, "deposit") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			sem_wait(reado);
			sem_wait(welcome);
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(writeo);
			}
			sem_post(welcome);
			sem_post(reado);
			//Try scanfing a number. truncate to the size of a float,
			//error check if string was greater
			//call the deposit,

			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);


			//send new balance, or error if broken
		}
		else if(strcmp(buffer, "withdraw") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			sem_wait(read);
			sem_wait(welcome);
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(writeo);
			}
			sem_post(welcome);
			sem_post(read);
			//Try scanfing the next entry as a float,
			//call the withdraw,

			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(writeo);
			}
			sem_post(welcome);

			//send new balance, or error if broken

		}
		else if(strcmp(buffer, "query") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			sem_wait(reado);
			sem_wait(welcome);
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(writeo);
			}
			sem_post(welcome);
			sem_post(reado);
			//Send account balance

			sem_wait(welcome);
			readers--;
			if(readers == 0){//If last reader, unlock write.
				sem_post(&writeo);
			}
			sem_post(welcome);
		}
		else if(strcmp(buffer, "end") == 0){
			if(insesh == 0){
				//Send something like 'you must be in a session to use this operation.'
				continue;
			}
			insesh = 0;


		}
		else if(strcmp(buffer, "quit") == 0){
			//Unlock the mutex in the act...
			return;
		}
		else{
			printf("Please enter a valid command, in all lowercase. ");
		}


	}

}

