#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define ERROR 1

int main()
{	
	printf("Real user id: %d, effective user id: %d\n", getuid(), geteuid());

	FILE* file = fopen("text.txt", "r");
	if (file == NULL) {
		perror("File (first try)");
		exit(1);
	}
	else {
		printf("File opened (first try)\n");
		fclose(file);
	}

 	int effect = setuid(getuid());
 	if (effect == ERROR) {
 		perror("setuid");
 		return ERROR;
 	}

	printf("New real user id: %d, new effective user id: %d\n", getuid(), geteuid());

	FILE* ffile = fopen("text.txt", "r");
	if (ffile == NULL) {
		perror("File (second try)");
		exit(2);
	}
	else {
		printf("File opened (second try)\n");
		fclose(ffile);
	}
	return(0);
}
