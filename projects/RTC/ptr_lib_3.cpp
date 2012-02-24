#if 0
#include "foo2_c.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>
#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49
#endif
#include "class_lib.h"

struct MetaData {
	uint64_t L;
	uint64_t H;
	uint64_t lock_loc;
	uint64_t key;
};

//class node;

typedef std::map<uint64_t, struct MetaData> MetaMap;
MetaMap TrackingDB;


struct __Pb____Pb__node__Pe____Pe__ 
{
  class node **ptr;
  unsigned long long addr;
}
;

struct __Pb____Pb__i__Pe____Pe__ 
{
  int **ptr;
  unsigned long long addr;
}
;

struct __Pb__node__Pe__ 
{
  class node *ptr;
  unsigned long long addr;
}
;

struct __Pb__i__Pe__ 
{
  int *ptr;
  unsigned long long addr;
}
;

struct __Pb__Ui__Pe__ 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;

struct __Pb__v__Pe__ 
{
  void *ptr;
  unsigned long long addr;
}
;


uint64_t find_lock(unsigned long long base) {
	// Some number
	return 0;
}

uint64_t find_key(uint64_t lock_loc) {
	// Some number
	return 0;
}

bool isValidEntry(unsigned long long addr) {
	MetaMap::iterator iter = TrackingDB.find(addr);

	return (iter != TrackingDB.end()); 
}


// This version accounts for blank entries
void create_entry_blank(unsigned long long dest,unsigned long long src) {
	
	MetaMap::iterator iter = TrackingDB.find(src);

	if(iter != TrackingDB.end()) {
		struct MetaData md;
		md.L = 0;
		md.H = 0;
		md.lock_loc = 0;
		md.key = 0;

		TrackingDB[dest] = md;
		return;
	}

	TrackingDB[dest] = iter->second;
}

void create_entry(unsigned long long dest,unsigned long long src) {

	assert(isValidEntry(src));
	
	TrackingDB[dest] = TrackingDB[src];
}


void create_entry(unsigned long long addr, unsigned long long base, unsigned long long size) {
	
	struct MetaData md;
	md.L = base;
	md.H = base + size;
	md.lock_loc = find_lock(base);
	md.key = find_key(md.lock_loc);

	TrackingDB[addr] = md;
}

struct __Pb__v__Pe__ malloc_overload(int size) {
	struct __Pb__v__Pe__ output;
	output.ptr = malloc(size);
	output.addr = reinterpret_cast<unsigned long long>(&output.ptr);
	create_entry(output.addr, reinterpret_cast<unsigned long long>(output.ptr), size);
	#ifdef DEBUG	
	printf("malloc overload\n");
	printf("output.ptr: %llu, output.addr: %llu\n", reinterpret_cast<unsigned long long>(output.ptr),
													output.addr);
	#endif
	return output;
}


struct __Pb__node__Pe__ create_struct_from_addr_UL___Pb__node__Pe__(unsigned long long addr) {
	
	struct __Pb__node__Pe__ output;
	output.ptr = *(reinterpret_cast<class node**>(addr));
	output.addr = addr;

	return output;
}

struct __Pb__i__Pe__ create_struct_from_addr_UL___Pb__i__Pe__(unsigned long long addr) {

	struct __Pb__i__Pe__ output;
	output.ptr = *(reinterpret_cast<int**>(addr));
	output.addr = addr;

	return output;

}

struct __Pb____Pb__node__Pe____Pe__ create_struct___Pb____Pb__node__Pe____Pe___UL_Ul___Pb____Pb__node__Pe____Pe__(class node **ptr , unsigned long long addr, unsigned long offset) {
	
	struct __Pb____Pb__node__Pe____Pe__ output;
	output.ptr = ptr;
	output.addr = addr;
	create_entry(addr, reinterpret_cast<unsigned long long>(ptr), offset);

	return output;
}

struct __Pb____Pb__i__Pe____Pe__ create_struct___Pb____Pb__i__Pe____Pe___UL_Ul___Pb____Pb__i__Pe____Pe__(int ** ptr,unsigned long long addr,unsigned long size) {
	struct 	__Pb____Pb__i__Pe____Pe__ output;
	output.ptr = ptr;
	output.addr = addr;



