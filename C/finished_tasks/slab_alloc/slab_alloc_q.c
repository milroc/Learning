#include <errno.h> //CHECK
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
/* Misordered thanks to libc */
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/un.h>

#define MEMORY_SIZE 4000
#define MAX_SLAB_SIZE 50
#define CHUNK_SIZE 100

typedef enum {
	FREE;
	USED;
	PARTIAL;
} state;

typedef struct _obj {
	state current;
	size_t size;
	void *data; //set size array?
	_obj *prev;
	_obj *next;
} obj;

typedef struct _slab {
	size_t size;
	size_t chunk_size;
	obj *head;
	obj *tail;
} slab;


slab *
create_slab_allocator(int size)
{
	if (0 >= size) {
		perror("Size is less than or equal to 0");
		exit(-1);
	} else {
		slab *slab_allocator = malloc(sizeof(slab));
		slab_allocator->chunk_size = size;
		_alloc_chunk(slab_allocator);
	}
}

int 
slab_free(slab *slab_allocator, obj *obj)
{
	if (FREE == obj->current) {
		perror("slab_free() Already Free?");
		exit(-1);
	} 
	push(obj);
	return 1;
}

obj *
slab_alloc(slab* slab_allocator)
{
	if (0 >= slab_allocator->size) {
		_alloc_chunk(slab_allocator);
	} 
	return pop();
}

slab *
_alloc_chunk(slab *slab_allocator)
{
	obj *obj_list = malloc(sizeof(obj)*slab_allocator->chunk_size);
	int i;
	for (i = 0; i < slab_allocator->chunk_size; i++) 
		push(obj_list[i]);
}