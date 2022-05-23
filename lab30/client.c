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

int clientfd;

int f_close(int clientfd){
    int num_close = close(clientfd);
    if(num_close == ERROR_CLOSE){
	perror("error close");
	return ERROR_CLOSE;
    }
}
int f_unlink(const char *name){
    if(unlink(name) == ERROR_UNLINK){
	perror("error unlink");
	return ERROR_UNLINK;
    }
}

void sigcatch(int sig) {
    if(sig == SIGINT) {
        f_close(clientfd);
        f_unlink(FILENAME);
	exit(1);
    }
}

int ch_socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    if(res == ERROR_SOCKET) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

void ch_connect(int clientfd, struct sockaddr *addr, socklen_t addrlen) {
    if(connect(clientfd, addr, addrlen) == ERROR_CONNECT) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
}


int main() {
    struct sockaddr_un c_addr;
    signal(SIGINT, sigcatch);

    clientfd = ch_socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&c_addr, 0, sizeof(struct sockaddr_un));
    c_addr.sun_family = AF_UNIX;
    strncpy(c_addr.sun_path, FILENAME, sizeof(c_addr.sun_path) - 1);

    ch_connect(clientfd, (struct sockaddr*)&c_addr, sizeof(c_addr));

    int num_read, num_write;
    char buffer[BUFSIZ];
    
    while((num_read = read(1, buffer, BUFSIZ)) > 0) {
	if(num_read == ERROR_READ){
	    perror("error read");
	    return ERROR_READ;
	}
	num_write = write(clientfd, buffer, num_read);
	if(num_write == ERROR_WRITE){
	    perror("error write");
	    return ERROR_WRITE;
	}
        if(num_write != num_read) {
            printf("writing failed");
            f_close(clientfd);
            unlink(FILENAME);
            exit(1);
        }
    } 
    f_close(clientfd);   
    f_unlink(FILENAME);
    return 0;
}
