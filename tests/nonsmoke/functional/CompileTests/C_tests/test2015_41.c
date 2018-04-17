struct interface_operation 
   {
     void *func;
   };

int foobar ()
   {
     return 0;
   }

// Unparses as:
// struct interface_operation xyz = {.func = (0L == ((typeof(int  ) *() ) ((void *)0)) ? foobar : foobar ) };

// The problem is that both parts of the function type need to be unparsed as the typeof operand.
// Should unparses as:
// struct interface_operation xyz = {.func = (0L == ((typeof(int () ) * ) ((void *)0)) ? foobar : foobar ) };

struct interface_operation    xyz = { .func = ( ( 0L == ( ( typeof ( int () ) * ) ((void *)0) ) ) ? foobar : foobar ) };
