#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    int fd[2], nbytes;
    pid_t childpid = 0;
    char string[80];
    char readbuffer[80];
                         
    pipe(fd);
          
    if((childpid = fork()) < 0) 
    {
        perror("fork");
        exit(1);
    }
                              
    if(childpid == 0) /*Child process*/
    {
        /*close Child input*/
        /*send to parent output*/
        dup2(fd[1], 1);
        char * argv [] = {
            "./helloWorld",
            NULL
        };
        execv("./helloWorld",argv);
    }
    else /*Parent process*/
    {
        /*close parent output*/
        dup2(fd[0], 0);
        /*receive input from pipe*/
        char *argv [] = {
            "./welloHorld",
            NULL
        };
        execv("./welloHorld",argv);
    }
                                            
}

