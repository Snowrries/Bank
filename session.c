#import 'session.h'
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
	account *act;
	int readers;
	sem_t read;
	sem_t write;
	sem_t welcome;

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

		buffer = malloc(sizeof(char)*9);
		memcpy(buffer, command, sizeof(char)*9);
		buffer[8] = '\0';
		//Maybe consider converting to lowercase instead of telling them to type in lower.
		if(strcmp(buffer, "create") == 0){
			sem_wait(read);//This, right now, will cause deadlock.
			sem_wait(welcome);
			sem_wait(write);
			buffer = realloc(sizeof(char)*101);
			sscanf(storage, "%s", &buffer);
			buffer[100]='\0';
			for(i = 0; i < 20; i++){
				if(p[i] == NULL){
					//Make account...
					p[i] = create(buffer);
				}
				else if(strcmp(p[i]->name, buffer) == 0){
					//account already exists. 
					//Handle ... somehow.
					break;
				}
			}
			
			sem_post(read);
			sem_post(welcome);	
			sem_post(write);
		}
		else if(strcmp(buffer, "serve") == 0){
			sem_wait(read);//This, right now, will cause deadlock.
			sem_wait(welcome);
			sem_wait(write);
			buffer = realloc(sizeof(char)*101);
			sscanf(storage, "%s", &buffer);
			buffer[100]='\0';
			for(i = 0; i < 20; i++){
				if((p[i] != NULL) && (strcmp(p[i]->name, buffer) == 0)){
					serve(buffer);
					break;//I hope this exits the loop
				}
			}
			if(i == 20){
				//Could not serve. Account not found. Return such?
			}
			sem_post(read);
			sem_post(welcome);	
			sem_post(write);
		}
		else if(strcmp(buffer, "deposit") == 0){
			
		}
		else if(strcmp(buffer, "withdraw") == 0){
			
		}
		else if(strcmp(buffer, "query") == 0){
			readers++;
			if(readers == 1){//If first reader, lock write.
				sem_wait(write);
			}
		}
		else if(strcmp(buffer, "end") == 0){
			
		}
		else if(strcmp(buffer, "quit") == 0){
			
		}
		else{
			printf("Please enter a valid command, in all lowercase. ");
		}
		
		
	}

}
