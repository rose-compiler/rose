#if 0
int x;
int y;
int z;
#endif

typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;

typedef struct siginfo
   {
//   int si_signo;
     int si_errno;
     int si_code;

#if 1
     union
        {
          int _pad[((128 / sizeof (int)) - 4)];
          struct
             {
               void *si_addr;
             } _sigfault;

#if 1
          struct
             {
               long int si_band;
               int si_fd;
             } _sigpoll;
#endif
        } _sifields;
#endif
   } siginfo_t;

struct X
  {
#if 0
int x;
int y;
int z;
#endif
  };
