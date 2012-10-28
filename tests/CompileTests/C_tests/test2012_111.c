

typedef int __pid_t;
typedef __pid_t pid_t;

typedef struct process *Process;
struct job {
    pid_t gleader;
    pid_t other;
    int stat;
    char *pwd;

    struct process *procs;
//  struct process *auxprocs;
//  LinkList filelist;
    int stty_in_env;
//  struct ttyinfo *ty;
};
struct process {
    struct process *next;
    pid_t pid;
    char text[80];
    int status;
//  child_times_t ti;
//  struct timeval bgtime;
//  struct timeval endtime;
};

typedef struct job *Job;

extern int kill (__pid_t __pid, int __sig) __attribute__ ((__nothrow__));

extern int killpg (__pid_t __pgrp, int __sig) __attribute__ ((__nothrow__));


static int
handle_sub(int job, int fg)
{
   Job jn,sj; // = jobtab + job, sj = jobtab + jn->other;

 // if ((sj->stat & (0x0008)) || (!sj->procs && !sj->auxprocs)) {
    if (0) {
 struct process *p;

#if 1
// for (p = sj->procs; p; p = p->next)
 for (;;)
     if ((((signed char) ((((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f) + 1) >> 1) > 0)) {
#if 1
  if (jn->gleader != 0 && jn->procs->next)
      killpg(jn->gleader, (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f));
  else
      kill(jn->procs->pid, (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f));
#endif
//  kill(sj->other, 18);
    kill(sj->other, (((__extension__ ({ union { __typeof(p->status) __in; int __i; } __u; __u.__in = (p->status); __u.__i; }))) & 0x7f));
  break;
     }
#endif
 // curjob = jn - jobtab;
    } else if (sj->stat & (0x0002)) {
 struct process *p;

// jn->stat |= (0x0002);
 for (p = jn->procs; p; p = p->next);

 return 1;
    }
    return 0;
}

