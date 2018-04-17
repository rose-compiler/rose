/* Type definitions for a the hash tables used in pgs-lib */

/* $Id: pgshash.h,v 1.1 2004/07/07 10:27:16 dquinlan Exp $ */

#define HASH_INCLUDE_FILE 1

#define HASH_EXPANSION_RATE 2
#define HASH_INITIAL_SIZE   512
#define HASH_MINIMUM_ITEM -1

/*typedef struct itemlink *itemlink_ptr;*/

typedef struct itemlink_t{
  int              items;
  int              redirect;
  struct itemlink_t *links; 
}itemlink;


/*typedef struct itemlinkchunk *itemlinkchunk_ptr;*/

typedef struct itemlinkchunk_t {
  itemlink      *itemlinkbuffer;
  int                  size;
  struct itemlinkchunk_t *nextitemlinkchunk;
}itemlinkchunk;

typedef struct{
  int           totalslots;
  int           nentries;
  itemlinkchunk *firstitemlinkchunk;
  itemlink      *newslot;
} HASH_TABLE_ENTRY;

/* HASH_TABLE_ENTRY points to a chunk of itemlinks.  Each chunk of itemlinks
   points to a buffer of itemlinks.  It also points to the next chunk of 
   itemlinks.  itemlinks are allocated chunks at a time.  
*/

/* Function declarations for functions in hash.c */
void add_item_to_hash_table(HASH_TABLE_ENTRY *, int *, int *);
void initialize_item_hash_table(HASH_TABLE_ENTRY **, int);
int  hash_count_items_in_table(HASH_TABLE_ENTRY *, int);
void move_items_from_hash(HASH_TABLE_ENTRY, int, int, int *);	

/*   int  get_redirection(HASH_TABLE_ENTRY *, int *, int *, GS_EN_STRUCT **);
   void free_hash_table(HASH_TABLE_ENTRY *, int);
*/
