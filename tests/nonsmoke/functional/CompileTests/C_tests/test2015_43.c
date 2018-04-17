struct process_descriptor 
   {
     void ( * step ) ( void *object );
   };

struct numeric_resolv 
   {
     int rc;
   };

static void numeric_step ( struct numeric_resolv *numeric ) { }
// static void numeric_step ( void *numeric ) { }

static struct process_descriptor numeric_process_desc =
   {
  // .step = ( ( ( ( typeof ( numeric_step ) * ) ((void *)0) ) == ( ( void ( * ) ( struct numeric_resolv *object ) ) ((void *)0) ) ) ? ( void ( * ) ( void *object ) ) numeric_step : ( void ( * ) ( void *object ) ) numeric_step ), 

  // Unparses as:
  // .step = (((void *(struct numeric_resolv *))((void *)0)) == 0L?0L : 0L)
     .step = ( ( ( ( typeof ( numeric_step ) * ) ((void *)0) ) == 0L) ? 0L : 0L ), 
   };
