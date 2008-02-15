
#if 0
typedef unsigned int wint_t;

/* typedef struct */
typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;		/* Value so far.  */
} __mbstate_t;

// problem statements
typedef struct { } __quad_t; 
typedef struct { } __u_quad_t; 
typedef struct { } __fsid_t; 
typedef struct {} pthread_cond_t; 
typedef struct {} pthread_condattr_t; 
typedef struct {} pthread_mutex_t; 
typedef struct {} pthread_mutexattr_t; 
typedef struct {} __mbstate_t; 
typedef struct {} _G_fpos_t; 
typedef struct {} _G_fpos64_t; 

typedef union {} _G_iconv_t; 

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;

struct __gconv_step
{
  struct __gconv_loaded_object *__shlib_handle;
  __const char *__modname;

  int __counter;

  char *__from_name;
  char *__to_name;

  __gconv_fct __fct;
  __gconv_init_fct __init_fct;
  __gconv_end_fct __end_fct;

  /* Information about the number of bytes needed or produced in this
     step.  This helps optimizing the buffer sizes.  */
  int __min_needed_from;
  int __max_needed_from;
  int __min_needed_to;
  int __max_needed_to;

  /* Flag whether this is a stateful encoding or not.  */
  int __stateful;

  void *__data;		/* Pointer to step-local data.  */
};

struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;
};

// struct _IO_marker {struct _IO_marker * _next;struct _IO_FILE ;* _sbuf;int _pos;}; 

#else