	return output;
}

struct __Pb__i__Pe__ create_struct___Pb__i__Pe___UL_Ul___Pb__i__Pe__(int * ptr,unsigned long long addr,unsigned long size) {
	struct __Pb__i__Pe__ output;
	output.ptr = ptr;
	output.addr = addr;
	create_entry(addr, reinterpret_cast<unsigned long long>(output.ptr), size);

	return output;
}


struct __Pb__node__Pe__ create_struct___Pb__node__Pe___UL_Ul___Pb__node__Pe__(class node * ptr,unsigned long long addr,unsigned long offset) {
	struct __Pb__node__Pe__ output;
	output.ptr = ptr;
	output.addr = addr;
	create_entry(addr, reinterpret_cast<unsigned long long>(ptr), offset);
	
	return output;
}

struct __Pb__Ui__Pe__ create_struct___Pb__Ui__Pe___UL___Pb__Ui__Pe__(unsigned int* ptr, unsigned long long addr) {
	struct __Pb__Ui__Pe__ output;
	output.ptr = ptr;
	output.addr = addr;

	return output;
}


class node **__Pb____Pb__node__Pe____Pe___PntrArrRef___Pb____Pb__node__Pe____Pe___Ui(struct __Pb____Pb__node__Pe____Pe__ input ,unsigned int index) {
	return input.ptr + index;
}

int **__Pb____Pb__i__Pe____Pe___PntrArrRef___Pb____Pb__i__Pe____Pe___Ui(struct __Pb____Pb__i__Pe____Pe__ input,unsigned int index) {
	return input.ptr + index;
}

class node *__Pb__node__Pe___PntrArrRef___Pb__node__Pe___Ui(struct __Pb__node__Pe__ input,unsigned int index){
	return input.ptr + index;
}

int *__Pb__i__Pe___PntrArrRef___Pb__i__Pe___i(struct __Pb__i__Pe__ input,int index) {
	return input.ptr + index;
}

int *__Pb__i__Pe___PntrArrRef___Pb__i__Pe___Ui(struct __Pb__i__Pe__ input,unsigned int index) {
	return input.ptr - index;
}


	

struct __Pb__i__Pe__ __Pb__i__Pe___Assign___Pb__i__Pe_____Pb__i__Pe__(unsigned long long str_ref, struct __Pb__i__Pe__ src) {

	struct __Pb__i__Pe__* str_ptr = reinterpret_cast<struct __Pb__i__Pe__*>(str_ref);
	create_entry(str_ptr->addr, src.addr);

	str_ptr->ptr = src.ptr;

	return *str_ptr;

}

struct __Pb__Ui__Pe__ __Pb__Ui__Pe___Assign___Pb__Ui__Pe_____Pb__Ui__Pe__(unsigned long long str_ref,struct __Pb__Ui__Pe__ src) {
	struct __Pb__Ui__Pe__* str_ptr = reinterpret_cast<struct __Pb__Ui__Pe__*>(str_ref);
	create_entry(str_ptr->addr, src.addr);
	
	str_ptr->ptr = src.ptr;

	return *str_ptr;
}

// This assign comes out of a variable initialization, instead of the rest, which are
// due to variable reassignment.
// The third case is that of parameter initialization -- which hasn't been handled yet
struct __Pb__Ui__Pe__ Assign(struct __Pb__Ui__Pe__ * dest,struct __Pb__Ui__Pe__ src) {
	#ifdef DEBUG
	printf("Assign\n");
	printf("src.ptr: %llu, src.addr: %llu\n", reinterpret_cast<unsigned long long>(src.ptr),
											  src.addr);
	#endif
	dest->ptr = src.ptr;
	create_entry(dest->addr, src.addr);
	#ifdef DEBUG
	printf("dest->ptr: %llu, dest->addr: %llu\n", reinterpret_cast<unsigned long long>(dest->ptr),
											  dest->addr);
	#endif
	return *dest;
}

