#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>

#define SERVER_PORT 1340
#define UN_SOCK_PATH "un_pat"
#define BUFF_SIZE 4096
#define ITERATIONS 200

#define DEBUGP(x, y...) \
		printf("(%s@%d:%s_LOG_" x "\n", __FILE__, \
		__LINE__, __func__, ##y)

void perror_exit(const char *str);

void perror_exit(const char *str) {
	perror(str);
	exit(-1);
}


int main(void) {
	char 	*buf, *i_buf;
	char 	*http_req;
	int		listen_sd, accept_sd;
	int		i, alloc_size, actual_size, n;
	int 	bool = 1;
	int		bool_1 = 1;
	struct 	sockaddr_in in_addr;
	struct 	sockaddr_un local, remote;
	
	
	buf = malloc(BUFF_SIZE);
	i = 0;
	alloc_size = BUFF_SIZE;
	actual_size = 0;
	
	/*UNIX server*/
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
		if (0 > (accept_sd = accept(listen_sd, 
									(struct sockaddr *)&remote, &n)))
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
					DEBUGP("%i:%i:%i", actual_size, n, alloc_size);
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
	n = 1;
	bool = 1;
	bool_1 = 1;
	write(1, buf, actual_size);
	printf("UNIX FINISHED\n");
	
	/*INET Server*/
	if (0 > (listen_sd = socket(AF_INET, SOCK_STREAM, 0)))
		perror_exit("socket() failed");
	if (0 > setsockopt(listen_sd,
			 	    SOL_SOCKET,  SO_REUSEADDR,
		   			(char *)&n, sizeof(n)))  
		perror_exit("setsockopt() failed");
	memset(&in_addr, 0, sizeof(in_addr));
	in_addr.sin_family = AF_INET;
	in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	in_addr.sin_port = htons(SERVER_PORT);
	if (0 > bind(listen_sd, (struct sockaddr *)&in_addr, sizeof(in_addr))) 
		perror_exit("bind() failed");
	if(0 > listen(listen_sd, 5)) 
		perror_exit("listen() failed");
	printf("The server is ready\n");
	while(1) {
		printf("Iteration: %d\n", i);
		printf("	waiting on accept()\n");
		if (0 > (accept_sd = accept(listen_sd, NULL, NULL))) 
			perror_exit("accept() failed");
		printf("	accept completed successfully\n");
		printf("	wait for client to send us a message\n");
		if (0 >= (n = recv(accept_sd, i_buf, sizeof(buf), 0)))
			perror_exit("recv() failed");
		//NOT >= but rather nested (see UNIX)
		printf("	<%s>\n", buf);
		printf("	echoit back\n");
		if (0 >= send(accept_sd, buf, actual_size, 0)) 
			perror_exit("send() failed");		
		close(accept_sd);
	}
	close(listen_sd);
}


