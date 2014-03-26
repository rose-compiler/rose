typedef struct ebml_master 
   {
     int   pos;
     int   sizebytes;
   } ebml_master;

static ebml_master start_ebml_master()
   {
     return (ebml_master){ 1, 2 };
   }