struct __Pb__node__Pe__ __Pb__node__Pe___Assign___Pb__node__Pe_____Pb__node__Pe__(unsigned long long str_ref,struct __Pb__node__Pe__ src)
 {
	struct __Pb__node__Pe__* str_ptr = reinterpret_cast<struct  __Pb__node__Pe__*>(str_ref);
	create_entry(str_ptr->addr, src.addr);

	str_ptr->ptr = src.ptr;

	return *str_ptr;
}



struct __Pb__i__Pe__ __Pb__i__Pe___Cast___Pb__v__Pe___PntrArr(struct __Pb__v__Pe__ input) {

	struct __Pb__i__Pe__ output;
	output.ptr = (int*)input.ptr;
	output.addr = input.addr;
	create_entry(output.addr, input.addr);

	return output;

}

struct __Pb__Ui__Pe__ __Pb__Ui__Pe___Cast___Pb__v__Pe___PntrArr(struct __Pb__v__Pe__ input) {
	#ifdef DEBUG
	printf("Cast\n");
	printf("input.ptr: %llu, input.addr: %llu\n", reinterpret_cast<unsigned long long>(input.ptr), 
												  input.addr);
	#endif
	struct __Pb__Ui__Pe__ output;
	output.ptr = (unsigned int*)input.ptr;
	output.addr = input.addr;
	// if input.addr == output.addr, create_entry is useless below!
	create_entry(output.addr, input.addr);
	#ifdef DEBUG
	printf("output.ptr: %llu, output.addr: %llu\n", reinterpret_cast<unsigned long long>(output.ptr), 
												  output.addr);
	#endif
	return output;
}

struct __Pb__node__Pe__ __Pb__node__Pe___Cast___Pb__v__Pe___PntrArr(struct __Pb__v__Pe__ input) {

	struct __Pb__node__Pe__ output;
	
	output.ptr = reinterpret_cast<class node*>(input.ptr);
	output.addr = input.addr;

	return output;
}

unsigned long Ul_Cast___Pb__Ui__Pe___ValRet(struct __Pb__Ui__Pe__ str) {
	return reinterpret_cast<unsigned long>(str.ptr);
}

int *Deref___Pb__i__Pe_____Pb__i__Pe__(struct __Pb__i__Pe__ input) {
	return input.ptr;
}


unsigned int *Deref___Pb__Ui__Pe_____Pb__Ui__Pe__(struct __Pb__Ui__Pe__ input) {
	return input.ptr;
}

class node *Deref___Pb__node__Pe_____Pb__node__Pe__(struct __Pb__node__Pe__ input) {
	return input.ptr;
}

struct __Pb__Ui__Pe__ __Pb__Ui__Pe___Decrement___Pb__Ui__Pe__(unsigned long long str_ref) {
	
	struct __Pb__Ui__Pe__* str_ptr = reinterpret_cast<struct __Pb__Ui__Pe__*>(str_ref);
	str_ptr->ptr--;
	return *str_ptr;
}


struct __Pb__Ui__Pe__ __Pb__Ui__Pe___Increment___Pb__Ui__Pe__(unsigned long long str_ref) {
	struct __Pb__Ui__Pe__* str_ptr = reinterpret_cast<struct __Pb__Ui__Pe__*>(str_ref);
	str_ptr->ptr++;
	return *str_ptr;
}

bool b_GreaterOrEqual___Pb__Ui__Pe_____Pb__Ui__Pe__(struct __Pb__Ui__Pe__ lhs,struct __Pb__Ui__Pe__ rhs) {
	return lhs.ptr >= rhs.ptr;
}

bool b_LessThan___Pb__Ui__Pe_____Pb__Ui__Pe__(struct __Pb__Ui__Pe__ lhs,struct __Pb__Ui__Pe__ rhs) {
	return lhs.ptr < rhs.ptr;
}

struct __Pb__Ui__Pe__ __Pb__Ui__Pe___Sub___Pb__Ui__Pe___i(struct __Pb__Ui__Pe__ str,int val) {
	str.ptr -= val;
	return str;
}


struct __Pb__Ui__Pe__ __Pb__Ui__Pe___Add___Pb__Ui__Pe___i(struct __Pb__Ui__Pe__ str, int val) {
	str.ptr += val;
	return str;
}









