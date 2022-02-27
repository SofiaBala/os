#include <sys/types.h>
#include <stdio.h> 
#include <time.h> 
#include <stdlib.h> 
#define ERROR_PUTENV -1
#define ERROR_TIME -1

int main() {
	time_t now;
	char *ctDate;

	int area = putenv("TZ=America/Los_Angeles");
	if (area == ERROR_PUTENV){
		perror("Error putenv");
		exit(1);
	}

	int t = time(&now);
	if (t == ERROR_TIME){
            perror("Error time");
            exit(1);
    	}

	ctDate = ctime(&now);
	if (ctDate == NULL){
            perror("Error ctime");
            exit(1);
    	}

    	printf("%s", ctDate);

	exit(0);
}
