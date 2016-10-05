

#if 0

// Original code:
struct _IO_FILE;

struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;
};

struct _IO_FILE {
  int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */
  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;	/* Current read pointer */
  char* _IO_read_end;	/* End of get area. */
  char* _IO_read_base;	/* Start of putback+get area. */
  char* _IO_write_base;	/* Start of put area. */
  char* _IO_write_ptr;	/* Current put pointer. */
  char* _IO_write_end;	/* End of put area. */
  char* _IO_buf_base;	/* Start of reserve area. */
  char* _IO_buf_end;	/* End of reserve area. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
  int _blksize;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
};

// Generated code
struct _IO_FILE ; 

struct _IO_marker
   {
     struct _IO_marker * _next;
     struct _IO_FILE ;
         * _sbuf;  /* <------------ ERROR */
     int _pos;
   }; 

struct _IO_FILE
   {
     int _flags;
     char * _IO_read_ptr;
     char * _IO_read_end;
     char * _IO_read_base;
     char * _IO_write_base;
     char * _IO_write_ptr;
     char * _IO_write_end;
     char * _IO_buf_base;
     char * _IO_buf_end;
     char * _IO_save_base;
     char * _IO_backup_base;
     char * _IO_save_end;
     struct _IO_marker * _markers;
     struct _IO_FILE * _chain;
     int _fileno;
     int _blksize;
     __off_t _old_offset;
     unsigned short _cur_column;
     signed char _vtable_offset;
     char _shortbuf[1];
     _IO_lock_t * _lock;
     __off64_t _offset;
     void * __pad1;
     void * __pad2;
     int _mode;
     char _unused2[52];
   }; 
#endif



// Original code:
struct _IO_FILE;

struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;
};

struct _IO_FILE {
  int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */
  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;	/* Current read pointer */
  char* _IO_read_end;	/* End of get area. */
  char* _IO_read_base;	/* Start of putback+get area. */
  char* _IO_write_base;	/* Start of put area. */
  char* _IO_write_ptr;	/* Current put pointer. */
  char* _IO_write_end;	/* End of put area. */
  char* _IO_buf_base;	/* Start of reserve area. */
  char* _IO_buf_end;	/* End of reserve area. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
  int _blksize;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
};


