#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define   BEL   '\07'
#define ERROR 1

int flag = 1;
int count_sig;
void sigcatch(int sig);

int main()
{
	void* signals = sigset(SIGINT, sigcatch);
	if(signals == SIG_ERR){
		perror("sigset");
        	return ERROR;
	}
	signals = sigset(SIGQUIT, sigcatch);
	if(signals == SIG_ERR){
		perror("sigset");
        	return ERROR;
	}

	while(flag){
        	pause();
    	}
}

void sigcatch(int sig)
{
	if (sig == SIGQUIT) {
		printf("\n bell was rung %d times\n", count_sig);
		exit(0);
	}
	
	count_sig++;
	printf("%c\n", BEL);
}
