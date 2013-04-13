#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include "../includes/message.h"
#include <time.h>

Message* prepareMessage();
Message* fillMessageData(Message* msg, char* method, char* resource, char* body);
void dispatchEvent(Message* msg);
void listenForConnection();
char* createPipe();
void sendData(char* resource, char* method, char* body);
void setLogin(char* name);
void writeEmail();
void grabNewEmails();
void readConsole();

int status = 0;
int receiver;
int sender;
char username[7];
fd_set active_fd_set;

Message* prepareMessage(){
	Message* info = (Message*) malloc(sizeof(Message)); 
	return info;
}

Message* fillMessageData(Message* msg, char* method, char* resource, char* body){
	strcpy(msg->protocol, "TPSO 0.12");
	strcpy(msg->method, method);
	strcpy(msg->resource, resource);
	time(&(msg->time));
	msg->referer = getpid();
	strcpy(msg->body, body);
	return msg;

}

void dispatchEvent(Message* msg){
	if(strcmp(msg->resource, "login") == 0){
		if(strcmp(msg->method, "success") == 0){
			setLogin(msg->body);
		}else if(strcmp(msg->method, "error") == 0){
			printf("%s\n", msg->body);
		}
	}else if(strcmp(msg->resource, "register") == 0){
		if(strcmp(msg->method, "success") == 0){
			printf("User Registered Successfully, proceed to Log In\n");
		}else if(strcmp(msg->method, "error") == 0){
			printf("%s\n", msg->body);
		}
	}else if(strcmp(msg->resource, "client") ==0){
		if(strcmp(msg->method, "success") == 0){
			printf("Client Registered Successfully\n");
		}
	}
}

void listenForConnection(){
	int status = 0;
	Message* msg = malloc(sizeof(Message));
    if(select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) <0){
		perror("select");
	}
	status = read(receiver, msg, sizeof(Message));
	if(status == -1){
		free(msg);
		//perror("read");
	}else if(status >=1){
		dispatchEvent(msg);
	}
	
}

char* createPipe(){
	char pid[7];
	char* route = malloc(sizeof(char)*16);
	sprintf(pid,"%d", getpid());
	strcpy(route, "/tmp/serv.");
	strcat(route,pid);
	printf("%d",mkfifo(route, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH));
	return route;
}

void sendData(char* resource, char* method, char* body){
	int status;
	Message* msg = prepareMessage();
	msg = fillMessageData(msg, method, resource, body);
	while ((status = write(sender, msg, sizeof(Message))) == 0){
		printf("paso2\n");
	}
	if(status == -1){
		printf("ERROR ON PIPE write\n");
	}else{
		printf("wrote\n");
	}
	free(msg);
	listenForConnection();
}

void setLogin(char* name){
	strcpy(username, name);
	status = 1;
	printf("Welcome %s to your Mail\n", name);
}

void writeEmail(){
	char result[30];
	printf("Send Email");
}

void grabNewEmails(){
	sendData("mail", "receive", "");
}

void readConsole(){
	int a;
	char result[30];
	if(status == 0){
		printf("Ingrese Numero de Opcion:\n");
		printf("1: Login\n");
		printf("2: Registrarse\n");
		scanf("%d",&a);
		if(a == 1){
			printf("Ingrese Usuario y Contraseña\n Separados por ',' de la forma Usuario,Contraseña\n");
			scanf("%30s", result);
			sendData("login", "login", result);

		}else if(a == 2){
			printf("Ingrese Nombre, Usuario y Contraseña\nSeparados por ',' de la forma\nNombre,Usuario,Contraseña\n");
			scanf("%30s", result);
			sendData("login", "register", result);

		}
	}else if(status == 1){
		printf("Ingrese Numero de Opcion:\n");
		printf("1: Enviar Email\n");
		printf("2: Recibir Emails\n");
		scanf("%d",&a);
		if(a == 1){
			writeEmail();	
		}else if(a == 2){
			grabNewEmails();
		}
	}
}

int main() {
	char* route; 
	route = createPipe(); 
	printf("%s\n", route);
	sender = open("/tmp/serv.xxxxx", O_WRONLY | O_NONBLOCK);
	receiver = open(route, O_RDONLY | O_NONBLOCK);
	FD_ZERO (&active_fd_set);
    FD_SET (receiver, &active_fd_set);
	sendData("client", "register", "");
	while(1){
		readConsole();
	}
	return 1;
}
