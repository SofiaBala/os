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
#define ERROR_ACCEPT -1
#define ERROR 1
#define END_OF_FILE 0
#define TRUE 1

int serverfd;
int new_socket;

int close_socket(int socketfd){
    int num_close = close(socketfd);
    if(num_close == ERROR_CLOSE){
	perror("error close");
	return ERROR;
    }
}

int unlink_file(const char *name){
    if(unlink(name) == ERROR_UNLINK){
	perror("error unlink");
	return ERROR;
    }
}

void sigcatch(int sig) {
    if(sig == SIGINT) {
        close_socket(serverfd);
        close_socket(new_socket);
        unlink_file(FILENAME);
        exit(0);
    }
}

int read_info(int clientfd){
    int num_read, num_write;
    char buffer[BUFSIZ];

    int i;

    while(TRUE) {
	num_read = read(clientfd, buffer, BUFSIZ);
	if(num_read == ERROR_READ){
	    perror("error read");
	    return ERROR;
	}
	if (num_read == END_OF_FILE) break;
        for(i = 0; i < num_read; ++i) {
            buffer[i] = toupper(buffer[i]);
        }
	num_write = write(STDOUT_FILENO, buffer, num_read);
	if(num_write == ERROR_WRITE){
	    perror("error write");
	    return ERROR;
	}
    }
    return 0;
}

int establish_connect(int serverfd,  struct sockaddr* addr){
    int bind_res = bind(serverfd, addr, sizeof(*addr));
    if(bind_res == ERROR_BIND) {
        close_socket(serverfd);
        perror("bind failed");
        return ERROR;
    }
    int listen_res = listen(serverfd, BACKLOG);
    if(listen_res == ERROR_LISTEN) {
        perror("listen failed");
        return ERROR;
    }
    new_socket = accept(serverfd, NULL, NULL);
    if(new_socket == ERROR_ACCEPT) {
        perror("accept failed");
        return ERROR;
    }
    return new_socket;
}

int main() {
    struct sockaddr_un addr; 
    void* sigset_res = sigset(SIGINT, sigcatch);
    if (sigset_res == SIG_ERR){
        perror("sigset");
        return ERROR;
    }


    serverfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serverfd == ERROR_SOCKET) {
        perror("socket failed");
        return ERROR_SOCKET;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, FILENAME, sizeof(addr.sun_path) - 1);
 
    int link = establish_connect(serverfd,(struct sockaddr*)&addr);
    if(link == ERROR){
        unlink_file(FILENAME);
	return ERROR;
    }

    int read = read_info(link);
    if(read == ERROR){
	close_socket(serverfd);
        close_socket(new_socket);
        unlink_file(FILENAME);
	return ERROR;
    }
    close_socket(serverfd);
    close_socket(new_socket);
    unlink_file(FILENAME);
    return 0;
}
