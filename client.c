#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "message.h"
#include <time.h>

Message* prepareMessage(){
	Message* info = (Message*) malloc(sizeof(Message)); 
	return info;
}

Message* fillMessageData(Message* msg, char* method, char* resource, int referer, char* body){
	strcpy(msg->protocol, "TPSO 0.12");
	strcpy(msg->method, method);
	strcpy(msg->resource, resource);
	time(&(msg->time));
	msg->referer = referer;
	strcpy(msg->body, body);
	return msg;

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

void sendData(int fd, char* route){
	int status;
	Message* msg = prepareMessage();
	msg = fillMessageData(msg, "register", "login", getpid(), "");

	printf("paso\n");

	while ((status = write(fd, msg, sizeof(Message))) == 0){
		printf("paso2\n");
	}
	if(status == -1){
		printf("ERROR ON PIPE");
	}else{
		close(fd);
		printf("wrote");
	}
}

void listenForConnection(int fd){
	int status = 0;
	Message* msg = malloc(sizeof(Message));
	while ((status = read(fd, msg, sizeof(Message))) == 0);
	if(status == -1){
		perror("read");
	}else{
		printf("New Message\n");
	}
}

int main() {
	int fd, fd2;
	char* route; 
	route = createPipe(); 
	printf("%s\n", route);
	fd = open("/tmp/serv.xxxxx", O_WRONLY | O_NONBLOCK);
	fd2 = open(route, O_RDONLY | O_NONBLOCK);
	sendData(fd, route);
	//while(1){
		//listenForConnection(fd2);
		//sendData(fd,route);
	//}
	return 1;
}
