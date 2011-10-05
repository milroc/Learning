#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>


#define UN_SOCK_PATH "un_pat"

#define DEBUGP(x, y...) \
        printf("(%s@%d:%s)_LOG_" x "\n", __FILE__, \
        __LINE__,__func__, ##y)
        
int main(void) {
	int		num;
	int 	connect_sd;
	int		sc; 
	int 	send_ack;
	char	buf[] = "HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\nHello world!";
	struct 	sockaddr_un serv_addr;
	
	if(0 > (connect_sd = socket(AF_UNIX, SOCK_STREAM, 0))) {
		perror("socket() failed");
		exit(-1);
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, UN_SOCK_PATH);
	if(0 > connect(connect_sd, 
					(struct sockaddr *) &serv_addr, sizeof(serv_addr))) { 
		perror("connect() failed");
		close(connect_sd);
		exit(-1);
	}
	if(0 > send(connect_sd, buf, sizeof(buf), 0)) {
		perror("send() failed");
		close(connect_sd);
		exit(-1);
	}
	close(connect_sd);
}