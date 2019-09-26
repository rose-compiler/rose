
typedef struct 
   {
     struct hash_table *fts_leaf_optimization_works_ht;
     union 
        {
          struct hash_table *ht;
        } fts_cycle;
   } FTS;

struct hash_table;

typedef struct hash_table Hash_table;

