#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#define ERROR_OPEN -1
#define ERROR_TCGETATTR -1
#define ERROR_TCSETATTR -1
#define ERROR_READ -1
#define ERROR_CLOSE -1

int main(int argc, char* argv[]) {
    int fd, tcs, tcg;

    struct termios tty, savetty; 
    fd = open("/dev/tty", O_RDWR);
    if (fd == ERROR_OPEN){
	perror("error open");
	return ERROR_OPEN;
    } 


    tcg = tcgetattr(fd, &tty);
    if (tcg == ERROR_TCGETATTR){
	perror("error tcgetattr");
	return ERROR_TCGETATTR;
    } 

    savetty = tty; 
    tty.c_cc[VMIN] = 1; 
    tty.c_lflag &= ~(ECHO | ICANON); 

    tcs = tcsetattr(fd, TCSANOW, &tty); 
    if (tcs == ERROR_TCSETATTR){
	perror("error tcgetattr");
	return ERROR_TCSETATTR;
    }

    printf("What's your favourite animal: cat(c),dog(d),horse(h),rabbit(r)?\n");
    char ch;
    ssize_t readd = read(fd, &ch, 1);
    if (readd == ERROR_READ){
	perror("error read");
	return ERROR_READ;
    }

    switch (ch) {
        case 'c':
            printf("You chose cat!\n");
            break;
        case 'd':
            printf("You chose dog!\n");
            break;
        case 'h':
            printf("You chose horse!\n");
            break;
	case 'r':
            printf("You chose rabbit!\n");
            break;
        default:
            printf("Incorrect answer (%c)\n", ch);
            break;
    }
    tcs = tcsetattr(fd, TCSANOW, &savetty); 
    if (tcs == ERROR_TCSETATTR){
	perror("error tcsetattr");
	return ERROR_TCSETATTR;
    }
    int close_f = close(fd);
    if(close_f == ERROR_CLOSE){
	perror("error close");
	return ERROR_CLOSE;
    }
    return 0;
}
