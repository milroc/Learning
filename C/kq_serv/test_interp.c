#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static char * 
interpret_buf(char *read_buf)
{
	/* 
	I realize this is 
	1) a terrible implementation. 
	2) not testing for anything other 
	then an expected GET request.
	
	However I lost my buf interpretation code pre-setting up github, 
	so I rewrote this quickly.
	*/
	int get = 0;
	int http = 0;
	char *line = NULL;
	char *last_line = NULL;
	char *word = NULL;
	char *last_word = NULL;
	int len = strlen(
	line = strtok_r(read_buf, "\n", &last_line);
	word = strtok_r(line, " ", &last_word); 
	while (word != NULL) {
		if (0 == strcmp(word, "GET"))
			get = 1; 
		if (bool && word[0] == '/')
			ret = word;
		if (0 == strcmp(word, "HTTP/1.1"))
			http = 1;
		word = strtok_r(NULL, " ", &last_word);
	} 
	if (strcmp(
	printf("%s\n",line);
	line = strtok_r(NULL, "\n", &last_line);
	
	return "UNSUPPORTED METHOD"; 
}


int 
main(void) 
{
	printf("Hello\n");
	char read_buf[3000] = "GET /helloworld.html HTTP/1.1\nUser-Agent: curl/7.19.4 (i386-portbld-freebsd7.2) libcurl/7.19.4 OpenSSL/0.9.8k zlib/1.2.3\nHost: www.google.com\nAccept: */*\n\n";
	
	char *ret = interpret_buf(read_buf);
	printf("%s\n", ret);
}

