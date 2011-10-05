#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>



#define SERVER_PORT  80
#define UN_SOCK_PATH "un_pat"
#define BUFF_SIZE 80

#define DEBUGP(x, y...) \
        printf("(%s@%d:%s)_LOG_" x "\n", __FILE__, \
        __LINE__,__func__, ##y)

int main (void)
{
	int    	i, len, num, rc, urc, size, on = 1;
	int		un_done = 0;
	int		un_total_size = 0;
	int		un_total_alloc_size = BUFF_SIZE;
	int    	listen_sd, accept_sd;
	int    	listen_usd, accept_usd;
	char   	*buffer;
	char	*un_buf;
	char	*un_buf_temp;
	char 	*un_call;
	struct 	sockaddr_in	in_addr;
	struct 	sockaddr_un local, remote;
	
	un_buf = (char *) malloc(BUFF_SIZE);
	
	/*connect to unix controller*/
	listen_usd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (0 > listen_usd) {
		perror("UNIX socket() failed");
		exit(-1);
	}
	
    memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
    local.sun_len = strlen(UN_SOCK_PATH);
    memcpy(local.sun_path, UN_SOCK_PATH, local.sun_len);
	//strncpy(local.sun_path, UN_SOCK_PATH, sizeof(local.sun_path));
	//unlink section
	urc = unlink(local.sun_path);
	// if (0 > urc) {
// 		perror("UNIX unlink() failed");
// 		close(listen_usd);
// 		exit(-1);
// 	}
	
	//Have to have space at end in order to bind properly???
    len = sizeof(local);
	urc = bind(listen_usd, (struct sockaddr *)&local, len);
	if (0 > urc) { 
		perror("UNIX bind() failed");
		close(listen_usd);
		exit(-1);
	}
	
	urc = listen(listen_usd, 5);
	if (0 > urc) {
		perror("UNIX listen() failed");
		close(listen_usd);
		exit(-1);
	}
	
	while (!un_done) {
		int done, n;
		printf("Waiting for a UNIX connection...\n");
		size = sizeof(remote);
		if (0 > (accept_usd = accept(listen_usd, 
								(struct sockaddr *)&remote, &size))) {
			perror("accept");
			exit(-1);
		}
		printf("Connected.\n");
		done = 0;
		while (!done) {
			printf("%sAA",un_buf);
			if (0 < (n = recv(accept_usd, un_buf_temp, BUFF_SIZE, 0))) {
				un_total_size += n;
				if (un_total_size >= un_total_alloc_size) {
					un_total_alloc_size += BUFF_SIZE;
					un_buf = realloc(un_buf, un_total_alloc_size);
				}
				un_buf = strcat(un_buf, un_buf_temp);
			}
			if (0 >= n) { 
				if (0 > n) { 
					perror("UNIX recv() failed");
					close(listen_usd);
					close(accept_usd);
					exit(-1);
				}
				done = 1;
			}
		}
		close(accept_usd);
		un_done = 1;
		printf("\n");
	}
	printf("after:%s\n", un_buf);
	close(listen_usd);
	
	
	/*Iterative Server Section*/
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0) {
		perror("socket() failed");
		exit(-1);
	}

	
	rc = setsockopt(listen_sd,
			 	    SOL_SOCKET,  SO_REUSEADDR,
		   			(char *)&on, sizeof(on));
	if (rc < 0) {
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}

	memset(&in_addr, 0, sizeof(in_addr));
	in_addr.sin_family      = AF_INET;
	in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	in_addr.sin_port        = htons(SERVER_PORT);
	rc = bind(listen_sd, (struct sockaddr *)&in_addr, 
				sizeof(in_addr));
	if (rc < 0)	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}
	
	rc = listen(listen_sd, 5);
	if (rc < 0)	{
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}
	
	printf("The server is ready\n");
	
	for (i=0; i < num; i++) {
		printf("Interation: %d\n", i+1);
		printf("  waiting on accept()\n");
		accept_sd = accept(listen_sd, NULL, NULL);
		if (accept_sd < 0) {
			perror("accept() failed");
			close(listen_sd);
			exit(-1);
		}
		printf("  accept completed successfully\n");
		
		printf("  wait for client to send us a message\n");
		rc = recv(accept_sd, buffer, sizeof(buffer), 0);
		if (rc <= 0) {
			perror("recv() failed");
			close(listen_sd);
			close(accept_sd);
			exit(-1);
		}
		printf("  <%s>\n", buffer);
		
		printf("  echo it back\n");
		len = rc;
		rc = send(accept_sd, buffer, len, 0);
		if (rc <= 0) {
			perror("send() failed");
			close(listen_sd);
			close(accept_sd);
			exit(-1);
		}
		
		close(accept_sd);
	}
	
	close(listen_sd);
}
