#ifndef _METALIB_ALT_H
#define _METALIB_ALT_H 1

#include <stdint.h>

#ifdef NO_MANGLE
#ifdef __cplusplus
extern "C" {
#endif
#endif

#include "rtc-defines.h"

struct __Pb__v__Pe___Type
{
  void *ptr;
  unsigned long long addr;
};

uint64_t find_lock(unsigned long long base);
uint64_t find_key(uint64_t lock);
void create_blank_entry(unsigned long long addr);
bool isValidEntry(unsigned long long addr);
void spatial_check(unsigned long long ptr, unsigned long long lower, unsigned long long upper);
void temporal_check(uint64_t lock, uint64_t key);
void remove_entry_from_addr(unsigned long long addr);
void remove_entry(struct __Pb__v__Pe___Type input);
void create_entry_with_new_lock(unsigned long long addr, unsigned long long base, unsigned long size);
void lower_bound_check(unsigned long long ptr, unsigned long long addr);
void metadata_free_overload(unsigned long long ptr, unsigned long long addr);
void metadata_realloc_overload_2(unsigned long long ptr, unsigned long long addr, unsigned long size);
void metadata_realloc_overload_1(unsigned long long ptr, unsigned long addr);
void metadata_create_entry_if_src_exists(unsigned long long dest, unsigned long long src);
void metadata_malloc_overload(unsigned long long addr, unsigned long long base, unsigned long size);
void metadata_check_entry(unsigned long long ptr, unsigned long long addr);
void metadata_array_bound_check_using_lookup(unsigned long long addr, unsigned long long index);
#if INT_ARRAY_INDEX
void metadata_array_bound_check(unsigned int size, int index);
#else
void metadata_array_bound_check(unsigned int size, unsigned int index);
#endif /* INT_ARRAY_INDEX */
void metadata_create_entry_3(unsigned long long addr, unsigned long long base, unsigned long size);
void metadata_create_entry_4(unsigned long long addr, unsigned long long base, unsigned long size, unsigned long long lock);
void metadata_create_entry_dest_src(unsigned long long dest, unsigned long long src);
void metadata_create_dummy_entry(unsigned long long addr);
void metadata_execAtLast();
void metadata_checkInitInfo(long long lock, unsigned long long addr);
void metadata_updateInitInfo(long long lock, unsigned long long addr);
void metadata_ExitScope();
void metadata_EnterScope();
void metadata_execAtFirst();

void metadata_push_to_stack(unsigned long long addr);
unsigned long long metadata_get_from_stack(unsigned int index);
unsigned long long metadata_pop_from_stack(unsigned int evict);

#ifdef NO_MANGLE
#ifdef __cplusplus
}
#endif
#endif

#endif /* _METALIB_ALT_H */
