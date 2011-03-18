#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <cassert>

const int RSIM_SignalHandling::SIG_WAKEUP = 49; /* arbitrarily SIGRT_17 */

RSIM_SignalHandling::siginfo_32
RSIM_SignalHandling::mk_kill(int signo, int code)
{
    siginfo_32 info;
    memset(&info, 0, sizeof info);
    info.si_signo = signo;
    info.si_code  = code;
    info.kill.pid = getpid();
    info.kill.uid = getuid();
    return info;
}

RSIM_SignalHandling::siginfo_32
RSIM_SignalHandling::mk_sigfault(int signo, int code, uint32_t addr)
{
    siginfo_32 info;
    memset(&info, 0, sizeof info);
    info.si_signo = signo;
    info.si_code = code;
    info.sigfault.addr = addr;
    info.sigfault.addr_lsb = addr;
    return info;
}

RSIM_SignalHandling::siginfo_32
RSIM_SignalHandling::mk_rt(int signo, int code)
{
    return mk_kill(signo, code);
}

RSIM_SignalHandling::siginfo_32
RSIM_SignalHandling::mk(const siginfo_t *host)
{
    siginfo_32 info;
    memset(&info, 0, sizeof info);
    info.si_signo = host->si_signo;
    info.si_errno = host->si_errno;
    info.si_code  = host->si_code;

    switch (info.si_signo) {
        case SIGCHLD:
            info.sigchld.pid    = host->si_pid;
            info.sigchld.uid    = host->si_uid;
            info.sigchld.status = host->si_status;
            info.sigchld.utime  = host->si_utime;
            info.sigchld.stime  = host->si_stime;
            break;
        case SIGILL:
        case SIGFPE:
        case SIGSEGV:
        case SIGBUS:
            info.sigfault.addr  = (uint32_t)(uint64_t)host->si_addr;
            break;
        case SIGPOLL:
            info.sigpoll.band   = host->si_band;
            info.sigpoll.fd     = host->si_fd;
            break;
        default:
            /* FIXME: what about POSIX.1b timers? */
            if (info.si_signo < FIRST_RT) {
                info.kill.pid   = host->si_pid;
                info.kill.uid   = host->si_uid;
            } else {
                info.rt.pid     = host->si_pid;
                info.rt.uid     = host->si_uid;
                info.rt.sigval  = host->si_int; /* FIXME: what about si_ptr? */
            }
            break;
    }

    return info;
}

RSIM_SignalHandling::sigset_32
RSIM_SignalHandling::mask_of(int signo)
{
    assert(signo>0 && (size_t)signo<=8*sizeof(sigset_32));
    return (sigset_32)1 << (signo-1);
}

uint32_t
RSIM_SignalHandling::get_sigframe(const sigaction_32 *sa, size_t frame_size, uint32_t sp)
{
    bool on_alt_stack = on_signal_stack(sp);
    RTS_MUTEX(mutex) {
        if ((sa->flags & SA_ONSTACK) && !on_alt_stack &&
            0==(stack.ss_flags & SS_ONSTACK) && 0!=(sa->flags & SA_ONSTACK) && stack.ss_size>0)
            sp = stack.ss_sp + stack.ss_size;
    } RTS_MUTEX_END;

    uint32_t frame_va = sp - frame_size;
    frame_va = ((frame_va+4) & -16ul) - 4; /* Align for i386 ABI: so on function entry ((sp+4) & 0xf == 0 */
    return frame_va;
}

