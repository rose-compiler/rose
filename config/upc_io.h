#include <stdint.h>

#define UPC_RDONLY            ((int)(1 << 0))
#define UPC_WRONLY            ((int)(1 << 1))
#define UPC_RDWR              ((int)(1 << 2))
#define UPC_INDIVIDUAL_FP     ((int)(1 << 3))
#define UPC_COMMON_FP         ((int)(1 << 4))
#define UPC_APPEND            ((int)(1 << 5))
#define UPC_CREATE            ((int)(1 << 6))
#define UPC_EXCL              ((int)(1 << 7))
#define UPC_STRONG_CA         ((int)(1 << 8))
#define UPC_TRUNC             ((int)(1 << 9))
#define UPC_DELETE_ON_CLOSE   ((int)(1 << 10))

#define UPC_GET_CA_SEMANTICS          ((int)(1 << 0))
#define UPC_SET_WEAK_CA_SEMANTICS     ((int)(1 << 1))
#define UPC_SET_STRONG_CA_SEMANTICS   ((int)(1 << 2))
#define UPC_GET_FP                    ((int)(1 << 3))
#define UPC_SET_COMMON_FP             ((int)(1 << 4))
#define UPC_SET_INDIVIDUAL_FP         ((int)(1 << 5))
#define UPC_GET_FL                    ((int)(1 << 6))
#define UPC_GET_FN                    ((int)(1 << 7))
#define UPC_GET_HINTS                 ((int)(1 << 8))
#define UPC_SET_HINT                  ((int)(1 << 9))
#define UPC_ASYNC_OUTSTANDING         ((int)(1 << 10))

#define UPC_SEEK_SET          ((int)(1 << 0))
#define UPC_SEEK_CUR          ((int)(1 << 1))
#define UPC_SEEK_END          ((int)(1 << 2))

typedef int32_t upc_off_t;

#define UPCRI_MAX_OFF_T ((upc_off_t)(((uint32_t)-1) >> 1))

typedef int upc_file_t ;

typedef struct {
    void* baseaddr;
    size_t len;
}  upc_local_memvec;

typedef struct {
    shared void *baseaddr;
    size_t blocksize;
    size_t len;
} upc_shared_memvec;

typedef struct {
    upc_off_t offset;
    size_t len;
} upc_filevec;

typedef struct {
    const char *key;
    const char *value;
} upc_hint;


/* I/O fonctions A.1.3 */
extern upc_file_t*  upc_all_fopen( const char *fname, int flags, size_t numhints, const upc_hint *hints );
extern int          upc_all_fclose (upc_file_t *fd );
extern int          upc_all_fsync( upc_file_t *fd );
extern upc_off_t    upc_all_fseek( upc_file_t *fd, upc_off_t offset, int origin );
extern int          upc_all_fset_size( upc_file_t *fd, upc_off_t size );
extern upc_off_t    upc_all_fget_size( upc_file_t *fd );
extern int          upc_all_fpreallocate( upc_file_t *fd, upc_off_t size );
extern int          upc_all_fcntl( upc_file_t *fd, int cmd, void *arg );
extern upc_off_t    upc_all_fread_local( upc_file_t *fd, void *buffer, size_t size, size_t nmemb, upc_flag_t flags );
extern upc_off_t    upc_all_fread_shared( upc_file_t *fd, shared void *buffer, size_t blocksize, size_t size, size_t nmemb, upc_flag_t flags );
extern upc_off_t    upc_all_fwrite_local( upc_file_t *fd, void *buffer, size_t size, size_t nmemb, upc_flag_t flags );
extern upc_off_t    upc_all_fwrite_shared( upc_file_t *fd,shared void *buffer, size_t blocksize, size_t size, size_t nmemb, upc_flag_t flags );
extern upc_off_t    upc_all_fread_list_local(upc_file_t *fd, size_t memvec_entries, upc_local_memvec *memvec, size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);
extern upc_off_t    upc_all_fread_list_shared(upc_file_t *fd, size_t memvec_entries, upc_shared_memvec *memvec, size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);
extern upc_off_t    upc_all_fwrite_list_local(upc_file_t *fd, size_t memvec_entries, upc_local_memvec *memvec, size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);
extern upc_off_t    upc_all_fwrite_list_shared(upc_file_t *fd, size_t memvec_entries, upc_shared_memvec *memvec, size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);

/* Asynchronous I/O functions */
extern void         upc_all_fread_local_async(upc_file_t *fd, void *buffer, size_t size, size_t nmemb, upc_flag_t flags);
extern void         upc_all_fread_shared_async(upc_file_t *fd, shared void *buffer, size_t blocksize, size_t size, size_t nmemb, upc_flag_t flags);
extern void         upc_all_fwrite_local_async(upc_file_t *fd, void *buffer, size_t size, size_t nmemb, upc_flag_t flags);
extern void         upc_all_fwrite_shared_async(upc_file_t *fd, shared void *buffer, size_t blocksize,size_t size, size_t nmemb, upc_flag_t flags);
extern void         upc_all_fread_list_local_async(upc_file_t *fd, size_t memvec_entries, upc_local_memvec  *memvec, size_t filevec_entries, upc_filevec  *filevec,  upc_flag_t flags);
extern void         upc_all_fread_list_shared_async(upc_file_t *fd, size_t memvec_entries, upc_shared_memvec  *memvec,size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);
extern void         upc_all_fwrite_list_local_async(upc_file_t *fd, size_t memvec_entries, upc_local_memvec  *memvec, size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);
extern void         upc_all_fwrite_list_shared_async(upc_file_t *fd, size_t memvec_entries, upc_shared_memvec  *memvec, size_t filevec_entries, upc_filevec  *filevec, upc_flag_t flags);
extern upc_off_t    upc_all_fwait_async(upc_file_t *fd);
extern upc_off_t    upc_all_ftest_async(upc_file_t *fd, int *flag);

