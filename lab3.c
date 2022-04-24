#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#define SETUID_ERROR 2
#define ERROR -1
#define FOPEN_ERROR 3
#define FCLOSE_ERROR 4

int print_id(){
	uid_t uid = getuid();
	uid_t euid = geteuid();

	printf("Real user id: %d, effective user id: %d\n", uid, euid);	
	
	FILE* file = fopen("text.txt", "r");
	if (file == NULL) {
		perror("File");
		return FOPEN_ERROR;
	}
	int close = fclose(file);
	if(close == EOF){
		perror("fclose");
		return FCLOSE_ERROR;
	}
	return(0);
}

int main()
{	
	int id = print_id();
	if (id == FCLOSE_ERROR){
		return FCLOSE_ERROR;
	}	

 	int effect = setuid(getuid());
 	if (effect == ERROR) {
 		perror("setuid");
 		return SETUID_ERROR;
 	}
	id = print_id();
	if (id == FCLOSE_ERROR){
		return FCLOSE_ERROR;
	}
	return(0);
}
