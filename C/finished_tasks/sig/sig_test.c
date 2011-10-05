#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int g_flag = 0;
int sig;
void signal_handler(int sign);

void signal_handler(int sign) {
	g_flag = 1;
	sig = sign;
}

int main(void) { 
	int bool = 1;
	while (bool) {
		signal(SIGUSR1, &signal_handler);
		signal(SIGUSR2, &signal_handler);
		signal(SIGILL, &signal_handler);
		signal(SIGPIPE, &signal_handler);
		if (g_flag) {
			printf("It works\n"); //use write here
			printf("sig: %i\n", sig);
			if (SIGUSR1 == sig)
				printf("SIGUSR1\n");
			else if (SIGUSR2 == sig)
				printf("SIGUSR2\n");
			else if (SIGPIPE == sig)
				printf("SIGPIPE\n");
			else if (SIGILL == sig)
				printf("SIGILL\n");
			else
				printf("Unknown Signal\n");
			bool = 0;
		}
	}
}