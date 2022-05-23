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
#define BACKLOG 5
#define ERROR_SOCKET -1
#define ERROR_UNLINK -1
#define ERROR_BIND -1
#define ERROR_LISTEN -1
#define ERROR_READ -1
#define ERROR_WRITE -1
#define ERROR_CLOSE -1

int serverfd;
int clientfd;

int f_close(int some){
    int num_close = close(some);
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
        f_close(serverfd);
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

void ch_bind(int serverfd, struct sockaddr* addr, socklen_t addrlen) {
    if(bind(serverfd, addr, addrlen) == ERROR_BIND) {
        f_close(serverfd);
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void ch_listen(int descriptor, int backlog) {
    if(listen(descriptor, backlog) == ERROR_LISTEN) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    struct sockaddr_un addr; 
    signal(SIGINT, sigcatch);

    serverfd = ch_socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, FILENAME, sizeof(addr.sun_path) - 1);

    ch_bind(serverfd, (struct sockaddr*)&addr, sizeof(addr));

    ch_listen(serverfd, BACKLOG);

    struct sockaddr client_addr;
    socklen_t cl_addrlen = sizeof(client_addr);

    printf("Waiting to accept a connection...\n");
    clientfd = accept(serverfd, &client_addr, &cl_addrlen);
    printf("Accepted\n");

    int num_read, num_write;
    char buffer[BUFSIZ];

    int i;

    while((num_read = read(clientfd, buffer, BUFSIZ)) > 0) {
	if(num_read == ERROR_READ){
	    perror("error read");
	    return ERROR_READ;
	}
        for(i = 0; i < num_read; ++i) {
            buffer[i] = toupper(buffer[i]);
        }
	num_write = write(1, buffer, num_read);
	if(num_write == ERROR_WRITE){
	    perror("error write");
	    return ERROR_WRITE;
	}
        if(num_write != num_read) {
            printf("writing failed");
            f_close(serverfd);
            f_close(clientfd);
            f_unlink(FILENAME);
            exit(EXIT_FAILURE);
        }
    }
    f_close(serverfd);
    f_close(clientfd);
    f_unlink(FILENAME);
    return 0;
}
