#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "Diagnostics.h"
#include "TraceSemantics2.h"
#include <errno.h>
#include <stdarg.h>
#include <syscall.h>

#include <sys/user.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;

size_t RSIM_Thread::next_sequence_number = 1;

/* Constructor */
void
RSIM_Thread::ctor()
{
    set_tid();
    my_seq = next_sequence_number++;

    memset(&last_report, 0, sizeof last_report);

    reopen_trace_facilities();

    dispatcher_ = RSIM_Semantics::createDispatcher(this);
}

BaseSemantics::SValuePtr
RSIM_Thread::pop() {
    RegisterDescriptor SP = get_process()->disassembler()->stackPointerRegister();
    RegisterDescriptor SS = get_process()->disassembler()->stackSegmentRegister();
    size_t resultWidth = SP.nBits();                    // number bits to read from memory
    BaseSemantics::SValuePtr oldSp = operators()->readRegister(SP);
    BaseSemantics::SValuePtr dflt = operators()->undefined_(resultWidth);
    BaseSemantics::SValuePtr retval = operators()->readMemory(SS, oldSp, dflt, operators()->boolean_(true));
    BaseSemantics::SValuePtr newSp = operators()->add(oldSp, operators()->number_(oldSp->get_width(), resultWidth/8));
    operators()->writeRegister(SP, newSp);
    return retval;
}

void
RSIM_Thread::set_tid()
{
    my_tid = syscall(SYS_gettid);
    assert(my_tid>0);
}

void
RSIM_Thread::reopen_trace_facilities()
{
    for (int tf=0; tf<TRACE_NFACILITIES; tf++) {
        if (trace_mesg[tf]==NULL)
            trace_mesg[tf] = new Sawyer::Message::Stream(RSIM_Simulator::mlog[INFO]);
        bool shouldEnable = (process->tracingFlags() & tracingFacilityBit((TracingFacility)tf));
        trace_mesg[tf]->enable(shouldEnable);
    }
}

std::string
RSIM_Thread::id()
{
    struct timeval now;
    gettimeofday(&now, NULL);

    const struct timeval &ctime = get_process()->get_ctime();

    double elapsed = (now.tv_sec - ctime.tv_sec) + 1e-6 * ((double)now.tv_usec - ctime.tv_usec);
    char buf1[32];
    sprintf(buf1, "%1.3f", elapsed);

    char buf2[64];
    RegisterDescriptor IP = get_process()->disassembler()->instructionPointerRegister();
    uint64_t eip = operators()->readRegister(IP)->get_number();

    int n = snprintf(buf2, sizeof(buf2), "0x%08" PRIx64"[%zu]: ", eip, operators()->nInsns());
    assert(n>=0 && (size_t)n<sizeof(buf2)-1);
    memset(buf2+n, ' ', sizeof(buf2)-n);
    buf2[std::max(n, 21)] = '\0';

    return (StringUtility::numberToString(getpid()) + ":" + StringUtility::numberToString(my_seq) +
            " " + buf1 +
            " " + buf2);

}

Sawyer::Message::Stream&
RSIM_Thread::tracing(TracingFacility tf)
{
    assert(tf>=0 && tf<TRACE_NFACILITIES);
    assert(trace_mesg[tf]!=NULL);
    return *trace_mesg[tf];
}

RSIM_Semantics::RiscOperatorsPtr
RSIM_Thread::operators() const
{
    BaseSemantics::RiscOperatorsPtr baseOps = dispatcher_->get_operators();

    // This is the usual case, so keep it here to be fast.
    if (RSIM_Semantics::RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RSIM_Semantics::RiscOperators>(baseOps))
        return retval;

    // This unusual case might happen when we turn on semantic tracing when debugging the simulator.
    if (TraceSemantics::RiscOperatorsPtr traceOps = boost::dynamic_pointer_cast<TraceSemantics::RiscOperators>(baseOps)) {
        BaseSemantics::RiscOperatorsPtr subOps = traceOps->subdomain();
        if (RSIM_Semantics::RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RSIM_Semantics::RiscOperators>(subOps))
            return retval;
    }

    ASSERT_not_reachable("invalid RiscOperators type");
}

SgAsmInstruction *
RSIM_Thread::current_insn()
{
    RegisterDescriptor IP = get_process()->disassembler()->instructionPointerRegister();
    rose_addr_t ip = operators()->readRegister(IP)->get_number();
    SgAsmInstruction *insn = get_process()->get_instruction(ip);
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    return insn;
}

Printer
RSIM_Thread::print(Sawyer::Message::Stream &m, const std::string &atEnd) {
    return Printer(m, this, atEnd);
}

Printer
RSIM_Thread::print(Sawyer::Message::Stream &m, const uint32_t *args, const std::string &atEnd) {
    return Printer(m, this, args, atEnd);
}

Printer
RSIM_Thread::print(Sawyer::Message::Stream &m, const uint64_t *args, const std::string &atEnd) {
    return Printer(m, this, args, atEnd);
}

Printer
RSIM_Thread::print(TracingFacility tf, const std::string &atEnd) {
    return print(tracing(tf), atEnd);
}

Printer
RSIM_Thread::syscall_enter(const std::string &name) {
    Sawyer::Message::Stream &m = tracing(TRACE_SYSCALL);
    SAWYER_MESG(m) <<name <<"[" <<syscall_arg(-1) <<"](";
    return print(m, ")");
}

Printer
RSIM_Thread::syscall_enter(const uint32_t *args, const std::string &name) {
    Sawyer::Message::Stream &m = tracing(TRACE_SYSCALL);
    SAWYER_MESG(m) <<name <<"[" <<syscall_arg(-1) <<"](";
    return print(m, args, ")");
}

Printer
RSIM_Thread::syscall_enter(const uint64_t *args, const std::string &name) {
    Sawyer::Message::Stream &m = tracing(TRACE_SYSCALL);
    SAWYER_MESG(m) <<name <<"[" <<syscall_arg(-1) <<"](";
    return print(m, args, ")");
}

Printer
RSIM_Thread::syscall_leave() {
    Sawyer::Message::Stream &m = tracing(TRACE_SYSCALL);
    Printer p = print(m, "\n");
    p.str(" = ").arg(-1);
    return p;
}

Printer
RSIM_Thread::syscall_leave(const uint32_t *args) {
    Sawyer::Message::Stream &m = tracing(TRACE_SYSCALL);
    Printer p = print(m, args, "\n");
    p.str(" = ").arg(-1);
    return p;
}

Printer
RSIM_Thread::syscall_leave(const uint64_t *args) {
    Sawyer::Message::Stream &m = tracing(TRACE_SYSCALL);
    Printer p = print(m, args, "\n");
    p.str(" = ").arg(-1);
    return p;
}

uint64_t
RSIM_Thread::syscall_arg(int idx)
{
    RegisterDescriptor reg;
    if (-1 == idx) {
        reg = get_process()->get_simulator()->syscallReturnRegister();
    } else {
        ASSERT_require((size_t)idx < get_process()->get_simulator()->syscallArgumentRegisters().size());
        reg = get_process()->get_simulator()->syscallArgumentRegisters()[idx];
    }
    return operators()->readRegister(reg)->get_number();
}

