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
	FREE,
	USED,
	PARTIAL
} state;

typedef struct obj {
	state current;
	size_t size;
	void *data; //set size array?
	struct obj *prev;
	struct obj *next;
} obj;

typedef struct {
	size_t size;
	size_t chunk_size;
	obj *head;
	obj *tail;
} slab;

slab *
_alloc_chunk(slab *slab_allocator)
{
	obj *obj_list = malloc(sizeof(obj)*slab_allocator->chunk_size);
	int i;
	for (i = 0; i < slab_allocator->chunk_size; i++) {
		obj_list[i].current = FREE;
		if (0 == i) {
			obj_list[i].prev = slab_allocator->tail;
			slab_allocator->tail->next = &obj_list[i];
			obj_list[i].next = &obj_list[i+1];
		} else if (slab_allocator->chunk_size - 1 == i) {
			obj_list[i].prev = &obj_list[i-1];
			obj_list[i-1].next = &obj_list[i];
			obj_list[i].next = NULL;
			slab_allocator->tail = &obj_list[i];
		} else {
			obj_list[i].next = &obj_list[i+1];
			obj_list[i].prev = &obj_list[i-1];
		}
	}
	slab_allocator->size += slab_allocator->chunk_size;
}

slab *
create_slab_allocator(int size)
{
	if (0 >= size) {
		perror("Size is less than or equal to 0");
		exit(-1);
	} else {
		slab *slab_allocator = malloc(sizeof(slab));
		slab_allocator->size = size;
		slab_allocator->chunk_size = size;
		_alloc_chunk(slab_allocator);
	}
}

int 
slab_free(slab *slab_allocator, obj *object)
{
	if (FREE == object->current) {
		perror("slab_free() Already Free?");
		exit(-1);
	} else if (0 == slab_allocator->size) {
		slab_allocator->head = object;
		slab_allocator->tail = object;
		object->prev = NULL;
	} else {
		slab_allocator->tail->next = object;
		object->prev = slab_allocator->tail;
	}
	slab_allocator->size++;
	object->next = NULL;
	object->current = FREE;
	return 1;
}

obj *
slab_alloc(slab* slab_allocator)
{
	if (0 >= slab_allocator->size) {
		_alloc_chunk(slab_allocator);
	} 
	slab_allocator->size--;
	obj *object = slab_allocator->head;
	slab_allocator->head = object->next;
	return object;
}

int
main(void) 
{
	slab *
}

