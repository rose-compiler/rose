struct ngx_list_part_s;

typedef struct ngx_list_part_s ngx_list_part_t;

struct ngx_list_part_s 
   {
     void *elts;
   };

typedef struct 
   {
     ngx_list_part_t part;
   } ngx_list_t;

void ngx_list_init(ngx_list_t *list)
   {
     list->part.elts = 0L;
   }
