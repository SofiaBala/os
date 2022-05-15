#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#define ERROR_EXECL -1
#define ERROR_FORK -1
#define ERROR_WAIT -1
#define ERROR 1
#define CHILD 0

int main() {
  pid_t pid = fork();

  if (pid == ERROR_FORK) {
	perror("fork error");
        return ERROR_FORK;
  }
  if (pid == CHILD) {
        execl("/bin/cat", "cat", "file.txt", NULL);
       	perror("execl error");
       	return ERROR_EXECL;
  } 
  pid_t wait_p = wait(NULL);
  if (wait_p == ERROR_WAIT) {
	perror("wait error");
	return ERROR_WAIT;
  }
  printf("\nit is from parent process\n");
	
  return 0;
}
