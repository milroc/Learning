#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1
#define DEBUGP(x, y...) \
        printf("(%s@%d:%s)_LOG_" x "\n", __FILE__,__LINE__,__func__, ##y)


pid_t popen_test(const char *cmd, int *infp, int *outfp);

pid_t popen_test(const char *cmd, int *infp, int *outfp) {
    int p_stdin[2], p_stdout[2];
    pid_t pid;
    if (0 != pipe(p_stdin)) {
        return -1;
    } else if(pipe(p_stdout) != 0) {
        close(p_stdin[READ]);
        close(p_stdin[WRITE]);
        close(p_stdout[READ]);
        close(p_stdout[WRITE]);  
        return -1;
    }
     
    if (0 > (pid  = fork())) {
        close(p_stdin[READ]);
        close(p_stdin[WRITE]);
        close(p_stdout[READ]);
        close(p_stdout[WRITE]);
        return -1;
    }
    
    if (0 > pid) {
        close(p_stdin[READ]);
        close(p_stdin[WRITE]);
        close(p_stdout[READ]);
        close(p_stdout[WRITE]);  
        return pid;
    } else if(0 == pid) {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);
        printf("hai");
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        perror("execl");
    }
    if(NULL == infp)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if(NULL == outfp)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];
    return pid;

}

int main(void) 
{
    char *cmd = "ls";
    pid_t pid_result, pid_w;
    int infp = -1;
    int outfp = -1;
    int i = 0;
    char* buf[10]; 


    if(0 > (pid_result = popen_test(cmd, &infp, &outfp))){
        printf("unable to sort");
        exit(1);
    
    }
    pid_w = waitpid(pid_result, NULL, 0);
    read(outfp, buf, sizeof(buf)-1);

    buf[sizeof(buf)-1]= '\0';
    printf("get \"%s\"\n", buf);
    i++;
    
    pid_w = waitpid(pid_result, NULL, 0);

    return 0;        
}


