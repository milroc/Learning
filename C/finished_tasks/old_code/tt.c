#include <stdio.h>
#include <stdlib.h>

typedef void (*dispatch_fn)(void *);

typedef struct test {
	void *arg;
	void (*func)(void *);
} test;

void * 
print_test(void *arg)
{
	char *temp = (char *)arg;
	printf("%s\n", temp);
}

int 
main(void) 
{
	void *arg;
	dispatch_fn func = (dispatch_fn)print_test;
	arg = "Hello World";
	test *t; 
	t = malloc(sizeof(test));
	t->func = func;
	t->arg = arg;
	
	(t->func)(t->arg);
}