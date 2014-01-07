
enum Y
   {
     a,
     b
   };

struct X
   {
     int bufsize;

     enum Y (*foobar)(void);
   };

