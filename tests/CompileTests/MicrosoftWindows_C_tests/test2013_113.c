/* #define PACKED __attribute__ ((__packed__)) */

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

static int check_header_gzip()
   {
     union 
        {
          unsigned char raw[8];
          struct
             {
               uint8_t gz_method;
               uint8_t flags;
               uint32_t mtime;
               uint8_t xtra_flags_UNUSED;
               uint8_t os_flags_UNUSED;
             } formatted;
        } header;

     struct BUG_header 
        {
       /* MSVC does not have same size for the struct as GNU, so this causes MSVC to have a negative array size, which is not allowed.
          char BUG_header[sizeof(header) == 8 ? 1 : -1]; 
        */
          int x;
        };
   }
