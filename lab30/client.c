#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h> 

#define FILENAME "socket"
#define ERROR_SOCKET -1
#define ERROR_CONNECT -1
#define ERROR_UNLINK -1
#define ERROR_READ -1
#define ERROR_WRITE -1
#define ERROR_CLOSE -1
#define ERROR 1
#define END_OF_FILE 0
#define TRUE 1

int clientfd;

int close_socket(int clientfd){
    int num_close = close(clientfd);
    if(num_close == ERROR_CLOSE){
	perror("error close");
	return ERROR;
    }
}

int write_info(int clientfd){
	
    int num_read, num_write;
    char buffer[BUFSIZ];

        while(TRUE) {
	num_read = read(STDIN_FILENO, buffer, BUFSIZ);
	if(num_read == ERROR_READ){
	    perror("error read");
	    return ERROR;

	}
	if (num_read == END_OF_FILE) break;
	num_write = write(clientfd, buffer, num_read);
	if(num_write == ERROR_WRITE){
	    perror("error write");
	    return ERROR;
	}
    }
    return 0;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    struct sockaddr_un c_addr;
    clientfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(clientfd == ERROR_SOCKET) {
        perror("socket failed");
        return ERROR;
    }

    memset(&c_addr, 0, sizeof(struct sockaddr_un));
    c_addr.sun_family = AF_UNIX;
    strncpy(c_addr.sun_path, FILENAME, sizeof(c_addr.sun_path) - 1);

    int connected = connect(clientfd, (struct sockaddr*)&c_addr, sizeof(c_addr));
    if(connected == ERROR_CONNECT) {
        perror("connect failed");
        return ERROR;
    }
    
    int write = write_info(clientfd);
    if(write == ERROR){
	close_socket(clientfd);
	return ERROR;
    }
    close_socket(clientfd);   
    return 0;
}
