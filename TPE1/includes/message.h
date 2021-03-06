#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <time.h>

typedef struct _message{
	char protocol[9]; // "TPSO 0.1"
	char method[12];
	char resource[12];
	time_t time; 
	int referer;
	char body[30];
}Message;

typedef struct _task{
	Message* msg;
	struct _task * next;
}Task;

#endif
