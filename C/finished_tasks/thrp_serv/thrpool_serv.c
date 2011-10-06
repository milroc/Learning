#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

/* Constants */
#define BUFF_SIZE 4096
#define ITERATIONS 200
#define MAX_THREADS 10000
#define SERVER_PORT 1340
#define UN_SOCK_PATH "un_path"

/* Macros */
#define DEBUGP(x, y...) \
		printf("(%s@%d:%s_LOG_" x "\n", __FILE__, \
		__LINE__, __func__, ##y)

/* Structures */
typedef struct thread_data {
	int i;
	int accept_sd;
} thread_data;

/* Global Variables */
volatile int curr_threads = 0;
int	actual_size;
char *buf;

/* Functions */
void
perror_exit(const char *str)
{
	perror(str);
	exit(-1);
}

int 
unix_serv(void)
{
	int	accept_sd;
	int listen_sd;
	int alloc_size;
	int n;
	int	bool = 1;
	int	bool_1 = 1;
	struct sockaddr_un local; 
	struct sockaddr_un remote;
	
	alloc_size = BUFF_SIZE;
	buf = malloc(BUFF_SIZE);
	actual_size = 0;
	
	if (0 > (listen_sd = socket(AF_UNIX, SOCK_STREAM, 0)))
		perror_exit("UNIX socket() failed");
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	local.sun_len = strlen(UN_SOCK_PATH);
	memcpy(local.sun_path, UN_SOCK_PATH, local.sun_len);
	unlink(local.sun_path);
	if (0 > bind(listen_sd, (struct sockaddr *)&local, sizeof(local)))
		perror_exit("UNIX bind() failed");
	if (0 > listen(listen_sd, 5))
		perror_exit("UNIX listen() failed");
	while (bool) {
		printf("Waiting for a UNIX connection....\n");
		n = sizeof(remote);
		if (0 > (accept_sd = accept(listen_sd, (struct sockaddr *)&remote, &n)))
			perror_exit("UNIX accept() failed");
		printf("Connected.\n");
		while(bool_1) {
			if (0 < (n = recv(accept_sd, buf+actual_size, BUFF_SIZE-actual_size, 0))) {
				actual_size+=n;
				if (actual_size >= alloc_size) {
					alloc_size+=BUFF_SIZE;
					void *temp = realloc(buf, alloc_size);
					if (temp == NULL)
						perror_exit("REALLOC BUG");
				}
			}
			if (0 >= n) {
				if (0 > n)
					perror_exit("UNIX recv() failed");
				bool_1 = 0;
			}
		}
		bool = 0;
		close(accept_sd);
	}
	close(listen_sd);
	printf("Actual_size: %i\nBuf: %s\n", actual_size, buf);
	printf("UNIX FINISHED\n");
}

void *
thread_process(void * arg)
{
	thread_data data;
    data.i = (*(struct thread_data*)arg).i;
    data.accept_sd = (*(struct thread_data*)arg).accept_sd;
    char read_buf[4096];
	int n, bool = 1;
	if (0 > (n = recv(data.accept_sd, read_buf, sizeof(read_buf), 0)))
		perror_exit("recv() failed");
	/*NOT >= but rather nested (see UNIX)*/
	char *tmp = "HTTP/1.1 200 OK\n\n";
	if (0 >= send(data.accept_sd, tmp, strlen(tmp), 0))
		perror_exit("send() failed");
	shutdown(data.accept_sd, SHUT_RDWR);
	close(data.accept_sd);
	--curr_threads;
	return NULL;
}

void *
inet_serv(void)
{
    int listen_sd;
	int n = 1;
	struct 	sockaddr_in in_addr;
	if (0 > (listen_sd = socket(AF_INET, SOCK_STREAM, 0)))
		perror_exit("socket() failed");
	if (0 > setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,
		   			(char *)&n, sizeof(n)))
		perror_exit("setsockopt() failed");
	memset(&in_addr, 0, sizeof(in_addr));
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(SERVER_PORT);
	if (0 > bind(listen_sd, (struct sockaddr *)&in_addr, sizeof(in_addr)))
		perror_exit("bind() failed");
	if(0 > listen(listen_sd, 128))
		perror_exit("listen() failed");
	printf("The server is ready\n");
	
	pthread_t pthreads[MAX_THREADS];
	thread_data data[MAX_THREADS];
	int i = 0;
	while(1) {
		(data[i]).i = i;
		if ((MAX_THREADS > curr_threads) &&
		    (0 <= (data[i].accept_sd = accept(listen_sd, NULL, NULL)))) {
			if (0 > pthread_create(&pthreads[i], NULL,
								thread_process, (void *)&data[i]))
				perror_exit("pthread_create() failed");
			printf("%i:%i:::%i\n",i, (data[i]).i, curr_threads);
			++curr_threads;
			if (MAX_THREADS == i) 
				break;
		}
		else perror_exit("accept() failed");
		i++;
	}
	close(listen_sd);
	sleep(10);
}

/* Main */
int
main(void)
{
	unix_serv();
	pthread_t p;
	pthread_create(&p, NULL,inet_serv, NULL);
	pthread_join(p, NULL);
	return 0;
	
	
}
