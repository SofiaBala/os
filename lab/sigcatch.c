#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


#define   BEL   '\07'
int count_sig;

void sigcatch(int sig)
{
	if (sig == SIGQUIT) {
		printf("\n bell was rung %d times\n", count_sig);
		exit(0);
	}
	
	count_sig++;
	printf("%c\n", BEL);
}
