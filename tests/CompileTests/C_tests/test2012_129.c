typedef int __pid_t;
typedef __pid_t pid_t;

typedef struct process *Process;

typedef struct job *Job;

typedef union linkroot *LinkList;

typedef struct linknode *LinkNode;

struct job {
    pid_t gleader;
    pid_t other;
    int stat;
    char *pwd;

    struct process *procs;
    struct process *auxprocs;
    LinkList filelist;
    int stty_in_env;
    struct ttyinfo *ty;
};

struct process {
    struct process *next;
    pid_t pid;
    char text[80];
    int status;
};

struct linknode {
    LinkNode next;
    LinkNode prev;
    void *dat;
};

struct linklist {
    LinkNode first;
    LinkNode last;
    int flags;
};

union linkroot {
    struct linklist list;
    struct linknode node;
};

int thisjob;
extern int pipestats[256];
typedef long zlong;
extern zlong lastval;
extern int numpipestats;
extern struct job* jobtab;

enum {
    MONITOR,
    OPT_SIZE
};

extern char opts[OPT_SIZE];

extern int sigtrapped[(31 +3)];

extern int errflag;

extern int breaks;

extern int loops;

// DQ: If these are missing then ROSE generates the declaration after the first reference to the function (for all 4 of these functions).
static int super_job (int sub);
static int handle_sub (int job,int fg);
extern void dotrap (int sig);
extern void check_cursh_sig (int sig);



void
update_job(Job jn)
   {
     int job;
     int status = 0;
     int inforeground = 0;

     if (job == thisjob && (jn->stat & (0x0008)))
        {
          int i;
          Process p;

          for (p = jn->procs, i = 0; p && i < 256; p = p->next, i++)
               pipestats[i] = (((((signed char) ((((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f) + 1) >> 1) > 0)) ?
                                   0200 | (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f) :
                                   ((((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0xff00) >> 8));

          if ((jn->stat & (0x0400)) && i < 256)
               pipestats[i++] = lastval;
          numpipestats = i;
        }

     if (!inforeground && (jn->stat & ((0x0100) | (0x0008))) == ((0x0100) | (0x0008)))
        {
          int su;

          if ((su = super_job(jn - jobtab)))
               handle_sub(su, 0);
        }

     if (inforeground == 2 && (opts[MONITOR]) && (((signed char) ((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x7f) + 1) >> 1) > 0))
        {
          int sig = (((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x7f);

          if (sig == 2 || sig == 3) 
             {
               if (sigtrapped[sig]) 
                  {
                    dotrap(sig);

                    if (errflag)
                         breaks = loops;
                  }
                 else
                  {
                    breaks = loops;
                    errflag = 1;
                  }

               check_cursh_sig(sig);
             }
        }
   }


