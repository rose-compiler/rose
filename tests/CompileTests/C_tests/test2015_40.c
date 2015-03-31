
struct interface_operation 
   {
     void *func;
   };

int foobar ()
   {
     return 0;
   }

static struct interface_operation downloader_xfer_operations[] = {
   { .func = ( ( ( ( typeof ( foobar ) * ) ((void *)0) ) == ( ( typeof ( int () ) * ) ((void *)0) ) ) ? foobar : foobar ), }
};
