#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef PTR_HDR_H
#define PTR_HDR_H
#include <assert.h>

#if 1
struct IntStruct{
	unsigned int* ptr2obj;
	unsigned int *L;
	unsigned int *H;
};
#endif

struct IntStruct malloc_wrap(unsigned int size) {
	
	struct IntStruct ptr;
	ptr.ptr2obj = (unsigned int*)malloc(size);
	ptr.L = (ptr.ptr2obj);
	ptr.H = (ptr.L + size);

	return ptr;
}

unsigned int* Deref_Overload(struct IntStruct ptr) {
	return ptr.ptr2obj;
}

struct IntStruct Cast_Overload(struct IntStruct ptr) {
	return ptr;
}

#if 0
	IntStruct(int* buffer) {
		ptr2obj = buffer;
	}

	IntStruct(IntStruct& other) {
		ptr2obj = other.ptr2obj;
	}
#endif
#endif
