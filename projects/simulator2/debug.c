/* Just a little test program to help developers analyze what a system call does.
 *
 *     1. Modify do_child() to perform the system call(s) you need to investigate.
 *
 *     2. Compile with "gcc -m32 -static -Wall -g debug.c"
 *
 *     3. Run with no arguments as, "i386 -LRB3 a.out"
 *
 * The output will show:
 *
 *     Register contents before and after every system call.
 *
 *     Register contents at the time every signal is delivered.
 *
 *     Complete exit status information.
 *
 * Other Notes
 *
 *     Tracing follows forks, so the do_child() function may fork if desired.
 *
 *     I haven't tried this with a multi-threaded do_child().
 *
 *     If you want a version that just runs do_child(), then compile with -DNO_TRACE
 */

#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>

static void delay(size_t);

/******************************************************************************************************************************
 *                                      User-configurable function.
 ******************************************************************************************************************************/

static int
do_child()
{
#if 1 /* Testing signal delivery to a forked process, and whether waitpid returns the proper value. [RPM 2011-02-14] */
    delay(1);                                           /* delays used to throttle output; not for synchronization */
    int pid = fork();

    if (-1==pid) {
        exit(1);
    } else if (0==pid) {
        /* child */
        delay(1);
        write(2, "CHILD\n", 6);                         /* at delay 1 */
        delay(2);
        exit(2);                                        /* at delay 3 */
    } else {
        /* parent */
        delay(2);
        kill(pid, SIGHUP);                              /* at delay 2 */
        delay(2);
        int status;
        waitpid(pid, &status, 0);                       /* at delay 4 */
        delay(2);

        if (WIFSIGNALED(status) && WTERMSIG(status)==SIGHUP) {
            write(2, "OK\n", 3);
        } else {
            write(2, "FAIL\n", 5);
        }

        delay(2);
        exit(3);                                        /* at delay 6 */
    }
#endif
}

static int running_in_simulator;                /* non-zero if running in the ROSE simulator */

#define MAX_NCHILDREN ((size_t)100)
typedef struct {
    pid_t pid;
    int attached;                               /* 0=>no, 1=>attached; 2=>initialized */
    int syscall_entered;                        /* true when we stepped into a system call */
} child_info_t;
static child_info_t children[MAX_NCHILDREN];
static size_t nchildren = 0;

static void
print(FILE *f, pid_t pid, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    static struct timeval start_time;
    struct timeval now;
    gettimeofday(&now, NULL);

    if (0==start_time.tv_usec)
        start_time = now;

    double elapsed = now.tv_sec - start_time.tv_sec + 1e-6 * (now.tv_usec - start_time.tv_usec);
    fprintf(f, "[%7.3f %d] ", elapsed, pid);
    vfprintf(f, fmt, ap);
    va_end(ap);
}

/* Delay for a short time without using a system call or signal.  Each delay unit is about a second or less.  We don't use
 * signals or system calls (even though "sleep(units)" would be much simpler and more accurate) because we don't want to
 * pollute the output with extraneous system call traces. */
__attribute__((unused))
static void
delay(size_t units)
{
    size_t u, i, j;
    size_t lim = running_in_simulator ? 4 : 45000;
    for (u=0; u<units; u++) {
        for (i=0; i<25000; i++) {
            for (j=0; j<lim; j++);
        }
    }
}


__attribute__((unused))
static child_info_t *
find_child(pid_t pid)
{
    size_t i;
    for (i=0; i<nchildren; i++) {
        if (children[i].pid==pid) {
            return children+i;
        }
    }
    assert(nchildren <= MAX_NCHILDREN);
    memset(children+nchildren, 0, sizeof(children[0]));
    children[nchildren].pid = pid;
    return children + nchildren++;
}

__attribute__((unused))
static void
print_registers(FILE *f, pid_t pid, const struct user_regs_struct *r, const char *prefix) 
{
    print(f, pid, "%sebx=0x%08lx, ecx=0x%08lx, edx=0x%08lx, esi=0x%08lx, edi=0x%08lx, ebp=0x%08lx,\n",
          prefix, r->ebx, r->ecx, r->edx, r->esi, r->edi, r->ebp);
    print(f, pid, "%seax=0x%08lx,  ds=0x%08lx,  es=0x%08lx,  fs=0x%08lx,  gs=0x%08lx,\n",
          prefix, r->eax, r->xds, r->xes, r->xfs, r->xgs);
    print(f, pid, "%seip=0x%08lx,  cs=0x%08lx, esp=0x%08lx,  ss=0x%08lx, flg=0x%08lx, orig_eax=0x%08lx\n",
          prefix, r->eip, r->xcs, r->esp, r->xss, r->eflags, r->orig_eax);
}

int
main()
{
    running_in_simulator = (0==syscall(1000000));

#ifdef NO_TRACE
    exit(do_child());
#else
    static const unsigned long options = PTRACE_O_TRACEFORK | PTRACE_O_TRACESYSGOOD;
    FILE *f = stdout;
    int pstat;
    setbuf(f, NULL);

    {
        /* Fork a child, which we will trace. */
        pid_t pid = fork();
        if (pid<0)
            exit(1);
        if (0==pid)
            exit(do_child());

        /* Initialize first child's data structure */
        child_info_t *child = find_child(pid);
        print(f, pid, "attaching\n");
        pstat = ptrace(PTRACE_ATTACH, pid);
        assert(0==pstat);
        child->attached = 1; /* attached but not initialized yet */
    }

    /* Trace children. The term "children" for the sake of ptrace means all processes being traced. */
    while (1) {
        int status;
        pid_t p = waitpid(-1, &status, 0);
        if (-1==p && ECHILD==errno)
            exit(0);
        assert(p>0);
        child_info_t *current = find_child(p);
        assert(current);
        assert(current->pid==p);

        if (0==current->attached) {
            pstat = ptrace(PTRACE_ATTACH, p);
            current->attached = 1;
            print(f, p, "attaching\n");
        }

        if (WIFSTOPPED(status)) {
            int signo = WSTOPSIG(status);
            /* Print registers */
            struct user_regs_struct regs;
            pstat = ptrace(PTRACE_GETREGS, p, NULL, &regs);
            assert(0==pstat);
            print_registers(f, p, &regs, "");


            if (signo == (SIGTRAP | 0x80)) {
                /* Print syscall enter/leave */
                if (0==current->syscall_entered) {
                    print(f, p, "syscall %ld\n", regs.orig_eax);
                } else {
                    print(f, p, "returns %ld%s%s\n", regs.eax, regs.eax<0?" ":"", regs.eax<0?strerror(-regs.eax):"");
                }
                current->syscall_entered = !current->syscall_entered;
                signo = 0; /* we don't want to deliver this to the child */

            } else {
                /* Child received some other kind of signal. */
                print(f, p, "stopped by signal %d %s\n", signo, strsignal(signo));
                if (SIGTRAP==signo) {
                    print(f, p, "  not delivering\n");
                    signo = 0;
                } else if (current->attached<2 && SIGSTOP==signo) {
                    print(f, p, "  not delivering\n");
                    signo = 0; /* SIGSTOP is generated by attaching */
                }
            }
            
            /* Make sure we've set all necessary ptrace flags for the process */
            if (1==current->attached) {
                print(f, p, "  initializing\n");
                pstat = ptrace(PTRACE_SETOPTIONS, p, NULL, (void*)options);
                assert(0==pstat);
                current->attached = 2; /* now it's initialized */
            }
            
            /* Resume child */
            fputc('\n', f);
            pstat = ptrace(PTRACE_SYSCALL, p, NULL, (void*)(long)signo);
            assert(0==pstat);

        } else if (WIFEXITED(status)) {
            print(f, p, "terminated with status %d\n", WEXITSTATUS(status));
            exit(0);
            
        } else if (WIFSIGNALED(status)) {
            print(f, p, "terminated by signal %d %s\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
            exit(0);
        }
    }
#endif
    return 0;
}