/* Deliver the specified signal. The signal is not removed from the signal_pending vector, nor is it added if it's masked. */
int
RSIM_Thread::signal_deliver(const RSIM_SignalHandling::SigInfo &_info)
{
    RSIM_SignalHandling::SigInfo info = _info;
    int signo = info.si_signo;

    bool cb_status = get_callbacks().call_signal_callbacks(RSIM_Callbacks::BEFORE, this, signo, &info,
                                                           RSIM_Callbacks::SignalCallback::DELIVERY, true);

    if (cb_status && signo>0) {
        assert(signo<=64);

        Sawyer::Message::Stream mesg(tracing(TRACE_SIGNAL));
        SigAction sa;
        int status = get_process()->sys_sigaction(signo, NULL, &sa);
        ASSERT_always_require(status>=0);

        if (sa.handlerVa==(rose_addr_t)SIG_IGN) {
            /* The signal action may have changed since the signal was generated, so we need to check this again. */
            mesg <<"signal delivery ignored: ";
            print_SigInfo(mesg, (const uint8_t*)&info, sizeof info);
            mesg <<"\n";
        } else if (sa.handlerVa==(rose_addr_t)SIG_DFL) {
            mesg <<"signal delivery via default: ";
            print_SigInfo(mesg, (const uint8_t*)&info, sizeof info);
            mesg <<"\n";

            switch (signo) {
                case SIGFPE:
                case SIGILL:
                case SIGSEGV:
                case SIGBUS:
                case SIGABRT:
                case SIGTRAP:
                case SIGSYS:
                    /* Exit process with core dump */
                    tracing(TRACE_MISC) <<"dumping core...\n";
                    get_process()->mem_showmap(tracing(TRACE_MISC), "map at time of core dump:\n");
                    get_process()->dump_core(signo);
                    tracing(TRACE_MISC) <<*operators();
                    report_stack_frames(tracing(TRACE_MISC));
                    throw RSIM_Process::Exit((signo & 0x7f) | __WCOREFLAG, true);
                case SIGTERM:
                case SIGINT:
                case SIGQUIT:
                case SIGKILL:
                case SIGHUP:
                case SIGALRM:
                case SIGVTALRM:
                case SIGPROF:
                case SIGPIPE:
                case SIGXCPU:
                case SIGXFSZ:
                case SIGUSR1:
                case SIGUSR2:
                    /* Exit without core dump */
                    throw RSIM_Process::Exit(signo & 0x7f, true);
                case SIGIO:
                case SIGURG:
                case SIGCHLD:
                case SIGCONT:
                case SIGSTOP:
                case SIGTTIN:
                case SIGTTOU:
                case SIGWINCH:
                    /* Signal is ignored by default */
                    return 0;
                default:
                    /* Exit without a core dump */
                    throw RSIM_Process::Exit(signo & 0x7f, true);
            }

        } else {
            /* Most of the code here is based on __setup_frame() in Linux arch/x86/kernel/signal.c */
            mesg <<"signal delivery to " <<StringUtility::addrToString(sa.handlerVa) <<"\n";
            print_SigInfo(mesg, (const uint8_t*)&info, sizeof info);
            mesg <<"\n";

            PtRegs regs = get_regs();
            RSIM_SignalHandling::SigSet signal_mask;
            status = sighand.sigprocmask(0, NULL, &signal_mask);
            assert(status>=0);
            uint32_t frame_va = 0;

            if (sa.flags & SA_SIGINFO) {
#if 1 // [Robb P. Matzke 2015-05-28]
                // Perhaps we don't need this right now, otherwise we'll need 32- and 64-bit versions of siginfo
                ASSERT_not_implemented("[Robb P. Matzke 2015-05-28]");
#else
                /* Use the extended signal handler frame */
                RSIM_SignalHandling::rt_sigframe_32 frame;
                memset(&frame, 0, sizeof frame);
                stack_32 stack; /* signal alternate stack */
                int status __attribute__((unused)) = sighand.sigaltstack(NULL, &stack, regs.sp);
                assert(status>=0);
                frame_va = sighand.get_sigframe(sa, sizeof frame, regs.sp);

                frame.signo = signo;
                frame.pinfo = frame_va + OFFSET_OF_MEMBER(frame, info);
                frame.puc = frame_va + OFFSET_OF_MEMBER(frame, uc);

                frame.info = info;

                frame.uc.uc_flags = 0; /* zero unless cpu_has_xsave; see Linux ia32_setup_rt_frame() */
                frame.uc.uc_link_va = 0;
                frame.uc.uc_stack.ss_sp = stack.ss_sp;
                frame.uc.uc_stack.ss_flags = sighand.on_signal_stack(frame_va) ? SS_ONSTACK : SS_DISABLE;
                frame.uc.uc_stack.ss_size = stack.ss_size;
                sighand.setup_sigcontext(&frame.uc.uc_mcontext, regs, signal_mask);
                frame.uc.uc_sigmask = signal_mask;

                /* Restorer. If sa_flags 0x04000000 is set, then stack frame "pretcode" is set to the sa_restorer address
                 * passed to sigaction. Otherwise pretcode points either to the "retcode" member of the stack frame (eight
                 * bytes of x86 code that invoke syscall 119) or to the rt_sigreturn address in the VDSO. */
                if (sa.flags & 0x04000000/*SA_RESTORER, deprecated*/) {
                    frame.pretcode = sa.restorerVa;
                } else {
                    frame.pretcode = SIGHANDLER_RT_RETURN; /* or could point to frame.retcode */
                    //frame.preturn = frame_va + OFFSET_OF_MEMBER(frame, retcode);
                    //frame.preturn = VDSO32_SYMBOL(vdso, rt_sigreturn); /* NOT IMPLEMENTED YET */
                }

                /* Signal handler return code. For pre-2.6 kernels, this was an eight-byte chunk of x86 code that calls
                 * sys_sigreturn().  Newer kernels still push these bytes but never execute them. GDB uses them as a magic
                 * number to recognize that it's at a signal stack frame. Instead, pretcode is the address of sigreturn in the
                 * VDSO.  For now, we hard code it to a value that will be recognized by RSIM_Thread::main() as a
                 * return-from-signal-handler. */
                frame.retcode[0] = 0xb8;    /* b8 ad 00 00 00 | mov eax, 119 */
                frame.retcode[1] = 0xad;
                frame.retcode[2] = 0x00;
                frame.retcode[3] = 0x00;
                frame.retcode[4] = 0x00;
                frame.retcode[5] = 0xcd;    /* cd 80          | int 80 */
                frame.retcode[6] = 0x80;
                frame.retcode[7] = 0x00;    /* 00             | padding; not reached */

                /* Write frame to stack */
                if (sizeof(frame)!=process->mem_write(&frame, frame_va, sizeof frame))
                    return -EFAULT;
#endif
            } else {
                /* Use the plain signal handler frame */
                RSIM_SignalHandling::sigframe_32 frame;
                memset(&frame, 0, sizeof frame);
                frame_va = sighand.get_sigframe(sa, sizeof frame, regs.sp);

                frame.signo = signo;
                sighand.setup_sigcontext(&frame.sc, regs, signal_mask);
                frame.extramask = signal_mask >> 32;
                if (sa.flags & 0x04000000/*SA_RESTORER, deprecated*/) {
                    frame.pretcode = sa.restorerVa;
                } else {
                    frame.pretcode = SIGHANDLER_RETURN; /* or could point to frame.retcode */
                    //frame.preturn = frame_va + OFFSET_OF_MEMBER(frame, retcode);
                    //frame.preturn = VDSO32_SYMBOL(vdso, rt_sigreturn); /* NOT IMPLEMENTED YET */
                }

                frame.retcode[0] = 0x58;    /* 58             | pop eax */
                frame.retcode[1] = 0xb8;    /* b8 77 00 00 00 | mov eax, 119 */
                frame.retcode[2] = 0x77;
                frame.retcode[3] = 0x00;
                frame.retcode[4] = 0x00;
                frame.retcode[5] = 0x00;
                frame.retcode[6] = 0xcd;    /* cd 80          | int 80 */
                frame.retcode[7] = 0x80;

                /* Write the frame to the stack */
                if (sizeof(frame)!=process->mem_write(&frame, frame_va, sizeof frame))
                    return -EFAULT;
            }

            /* New signal mask */
            signal_mask |= sa.mask;
            signal_mask |= (uint64_t)1 << (signo-1);
            sighand.sigprocmask(SIG_SETMASK, &signal_mask, NULL);

            if (DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get())) {
                /* Clear flags per ABI for function entry. */
                operators()->writeRegister(x86->REG_DF, operators()->boolean_(false));
                operators()->writeRegister(x86->REG_TF, operators()->boolean_(false));

                /* Set up registers for signal handler. */
                size_t wordWidth = x86->REG_anyIP.nBits();
                ASSERT_require(wordWidth==32 || wordWidth==64);
                operators()->writeRegister(x86->REG_anyAX, operators()->number_(wordWidth, signo));
                operators()->writeRegister(x86->REG_anyCX, operators()->number_(wordWidth, 0));
                operators()->writeRegister(x86->REG_DS, operators()->number_(16, 0x2b));
                operators()->writeRegister(x86->REG_ES, operators()->number_(16, 0x2b));
                operators()->writeRegister(x86->REG_SS, operators()->number_(16, 0x2b));
                operators()->writeRegister(x86->REG_CS, operators()->number_(16, 0x23));
                operators()->writeRegister(x86->REG_anySP, operators()->number_(wordWidth, frame_va));
                operators()->writeRegister(x86->REG_anyIP, operators()->number_(wordWidth, sa.handlerVa));
            } else {
                TODO("architecture not supported");
            }
        }
    }

    get_callbacks().call_signal_callbacks(RSIM_Callbacks::AFTER, this, signo, &info,
                                          RSIM_Callbacks::SignalCallback::DELIVERY, cb_status);
    return 0;
}

