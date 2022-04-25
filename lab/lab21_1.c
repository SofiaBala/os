#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "sigcatch.h"

#define ERROR 1

int flag = 1;
int count_sig;

int main()
{
	void* signals = (int*)sigset(SIGINT, sigcatch);
	if(signals == SIG_ERR){
		perror("sigset");
        	return ERROR;
	}
	signals = (int*)sigset(SIGQUIT, sigcatch);
	if(signals == SIG_ERR){
		perror("sigset");
        	return ERROR;
	}

	while(flag){
        	pause();
    	}
}
