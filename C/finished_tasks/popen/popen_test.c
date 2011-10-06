#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>

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
    } else if (pipe(p_stdout) != 0) { 
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
    
    if (0 < pid) { 
        close(p_stdin[READ]);
        close(p_stdout[WRITE]);    
    } else if (0 == pid) {
        dup2(p_stdin[READ], READ);
        dup2(p_stdout[WRITE], WRITE);
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        perror("execl");
    } else {
    	assert(0);
    }

    if (NULL == infp)
		close(p_stdin[WRITE]);
  	else
        *infp = p_stdin[WRITE];

    if (NULL == outfp)
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
    char* buf; 
	int total = 0;
	int val = 0;
	int alloc_size = 10;

    if (0 > (pid_result = popen_test(cmd, &infp, &outfp))) {
        printf("unable to sort");
        exit(1);
    }
    
    pid_w = waitpid(pid_result, NULL, 0);
	buf = (char *) malloc(alloc_size);
    
    while (0 < (val = read(outfp, buf+total, 10))) {
   	    total += val;
  	    if(total >= alloc_size) {
   		    alloc_size += val;
   		    buf = realloc(buf, alloc_size);
   	    }
    }
    
    printf("get \"%s\"\n", buf);

    return 0;        
}


