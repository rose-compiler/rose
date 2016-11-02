// extern "C" {

typedef struct list_tag 
   {
     int a;
     struct list_tag * next;
     list_tag * prev;
   } * list;

list a;

// }

#if 0
typedef struct mylist {
     int a;
  // struct list * next;
     mylist * prev;
   } * mylist;
#endif

