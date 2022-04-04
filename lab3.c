#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define ERROR 1

int main()
{	
	printf("Real user id: %d, effective user id: %d\n", getuid(), geteuid());// Печать реального и эффективного идентификатора пользователя
	
	FILE* file = fopen("text.txt", "r");
	// NULL - код ошибки, если файл не удалось открыть
	if (file == NULL) {
		perror("File (first try)");
		exit(1);
	}
	else {
		printf("File opened (first try)\n");
		fclose(file);
	}

 	int effect = setuid(getuid());// Устанавливаем эффективный идентификатор равным реальному
 	if (effect == ERROR) {
 		perror("setuid");
 		return ERROR;
 	}

	printf("New real user id: %d, new effective user id: %d\n", getuid(), geteuid());// Печать нового реального и нового эффективного идентификатора пользователя

	FILE* ffile = fopen("text.txt", "r");
	// NULL - код ошибки, если файл не удалось открыть
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