void
RSIM_SignalHandling::setup_sigcontext(sigcontext_32 *sc, const pt_regs_32 &regs, sigset_32 mask)
{
    sc->gs = regs.gs;
    sc->fs = regs.fs;
    sc->es = regs.es;
    sc->ds = regs.ds;
    sc->di = regs.di;
    sc->si = regs.si;
    sc->bp = regs.bp;
    sc->sp = regs.sp;
    sc->bx = regs.bx;
    sc->dx = regs.dx;
    sc->cx = regs.cx;
    sc->ax = regs.ax;
    sc->trapno = 0;    /* FIXME: check this */
    sc->err = 0;       /* FIXME: check this */
    sc->ip = regs.ip;
    sc->cs = regs.cs;
    sc->flags = regs.flags;
    sc->sp_at_signal = regs.sp;
    sc->ss = regs.ss;
    sc->fpstate_ptr = 0; /* no floating point state in simulator */
    sc->oldmask = mask & 0xffffffff; /* mask for classic signals */
    sc->cr2 = 0; /* not used by simulator */
}

void
RSIM_SignalHandling::restore_sigcontext(const sigcontext_32 &sc, uint32_t cur_flags, pt_regs_32 *regs)
{
    memset(regs, 0, sizeof(*regs));
    regs->gs = sc.gs;
    regs->fs = sc.fs;
    regs->es = sc.es;
    regs->ds = sc.ds;
    regs->di = sc.di;
    regs->si = sc.si;
    regs->bp = sc.bp;
    regs->sp = sc.sp;
    regs->bx = sc.bx;
    regs->dx = sc.dx;
    regs->cx = sc.cx;
    regs->ax = sc.ax;
    regs->ip = sc.ip;
    regs->cs = sc.cs;
    regs->flags = sc.flags;
    regs->ss = sc.ss;

    /* Restore flags mentioned in flag_mask, but not the others. */
    uint32_t flag_mask = (1ul<<x86_flag_ac) |
                         (1ul<<x86_flag_of) |
                         (1ul<<x86_flag_df) |
                         (1ul<<x86_flag_tf) |
                         (1ul<<x86_flag_sf) |
                         (1ul<<x86_flag_zf) |
                         (1ul<<x86_flag_af) |
                         (1ul<<x86_flag_pf) |
                         (1ul<<x86_flag_cf) |
                         (1ul<<x86_flag_rf);
    regs->flags = (cur_flags & ~flag_mask) | (regs->flags & flag_mask);
}

int
RSIM_SignalHandling::sigprocmask(int how, const sigset_32 *in, sigset_32 *out)
{
    /* We don't want to play with some bits of the mask. */
    static bool called = false;
    static std::set<int> keep_signals;
    if (!called) {
        keep_signals.insert(__SIGRTMIN+0);       /* used by NPTL */
        keep_signals.insert(__SIGRTMIN+1);       /* used by NPTL */
        keep_signals.insert(SIG_WAKEUP);        /* used internally */
    }

    int result = 0;
    RTS_MUTEX(mutex) {
        sigset_32 saved = mask;

        if (out)
            *out = mask;

        if (in) {
            switch (how) {
                case 0: /* SIG_BLOCK */
                    mask |= *in;
                    break;
                case 1: /* SIG_UNBLOCK */
                    mask &= ~(*in);
                    break;
                case 2: /* SIG_SETMASK */
                    mask = *in;
                    break;
                default:
                    result = -EINVAL;
                    break;
            }
            mask &= ~(mask_of(SIGKILL) | mask_of(SIGSTOP));     /* not blockable, but not an error either */
        }

        /* We have to reprocess signal delivery only if a bit was removed from the mask */
        if ((saved & ~mask))
            reprocess = true;
    } RTS_MUTEX_END;
    return result;
}

int
RSIM_SignalHandling::sigpending(sigset_32 *result) const
{
    if (result) {
        RTS_MUTEX(mutex) {
            *result = pending;
            for (size_t i=queue_head; i!=queue_tail; i=(i+1)%QUEUE_SIZE)
                *result |= mask_of(queue[i].si_signo);
        } RTS_MUTEX_END;
    }
    return 0;
}

