#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define   BEL   '\07'
#define ERROR 1

int flag = 1;
int count_sig;

int main(){
	void sigcatch(int);

	void* signals = signal(SIGINT, sigcatch);
	if(signals == SIG_ERR){
		perror("signal");
        	return ERROR;
	}
	signals = signal(SIGQUIT, sigcatch);
	if(signals == SIG_ERR){
		perror("signal");
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
	printf("%c\n", BEL);
	count_sig++;
	signal(sig, sigcatch);
}
