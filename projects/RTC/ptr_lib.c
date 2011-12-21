#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef PTR_HDR_H
#define PTR_HDR_H
#include <assert.h>

#if 1
struct VoidStruct malloc_wrap(unsigned int size);

struct VoidStruct 
{
  void *ptr2obj;
  void *L;
  void *H;
}
;

struct CharStruct 
{
  char *ptr2obj;
  char *L;
  char *H;
}
;

struct DoubleStruct 
{
  double *ptr2obj;
  double *L;
  double *H;
}
;

struct FloatStruct 
{
  float *ptr2obj;
  float *L;
  float *H;
}
;

struct UIntStruct 
{
  unsigned int *ptr2obj;
  unsigned int *L;
  unsigned int *H;
}
;
#endif

struct VoidStruct malloc_wrap(unsigned int size) {
	
	struct VoidStruct ptr;
	ptr.ptr2obj = malloc(size);
	ptr.L = (ptr.ptr2obj);
	ptr.H = (ptr.L + size);

	return ptr;
}

unsigned int* UInt_Deref(struct UIntStruct input) {
	return input.ptr2obj;
}

struct UIntStruct UInt_Void_Cast(struct VoidStruct input) {
	struct UIntStruct output;
	output.ptr2obj = (unsigned int*)input.ptr2obj;
	output.L = (unsigned int*)input.L;
	output.H = (unsigned int*)input.H;
	return output;
}


struct FloatStruct Float_UInt_Cast(struct UIntStruct input) {
	struct FloatStruct output;
	output.ptr2obj = (float*)input.ptr2obj;
	output.L = (float*)input.L;
	output.H = (float*)input.H;
	return output;
}

struct DoubleStruct Double_UInt_Cast(struct UIntStruct input) {
	struct DoubleStruct output;
	output.ptr2obj = (double*)input.ptr2obj;
	output.L = (double*)input.L;
	output.H = (double*)input.H;
	return output;
}

struct CharStruct Char_UInt_Cast(struct UIntStruct input) {
	struct CharStruct output;
	output.ptr2obj = (char*)input.ptr2obj;
	output.L = (char*)input.L;
	output.H = (char*)input.H;
	return output;
}

struct UIntStruct UInt_Int_SpecialCast(int val) {
	struct UIntStruct output;
	output.ptr2obj = (unsigned int*)val;
	output.L = (unsigned int*)val;
	output.H = (unsigned int*)val;
	return output;
}

struct UIntStruct UInt_Int_Normal_Add(struct UIntStruct input, int val) {
	input.ptr2obj += val;
	return input;
}

bool UInt_UInt_Struct_LessThan(struct UIntStruct lhs, struct UIntStruct rhs) {
	return (lhs.ptr2obj < rhs.ptr2obj);
}

bool UInt_UInt_Struct_NotEqual(struct UIntStruct lhs, struct UIntStruct rhs) {
	return (lhs.ptr2obj != rhs.ptr2obj);
}

bool UInt_UInt_Struct_Equality(struct UIntStruct lhs, struct UIntStruct rhs) {
	return (lhs.ptr2obj == rhs.ptr2obj);
}



unsigned int Deref_Overload(struct IntStruct ptr) {
	return *ptr.ptr2obj;
}

struct IntStruct Cast_Overload(struct IntStruct ptr) {
	return ptr;
}

bool Equality(struct IntStruct ptr,struct IntStruct other_ptr) {
	return (ptr.ptr2obj == other_ptr.ptr2obj);
}

bool NotEqual(struct IntStruct ptr,struct IntStruct other_ptr) {
	return !Equality(ptr,other_ptr);
}	

bool LessThan(struct IntStruct ptr,struct IntStruct other_ptr) {
	return ptr.ptr2obj < other_ptr.ptr2obj;
}

bool GreaterThan(struct IntStruct ptr,struct IntStruct other_ptr) {
	return ptr.ptr2obj > other_ptr.ptr2obj;
}

bool GreaterOrEqual(struct IntStruct ptr,struct IntStruct other_ptr) {
	return GreaterThan(ptr, other_ptr) || Equality(ptr, other_ptr);
}

bool LessOrEqual(struct IntStruct ptr,struct IntStruct other_ptr) {
	return LessThan(ptr, other_ptr) || Equality(ptr, other_ptr);
}

#if 0
struct IntStruct Div(struct IntStruct ptr,unsigned int val) {
	ptr.ptr2obj /= val;
	return ptr;
}

struct IntStruct Mult(struct IntStruct ptr,unsigned int val) {
	ptr.ptr2obj *= val;
	return ptr;
}
#endif

struct IntStruct Sub(struct IntStruct ptr,unsigned int val) {
	ptr.ptr2obj -= val;
	return ptr;
}

struct IntStruct Add(struct IntStruct ptr,unsigned int val) {
	ptr.ptr2obj += val;
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
