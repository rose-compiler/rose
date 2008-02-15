#include "sys.h"
#include "ckpt.h"
#include "ckptimpl.h"

/* FIXME: Save the sigaltstack.  This exists on solaris, but we
   haven't seen it on Linux. */

/* The signal state is stored here */
static sigset_t blocked;	            /* Blocked signals */
static sigset_t pending;                    /* Pending signals */
static struct sigaction action[NSIG];       /* Signal actions */
static unsigned alarm_cnt;                  /* Remaining alarm() time */
static struct timeval time_ckpt;            /* Time of ckpt */
static struct itimerval timer_real;         /* ITIMER_REAL */
static struct itimerval timer_virt;         /* ITIMER_VIRTUAL */
/* FIXME: Two itimers are not checkpointed yet.  We haven't looked
   into checkpointing their associated profile state.  ITIMER_PROF,
   ITIMER_REALPROF */

int
ckpt_signals() {
	sigset_t blkall;
	struct itimerval timer_zero;
	int i;

	/* Save current mask and block all signals */
	sigfillset(&blkall);
	sigemptyset(&blocked);
	sigprocmask(SIG_SETMASK, &blkall, &blocked);

	/* Save the timers state */
	memset(&timer_zero, 0, sizeof(timer_zero));
	gettimeofday(&time_ckpt, NULL);
	syscall(SYS_setitimer, ITIMER_REAL, &timer_zero, &timer_real);
	syscall(SYS_setitimer, ITIMER_VIRTUAL, &timer_zero, &timer_virt);
#if 0
	/* Don't save both itimer and alarm state. */
	/* Can we do both if we do them in the right order? */
	alarm_cnt = alarm(0); 
	fprintf(stderr, "alarm_cnt=%d\n", alarm_cnt);
#else
	alarm_cnt = 0;
#endif

	/* Save the handler for each signal */
	for (i = 0; i < NSIG; i++)
		syscall(SYS_sigaction, i, NULL, &action[i]);

	/* Save the pending signals */
	sigemptyset(&pending);
	sigpending(&pending);

	/* FIXME: Signals posted here until exit will be lost.

	   But, take consolation in the fact that the timers have been
	   disabled, and that we don't claim to deal with
	   communication from other processes.  Any signal posted now
	   has earned its imminent demise. */
	
	return 0;
}

static int
restore_timer(int which, struct itimerval *value, struct timeval time_now)
{
	/* FIXME: We pretty much ignore all usec-level timing */

#if 0
	/* FIXME: What the fuck was I thinking? */

	if (value->it_value.tv_sec < time_ckpt.tv_sec) {
		/* No timer, or it expired before the ckpt */
		return 0;
	}
	if (value->it_interval.tv_sec != 0) {
		fprintf(stderr,
			"WARNING: Itimer (%d) uses intervals. Ignoring.\n",
			which);
	}
	if (time_now.tv_sec < time_ckpt.tv_sec)
		fprintf(stderr,
			"WARNING: This checkpoint is being restarted before it happened\n");
	else
		/* timer = now + (timer - ckpt) */
		value->it_value.tv_sec += time_now.tv_sec + time_ckpt.tv_sec;
#endif
	if (0 > setitimer(which, value, NULL)) {
		fprintf(stderr, "cannot to set itimer %d\n", which);
		perror("setitimer");
		return -1;
	}
	return 0;
}

int
restore_signals()
{
	sigset_t blkall;
	static struct timeval time_now;
	int i;
	int mypid;

	/* Make sure all signals are blocked */
	sigfillset(&blkall);
	sigprocmask(SIG_SETMASK, &blkall, NULL);

	/* It might be wise to clear any pending signals this process
           (unrelated to the ckpt) has received, although we're not
	   expecting any. */
	
	/* Restore the handlers */
	for (i = 0; i < NSIG; i++)
		syscall(SYS_sigaction, i, &action[i], NULL);

	/* Restore the timers */
	gettimeofday(&time_now, NULL);
	restore_timer(ITIMER_REAL, &timer_real, time_now);
	restore_timer(ITIMER_VIRTUAL, &timer_virt, time_now);
	if (alarm_cnt > 0)
		alarm(alarm_cnt);
		
	/* Post pending sigs */
	mypid = getpid();
	for (i = 0; i < NSIG; i++)
		if (sigismember(&pending, i))
			kill(mypid, i);

	/* Restore mask */
	sigprocmask(SIG_SETMASK, &blocked, NULL);

	return 0;
}

static void
asynchandler(int signum)
{
	ckpt_ckpt(ckpt_ckptname());
}

void
ckpt_async(int sig)
{
	if(SIG_ERR == signal(sig, asynchandler))
		fatal("cannot install checkpoint signal handler\n");
}

void
ckpt_cancelasync(int sig)
{
	if(SIG_ERR == signal(sig, SIG_DFL))
		fatal("cannot clear checkpoint signal handler\n");
}

void
ckpt_periodic(unsigned long msperiod)
{
	fatal("period checkpointing not implemented\n");
}
