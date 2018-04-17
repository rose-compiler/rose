struct _IO_FILE;
// struct _IO_FILE {};

struct _IO_FILE *_sbuf_alt;

struct _IO_FILE
   {
     int _flags;
     struct _IO_FILE *_chain_alt;

     int _fileno;
   };

typedef struct _IO_FILE _IO_FILE;