int
RSIM_SignalHandling::sigsuspend(const sigset_32 *new_mask_p, RSIM_Thread *thread)
{
    /* Signals that terminate a process by default */
    sigset_32 terminating = (sigset_32)(-1); // everything but...
    terminating &= ~(mask_of(SIGIO)   |
                     mask_of(SIGURG)  |
                     mask_of(SIGCHLD) |
                     mask_of(SIGCONT) |
                     mask_of(SIGKILL) |
                     mask_of(SIGSTOP) |
                     mask_of(SIGTTIN) |
                     mask_of(SIGTTOU) |
                     mask_of(SIGWINCH));

    int retval = 0; /* signal which has arrived, or negative error number */

    while (!retval) {
        RTS_MUTEX(mutex) {
            /* Use user supplied mask (or the current mask) augmented by also masking signals that are ignored or which are
             * using the default handler and do not terminate the process. */
            sigset_32 cur_mask = new_mask_p ? *new_mask_p : mask;
            for (size_t signo=1; signo<=8*sizeof(cur_mask); signo++) {
                sigaction_32 sa;
                int status = thread->get_process()->sys_sigaction(signo, NULL, &sa);
                if (status<0) {
                    retval = status;
                    break;
                }
                if (sa.handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
                    cur_mask |= mask_of(signo);
                } else if (sa.handler_va==(uint32_t)(uint64_t)SIG_DFL && 0==(terminating & mask_of(signo))) {
                    cur_mask |= mask_of(signo);
                }
            }
            cur_mask &= ~(mask_of(SIGKILL) | mask_of(SIGSTOP));
            
            /* What signals are ready to be delivered (assuming they're not masked). */
            if (!retval) {
                sigset_32 p = pending;
                for (size_t i=queue_head; i!=queue_tail; i=(i+1) % QUEUE_SIZE)
                    p |= mask_of(queue[i].si_signo);

                /* Are there any unmasked signals to deliver?  Choose one, preferably a real-time signal. */
                if (p & ~cur_mask) {
                    for (size_t i=0; !retval && i<8*sizeof(p); i++) {
                        int signo = (((FIRST_RT-1)+i) % (8*sizeof(p))) + 1;  /* real-time first, then wrap back to classic sigs */
                        if (0!=((p & ~cur_mask) & mask_of(signo)))
                            retval = signo;
                    }
                }
            }
        } RTS_MUTEX_END;
        
        /* If nothing arrived then block until another signal arrives. */
        if (!retval) {
            thread->tracing(TRACE_SIGNAL)->brief("sigsuspend is blocking");
            sigset_t susp_mask;
            sigemptyset(&susp_mask);
            ::sigsuspend(&susp_mask);
            thread->get_process()->signal_dispatch(); /* assign process-wide signals to threads */
        }
    }
    return retval;
}

bool
RSIM_SignalHandling::on_signal_stack(uint32_t va) const
{
    bool result;
    RTS_MUTEX(mutex) {
        result = (0==(stack.ss_flags & SS_DISABLE) &&
                  va >= stack.ss_sp &&
                  va < stack.ss_sp + stack.ss_size);
    } RTS_MUTEX_END;
    return result;
}

int
RSIM_SignalHandling::sigaltstack(const stack_32 *in, stack_32 *out, uint32_t sp)
{
    int result = 0;

    /* Are we currently executing on the alternate stack? */
    bool on_stack = on_signal_stack(sp);

    RTS_MUTEX(mutex) {
        if (out) {
            *out = stack;
            out->ss_flags &= ~SS_ONSTACK;
            if (on_stack)
                out->ss_flags |= SS_ONSTACK;
        }

        if (in) {
            if (on_stack) {
                result = -EINVAL; /* can't set sig stack while we're using it */
            } else if ((in->ss_flags & ~(SS_DISABLE|SS_ONSTACK))) {
                result = -EINVAL; /* invalid flags */
            } else if (0==(in->ss_flags & SS_DISABLE) && in->ss_size < 4096) {
                result = -ENOMEM;  /* stack must be at least one page large */
            } else {
                stack = *in;
                stack.ss_flags &= ~SS_ONSTACK;
            }
        }
    } RTS_MUTEX_END;
    return result;
}