typedef unsigned int size_t; 
typedef unsigned char __u_char; 
typedef unsigned short __u_short; 
typedef unsigned int __u_int; 
typedef unsigned long __u_long; 
typedef struct { } __quad_t; 
typedef struct { } __u_quad_t; 
typedef signed char __int8_t; 
typedef unsigned char __uint8_t; 
typedef short __int16_t; 
typedef unsigned short __uint16_t; 
typedef int __int32_t; 
typedef unsigned int __uint32_t; 
typedef __quad_t * __qaddr_t; 
typedef __u_quad_t __dev_t; 
typedef __u_int __uid_t; 
typedef __u_int __gid_t; 
typedef __u_long __ino_t; 
typedef __u_int __mode_t; 
typedef __u_int __nlink_t; 
typedef long __off_t; 
typedef __quad_t __loff_t; 
typedef int __pid_t; 
typedef int __ssize_t; 
typedef __u_long __rlim_t; 
typedef __u_quad_t __rlim64_t; 
typedef __u_int __id_t; 
typedef struct { } __fsid_t; 
typedef int __daddr_t; 
typedef char * __caddr_t; 
typedef long __time_t; 
typedef unsigned int __useconds_t; 
typedef long __suseconds_t; 
typedef long __swblk_t; 
typedef long __clock_t; 
typedef int __clockid_t; 
typedef int __timer_t; 
typedef int __key_t; 
typedef unsigned short __ipc_pid_t; 
typedef long __blksize_t; 
typedef long __blkcnt_t; 
typedef __quad_t __blkcnt64_t; 
typedef __u_long __fsblkcnt_t; 
typedef __u_quad_t __fsblkcnt64_t; 
typedef __u_long __fsfilcnt_t; 
typedef __u_quad_t __fsfilcnt64_t; 
typedef __u_quad_t __ino64_t; 
typedef __loff_t __off64_t; 
typedef long __t_scalar_t; 
typedef unsigned long __t_uscalar_t; 
typedef int __intptr_t; 
typedef unsigned int __socklen_t; 
struct __sched_param {int __sched_priority;}; 
struct _pthread_fastlock {long __status;int __spinlock;}; 
typedef struct _pthread_descr_struct * _pthread_descr; 
typedef struct __pthread_attr_s pthread_attr_t; 
typedef struct {} pthread_cond_t; 
typedef struct {} pthread_condattr_t; 
typedef unsigned int pthread_key_t; 
typedef struct {} pthread_mutex_t; 
typedef struct {} pthread_mutexattr_t; 
typedef int pthread_once_t; 
typedef unsigned long pthread_t; 
typedef struct _IO_FILE FILE; 
typedef struct _IO_FILE __FILE; 
typedef unsigned int wint_t; 
typedef struct {} __mbstate_t; 
typedef struct {} _G_fpos_t; 
typedef struct {} _G_fpos64_t; 
enum __T138910968 {__GCONV_OK,__GCONV_NOCONV,__GCONV_NODB,__GCONV_NOMEM,__GCONV_EMPTY_INPUT,__GCONV_FULL_OUTPUT,__GCONV_ILLEGAL_INPUT,__GCONV_INCOMPLETE_INPUT,__GCONV_ILLEGAL_DESCRIPTOR,__GCONV_INTERNAL_ERROR}; 
enum __T138912332 {__GCONV_IS_LAST=(1),__GCONV_IGNORE_ERRORS}; 
struct __gconv_step ; 
struct __gconv_step_data ; 
struct __gconv_loaded_object ; 
struct __gconv_trans_data ; 
typedef int (* __gconv_fct)(struct __gconv_step * , struct __gconv_step_data * , const unsigned char * * , const unsigned char * , unsigned char * * , size_t * , int , int ); 
typedef int (* __gconv_init_fct)(struct __gconv_step * ); 
typedef void (* __gconv_end_fct)(struct __gconv_step * ); 
typedef int (* __gconv_trans_fct)(struct __gconv_step * , struct __gconv_step_data * , void * , const unsigned char * , const unsigned char * * , const unsigned char * , unsigned char * * , size_t * ); 
typedef int (* __gconv_trans_context_fct)(void * , const unsigned char * , const unsigned char * , unsigned char * , unsigned char * ); 
typedef int (* __gconv_trans_query_fct)(const char * , const char * * * , size_t * ); 
typedef int (* __gconv_trans_init_fct)(void * * , const char * ); 
typedef void (* __gconv_trans_end_fct)(void * ); 
struct __gconv_trans_data {__gconv_trans_fct __trans_fct;__gconv_trans_context_fct __trans_context_fct;__gconv_trans_end_fct __trans_end_fct;void * __data;struct __gconv_trans_data * __next;}; 
struct __gconv_step {struct __gconv_loaded_object ;* __shlib_handle;const char * __modname;int __counter;char * __from_name;char * __to_name;__gconv_fct __fct;__gconv_init_fct __init_fct;__gconv_end_fct __end_fct;int __min_needed_from;int __max_needed_from;int __min_needed_to;int __max_needed_to;int __stateful;void * __data;}; 
struct __gconv_step_data {unsigned char * __outbuf;unsigned char * __outbufend;int __flags;int __invocation_counter;int __internal_use;__mbstate_t * __statep;__mbstate_t __state;struct __gconv_trans_data * __trans;}; 
typedef struct __gconv_info * __gconv_t; 
typedef union {} _G_iconv_t; 
typedef int _G_int16_t; 
typedef int _G_int32_t; 
typedef unsigned int _G_uint16_t; 
typedef unsigned int _G_uint32_t; 
typedef void * __gnuc_va_list; 
struct _IO_jump_t ; 
struct _IO_FILE ; 
typedef void _IO_lock_t; 
struct _IO_marker {struct _IO_marker * _next;struct _IO_FILE ;* _sbuf;int _pos;}; 
enum __codecvt_result {__codecvt_ok,__codecvt_partial,__codecvt_error,__codecvt_noconv}; 
struct _IO_FILE {int _flags;char * _IO_read_ptr;char * _IO_read_end;char * _IO_read_base;char * _IO_write_base;char * _IO_write_ptr;char * _IO_write_end;char * _IO_buf_base;char * _IO_buf_end;char * _IO_save_base;char * _IO_backup_base;char * _IO_save_end;struct _IO_marker * _markers;struct _IO_FILE * _chain;int _fileno;int _blksize;__off_t _old_offset;unsigned short _cur_column;signed char _vtable_offset;char _shortbuf[1];_IO_lock_t * _lock;__off64_t _offset;void * __pad1;void * __pad2;int _mode;char _unused2[52];}; 
struct _IO_FILE_plus ; 
extern "C" struct _IO_FILE_plus _IO_2_1_stdin_; 
extern "C" struct _IO_FILE_plus _IO_2_1_stdout_; 
extern "C" struct _IO_FILE_plus _IO_2_1_stderr_; 
typedef __ssize_t __io_read_fn; 
typedef __ssize_t __io_write_fn; 
typedef int __io_seek_fn; 
typedef int __io_close_fn; 
extern "C" {int __underflow(struct _IO_FILE * );} 
extern "C" {int __uflow(struct _IO_FILE * );} 
extern "C" {int __overflow(struct _IO_FILE * ,int );} 
extern "C" {wint_t __wunderflow(struct _IO_FILE * );} 
extern "C" {wint_t __wuflow(struct _IO_FILE * );} 
extern "C" {wint_t __woverflow(struct _IO_FILE * ,wint_t );} 
extern "C" {int _IO_getc(struct _IO_FILE * __fp);} 
extern "C" {int _IO_putc(int __c,struct _IO_FILE * __fp);} 
extern "C" {int _IO_feof(struct _IO_FILE * __fp);} 
extern "C" {int _IO_ferror(struct _IO_FILE * __fp);} 
extern "C" {int _IO_peekc_locked(struct _IO_FILE * __fp);} 
extern "C" {void _IO_flockfile(struct _IO_FILE * );} 
extern "C" {void _IO_funlockfile(struct _IO_FILE * );} 
extern "C" {int _IO_ftrylockfile(struct _IO_FILE * );} 
extern "C" {int _IO_vfscanf(struct _IO_FILE * ,const char * ,__gnuc_va_list ,int * );} 
extern "C" {int _IO_vfprintf(struct _IO_FILE * ,const char * ,__gnuc_va_list );} 
extern "C" {__ssize_t _IO_padn(struct _IO_FILE * ,int ,__ssize_t );} 
extern "C" {size_t _IO_sgetn(struct _IO_FILE * ,void * ,size_t );} 
extern "C" {__off64_t _IO_seekoff(struct _IO_FILE * ,__off64_t ,int ,int );} 
extern "C" {__off64_t _IO_seekpos(struct _IO_FILE * ,__off64_t ,int );} 
extern "C" {void _IO_free_backup_area(struct _IO_FILE * );} 
typedef _G_fpos_t fpos_t; 
extern "C" FILE * stdin; 
extern "C" FILE * stdout; 
extern "C" FILE * stderr; 
extern "C" {int remove(const char * __filename);} 
extern "C" {int rename(const char * __old,const char * __new);} 
extern "C" {FILE * tmpfile();} 
extern "C" {char * tmpnam(char * __s);} 
extern "C" {char * tmpnam_r(char * __s);} 
extern "C" {char * tempnam(const char * __dir,const char * __pfx);} 
extern "C" {int fclose(FILE * __stream);} 
extern "C" {int fflush(FILE * __stream);} 
extern "C" {int fflush_unlocked(FILE * __stream);} 
extern "C" {FILE * fopen(const char * __filename,const char * __modes);} 
extern "C" {FILE * freopen(const char * __filename,const char * __modes,FILE * __stream);} 
extern "C" {FILE * fdopen(int __fd,const char * __modes);} 
extern "C" {void setbuf(FILE * __stream,char * __buf);} 
extern "C" {int setvbuf(FILE * __stream,char * __buf,int __modes,size_t __n);} 
extern "C" {void setbuffer(FILE * __stream,char * __buf,size_t __size);} 
extern "C" {void setlinebuf(FILE * __stream);} 
extern "C" {int fprintf(FILE * __stream,const char * __format,...);} 
extern "C" {int printf(const char * __format,...);} 
extern "C" {int sprintf(char * __s,const char * __format,...);} 
extern "C" {int vfprintf(FILE * __s,const char * __format,__gnuc_va_list __arg);} 
extern "C" {int vprintf(const char * __format,__gnuc_va_list __arg);} 
extern "C" {int vsprintf(char * __s,const char * __format,__gnuc_va_list __arg);} 
extern "C" {int snprintf(char * __s,size_t __maxlen,const char * __format,...);} 
extern "C" {int vsnprintf(char * __s,size_t __maxlen,const char * __format,__gnuc_va_list __arg);} 
extern "C" {int fscanf(FILE * __stream,const char * __format,...);} 
extern "C" {int scanf(const char * __format,...);} 
extern "C" {int sscanf(const char * __s,const char * __format,...);} 
extern "C" {int fgetc(FILE * __stream);} 
extern "C" {int getc(FILE * __stream);} 
extern "C" {int getchar();} 
extern "C" {int getc_unlocked(FILE * __stream);} 
extern "C" {int getchar_unlocked();} 
extern "C" {int fgetc_unlocked(FILE * __stream);} 
extern "C" {int fputc(int __c,FILE * __stream);} 
extern "C" {int putc(int __c,FILE * __stream);} 
extern "C" {int putchar(int __c);} 
extern "C" {int fputc_unlocked(int __c,FILE * __stream);} 
extern "C" {int putc_unlocked(int __c,FILE * __stream);} 
extern "C" {int putchar_unlocked(int __c);} 
extern "C" {int getw(FILE * __stream);} 
extern "C" {int putw(int __w,FILE * __stream);} 
extern "C" {char * fgets(char * __s,int __n,FILE * __stream);} 
extern "C" {char * gets(char * __s);} 
extern "C" {int fputs(const char * __s,FILE * __stream);} 
extern "C" {int puts(const char * __s);} 
extern "C" {int ungetc(int __c,FILE * __stream);} 
extern "C" {size_t fread(void * __ptr,size_t __size,size_t __n,FILE * __stream);} 
extern "C" {size_t fwrite(const void * __ptr,size_t __size,size_t __n,FILE * __s);} 
extern "C" {size_t fread_unlocked(void * __ptr,size_t __size,size_t __n,FILE * __stream);} 
extern "C" {size_t fwrite_unlocked(const void * __ptr,size_t __size,size_t __n,FILE* __stream);} 
extern "C" {int fseek(FILE * __stream,long __off,int __whence);} 
extern "C" {long ftell(FILE * __stream);} 
extern "C" {void rewind(FILE * __stream);} 
extern "C" {int fgetpos(FILE * __stream,fpos_t * __pos);} 
extern "C" {int fsetpos(FILE * __stream,fpos_t * __pos);} 
extern "C" {void clearerr(FILE * __stream);} 
extern "C" {int feof(FILE * __stream);} 
extern "C" {int ferror(FILE * __stream);} 
extern "C" {void clearerr_unlocked(FILE * __stream);} 
extern "C" {int feof_unlocked(FILE * __stream);} 
extern "C" {int ferror_unlocked(FILE * __stream);} 
extern "C" {void perror(const char * __s);} 
extern "C" int sys_nerr; 
extern "C" const char * const sys_errlist[]; 
extern "C" {int fileno(FILE * __stream);} 
extern "C" {int fileno_unlocked(FILE * __stream);} 
extern "C" {FILE * popen(const char * __command,const char * __modes);} 
extern "C" {int pclose(FILE * __stream);} 
extern "C" {char * ctermid(char * __s);} 
extern "C" {void flockfile(FILE * __stream);} 
extern "C" {int ftrylockfile(FILE * __stream);} 
extern "C" {void funlockfile(FILE * __stream);} 
int x; 

#endif

