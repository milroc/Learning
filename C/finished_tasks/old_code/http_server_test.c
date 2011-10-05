#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



void error(const char *msg) { 
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	int socket_fd, new_socket_fd, port_number;
	socklen_t clilen; //??
	char buffer[256];
	struct socket_address_in server_address, cli_address; //CLI = command line interface?
	int n;
	
	if(argc < 2)  
		error("ERROR, no port provided\n");
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET? SOCK_STREAM?
	
	if(sockfd < 0)
		error("ERROR opening socket");
	
	bzero((char *) &server_address, sizeof(server_address));
	
	port_number = atoi(argv[1]); //atoi??
	
	server_address.sin_family = AF_INET; //sin_family? .anything
	server_address.sin_address.s_address = INADDR_ANY;
	server_address.sin_port = htons(port_number);
	
}