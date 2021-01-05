#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

static void sighandler(int signo){
    if(signo == SIGINT){
        unlink("input");
        unlink("output");
        unlink("public");
        printf("Server Disconnected\n");
        exit(0);
    }
}

void check_error(int check){
    if(check == -1){
        printf("Error %d: %s\n", errno, strerror(errno));
    }
}

void handshake(){
    char pipe[256], message[100];
    mkfifo("public", 0666);
    printf("Server awaiting new Client\n");

    int fd_write, fd_read, check;
    fd_read = open("public", O_RDONLY);
    check_error(fd_read);
    //server receives client's message
    check = read(fd_read, pipe, sizeof(pipe));
    check_error(check);
    printf("Server receiving Client's message\n");
    printf("Server removing WKP\n");
    unlink("public");

    fd_write = open(pipe, O_WRONLY);
    check_error(fd_write);
    //server sends response to client
    char response[100] = "initial acknowledgement message";
    check = write(fd_write, response, sizeof(response));
    check_error(check);

    //server receives final client's message
    check = read(fd_read, message, sizeof(message));
    check_error(check);
    printf("%s\n", message);

    close(fd_write);
    close(fd_read);
}

double c_to_f(double c){
    double f;
    f = (c * 9/5) + 32;
    return f;
}

void operation(){
    int fd_in, fd_out;
    fd_in = open("input", O_RDONLY);
    fd_out = open("output", O_WRONLY);
    double celsius, fahrenheit; 
    while (read(fd_in, &celsius, sizeof(celsius))){
        fahrenheit = c_to_f(celsius);
        write(fd_out, &fahrenheit, sizeof(fahrenheit));
    }
    close(fd_in);
    close(fd_out); 
}

int main(){
    signal(SIGINT, sighandler);
    mkfifo("output", 0666);
    mkfifo("input", 0666);
    while(1){
        handshake();
        operation();
    }
    return 0;
}