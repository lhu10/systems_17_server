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
        printf("Client Disconnected\n");
        exit(0);
    }
}

void check_error(int check){
    if(check == -1){
        printf("Error %d: %s\n", errno, strerror(errno));
    }
}

void handshake(){
    char private[256], message[100];
    sprintf(private, "%d", getpid());
    mkfifo(private, 0666);
    
    int fd_write, fd_read, check; 
    printf("Client connecting to server\n");
    fd_write = open("public", O_WRONLY);
    check_error(fd_write);
    //client sends message to server
    check = write(fd_write, private, strlen(private)+1);
    check_error(check);

    printf("Client receiving server's message: ");
    fd_read = open(private, O_RDONLY);
    check_error(fd_read);
    //client receives server's message
    check = read(fd_read, message, sizeof(message));
    check_error(check);
    printf("%s\n", message);
    printf("Client removing private FIFO\n");
    unlink(private);
    
    //client sends response to server
    char response[50] = "Handshake Completed";
    check = write(fd_write, response, sizeof(response)); 
    check_error(check);

    close(fd_read);
    close(fd_write);

}

void operation(){
    double celsius, fahrenheit;
    int fd_in, fd_out;
    fd_in = open("input", O_WRONLY);
    fd_out = open("output", O_RDONLY); 
    printf("Converts Temperature in Celsius to Fahrenheit\n");
    while (1){
        printf("Temp in Celsius: ");
        scanf("%lf", &celsius);
        write(fd_in, &celsius, sizeof(celsius));
        read(fd_out, &fahrenheit, sizeof(fahrenheit));
        printf("Temp in Fahrenheit: %lf\n", fahrenheit);
    }
    close(fd_in);
    close(fd_out);
}

int main(){
    signal(SIGINT, sighandler);
    handshake();    
    operation();
    return 0;
}