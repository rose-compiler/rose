
// DQ: Added typedefs.
typedef int my_bool;
typedef unsigned long long ulonglong;
typedef unsigned char uchar;
typedef int File;
typedef ulonglong my_off_t;

typedef unsigned long int ulong;
typedef unsigned int uint;

typedef long unsigned int size_t;


struct st_block_link;
typedef struct st_block_link BLOCK_LINK;
struct st_hash_link;
typedef struct st_hash_link HASH_LINK;


typedef struct st_keycache_thread_var
{
  //  mysql_cond_t suspend;
  struct st_keycache_thread_var *next,**prev;
  void *opt_info;
} st_keycache_thread_var;


typedef struct st_keycache_wqueue
{
  st_keycache_thread_var *last_thread;
} KEYCACHE_WQUEUE;
// # 53 "/home/quinlan1/ROSE/APPLICATION_TESTS/rose-sh/workspace/mysql/phase_1/mysql-5.7.17/include/keycache.h"
typedef struct st_key_cache
{
  my_bool key_cache_inited;
  my_bool in_resize;
  my_bool resize_in_flush;
  my_bool can_be_used;
  size_t key_cache_mem_size;
  uint key_cache_block_size;
  ulonglong min_warm_blocks;
  ulonglong age_threshold;
  ulonglong keycache_time;
  uint hash_entries;
  int hash_links;
  int hash_links_used;
  int disk_blocks;
  ulong blocks_used;
  ulong blocks_unused;
  ulong blocks_changed;
  ulong warm_blocks;
  ulong cnt_for_resize_op;
  long blocks_available;
  HASH_LINK **hash_root;
  HASH_LINK *hash_link_root;
  HASH_LINK *free_hash_list;
  BLOCK_LINK *free_block_list;
  BLOCK_LINK *block_root;
  uchar *block_mem;
  BLOCK_LINK *used_last;
  BLOCK_LINK *used_ins;
  //  mysql_mutex_t cache_lock;
  KEYCACHE_WQUEUE resize_queue;

  KEYCACHE_WQUEUE waiting_for_resize_cnt;
  KEYCACHE_WQUEUE waiting_for_hash_link;
  KEYCACHE_WQUEUE waiting_for_block;
  BLOCK_LINK *changed_blocks[128];
  BLOCK_LINK *file_blocks[128];

  ulonglong param_buff_size;
  ulonglong param_block_size;
  ulonglong param_division_limit;
  ulonglong param_age_threshold;

  ulong global_blocks_changed;
  ulonglong global_cache_w_requests;
  ulonglong global_cache_write;
  ulonglong global_cache_r_requests;
  ulonglong global_cache_read;

  int blocks;
  my_bool in_init;
} KEY_CACHE;


// int __builtin_classify_type(...) __attribute__((nothrow, const));
// int __builtin_constant_p(...) __attribute__((nothrow, const));
int __builtin_classify_type(char*);
int __builtin_constant_p(unsigned long long);

extern char * my_filename(File fd);


__extension__ extern unsigned long long __sdt_unsp;

// # 2274 "test2017_04.c"
uchar *key_cache_read(KEY_CACHE *keycache,
                      st_keycache_thread_var *thread_var,
                      File file, my_off_t filepos, int level,
                      uchar *buff, uint length,
                      uint block_length __attribute__((unused)),
                      int return_buffer __attribute__((unused)))
{
  my_bool locked_and_incremented= (0);
  int error=0;
  uchar *start= buff;
  ;
  do { } while(0)
                                                     ;

  if (keycache->key_cache_inited)
  {

    BLOCK_LINK *block;
    uint read_length;
    uint offset;
    int page_st;

 // if (__builtin_expect (mysql_keycache__read__start_semaphore, 0))
    if (1)
    {
      do { 

#if 0
           __asm__ __volatile__ ("990: nop" "\n" ".pushsection .note.stapsdt" "," "\"?\"" "," "\"note\"" "\n" ".balign 4" "\n" ".4byte 992f-991f" "," "994f-993f" "," "3" "\n" "991: .asciz \"stapsdt\"" "\n" "992: .balign 4" "\n" "993: .8byte 990b" "\n" ".8byte _.stapsdt.base" "\n" ".8byte mysql_keycache__read__start_semaphore" "\n" ".asciz \"mysql\"" "\n" ".asciz \"keycache__read__start\"" "\n" ".asciz \"%n[_SDT_S1]@%[_SDT_A1] %n[_SDT_S2]@%[_SDT_A2] %n[_SDT_S3]@%[_SDT_A3] %n[_SDT_S4]@%[_SDT_A4]\"" "\n" "994: .balign 4" "\n" ".popsection" "\n" :: [_SDT_S1] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, (my_filename(file)), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, (my_filename(file)), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, (my_filename(file)), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type (my_filename(file)) == 14 || __builtin_classify_type (my_filename(file)) == 5) ? sizeof (void *) : sizeof (my_filename(file)))), [_SDT_A1] "nor" ((my_filename(file))), [_SDT_S2] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (length) + 3) & -4) == 4, (length), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type (length) + 3) & -4) == 4, (length), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type (length) + 3) & -4) == 4, (length), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type (length) == 14 || __builtin_classify_type (length) == 5) ? sizeof (void *) : sizeof (length))), [_SDT_A2] "nor" ((length)), [_SDT_S3] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) == 14 || __builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) == 5) ? sizeof (void *) : sizeof ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)))), [_SDT_A3] "nor" (((ulong) (keycache->blocks_used * keycache->key_cache_block_size))), [_SDT_S4] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) == 14 || __builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) == 5) ? sizeof (void *) : sizeof ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)))), [_SDT_A4] "nor" (((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)))); 

