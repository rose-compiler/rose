#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;

//--------------------------------
struct MetaData {
	uint64_t L;
	uint64_t H;
	uint64_t lock_loc;
	uint64_t key;
};

typedef std::map<uint64_t, struct MetaData> MetaMap;
MetaMap TrackingDB;

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

void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long dest,unsigned long long src) {

	assert(isValidEntry(src));
	
	TrackingDB[dest] = TrackingDB[src];
}


void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long long base,unsigned long size) {
	
	struct MetaData md;
	md.L = base;
	md.H = base + size;
	md.lock_loc = find_lock(base);
	md.key = find_key(md.lock_loc);

	TrackingDB[addr] = md;
}

struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long size) {
	struct __Pb__v__Pe___Type output;
	output.ptr = malloc(size);
	output.addr = reinterpret_cast<unsigned long long>(&output.ptr);
	v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(output.addr, reinterpret_cast<unsigned long long>(output.ptr), size);
	#ifdef DEBUG	
	printf("malloc overload\n");
	printf("output.ptr: %llu, output.addr: %llu\n", reinterpret_cast<unsigned long long>(output.ptr),
													output.addr);
	#endif
	return output;
}

void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input) {
	free(input.ptr);
}
//------------------------------

