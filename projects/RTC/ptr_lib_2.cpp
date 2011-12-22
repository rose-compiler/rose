#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>

struct MetaData {
	uint64_t L;
	uint64_t H;
	uint64_t lock_loc;
	uint64_t key;
};

typedef std::map<uint64_t, struct MetaData> MetaMap;
MetaMap TrackingDB;


struct Void_Type4 
{
  void *ptr;
  unsigned long long addr;
}
;

struct Char_Type3 
{
  char *ptr;
  unsigned long long addr;
}
;

struct Double_Type2 
{
  double *ptr;
  unsigned long long addr;
}
;

struct Float_Type1 
{
  float *ptr;
  unsigned long long addr;
}
;

struct UnsignedInt_Type0 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;

bool isValidEntry(unsigned long long addr) {
	MetaMap::iterator iter = TrackingDB.find(addr);

	return (iter != TrackingDB.end()); 
}


// This version accounts for blank entries
void create_entry_blank(unsigned long long addr1,unsigned long long addr2) {
	
	MetaMap::iterator iter = TrackingDB.find(addr2);

	if(iter != TrackingDB.end()) {
		struct MetaData md;
		md.L = 0;
		md.H = 0;
		md.lock_loc = 0;
		md.key = 0;

		TrackingDB[addr1] = md;
		return;
	}

	TrackingDB[addr1] = iter->second;
}

void create_entry(unsigned long long addr1,unsigned long long addr2) {

	assert(isValidEntry(addr2));
	
	TrackingDB[addr1] = TrackingDB[addr2];
}

uint64_t find_lock(unsigned long long base) {
	// Some number
	return 0;
}

uint64_t find_key(uint64_t lock_loc) {
	// Some number
	return 0;
}

void create_entry(unsigned long long addr, unsigned long long base, int size) {

	struct MetaData md;
	md.L = base;
	md.H = base + size;
	md.lock_loc = find_lock(base);
	md.key = find_key(md.lock_loc);

	TrackingDB[addr] = md;
}

#if 0
template<class T>
T foo(int *bar)
{
 return (T)bar;
}

foo(float*)


float* foo(bar)
char* foo(bar)
#endif



struct UnsignedInt_Type0 UnsignedInt_Type0_PlusAssign_UnsignedInt_Type0_i(struct UnsignedInt_Type0 *str,int val) {

	str->ptr += val;
	return *str;

}


bool b_Equality_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 str1 ,struct UnsignedInt_Type0 str2) {

	return (str1.ptr == str2.ptr);
}


bool b_NotEqual_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 str1 ,struct UnsignedInt_Type0 str2 ) {


	return !b_Equality_UnsignedInt_Type0_UnsignedInt_Type0(str1,str2);
}




struct UnsignedInt_Type0 UnsignedInt_Type0_Increment_UnsignedInt_Type0(struct UnsignedInt_Type0 *str) {

	// valid entry check
	assert(isValidEntry(str->addr));
	str->ptr++;
	return *str;
}


bool b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 str1,struct UnsignedInt_Type0 str2) {

	return (str1.ptr < str2.ptr);
}

// Whenever the variable which is being returned, is passed by value, its contents need to be updated with
// a create_entry. This is because, the address in the variable is from the parent, and its clinging onto that
// address due to the pass-by-value paradigm. Not because, its parent is being updated in this function.
// Precisely for this reason, if a variable is passed by reference, it needn't be updated because its the 
// parent's version of the variable anyways! 
// If there is a local version of a variable, which is being given back, it should have the correct values
// inside it -- for address, it should use its own entry, not the one it received from its parent
// Examples are: add, subtract, cast -- all these get the variable by value, and return it... 
// and for that reason, need to updated
struct UnsignedInt_Type0 UnsignedInt_Type0_Add_UnsignedInt_Type0_i(struct UnsignedInt_Type0 str,int val) {

	str.ptr += val;

	create_entry(reinterpret_cast<unsigned long long>(&str.ptr), str.addr); 

	str.addr = reinterpret_cast<unsigned long long>(&str.ptr);

	return str;
}

	

struct UnsignedInt_Type0 UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0* str1,struct UnsignedInt_Type0 str2) {

	str1->ptr = str2.ptr;	
	// Copy contents over from str2.addr to str1->addr
	create_entry(str1->addr, str2.addr);

	// Return str1 by value
	return *str1;
}


struct UnsignedInt_Type0 UnsignedInt_Type0_Cast_i_ValueCast(int val) {

	struct UnsignedInt_Type0 str;
	str.ptr = reinterpret_cast<unsigned int*>(val);
	str.addr = reinterpret_cast<unsigned long long>(&str.ptr);

	create_entry_blank(str.addr, val);

	return str; 
}


struct UnsignedInt_Type0 UnsignedInt_Type0_Cast_Void_Type4(struct Void_Type4 str_void) {


	struct UnsignedInt_Type0 str_int;
	str_int.ptr = (unsigned int*)str_void.ptr;
	// Assign str_int.addr = &str_int.ptr ... ?
	create_entry(str_int.addr, str_void.addr);

	return str_int;
}


struct Void_Type4 malloc_overload(int size) {

	struct Void_Type4 str_void;
	str_void.ptr = malloc(size);
	str_void.addr = reinterpret_cast<unsigned long long>(&str_void.ptr);

	create_entry(str_void.addr, reinterpret_cast<unsigned long long>(str_void.ptr), size);

	return str_void;

}


unsigned int *Deref(struct UnsignedInt_Type0 str) {
	
	return str.ptr;
}

struct UnsignedInt_Type0 Assign(struct UnsignedInt_Type0 *str1,struct UnsignedInt_Type0 str2) {
	
	str1->ptr = str2.ptr;
	create_entry(str1->addr, str2.addr);

	return *str1;
}

struct Char_Type3 Char_Type3_Cast_UnsignedInt_Type0(struct UnsignedInt_Type0 str) {

	struct Char_Type3 str_char;
	str_char.ptr = (char*)str.ptr;

	create_entry(str_char.addr, str.addr);

	return str_char;
}

struct Double_Type2 Double_Type2_Cast_UnsignedInt_Type0(struct UnsignedInt_Type0 str) {
	
	struct Double_Type2 str_double;
	str_double.ptr = (double*)str.ptr;

	create_entry(str_double.addr, str.addr);

	return str_double;
}
	

struct Float_Type1 Float_Type1_Cast_UnsignedInt_Type0(struct UnsignedInt_Type0 str) {
	
	struct Float_Type1 str_float;
	str_float.ptr = (float*)str.ptr;

	create_entry(str_float.addr, str.addr);

	return str_float;

	
}