int
RSIM_Thread::sys_rt_sigreturn()
{
#if 1 // [Robb P. Matzke 2015-05-28]
    ASSERT_not_implemented("[Robb P. Matzke 2015-05-28]");
#else
    Sawyer::Message::Stream mesg(tracing(TRACE_SIGNAL));

    /* Sighandler frame address is four less than the current SP because the return from sighandler popped the frame's
     * pretcode. Unlike the sigframe_32 stack frame, the rt_sigframe_32 stack frame's retcode does not pop the signal number
     * nor the other handler arguments, and why should it since we're about to restore the hardware context anyway. */
    uint32_t sp = operators()->readRegister(dispatcher()->REG_ESP)->get_number();
    uint32_t frame_va = sp - 4;

    RSIM_SignalHandling::rt_sigframe_32 frame;
    if (sizeof(frame)!=process->mem_read(&frame, frame_va, sizeof frame)) {
        mfprintf(mesg)("bad frame 0x%08" PRIu32" in sigreturn (sp=0x%08" PRIu32")\n", frame_va, sp);
        return -EFAULT;
    }

    if (mesg) {
        mesg <<"returning from ";
        print_enum(mesg, signal_names, frame.signo);
        mesg <<" handler\n";
    }

    /* Restore previous signal mask */
    int status = sighand.sigprocmask(SIG_SETMASK, &frame.uc.uc_sigmask, NULL);
    if (status<0)
        return -EFAULT;

    /* Restore hardware context */
    pt_regs_32 regs;
    uint32_t eflags = operators()->readRegister(dispatcher()->REG_EFLAGS)->get_number();
    sighand.restore_sigcontext(frame.uc.uc_mcontext, eflags, &regs);
    init_regs(regs);
    return 0;
#endif
}

/* Note: if the specimen's signal handler never returns then this function is never invoked.  The specimen may do a longjmp()
 * or siglongjmp(), in which case the original stack, etc are restored anyway. Additionally, siglongjmp() may do a system call
 * to set the signal mask back to the value saved by sigsetjmp(), if any. */
int
RSIM_Thread::sys_sigreturn()
{
    Sawyer::Message::Stream mesg(tracing(TRACE_SIGNAL));

    DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get());
    ASSERT_not_null(x86);

    /* Sighandler frame address is eight less than the current SP because the return from sighandler popped the frame's
     * pretcode, and the retcode popped the signo. */
    uint32_t sp = operators()->readRegister(x86->REG_ESP)->get_number();
    uint32_t frame_va = sp - 8; 

    RSIM_SignalHandling::sigframe_32 frame;
    if (sizeof(frame)!=process->mem_read(&frame, frame_va, sizeof frame)) {
        mfprintf(mesg)("bad frame 0x%08" PRIu32" in sigreturn (sp=0x%08" PRIu32")\n", frame_va, sp);
        return -EFAULT;
    }

    if (mesg) {
        mesg <<"returning from ";
        Printer::print_enum(mesg, signal_names, frame.signo);
        mesg <<" handler\n";
    }

    /* Restore previous signal mask */
    RSIM_SignalHandling::SigSet old_sigmask = frame.extramask;
    old_sigmask = (old_sigmask << 32) | frame.sc.oldmask;
    int status = sighand.sigprocmask(SIG_SETMASK, &old_sigmask, NULL);
    if (status<0)
        return -EFAULT;

    /* Restore hardware context.  Restore only certain flags. */
    PtRegs regs;
    uint32_t eflags = operators()->readRegister(x86->REG_EFLAGS)->get_number();
    sighand.restore_sigcontext(frame.sc, eflags, &regs);
    init_regs(regs);
    return 0;
}

int
RSIM_Thread::sys_sigprocmask(int how, const RSIM_SignalHandling::SigSet *in, RSIM_SignalHandling::SigSet *out)
{
    return sighand.sigprocmask(how, in, out);
}

void
RSIM_Thread::signal_clear_pending()
{
    sighand.clear_all_pending();
}

int
RSIM_Thread::signal_accept(const RSIM_SignalHandling::SigInfo &_info)
{
    RSIM_SignalHandling::SigInfo info = _info; /* non-const copy because callbacks may modify it */
    RSIM_SignalHandling::SigSet mask;
    int status = sighand.sigprocmask(0, NULL, &mask);
    if (status<0)
        return status;

    if ((sighand.mask_of(info.si_signo) & mask))
        return -EAGAIN;

    int retval = 0;
    bool cb_status = get_callbacks().call_signal_callbacks(RSIM_Callbacks::BEFORE, this, info.si_signo, &info,
                                                           RSIM_Callbacks::SignalCallback::ARRIVAL, true);
    if (cb_status)
        retval = sighand.generate(info, get_process(), tracing(TRACE_SIGNAL));
    get_callbacks().call_signal_callbacks(RSIM_Callbacks::AFTER, this, info.si_signo, &info,
                                          RSIM_Callbacks::SignalCallback::ARRIVAL, cb_status);

    return retval;
}

int
RSIM_Thread::signal_dequeue(RSIM_SignalHandling::SigInfo *info/*out*/)
{
    int signo = sighand.dequeue(info);
    if (0==signo) {
        RSIM_SignalHandling::SigSet mask;
        int status __attribute__((unused)) = sighand.sigprocmask(0, NULL, &mask);
        assert(status>=0);
        signo = get_process()->sighand.dequeue(info, &mask);
    }
    return signo;
}

/* Called by the thread that is invoking the fork system call. */
void
RSIM_Thread::atfork_prepare()
{
    RSIM_Process *p = get_process();

    /* Grab the simulator semaphore. We can have only one thread doing this at a time. */
    int status __attribute__((unused)) = TEMP_FAILURE_RETRY(sem_wait(p->get_simulator()->get_semaphore()));
    assert(0==status);

    /* Flush some files so buffered content isn't output twice. */
    fflush(stdout);
    fflush(stderr);
    if (p->tracingFile())
        fflush(p->tracingFile());
}

/* Called in the parent by the thread that invoked the fork system call. */
void
RSIM_Thread::atfork_parent()
{
    /* The parent is responsible for posting the global IPC semaphore.  This is safe to do now since the child process has its
     * own copy of RSIM data structures now. I.e., atfork_parent() isn't called until after the process has forked. */
    RSIM_Process *p = get_process();
    assert(NULL!=p);
    int status __attribute__((unused)) = sem_post(p->get_simulator()->get_semaphore());
    assert(0==status);
}

/* Called in the child by its main (only) thread.  RSIM data structures look like they did in the parent process, so we need to
 * fix up some things. */
void
RSIM_Thread::atfork_child()
{
    /* The simulator global IPC semaphore might or might not have been released yet by the parent process.  However, we now
     * have our own copy of fork_info and our process has only this one thread. */
    RSIM_Process *p = get_process();
    assert(p!=NULL);

    /* All threads have died in the child process except this main thread. */
    set_tid();
    p->set_main_thread(this);

    /* Thread (re)initialization */
    signal_clear_pending();     /* pending signals are for the parent process */
    operators()->nInsns(0);     /* restart instruction counter for trace output */

    /* Redirect tracing output for new process */
    p->open_tracing_file();
    p->btrace_close();
    reopen_trace_facilities();

    /* FIXME: According to the Linux 2.6.32 man page for pthread_atfork(), all mutexes must be reinitialized with
     * pthread_mutex_init. (FIXME: There may be mutexes in other parts of ROSE that need to be reinitialized--we need to make
     * sure that appropriate atfork callbacks were registered by those layers to reinitialize their mutexes).
     * [RPM 2012-01-19] */
}

void
RSIM_Thread::report_progress_maybe()
{
    Sawyer::Message::Stream mesg(tracing(TRACE_PROGRESS));
    if (mesg) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double report_delta = (now.tv_sec - last_report.tv_sec) + 1e-6 * (now.tv_usec - last_report.tv_usec);
        if (report_delta > report_interval) {
            const struct timeval &ctime = get_process()->get_ctime();
            double elapsed = (now.tv_sec - ctime.tv_sec) + 1e-6 * (now.tv_usec - ctime.tv_usec);
            double insn_rate = elapsed>0.0 ? get_ninsns() / elapsed : 0;
            mfprintf(mesg)("processed %zu insns in %d sec (%d insns/sec)\n",
                           get_ninsns(), (int)(elapsed+0.5), (int)(insn_rate+0.5));
            last_report = now;
        }
    }
}

void
RSIM_Thread::report_stack_frames(Sawyer::Message::Stream &mesg, const std::string &title/*=""*/, bool bp_not_saved/*=false*/)
{
    using namespace StringUtility;

    if (!mesg)
        return;
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(get_process()->rwlock());
    if (title.empty()) {
        mesg <<"stack frames:";
    } else {
        mfprintf(mesg)("%s", title.c_str());
    }

    DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get());
    DispatcherM68k *m68k = dynamic_cast<DispatcherM68k*>(dispatcher().get());

    const RegisterDescriptor IP = get_process()->disassembler()->instructionPointerRegister();
    const RegisterDescriptor SP = get_process()->disassembler()->stackPointerRegister();
    RegisterDescriptor BP;
    if (x86) {
        BP = x86->REG_anyBP;
    } else if (m68k) {
        BP = m68k->REG_A[6];
    }

    size_t wordWidth = IP.nBits();
    rose_addr_t bp = operators()->readRegister(BP)->get_number();
    rose_addr_t ip = operators()->readRegister(IP)->get_number();

    static const int maxStackFrames = 32;               // arbitrary
    for (int i=0; i<maxStackFrames; i++) {
        mesg <<"\n  #" <<i <<": bp=" <<addrToString(bp) <<" ip=" <<addrToString(ip);
        SgAsmInstruction *insn = NULL;
        try {
            insn = process->get_instruction(ip);
        } catch (const Disassembler::Exception&) {
            /* IP is probably pointing to non-executable memory or a bad address. */
        }
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
        if (func && !func->get_name().empty() && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS)) {
            mesg <<" in function " <<func->get_name();
        } else if (process->get_memory()->at(ip).exists()) {
            const MemoryMap::Segment &sgmt = process->get_memory()->find(ip)->value();
            if (!sgmt.name().empty())
                mesg <<" in memory region " <<sgmt.name();
        }

        if (bp_not_saved) {
            /* Presumably being called after a CALL but before EBP is saved on the stack.  In this case, the return address
             * of the inner-most function should be at ss:[esp], and containing functions have set up their stack
             * frames. */
            rose_addr_t sp = operators()->readRegister(SP)->get_number();
            if (32 == wordWidth) {
                uint32_t tmp;
                if (4!=process->mem_read(&tmp, sp, 4))
                    break;
                ip = tmp;
            } else {
                ASSERT_require(64 == wordWidth);
                uint64_t tmp;
                if (8!=process->mem_read(&tmp, sp, 8))
                    break;
                ip = tmp;
            }
            bp_not_saved = false;
            mesg <<" [no stack frame]";
        } else {
            /* This function has stored its incoming EBP on the stack at ss:[ebp].  This is usually accomplished by the
             * instructions PUSH EBP; MOV EBP, ESP. */
            if (32 == wordWidth) {
                uint32_t tmp;
                if (4!=process->mem_read(&tmp, bp+4, 4))
                    break;
                ip = tmp;
                if (4!=process->mem_read(&tmp, bp, 4))
                    break;
                bp = tmp;
            } else {
                ASSERT_require(64 == wordWidth);
                uint64_t tmp;
                if (8!=process->mem_read(&tmp, bp+8, 8))
                    break;
                ip = tmp;
                if (8!=process->mem_read(&tmp, bp, 8))
                    break;
                bp = tmp;
            }
        }
    }
    mesg <<"\n";
}

void
RSIM_Thread::syscall_return(const BaseSemantics::SValuePtr &retval)
{
    RegisterDescriptor reg = get_process()->get_simulator()->syscallReturnRegister();
    operators()->writeRegister(reg, retval);
}

void
RSIM_Thread::syscall_return(uint64_t retval)
{
    RegisterDescriptor reg = get_process()->get_simulator()->syscallReturnRegister();
    operators()->writeRegister(reg, operators()->number_(reg.nBits(), retval));
}

void
RSIM_Thread::post_insn_semaphore()
{
    if (!insn_semaphore_posted) {
        int status __attribute__((unused)) = sem_post(process->get_simulator()->get_semaphore());
        assert(0==status);
        insn_semaphore_posted = true;
    }
}

void
RSIM_Thread::waitForState(RunState state) {
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(mutex_);
    while (runState_ != state)
        runStateChanged_.wait(mutex_);
}

void
RSIM_Thread::setState(RunState state) {
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(mutex_);
    runState_ = state;
    runStateChanged_.notify_all();
}

void
RSIM_Thread::start() {
    setState(RUNNING);
}

/* Executed by a real thread to simulate a specimen's thread. */
void *
RSIM_Thread::main()
{
    waitForState(RUNNING);
    get_callbacks().call_thread_callbacks(RSIM_Callbacks::BEFORE, this, true);
    RSIM_Process *process = get_process();

    const RegisterDescriptor IP = get_process()->disassembler()->instructionPointerRegister();
    DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get()); // might be null

    boost::this_thread::disable_interruption interruptionsAreDisabled;
    while (true) {
        boost::this_thread::interruption_point();
        report_progress_maybe();
        try {
            /* Returned from signal handler? This code simulates the sigframe_32 or rt_sigframe_32 "retcode" */
            uint64_t ip = operators()->readRegister(IP)->get_number();
            if (ip == SIGHANDLER_RETURN) {
                ASSERT_not_null(x86);
                pop();
                operators()->writeRegister(x86->REG_anyAX, operators()->number_(x86->REG_anyAX.nBits(), 119));
                sys_sigreturn();
                continue;
                
            } else if (ip == SIGHANDLER_RT_RETURN) {
                operators()->writeRegister(x86->REG_anyAX, operators()->number_(x86->REG_anyAX.nBits(), 173));
                sys_rt_sigreturn();
                continue;
            }

            /* Handle a signal if we have any pending that aren't masked */
            RSIM_SignalHandling::SigInfo info;
            process->signal_dispatch(); /* assign process signals to threads */
            if (signal_dequeue(&info)>0)
                signal_deliver(info);

            /* Find the instruction.  Callbacks might change the value of the EIP register, in which case we should re-fetch
             * the instruction. The pre-instruction callbacks will be invoked for each re-fetched instruction, but the
             * post-instruction callback is only invoked for the final instruction. */
            SgAsmInstruction *insn = NULL;
            bool cb_status;
            do {
                insn = current_insn();
                cb_status = callbacks.call_insn_callbacks(RSIM_Callbacks::BEFORE, this, insn, true);
            } while (insn->get_address()!=operators()->readRegister(IP)->get_number());

            /* Simulate an instruction.  In order to make our simulated instructions atomic (at least among the simulators) we
             * use a shared semaphore that was created in RSIM_Thread::ctor(). */
            if (cb_status) {
                process->binary_trace_add(this, insn);
                int status __attribute__((unused)) = TEMP_FAILURE_RETRY(sem_wait(process->get_simulator()->get_semaphore()));
                assert(0==status);
                insn_semaphore_posted = false;
                dispatcher()->processInstruction(insn); // blocking syscalls will post, and set insn_semaphore_posted
                post_insn_semaphore();
            }
            callbacks.call_insn_callbacks(RSIM_Callbacks::AFTER, this, insn, cb_status);

            Sawyer::Message::Stream &mesg = tracing(TRACE_STATE);
            if (mesg)
                mesg <<*operators()->currentState()->registerState();
        } catch (const Disassembler::Exception &e) {
            post_insn_semaphore();
            if (show_exceptions) {
                std::ostringstream s;
                s <<e;
                mfprintf(tracing(TRACE_MISC))("caught Disassembler::Exception: %s\n", s.str().c_str());
            }
            if (do_coredump) {
                mfprintf(tracing(TRACE_MISC))("dumping core...\n");
                process->dump_core(SIGSEGV);
                report_stack_frames(tracing(TRACE_MISC));
            }
            setState(TERMINATED);
            throw;
        } catch (const BaseSemantics::Exception &e) {
            /* Thrown for instructions whose semantics are not implemented yet. */
            post_insn_semaphore();
            if (show_exceptions) {
                std::ostringstream s;
                s <<e;
                mfprintf(tracing(TRACE_MISC))("semantics exception: %s\n", s.str().c_str());
            }
#ifdef X86SIM_STRICT_EMULATION
            if (do_coredump) {
                tracing(TRACE_MISC) <<"dumping core...\n";
                process->dump_core(SIGILL);
                report_stack_frames(tracing(TRACE_MISC));
                abort();
            } else {
                setState(TERMINATED);
                throw;
            }
#else
            if (show_exceptions) {
                report_stack_frames(tracing(TRACE_MISC));
                tracing(TRACE_MISC) <<"exception ignored; continuing with a corrupt state...\n";
            }

            // Advance to next instruction without regard for any branching
            SgAsmInstruction *insn = current_insn();
            operators()->writeRegister(IP, operators()->number_(IP.nBits(), insn->get_address() + insn->get_size()));
#endif
        } catch (const RSIM_Semantics::Halt &e) {
            // Thrown for the HLT instruction
            post_insn_semaphore();
            if (show_exceptions)
                tracing(TRACE_MISC) <<"semantics halt exception\n";
            setState(TERMINATED);
            throw;
        } catch (const RSIM_Semantics::Interrupt &e) {
            // thrown for the INT instruction if the interrupt is not handled
            post_insn_semaphore();
            if (show_exceptions)
                tracing(TRACE_MISC) <<"unhandled specimen interrupt from INT insn\n";
            setState(TERMINATED);
            throw;
        } catch (const RSIM_Process::Exit &e) {
            post_insn_semaphore();
            sys_exit(e);
            setState(TERMINATED);
            return NULL;
        } catch (RSIM_SignalHandling::SigInfo &e) {
            post_insn_semaphore();
            if (e.si_signo) {
                bool cb_status = get_callbacks().call_signal_callbacks(RSIM_Callbacks::BEFORE, this, e.si_signo, &e,
                                                                       RSIM_Callbacks::SignalCallback::ARRIVAL, true);
                if (cb_status)
                    sighand.generate(e, process, tracing(TRACE_SIGNAL));
                get_callbacks().call_signal_callbacks(RSIM_Callbacks::AFTER, this, e.si_signo, &e,
                                                      RSIM_Callbacks::SignalCallback::ARRIVAL, cb_status);
            }
        } catch (...) {
            post_insn_semaphore();
            setState(TERMINATED);
            throw;
        }
    }
}

PtRegs
RSIM_Thread::get_regs() const
{
    PtRegs regs;

    DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get());
    ASSERT_not_null(x86);

    // 32-bit registers (stored as 64-bit values)
    regs.ip = operators()->readRegister(x86->REG_anyIP)->get_number();
    regs.ax = operators()->readRegister(x86->REG_anyAX)->get_number();
    regs.bx = operators()->readRegister(x86->REG_anyBX)->get_number();
    regs.cx = operators()->readRegister(x86->REG_anyCX)->get_number();
    regs.dx = operators()->readRegister(x86->REG_anyDX)->get_number();
    regs.si = operators()->readRegister(x86->REG_anySI)->get_number();
    regs.di = operators()->readRegister(x86->REG_anyDI)->get_number();
    regs.bp = operators()->readRegister(x86->REG_anyBP)->get_number();
    regs.sp = operators()->readRegister(x86->REG_anySP)->get_number();
    regs.cs = operators()->readRegister(x86->REG_CS)->get_number();
    regs.ds = operators()->readRegister(x86->REG_DS)->get_number();
    regs.es = operators()->readRegister(x86->REG_ES)->get_number();
    regs.fs = operators()->readRegister(x86->REG_FS)->get_number();
    regs.gs = operators()->readRegister(x86->REG_GS)->get_number();
    regs.ss = operators()->readRegister(x86->REG_SS)->get_number();
    regs.flags = operators()->readRegister(x86->REG_anyFLAGS)->get_number();

    // additional 64-bit registers
    if (get_process()->wordSize() == 64) {
        regs.r15 = operators()->readRegister(x86->REG_R15)->get_number();
        regs.r14 = operators()->readRegister(x86->REG_R14)->get_number();
        regs.r13 = operators()->readRegister(x86->REG_R13)->get_number();
        regs.r12 = operators()->readRegister(x86->REG_R12)->get_number();
        regs.r11 = operators()->readRegister(x86->REG_R11)->get_number();
        regs.r10 = operators()->readRegister(x86->REG_R10)->get_number();
        regs.r9 = operators()->readRegister(x86->REG_R9)->get_number();
        regs.r8 = operators()->readRegister(x86->REG_R8)->get_number();
    }

    return regs;
}

void
RSIM_Thread::init_regs(const PtRegs &regs)
{
    size_t wordWidth = get_process()->wordSize();

    if (DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get())) {
        // Registers in common for a 32-bit and 64-bit architecture
        operators()->writeRegister(x86->REG_anyIP, operators()->number_(wordWidth, regs.ip));
        operators()->writeRegister(x86->REG_anyAX, operators()->number_(wordWidth, regs.ax));
        operators()->writeRegister(x86->REG_anyBX, operators()->number_(wordWidth, regs.bx));
        operators()->writeRegister(x86->REG_anyCX, operators()->number_(wordWidth, regs.cx));
        operators()->writeRegister(x86->REG_anyDX, operators()->number_(wordWidth, regs.dx));
        operators()->writeRegister(x86->REG_anySI, operators()->number_(wordWidth, regs.si));
        operators()->writeRegister(x86->REG_anyDI, operators()->number_(wordWidth, regs.di));
        operators()->writeRegister(x86->REG_anyBP, operators()->number_(wordWidth, regs.bp));
        operators()->writeRegister(x86->REG_anySP, operators()->number_(wordWidth, regs.sp));
        operators()->writeRegister(x86->REG_CS, operators()->number_(16, regs.cs));
        operators()->writeRegister(x86->REG_DS, operators()->number_(16, regs.ds));
        operators()->writeRegister(x86->REG_ES, operators()->number_(16, regs.es));
        operators()->writeRegister(x86->REG_FS, operators()->number_(16, regs.fs));
        operators()->writeRegister(x86->REG_GS, operators()->number_(16, regs.gs));
        operators()->writeRegister(x86->REG_SS, operators()->number_(16, regs.ss));
        operators()->writeRegister(x86->REG_anyFLAGS, operators()->number_(wordWidth, regs.flags));

        // Registers only existing on a 64-bit architecture
        if (64 == wordWidth) {
            operators()->writeRegister(x86->REG_R15, operators()->number_(wordWidth, regs.r15));
            operators()->writeRegister(x86->REG_R14, operators()->number_(wordWidth, regs.r14));
            operators()->writeRegister(x86->REG_R13, operators()->number_(wordWidth, regs.r13));
            operators()->writeRegister(x86->REG_R12, operators()->number_(wordWidth, regs.r12));
            operators()->writeRegister(x86->REG_R11, operators()->number_(wordWidth, regs.r11));
            operators()->writeRegister(x86->REG_R10, operators()->number_(wordWidth, regs.r10));
            operators()->writeRegister(x86->REG_R9, operators()->number_(wordWidth, regs.r9));
            operators()->writeRegister(x86->REG_R8, operators()->number_(wordWidth, regs.r8));
        }
    } else if (DispatcherM68k *m68k = dynamic_cast<DispatcherM68k*>(dispatcher().get())) {
        operators()->writeRegister(m68k->REG_PC, operators()->number_(32, regs.ip));
        operators()->writeRegister(m68k->REG_CCR, operators()->number_(8, regs.flags));
        operators()->writeRegister(m68k->REG_SR, operators()->number_(16, regs.sr));
        operators()->writeRegister(m68k->REG_D[0], operators()->number_(32, regs.d0));
        operators()->writeRegister(m68k->REG_D[1], operators()->number_(32, regs.d1));
        operators()->writeRegister(m68k->REG_D[2], operators()->number_(32, regs.d2));
        operators()->writeRegister(m68k->REG_D[3], operators()->number_(32, regs.d3));
        operators()->writeRegister(m68k->REG_D[4], operators()->number_(32, regs.d4));
        operators()->writeRegister(m68k->REG_D[5], operators()->number_(32, regs.d5));
        operators()->writeRegister(m68k->REG_D[6], operators()->number_(32, regs.d6));
        operators()->writeRegister(m68k->REG_D[7], operators()->number_(32, regs.d7));
        operators()->writeRegister(m68k->REG_A[0], operators()->number_(32, regs.a0));
        operators()->writeRegister(m68k->REG_A[1], operators()->number_(32, regs.a1));
        operators()->writeRegister(m68k->REG_A[2], operators()->number_(32, regs.a2));
        operators()->writeRegister(m68k->REG_A[3], operators()->number_(32, regs.a3));
        operators()->writeRegister(m68k->REG_A[4], operators()->number_(32, regs.a4));
        operators()->writeRegister(m68k->REG_A[5], operators()->number_(32, regs.a5));
        operators()->writeRegister(m68k->REG_A[6], operators()->number_(32, regs.a6));
        operators()->writeRegister(m68k->REG_A[7], operators()->number_(32, regs.a7));
    }
}

int
RSIM_Thread::set_gdt(const SegmentDescriptor &ud)
{
    SegmentDescriptor &entry = gdt_entry(ud.entry_number);
    entry = ud;

    DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get());
    ASSERT_not_null(x86);

    /* Make sure all affected shadow registers are reloaded. */
    operators()->writeRegister(x86->REG_CS, operators()->readRegister(x86->REG_CS));
    operators()->writeRegister(x86->REG_DS, operators()->readRegister(x86->REG_DS));
    operators()->writeRegister(x86->REG_ES, operators()->readRegister(x86->REG_ES));
    operators()->writeRegister(x86->REG_FS, operators()->readRegister(x86->REG_FS));
    operators()->writeRegister(x86->REG_GS, operators()->readRegister(x86->REG_GS));
    operators()->writeRegister(x86->REG_SS, operators()->readRegister(x86->REG_SS));

    return ud.entry_number;
}

SegmentDescriptor&
RSIM_Thread::gdt_entry(int idx)
{
    if (idx >= RSIM_Process::GDT_ENTRY_TLS_MIN &&
        idx <= RSIM_Process::GDT_ENTRY_TLS_MAX) {
        return tls_array[idx - RSIM_Process::GDT_ENTRY_TLS_MIN];
    }
    return get_process()->gdt_entry(idx);
}

int
RSIM_Thread::get_free_tls() const
{
    for (int idx=0; idx<RSIM_Process::GDT_ENTRY_TLS_ENTRIES; idx++) {
        if (tls_array[idx].entry_number == SegmentDescriptor().entry_number)
            return idx + RSIM_Process::GDT_ENTRY_TLS_MIN;
    }
    return -ESRCH;
}

int
RSIM_Thread::set_thread_area(SegmentDescriptor &info, bool can_allocate)
{
    int idx = info.entry_number;

    if (-1==idx && can_allocate) {
        idx = get_free_tls();
        assert(idx<0x7fffffffLL);
        if (idx < 0)
            return idx;
        info.entry_number = idx;
        mfprintf(tracing(TRACE_SYSCALL))("[entry #%d]", idx);
    }

    if (idx<(int)RSIM_Process::GDT_ENTRY_TLS_MIN || idx>(int)RSIM_Process::GDT_ENTRY_TLS_MAX)
        return -EINVAL;

    set_gdt(info);
    return idx;
}

int
RSIM_Thread::handle_futex_death(uint32_t futex_va, Sawyer::Message::Stream &trace)
{
    uint32_t futex;
    mfprintf(trace)("\n  handling death for futex at 0x%08" PRIx32"\n", futex_va);

    if (4!=get_process()->mem_read(&futex, futex_va, 4)) {
        mfprintf(trace)("    failed to read futex at 0x%08" PRIx32"\n", futex_va);
        return -EFAULT;
    }

    /* If this thread owns the futex then set the FUTEX_OWNER_DIED and signal the futex. */
    if (get_tid()==(int)(futex & 0x1fffffff)) {
        /* Set the FUTEX_OWNER_DIED bit */
        mfprintf(trace)("    setting FUTEX_OWNER_DIED bit\n");
        futex |= 0x40000000;
        if (4!=get_process()->mem_write(&futex, futex_va, 4)) {
            mfprintf(trace)("      failed to set FUTEX_OWNER_DIED for futex at 0x%08" PRIx32"\n", futex_va);
            return -EFAULT;
        }

        /* Wake another thread there's one waiting */
        if (futex & 0x80000000) {
            mfprintf(trace)("    waking futex 0x%08" PRIx32"\n", futex_va);
            int result = futex_wake(futex_va, 1);
            if (result<0) {
                mfprintf(trace)("      wake failed for futex at 0x%08" PRIu32"\n", futex_va);
                return result;
            }
        }
    } else {
        mfprintf(trace)("    futex is not owned by this thread; skipping\n");
    }
    return 0;
}
    
int
RSIM_Thread::exit_robust_list()
{
    int retval = 0;

    if (0==robustListHeadVa_)
        return 0;

    Sawyer::Message::Stream trace(tracing(TRACE_THREAD));
    trace <<"exit_robust_list()...";

    robust_list_head_64 head64;
    if (get_process()->wordSize() == 32) {
        robust_list_head_32 head32;
        if (sizeof(head32) != get_process()->mem_read(&head32, robustListHeadVa_, sizeof head32)) {
            trace <<" <failed to read robust list head at " <<StringUtility::addrToString(robustListHeadVa_) <<">";
            retval = -EFAULT;
        }
        head64 = robust_list_head_64(head32);
    } else {
        ASSERT_require(get_process()->wordSize() == 64);
        if (sizeof(head64) != get_process()->mem_read(&head64, robustListHeadVa_, sizeof head64)) {
            trace <<" <failed to read robust list head at " <<StringUtility::addrToString(robustListHeadVa_) <<">";
            retval = -EFAULT;
        }
    }

    if (0 == retval) {
        static const size_t max_locks = 1000000;
        size_t nlocks = 0;
        rose_addr_t lock_entry_va = head64.next_va;
        while (lock_entry_va != robustListHeadVa_ && nlocks++ < max_locks) {
            /* Don't process futex if it's pending; we'll catch it at the end instead. */
            if (lock_entry_va != head64.pending_va) {
                rose_addr_t futex_va = lock_entry_va + head64.futex_offset;
                if ((retval = handle_futex_death(futex_va, trace))<0)
                    break;
            }
        
            /* Advance lock_entry_va to next item in the list. */
            if (get_process()->wordSize() == 32) {
                lock_entry_va = 0;
                if (4!=get_process()->mem_read(&lock_entry_va, lock_entry_va, 4)) {
                    trace <<" <list pointer read failed at " <<StringUtility::addrToString(lock_entry_va) <<">";
                    retval = -EFAULT;
                    break;
                }
            } else {
                lock_entry_va = 0;
                if (8!=get_process()->mem_read(&lock_entry_va, lock_entry_va, 8)) {
                    trace <<" <list pointer read failed at " <<StringUtility::addrToString(lock_entry_va) <<">";
                    retval = -EFAULT;
                    break;
                }
            }
        }
        if (head64.pending_va)
            retval = handle_futex_death(head64.pending_va, trace);
    }

    trace <<" done.\n";
    return retval;
}

void
RSIM_Thread::do_clear_child_tid()
{
    if (clearChildTidVa_) {
        tracing(TRACE_SYSCALL) <<"clearing child tid...\n";
        uint32_t zero = 0;
        size_t n = get_process()->mem_write(&zero, clearChildTidVa_, sizeof zero);
        if (n!=sizeof zero) {
            tracing(TRACE_SYSCALL) <<"cannot write clear_child_tid address "
                                   <<StringUtility::addrToString(clearChildTidVa_) <<"\n";
        } else {
            tracing(TRACE_SYSCALL) <<"waking futex " <<StringUtility::addrToString(clearChildTidVa_) <<"\n";
            int nwoke = futex_wake(clearChildTidVa_, INT_MAX);
            if (nwoke<0)
                tracing(TRACE_SYSCALL) <<"wake futex " <<StringUtility::addrToString(clearChildTidVa_)
                                       <<" failed with " <<nwoke <<"\n";
        }
    }
}

int
RSIM_Thread::sys_exit(const RSIM_Process::Exit &e)
{
    RSIM_Process *process = get_process(); /* while we still have a chance */

    mfprintf(tracing(TRACE_THREAD))("this thread is terminating%s\n", e.exit_process?" (for entire process)":"");

    /* Clean up robust futexes */
    exit_robust_list();

    /* Clear and signal child TID if necessary (CLONE_CHILD_CLEARTID) */
    do_clear_child_tid();

    /* Remove the child from the process. */
    process->remove_thread(this); /* thread safe */
    this->process = NULL;         /* must occur after remove_thread() */

    /* Cause the entire process to exit if necesary. */
    if (e.exit_process)
        process->sys_exit(e.status);

    return e.status;
}

int
RSIM_Thread::sys_tgkill(pid_t pid, pid_t tid, int signo)
{
    int retval = 0;
    RSIM_Process *process = get_process();
    assert(process!=NULL);
    
    RSIM_SignalHandling::SigInfo info = RSIM_SignalHandling::mk_rt(signo, SI_TKILL);

    if (pid<0) {
        retval = -EINVAL;
    } else if (pid==getpid() && tid>=0) {
        RSIM_Thread *thread = process->get_thread(tid);
        if (!thread) {
            retval = -ESRCH;
        } else {
            bool cb_status = get_callbacks().call_signal_callbacks(RSIM_Callbacks::BEFORE, thread, signo, &info,
                                                                   RSIM_Callbacks::SignalCallback::ARRIVAL, true);
            if (cb_status)
                retval = thread->sighand.generate(info, process, thread->tracing(TRACE_SIGNAL));

            thread->get_callbacks().call_signal_callbacks(RSIM_Callbacks::AFTER, thread, signo, &info,
                                                          RSIM_Callbacks::SignalCallback::ARRIVAL, cb_status);
            if (thread!=this && retval>=0)
                retval = syscall(SYS_tgkill, pid, tid, RSIM_SignalHandling::SIG_WAKEUP);
        }
    } else {
        retval = sys_kill(pid, info);
    }
    return retval;
}

int
RSIM_Thread::sys_kill(pid_t pid, const RSIM_SignalHandling::SigInfo &info)
{
    RSIM_Process *process = get_process();
    assert(process!=NULL);
    return process->sys_kill(pid, info);
}

int
RSIM_Thread::sys_sigpending(RSIM_SignalHandling::SigSet *result)
{
    RSIM_SignalHandling::SigSet p1, p2;
    int status = sighand.sigpending(&p1);
    if (status<0)
        return status;

    status = get_process()->sighand.sigpending(&p2);
    if (status<0)
        return status;

    if (result)
        *result = p1 | p2;
    return 0;
}

int
RSIM_Thread::sys_sigsuspend(const RSIM_SignalHandling::SigSet *mask) {
    int signo = sighand.sigsuspend(mask, this);
    return signo;
}

int
RSIM_Thread::sys_sigaltstack(const stack_32 *in, stack_32 *out)
{
    const RegisterDescriptor SP = get_process()->disassembler()->stackPointerRegister();
    uint32_t sp = operators()->readRegister(SP)->get_number();
    return sighand.sigaltstack(in, out, sp);
}