void
RSIM_SignalHandling::clear_all_pending()
{
    RTS_MUTEX(mutex) {
        pending = 0;
        queue_head = queue_tail = 0;
    } RTS_MUTEX_END;
}

int
RSIM_SignalHandling::generate(const siginfo_32 &info, RSIM_Process *process, RTS_Message *mesg)
{
    int result = 0;
    const char *s = "";

    int signo = info.si_signo;
    assert(signo>=1 && (size_t)signo<=8*sizeof(sigset_32));

    sigaction_32 sa;
    int status = process->sys_sigaction(signo, NULL, &sa);
    assert(status>=0);

    RTS_MUTEX(mutex) {
        if (sa.handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to quiet gcc warning */
            s = " ignored";
        } else if (signo < FIRST_RT) {
            pending |= mask_of(signo);
            pending_info[signo] = info;
        } else if ((queue_tail+1) % QUEUE_SIZE == queue_head) {
            s = " ENOBUFS";
            result = -ENOMEM;
        } else {
            queue[queue_tail] = info;
            queue_tail = (queue_tail+1) % QUEUE_SIZE;
        }

        reprocess = true;

        /* Print messsage before we release the RSIM_SignalHandling mutex, otherwise the tracing output might have this signal
         * arrival message after the target thread's signal delivery message. */
        if (mesg) {
            mesg->multipart("signal", "arrival of ");
            print_enum(mesg, signal_names, signo);
            mesg->more("(%d)%s", signo, s);
            mesg->more(" {errno=%d, code=%d", info.si_errno, info.si_code);
            if (signo>=FIRST_RT) {
                mesg->more(", pid=%d, uid=%d, sigval=%d", info.rt.pid, info.rt.uid, info.rt.sigval);
            } else if (signo==SIGILL || signo==SIGFPE || signo==SIGSEGV || signo==SIGBUS) {
                mesg->more(", addr=0x%08"PRIx32, info.sigfault.addr);
            } else if (signo==SIGCHLD) {
                mesg->more(", pid=%d, uid=%d, status=%u", info.sigchld.pid, info.sigchld.uid, info.sigchld.status);
            } else {
                mesg->more(", pid=%d, uid=%d", info.kill.pid, info.kill.uid);
            }
            mesg->more("}\n");
            mesg->multipart_end();
        }
    } RTS_MUTEX_END;

    return result;
}

int
RSIM_SignalHandling::dequeue(siginfo_32 *info/*out*/, const sigset_32 *alt_mask/*=NULL*/)
{
    int result = 0;

    RTS_MUTEX(mutex) {
        sigset_32 mask = alt_mask ? *alt_mask : this->mask;

        if (reprocess || alt_mask) {
            /* Queued real-time signals */
            for (size_t i=queue_head; i!=queue_tail; i=(i+1) % QUEUE_SIZE) {
                if (0==(mask & mask_of(queue[i].si_signo))) {
                    if (info)
                        *info = queue[i];
                    result = queue[i].si_signo;

                    /* Remove item from queue */
                    if (i==queue_head) {
                        queue_head = (queue_head+1) % QUEUE_SIZE;
                    } else {
                        for (size_t j=(i+1)%QUEUE_SIZE; j!=queue_tail; j=(j+1) % QUEUE_SIZE) {
                            queue[i] = queue[j];
                            i = j;
                        }
                        queue_tail = i;
                    }
                }
            }

            /* Pending classic signals */
            if (pending & ~mask) {
                for (size_t signo=1; signo<=8*sizeof(pending); signo++) {
                    if ((pending & ~mask) & mask_of(signo)) {
                        if (info)
                            *info = pending_info[signo];
                        result = signo;
                        pending &= ~mask_of(signo);
                    }
                }
            }
        
            if (!result)
                reprocess = false;
        }

    } RTS_MUTEX_END;
    return result;
}

#endif /* ROSE_ENABLE_SIMULATOR */
