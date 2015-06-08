struct numeric_resolv 
   {
     int rc;
   };

static void numeric_step ( struct numeric_resolv *numeric ) { }
// static void numeric_step ( void *numeric ) { }

void ( * step ) ( void *object ) = ( ( ( ( typeof ( numeric_step ) * ) ((void *)0) ) == 0L) ? 0L : 0L );