void
RSIM_Thread::emulate_syscall()
{
    /* Post the instruction semphore because some system calls can block indefinitely. */
    int status __attribute__((unused)) = sem_post(get_process()->get_simulator()->get_semaphore());
    assert(0==status);
    insn_semaphore_posted = true;

    DispatcherX86 *x86 = dynamic_cast<DispatcherX86*>(dispatcher().get());
    ASSERT_not_null(x86);

    unsigned callno = operators()->readRegister(x86->REG_anyAX)->get_number();
    bool cb_status = callbacks.call_syscall_callbacks(RSIM_Callbacks::BEFORE, this, callno, true);
    if (cb_status) {
        RSIM_Simulator *sim = get_process()->get_simulator();
        if (sim->syscall_is_implemented(callno)) {
            RSIM_Simulator::SystemCall *sc = sim->syscall_implementation(callno);
            sc->enter.apply(true, RSIM_Simulator::SystemCall::Callback::Args(this, callno));
            sc->body .apply(true, RSIM_Simulator::SystemCall::Callback::Args(this, callno));
            sc->leave.apply(true, RSIM_Simulator::SystemCall::Callback::Args(this, callno));
        } else {
            char name[32];
            sprintf(name, "syscall_%u", callno);
            mfprintf(tracing(TRACE_MISC))("syscall_%u(", callno);
            for (int i=0; get_process()->get_simulator()->syscallArgumentRegisters().size(); ++i)
                mfprintf(tracing(TRACE_MISC))("%s0x%08" PRIx64, i?", ":"", syscall_arg(i));
            tracing(TRACE_MISC) <<") is not implemented\n";

            tracing(TRACE_MISC) <<"dumping core...\n";
            get_process()->dump_core(SIGSYS);
            report_stack_frames(tracing(TRACE_MISC));
            abort();
        }
    }
    callbacks.call_syscall_callbacks(RSIM_Callbacks::AFTER, this, callno, cb_status);
}

/* Not thread safe; obtain a process-wide lock first */
rose_addr_t
RSIM_Thread::futex_key(rose_addr_t va, uint32_t **val_ptr)
{
    Sawyer::Message::Stream trace(tracing(TRACE_FUTEX));

    /* Futexes must be word aligned */
    if (va & 3)
        return 0;

    /* Find the simulator address. */
    *val_ptr = (uint32_t*)process->my_addr(va, 4);
    rose_addr_t addr = (rose_addr_t)*val_ptr;
    mfprintf(trace)("futex: specimen va 0x%08" PRIx64" is at simulator address 0x%08" PRIx64"\n", va, addr);

    /* Does the simulator address fall inside a file? */
    FILE *f = fopen("/proc/self/maps", "r");
    assert(f!=NULL);
    char *line = NULL;
    size_t lsize = 0;
    while (getline(&line, &lsize, f)>0) {
        char *s=line, *rest;

        rose_addr_t lo_addr = strtoull(s, &rest, 16);
        assert(rest && *rest=='-');
        s = rest+1;
        
        rose_addr_t hi_addr = strtoull(s, &rest, 16);
        assert(rest && *rest==' ');
        s = rest+1;

        if (*s == 'r') ++s;
        if (*s == 'w') ++s;
        if (*s == 'x') ++s;
        if (*s == 'p') ++s;
        assert(' '==*s);
        s++;

        rose_addr_t map_offset = strtoull(s, &rest, 16);
        assert(rest && *rest==' ');
        s = rest+1;

        unsigned devno = strtoul(s, &rest, 16);
        assert(rest && *rest==':');
        s = rest+1;
        devno += (devno<<16) + strtoul(s, &rest, 16);
        assert(rest && *rest==' ');
        s = rest+1;

        unsigned __attribute__((unused)) filesz = strtoull(s, &rest, 16);
        assert(rest && *rest==' ');
        s = rest+1;
        
        while (' '==*s) s++;
        assert(*s);
        char *filename = s;
        s[strlen(s)-1] = '\0'; // chomp linefeed

        if (devno!=0 && addr>=lo_addr && addr+4<=hi_addr) {
            struct stat sb;
            if (stat(filename, &sb)<0) {
                mfprintf(trace)("futex: cannot stat file \"%s\"\n", filename);
                break;
            }

            rose_addr_t addr_in_file = (addr - lo_addr) + map_offset;
            mfprintf(trace)("futex: simulator address 0x%08" PRIx64" is at %s+0x%08" PRIx64"\n", addr, filename, addr_in_file);
            addr = - ((map_offset<<16) + addr_in_file);
            addr ^= sb.st_ino;
            addr &= 0xffffffff;
            addr |= 1; // so as not to conflict with simulator addresses, which are most likely word aligned.
            assert(0!=addr);
            mfprintf(trace)("futex: using generated address 0x%08" PRIx64"\n", addr);
            break;
        }
    }
    fclose(f);
    return addr;
}

int
RSIM_Thread::futex_wait(rose_addr_t va, uint32_t oldval, uint32_t bitset)
{
    int retval = 0;
    Sawyer::Message::Stream trace(tracing(TRACE_FUTEX));

    /* We need to access the futex memory atomically with respect to instructions from other threads and/or processes.  So we'll
     * use the simulator's global semaphore for that purpose. */
    assert(insn_semaphore_posted);
    int status = sem_wait(process->get_simulator()->get_semaphore());
    if (-1==status && EINTR==errno)
        return -errno;
    assert(0==status);

    uint32_t *futex_ptr = NULL;
    rose_addr_t key = futex_key(va, &futex_ptr);
    if (!key || !futex_ptr) {
        retval = -EFAULT;
    } else if (oldval != *futex_ptr) {
        mfprintf(trace)("futex wait: futex value %" PRIu32" but need %" PRIu32"\n", *futex_ptr, oldval);
        retval = -EWOULDBLOCK;
    }

    /* Place this process on the futex waiting queue. */
    int futex_number = -1;
    if (0==retval) {
        futex_number = process->get_futexes()->insert(key, bitset, RSIM_FutexTable::LOCKED);
        if (futex_number<0)
            retval = futex_number;
    }

    /* Release the global semaphore. */
    status = sem_post(process->get_simulator()->get_semaphore());
    assert(0==status);

    if (futex_number>=0) {
        /* Block until we're signaled */
        trace <<"futex wait: about to block...\n";
        status = process->get_futexes()->wait(futex_number);
        if (-EINTR==status) {
            trace <<"futex wait: interrupted by signal\n";
            retval = -EINTR;
        } else {
            assert(0==status);
            trace <<"futex wait: resumed\n";
        }

        /* Remove the semaphore from the table. */
        status = process->get_futexes()->erase(key, futex_number, RSIM_FutexTable::UNLOCKED);
        assert(0==status);
    }

    return retval;
}

int
RSIM_Thread::futex_wake(rose_addr_t va, int nprocs, uint32_t bitset)
{
    int retval = 0;
    Sawyer::Message::Stream trace(tracing(TRACE_FUTEX));

    /* The futex wait queues are protected by the simulator global semaphore (the same one used to make instructions atomic). */
    assert(insn_semaphore_posted);
    int status = sem_wait(process->get_simulator()->get_semaphore());
    if (-1==status && EINTR==errno)
        return -errno;
    assert(0==status);

    uint32_t *futex_ptr = NULL;
    rose_addr_t key = futex_key(va, &futex_ptr);
    if (!key || !futex_ptr) {
        retval = -EFAULT;
    }
    
    /* Wake processes that are waiting on this semaphore */
    retval = process->get_futexes()->signal(key, bitset, nprocs, RSIM_FutexTable::LOCKED);
    if (retval<0) {
        mfprintf(trace)("futex wake: failed with error %d\n", retval);
    } else {
        mfprintf(trace)("futex wake: %d proc%s have been signaled\n", retval, 1==retval?"":"s");
    }

    /* Release the lock */
    status = sem_post(process->get_simulator()->get_semaphore());
    assert(0==status);

    return retval;
}

#endif /* ROSE_ENABLE_SIMULATOR */
