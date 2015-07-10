/* Run multi-threaded tests of RSIM's futex layers */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

static void delay(double nsec)
{
    double sec, frac;
    frac = modf(nsec, &sec);
    struct timespec tv, rem;
    tv.tv_sec = sec;
    tv.tv_nsec = 1e9*frac;
    int status;
    while (-1==(status = nanosleep(&tv, NULL)) && EINTR==errno)
        tv = rem;
}

/******************************************************************************************************************************
 * Tests that signaling a futex wakes only the specified number of threads waiting on that futex.
 *
 * One caller becomes the "master", while the non-master threads all block waiting on a futex.  The master wakes N of those
 * threads, which participate on a barrier with the master.  Once the barrier is reached, the remaining threads are all woken
 * and exit.
 *
 * If the first set of wakes results in fewer threads being unblocked than were expected, all participating threads will block
 * indefinitely on the barrier.  If the first set of wakes results in more than the expected number of threads being unblocked,
 * the extra threads will block indifinitely on the barrier.
 ******************************************************************************************************************************/

#define SYS_RSIM_TEST 32100

static void test1_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("RSIM_test1", "pd");
}

static void test1_body(RSIM_Thread *t, int callno)
{
    RTS_Message *m     = t->tracing(TRACE_MISC);
    uint32_t futex_va  = t->syscall_arg(0);
    size_t nthreads    = t->syscall_arg(1); // including the master
    if (nthreads<2) {
        m->mesg("test1 must run with at least two threads.");
        t->syscall_return(-EINVAL);
        return;
    }
    size_t to_wake     = rand() % (nthreads-1);
    uint32_t bitset    = (uint32_t)(-1);

    static const bool verbose = false;
    static RTS_mutex_t mutex = RTS_MUTEX_INITIALIZER((RTS_Layer)144);
    static size_t nactive;                      // number of active callers
    static pthread_barrier_t enter_barrier;     // barrier for all participants to start the test
    static pthread_barrier_t leave_barrier;     // barrier for all participants to leave the test
    static pthread_barrier_t awake_barrier;     // barrier for master + intentionally awoken threads
    static bool done = false;                   // true when test is finished
    static size_t ntests;                       // number of tests run

    /* The first thread to arrive here is the master. */
    size_t my_position; // my position in the active callers
    RTS_MUTEX(mutex) {
        my_position = nactive++;
        if (0==my_position) {
            ++ntests;
            done = false;
            pthread_barrier_init(&enter_barrier, NULL, nthreads);
            pthread_barrier_init(&leave_barrier, NULL, nthreads);
            pthread_barrier_init(&awake_barrier, NULL, 1+to_wake); // including master
        }
    } RTS_MUTEX_END;

    /* Wait for everyone to arrive before we start the test.  This gives a better chance that the non-master callers will have
     * time to enter the futex wait queue before the master signals for the first time.  We want as many threads waiting on the
     * queue as possible for the best chance of the test failing. */
    if (0==my_position) {
        m->mesg("master: test %zu to wake %zu of %zu threads", ntests, to_wake, nthreads);
        if (verbose)
            m->mesg("master: waiting for all participants to arrive");
    }
    pthread_barrier_wait(&enter_barrier);

    if (0==my_position) {
        /* Delay long enough for most other threads to enter the futex wait queue, then wake up the specified number of
         * threads as fast as possible. */
        if (verbose)
            m->mesg("master: everyone has arrived; delaying a bit to allow them to enter the futex wait queue...");
        delay(0.01);
        if (verbose)
            m->mesg("master: resuming; about to wake %zu thread%s", to_wake, 1==to_wake?"":"s");
        for (size_t nwoke=0; nwoke<to_wake; /*void*/) {
            int status = t->futex_wake(futex_va, to_wake-nwoke, bitset);
            if (status<0) {
                m->mesg("master: futex wake failed: %s", strerror(-status));
                t->syscall_return(status);
                return;
            }
            assert(status+nwoke<=to_wake);
            nwoke += status;
        }

        /* Wait for the awoken threads to do their business.  We don't know how long this will take them, but we know how many
         * there should be.  Of course, if the futex subsystem is broken then the number might be different. If fewer woke than
         * expected then the master will hang in this loop; if more woke then the extras will hang in their barrier wait
         * calls. */
        if (verbose)
            m->mesg("master: waiting for awoken threads to arrive at barrier...");
        pthread_barrier_wait(&awake_barrier);
        if (verbose)
            m->mesg("master: all expected threads have arrived at barrier; waking the remaining threads...");
        RTS_MUTEX(mutex) {
            done = true;
            --nactive; // remove ourself
        } RTS_MUTEX_END;

        /* Wake the remaining threads so they can all return.  Don't be too aggressive here, or else we could end up with lots
         * of diagnostic output. */
        size_t n = 0;
        do {
            RTS_MUTEX(mutex) {
                n = nactive;
            } RTS_MUTEX_END;
            if (n>0) {
                int status = t->futex_wake(futex_va, n, bitset);
                if (status<0) {
                    m->mesg("master: futex wake failed: %s", strerror(-status));
                    t->syscall_return(status);
                    return;
                }
                if (status<(int)n)
                    delay(0.1);
            }
        } while (n>0);
        if (verbose)
            m->mesg("master: all threads have been awoken; the test is done.");
        
    } else {
        /* Non-master threads block.  When they wake up it's either due to the master's first set of futex_wake calls (the
         * ones that are the main part of the test), or the master's second set of calls (the ones that are waking up the
         * rest of the blocked threads).  The thread makes note of why it woke up, and then returns. */
        int status = t->futex_wait(futex_va, 0, bitset);
        if (status<0) {
            m->mesg("slave: futex wait failed: %s", strerror(-status));
            t->syscall_return(status);
            return;
        }

        bool intended;
        RTS_MUTEX(mutex) {
            intended = !done;
            --nactive;
        } RTS_MUTEX_END;

        if (intended) {
            if (verbose)
                m->mesg("slave: intended; about to wait on barrier...");
            pthread_barrier_wait(&awake_barrier);
        }
    }

    pthread_barrier_wait(&leave_barrier);
    if (0==my_position)
        m->mesg("master: test %zu passed", ntests);
    t->syscall_return(0);
}

static void test1_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d");
}

/*******************************************************************************************************************************/


int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    sim.install_callback(new RSIM_Tools::UnhandledInstruction); // needed for ld-linux.so

    assert(!sim.syscall_is_implemented(SYS_RSIM_TEST));
    sim.syscall_define(SYS_RSIM_TEST, test1_enter, test1_body, test1_leave);

    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);
    // sim.activate();
    sim.main_loop();
    // sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // noreturn
    return 0;
}


#else

int
main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif
