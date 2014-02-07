
typedef unsigned long size_t;

// typedef enum nss_status NSS_STATUS;

#define NS_SUCCESS  1
#define NS_NOTFOUND 2
#define NS_UNAVAIL  3
#define NS_TRYAGAIN 4

#define NSS_STATUS nss_status;

// typedef enum
enum nss_status
   {
     NSS_STATUS_SUCCESS  = NS_SUCCESS,
     NSS_STATUS_NOTFOUND = NS_NOTFOUND,
     NSS_STATUS_UNAVAIL  = NS_UNAVAIL,
     NSS_STATUS_TRYAGAIN = NS_TRYAGAIN
// } NSS_STATUS;
   };

struct nss_userdb_module
   {
  // struct userdb_module module;

     char *buf;
     size_t bufsize;

  // struct module nss_module;
  // enum nss_status (*getpwnam_r)(const char *name, struct passwd *pwd, char *buffer, size_t buflen, int *errnop);
     enum nss_status (*getpwnam_r)(const char *name, char *buffer, size_t buflen, int *errnop);
   };

