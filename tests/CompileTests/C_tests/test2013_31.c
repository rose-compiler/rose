struct H
   {
     int xyz;
   };

union X
   {
     struct H a;
     char* b;
   } __attribute__ ((__transparent_union__));

typedef union
   {
     struct H a;
     char* b;
   } Y __attribute__ ((__transparent_union__));

struct G 
   {
     int fd;
     struct H sockaddr;
     int socklen;
   };


void foo1 (union X x);
void foo2 (Y y);

void foobar()
   {
     struct G g;

     union X x = {.a = 1};
     foo1(x);
//   foo1(g.sockaddr);

     Y y = { .a = 2 };
     foo2(y);
   }



