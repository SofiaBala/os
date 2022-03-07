#include <sys/types.h>
#include <stdio.h> 
#include <sys/types.h>
#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>
 
#define ERROR_PUTENV -1
#define ERROR_TIME -1
#define PUTENV_EXIT_ERROR 1
#define TIME_EXIT_ERROR 2
#define CTIME_EXIT_ERROR 3

int main() {
	time_t now;
	char *ctDate;

	int area = putenv("TZ=America/Los_Angeles");
	if (area == ERROR_PUTENV){
		perror("Error putenv");
		exit(PUTENV_EXIT_ERROR);
	}

	int t = time(&now);
	if (t == ERROR_TIME){
            perror("Error time");
            exit(TIME_EXIT_ERROR);
    	}

	ctDate = ctime(&now);
	if (ctDate == NULL){
            perror("Error ctime");
            exit(CTIME_EXIT_ERROR);
    	}

    	printf("%s", ctDate);

	return(0);
}