#else

           __asm__ __volatile__ ("990: nop" "\n" ".pushsection .note.stapsdt" "," "\"?\"" "," "\"note\"" "\n" ".balign 4" "\n" ".4byte 992f-991f" "," "994f-993f" "," "3" "\n" "991: .asciz \"stapsdt\"" "\n" "992: .balign 4" "\n" "993: .8byte 990b" "\n" ".8byte _.stapsdt.base" "\n" ".8byte mysql_keycache__read__start_semaphore" "\n" ".asciz \"mysql\"" "\n" ".asciz \"keycache__read__start\"" "\n" ".asciz \"%n[_SDT_S1]@%[_SDT_A1] %n[_SDT_S2]@%[_SDT_A2] %n[_SDT_S3]@%[_SDT_A3] %n[_SDT_S4]@%[_SDT_A4]\"" "\n" "994: .balign 4" "\n" ".popsection" "\n" :: 

#if 0

[_SDT_S1] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, (my_filename(file)), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, (my_filename(file)), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, (my_filename(file)), 0U))) 0)) ? 1 : -1) /* * (int) ((__builtin_classify_type (my_filename(file)) == 14 || __builtin_classify_type (my_filename(file)) == 5) ? sizeof (void *) : sizeof (my_filename(file))) */ ), 

#else

// [_SDT_S1] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, 0U))) __sdt_unsp) ) == 0) )) ? 1 : -1) ), 
// [_SDT_S1] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, 0U))) __sdt_unsp) ) == 0) )) ? 1 : -1) ), 


[_SDT_S1] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (my_filename(file)) + 3) & -4) == 4, 0U))) __sdt_unsp) ) == 0) )) ? 1 : -1) ), 

#endif

[_SDT_A1] "nor" ((my_filename(file))), 

[_SDT_S2] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type (length) + 3) & -4) == 4, (length), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type (length) + 3) & -4) == 4, (length), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type (length) + 3) & -4) == 4, (length), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type (length) == 14 || __builtin_classify_type (length) == 5) ? sizeof (void *) : sizeof (length))), [_SDT_A2] "nor" ((length)), 

[_SDT_S3] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) == 14 || __builtin_classify_type ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)) == 5) ? sizeof (void *) : sizeof ((ulong) (keycache->blocks_used * keycache->key_cache_block_size)))), [_SDT_A3] "nor" (((ulong) (keycache->blocks_used * keycache->key_cache_block_size))), 

[_SDT_S4] "n" (((!__extension__ (__builtin_constant_p ((((unsigned long long) (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)), 0U))) __sdt_unsp) & ((unsigned long long)1 << (sizeof (unsigned long long) * 8 - 1))) == 0) || (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)), 0U))) -1 > (__typeof (__builtin_choose_expr (((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) + 3) & -4) == 4, ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)), 0U))) 0)) ? 1 : -1) * (int) ((__builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) == 14 || __builtin_classify_type ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)) == 5) ? sizeof (void *) : sizeof ((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)))), 

[_SDT_A4] "nor" (((ulong) (keycache->blocks_unused * keycache->key_cache_block_size)))); 
#endif


  // __asm__ __volatile__ (".ifndef _.stapsdt.base" "\n" ".pushsection .stapsdt.base" "," "\"aG\"" "," "\"progbits\"" "," ".stapsdt.base" "," "comdat" "\n" ".weak _.stapsdt.base" "\n" ".hidden _.stapsdt.base" "\n" "_.stapsdt.base: .space 1" "\n" ".size _.stapsdt.base" "," "1" "\n" ".popsection" "\n" ".endif" "\n"); 

} while (0)



                                                                         ;
    }

  }

no_key_cache:

end:
}



