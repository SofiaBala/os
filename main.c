#include <sys/types.h>
#include <stdio.h> 
#include <time.h> 
#include <stdlib.h> 
extern char* tzname[];

int main() {
	time_t now;
	struct tm* sp;
	char *ct;
	
	int area = putenv("TZ=America/Los_Angeles");
	if (area != 0){
		perror("Error");
		exit(1);
	}
	
	time_t = time(&now);
	if (t == -1){
            perror("Error");
            exit(1);
    	}

	ct = ctime(&now);
	if (ct == NULL){
            perror("Error");
            exit(1);
    	}

    	printf("%s", ct);

	sp = localtime(&now);
	if (sp == NULL){
		perror("Error");
		exit(1);
	}

	printf("%d/%d/%02d %d:%02d %s\n", 
	       sp->tm_mon + 1, sp->tm_mday, 
	       sp->tm_year -100, sp->tm_hour, 
	       sp->tm_min, tzname[sp->tm_isdst]);
	exit(0);
}
