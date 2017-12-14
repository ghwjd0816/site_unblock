#pragma once

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>

#define NUM_OF_METHOD 6
#define METHOD_MAX_LENGTH 7

struct thread_argument{
	int childfd;
};

void *th_function(void*);
void findhostname(char*,char*,int);
int dummyhttp(char*,char*,int);
