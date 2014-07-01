/*
 * main.c
 *
 *  Created on: Oct 25, 2013
 *      Author: lundburgerr
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/time.h>

typedef struct arg_p{
	char *msg;
	int val;
} arg_p;

arg_p arg;
pthread_t **tid;

void *printMe(void *arg);
int main(int argc, char *argv[]){
	int N = 5;
	unsigned int n;
	arg.msg = "HEJSAN";
	arg.val = 2;


    tid = malloc(N*sizeof(pthread_t*));

    for(n=0; n<N; n++){
    	pthread_create(&tid[n], NULL, printMe, (void*)&arg);
    }
    for(n=0; n<N; n++){
    	pthread_join(tid[n],NULL);
    }
	printf("Done\n");

	return EXIT_SUCCESS;
}

void *printMe(void *arg){
	arg_p *a = (arg_p*)arg;
	char *msg = (char*) a->msg;
	int val = a->val;
	printf("%s %d\n", msg, val);

	return NULL;
}
