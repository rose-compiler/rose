#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <errno.h>
#include <stdarg.h>
#include <syscall.h>

#include <sys/user.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

size_t RSIM_Thread::next_sequence_number = 1;

/* Constructor */
void
RSIM_Thread::ctor()
{
    set_tid();
    my_seq = next_sequence_number++;

    memset(&last_report, 0, sizeof last_report);
    memset(tls_array, 0, sizeof tls_array);

    reopen_trace_facilities(process->get_tracing_file());
}

void
RSIM_Thread::set_tid()
{
    my_tid = syscall(SYS_gettid);
    assert(my_tid>0);
}

void
RSIM_Thread::reopen_trace_facilities(FILE *file)
{
    for (int tf=0; tf<TRACE_NFACILITIES; tf++) {
        if (trace_mesg[tf]==NULL)
            trace_mesg[tf] = new RTS_Message(NULL, &mesg_prefix);
        if ((process->get_tracing_flags() & tracingFacilityBit((TracingFacility)tf)))
            trace_mesg[tf]->set_file(file);
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
    uint64_t eip = policy.readRegister<32>(policy.reg_eip).is_known() ? policy.readRegister<32>(policy.reg_eip).known_value() : 0;
    int n = snprintf(buf2, sizeof(buf2), "0x%08"PRIx64"[%zu]: ", eip, policy.get_ninsns());
    assert(n>=0 && (size_t)n<sizeof(buf2)-1);
    memset(buf2+n, ' ', sizeof(buf2)-n);
    buf2[std::max(n, 21)] = '\0';

    return (StringUtility::numberToString(getpid()) + ":" + StringUtility::numberToString(my_seq) +
            " " + buf1 +
            " " + buf2);

}

RTS_Message *
RSIM_Thread::tracing(TracingFacility tf)
{
    assert(tf>=0 && tf<TRACE_NFACILITIES);
    assert(trace_mesg[tf]!=NULL);
    return trace_mesg[tf];
}

SgAsmx86Instruction *
RSIM_Thread::current_insn()
{
    rose_addr_t ip = policy.readRegister<32>(policy.reg_eip).known_value();
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(get_process()->get_instruction(ip));
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    return insn;
}


void
RSIM_Thread::syscall_arginfo(char format, uint32_t val, ArgInfo *info, va_list *ap)
{
    ROSE_ASSERT(info!=NULL);
    info->val = val;
    switch (format) {
        case 'f':       /*flags*/
        case 'e':       /*enum*/
            info->xlate = va_arg(*ap, const Translate*);
            break;
        case 's': {     /*NUL-terminated string*/
            info->str = get_process()->read_string(val, 4096, &(info->str_fault));
            info->str_trunc = (info->str.size() >= 4096);
            break;
        }
        case 'b': {     /* buffer */
            size_t advertised_size = va_arg(*ap, size_t);
            assert(advertised_size<10*1000*1000);
            info->struct_buf = new uint8_t[advertised_size];
            info->struct_nread = get_process()->mem_read(info->struct_buf, info->val, advertised_size);
            info->struct_size = 64; /* max print width, measured in columns of output */
            break;
        }
        case 'P': {     /*ptr to a struct*/
            info->struct_size = va_arg(*ap, size_t);
            assert(info->struct_size<10*1000*1000);
            info->struct_printer = va_arg(*ap, ArgInfo::StructPrinter);
            info->struct_buf = new uint8_t[info->struct_size];
            info->struct_nread = get_process()->mem_read(info->struct_buf, info->val, info->struct_size);
            break;
        }
    }
}

void
RSIM_Thread::syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app)
{
    RTS_Message *m = tracing(TRACE_SYSCALL);

    if (m->get_file()) {
        assert(strlen(format)<=6);
        ArgInfo args[6];
        for (size_t i=0; format[i]; i++)
            syscall_arginfo(format[i], values?values[i]:syscall_arg(i), args+i, app);

        RTS_MESSAGE(*m) {
            m->multipart(name, "%s[%d](", name, syscall_arg(-1));
            for (size_t i=0; format && format[i]; i++) {
                if (i>0) m->more(", ");
                print_single(m, format[i], args+i);
            }
            m->more(")");
        } RTS_MESSAGE_END(false);
    }
}

void
RSIM_Thread::syscall_enter(uint32_t *values, const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_enterv(values, name, format, &ap);
    va_end(ap);
}

void
RSIM_Thread::syscall_enter(const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_enterv(NULL, name, format, &ap);
    va_end(ap);
}

void
RSIM_Thread::syscall_leavev(uint32_t *values, const char *format, va_list *app) 
{
    bool returns_errno = false;
    if ('d'==format[0]) {
        returns_errno = true;
    } else if ('D'==format[0]) {
        /* same as 'd' except use next letter for non-error return values */
        returns_errno = true;
        format++;
    }

    ROSE_ASSERT(strlen(format)>=1);
    RTS_Message *mesg = tracing(TRACE_SYSCALL);
    if (mesg->get_file()) {
        /* System calls return an integer (negative error numbers, non-negative success) */
        ArgInfo info;
        uint32_t retval = values ? values[0] : policy.readRegister<32>(policy.reg_eax).known_value();
        syscall_arginfo(format[0], retval, &info, app);

        RTS_WRITE(process->rwlock()) {
            RTS_MESSAGE(*mesg) {
                mesg->more(" = ");

                /* Return value */
                int error_number = (int32_t)retval<0 && (int32_t)retval>-256 ? -(int32_t)retval : 0;
                if (returns_errno && error_number!=0) {
                    mesg->more("%"PRId32" ", retval);
                    print_enum(mesg, error_numbers, error_number);
                    mesg->more(" (%s)\n", strerror(error_number));
                } else {
                    print_single(mesg, format[0], &info);
                    mesg->more("\n");
                }

                /* Additionally, output any other buffer values that were filled in by a successful system call. */
                int signed_retval = (int)retval;
                if (!returns_errno || (signed_retval<-1024 || signed_retval>=0) || -EINTR==signed_retval) {
                    for (size_t i=1; format[i]; i++) {
                        if ('-'!=format[i]) {
                            uint32_t value = values ? values[i] : syscall_arg(i-1);
                            syscall_arginfo(format[i], value, &info, app);
                            if ('P'!=format[i] || 0!=value) { /* no need to show null pointers */
                                mesg->more("    result arg%zu = ", i-1);
                                print_single(mesg, format[i], &info);
                                mesg->more("\n");
                            }
                        }
                    }
                }
                mesg->multipart_end();
            } RTS_MESSAGE_END(true);
        } RTS_WRITE_END;
    }
}

void
RSIM_Thread::syscall_leave(uint32_t *values, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_leavev(values, format, &ap);
    va_end(ap);
}

void
RSIM_Thread::syscall_leave(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_leavev(NULL, format, &ap);
    va_end(ap);
}

uint32_t
RSIM_Thread::syscall_arg(int idx)
{
    switch (idx) {
        case -1: return policy.readRegister<32>(policy.reg_eax).known_value();      /* syscall return value */
        case 0:  return policy.readRegister<32>(policy.reg_ebx).known_value();
        case 1:  return policy.readRegister<32>(policy.reg_ecx).known_value();
        case 2:  return policy.readRegister<32>(policy.reg_edx).known_value();
        case 3:  return policy.readRegister<32>(policy.reg_esi).known_value();
        case 4:  return policy.readRegister<32>(policy.reg_edi).known_value();
        case 5:  return policy.readRegister<32>(policy.reg_ebp).known_value();
        default: assert(!"invalid argument number"); abort();
    }
}

/* Deliver the specified signal. The signal is not removed from the signal_pending vector, nor is it added if it's masked. */
int
RSIM_Thread::signal_deliver(const RSIM_SignalHandling::siginfo_32 &_info)
{
    RSIM_SignalHandling::siginfo_32 info = _info;
    int signo = info.si_signo;

    bool cb_status = get_callbacks().call_signal_callbacks(RSIM_Callbacks::BEFORE, this, signo, &info,
                                                           RSIM_Callbacks::SignalCallback::DELIVERY, true);

    if (cb_status && signo>0) {
        assert(signo<=64);

        RTS_Message *mesg = tracing(TRACE_SIGNAL);
        sigaction_32 sa;
        int status = get_process()->sys_sigaction(signo, NULL, &sa);
        assert(status>=0);

        if (sa.handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
            /* The signal action may have changed since the signal was generated, so we need to check this again. */
            mesg->multipart("delivery", "signal delivery ignored: ");
            print_siginfo_32(mesg, (const uint8_t*)&info, sizeof info);
            mesg->multipart_end();

        } else if (sa.handler_va==(uint32_t)(uint64_t)SIG_DFL) {
            mesg->multipart("delivery", "signal delivery via default: ");
            print_siginfo_32(mesg, (const uint8_t*)&info, sizeof info);
            mesg->multipart_end();

            switch (signo) {
                case SIGFPE:
                case SIGILL:
                case SIGSEGV:
                case SIGBUS:
                case SIGABRT:
                case SIGTRAP:
                case SIGSYS:
                    /* Exit process with core dump */
                    tracing(TRACE_MISC)->mesg("dumping core...\n");
                    get_process()->mem_showmap(tracing(TRACE_MISC), "map at time of core dump:\n");
                    get_process()->dump_core(signo);
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
            mesg->multipart("delivery", "signal delivery to 0x%08"PRIx32": ", sa.handler_va);
            print_siginfo_32(mesg, (const uint8_t*)&info, sizeof info);
            mesg->multipart_end();

            pt_regs_32 regs = get_regs();
            RSIM_SignalHandling::sigset_32 signal_mask;
            status = sighand.sigprocmask(0, NULL, &signal_mask);
            assert(status>=0);
            uint32_t frame_va = 0;

            if (sa.flags & SA_SIGINFO) {
                /* Use the extended signal handler frame */
                RSIM_SignalHandling::rt_sigframe_32 frame;
                memset(&frame, 0, sizeof frame);
                stack_32 stack; /* signal alternate stack */
                int status __attribute__((unused)) = sighand.sigaltstack(NULL, &stack, regs.sp);
                assert(status>=0);
                frame_va = sighand.get_sigframe(&sa, sizeof frame, regs.sp);

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
                    frame.pretcode = sa.restorer_va;
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
            } else {
                /* Use the plain signal handler frame */
                RSIM_SignalHandling::sigframe_32 frame;
                memset(&frame, 0, sizeof frame);
                frame_va = sighand.get_sigframe(&sa, sizeof frame, regs.sp);

                frame.signo = signo;
                sighand.setup_sigcontext(&frame.sc, regs, signal_mask);
                frame.extramask = signal_mask >> 32;
                if (sa.flags & 0x04000000/*SA_RESTORER, deprecated*/) {
                    frame.pretcode = sa.restorer_va;
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

            /* Clear flags per ABI for function entry. */
            policy.writeRegister(policy.reg_df, policy.false_());
            policy.writeRegister(policy.reg_tf, policy.false_());

            /* Set up registers for signal handler. See RSIM_Semantics::InnerPolicy::ctor() for details about the segment
             * register values. */
            policy.writeRegister(policy.reg_eax, policy.number<32>(signo));
            policy.writeRegister(policy.reg_edx, policy.number<32>(0));
            policy.writeRegister(policy.reg_ecx, policy.number<32>(0));
            policy.writeRegister(policy.reg_ds,  policy.number<16>(0x2b));
            policy.writeRegister(policy.reg_es,  policy.number<16>(0x2b));
            policy.writeRegister(policy.reg_ss,  policy.number<16>(0x2b));
            policy.writeRegister(policy.reg_cs,  policy.number<16>(0x23));
            policy.writeRegister(policy.reg_esp, policy.number<32>(frame_va));
            policy.writeRegister(policy.reg_eip, policy.number<32>(sa.handler_va)); /* we're now in the signal handler... */
        }
    }

    get_callbacks().call_signal_callbacks(RSIM_Callbacks::AFTER, this, signo, &info,
                                          RSIM_Callbacks::SignalCallback::DELIVERY, cb_status);
    return 0;
}

int
RSIM_Thread::sys_rt_sigreturn()
{
    RTS_Message *mesg = tracing(TRACE_SIGNAL);

    /* Sighandler frame address is four less than the current SP because the return from sighandler popped the frame's
     * pretcode. Unlike the sigframe_32 stack frame, the rt_sigframe_32 stack frame's retcode does not pop the signal number
     * nor the other handler arguments, and why should it since we're about to restore the hardware context anyway. */
    uint32_t sp = policy.readRegister<32>(policy.reg_esp).known_value();
    uint32_t frame_va = sp - 4;

    RSIM_SignalHandling::rt_sigframe_32 frame;
    if (sizeof(frame)!=process->mem_read(&frame, frame_va, sizeof frame)) {
        mesg->mesg("bad frame 0x%08"PRIu32" in sigreturn (sp=0x%08"PRIu32")", frame_va, sp);
        return -EFAULT;
    }

    if (mesg->get_file()) {
        RTS_MESSAGE(*mesg) {
            mesg->multipart("sigreturn", "returning from ");
            print_enum(mesg, signal_names, frame.signo);
            mesg->more(" handler");
            mesg->multipart_end();
        } RTS_MESSAGE_END(true);
    }

    /* Restore previous signal mask */
    int status = sighand.sigprocmask(SIG_SETMASK, &frame.uc.uc_sigmask, NULL);
    if (status<0)
        return -EFAULT;

    /* Restore hardware context */
    pt_regs_32 regs;
    sighand.restore_sigcontext(frame.uc.uc_mcontext, policy.get_eflags(), &regs);
    init_regs(regs);
    return 0;
}

/* Note: if the specimen's signal handler never returns then this function is never invoked.  The specimen may do a longjmp()
 * or siglongjmp(), in which case the original stack, etc are restored anyway. Additionally, siglongjmp() may do a system call
 * to set the signal mask back to the value saved by sigsetjmp(), if any. */
int
RSIM_Thread::sys_sigreturn()
{
    RTS_Message *mesg = tracing(TRACE_SIGNAL);

    /* Sighandler frame address is eight less than the current SP because the return from sighandler popped the frame's
     * pretcode, and the retcode popped the signo. */
    uint32_t sp = policy.readRegister<32>(policy.reg_esp).known_value();
    uint32_t frame_va = sp - 8; 

    RSIM_SignalHandling::sigframe_32 frame;
    if (sizeof(frame)!=process->mem_read(&frame, frame_va, sizeof frame)) {
        mesg->mesg("bad frame 0x%08"PRIu32" in sigreturn (sp=0x%08"PRIu32")", frame_va, sp);
        return -EFAULT;
    }

    if (mesg->get_file()) {
        RTS_MESSAGE(*mesg) {
            mesg->multipart("sigreturn", "returning from ");
            print_enum(mesg, signal_names, frame.signo);
            mesg->more(" handler");
            mesg->multipart_end();
        } RTS_MESSAGE_END(true);
    }

    /* Restore previous signal mask */
    RSIM_SignalHandling::sigset_32 old_sigmask = frame.extramask;
    old_sigmask = (old_sigmask << 32) | frame.sc.oldmask;
    int status = sighand.sigprocmask(SIG_SETMASK, &old_sigmask, NULL);
    if (status<0)
        return -EFAULT;

    /* Restore hardware context.  Restore only certain flags. */
    pt_regs_32 regs;
    sighand.restore_sigcontext(frame.sc, policy.get_eflags(), &regs);
    init_regs(regs);
    return 0;
}

int
RSIM_Thread::sys_sigprocmask(int how, const RSIM_SignalHandling::sigset_32 *in, RSIM_SignalHandling::sigset_32 *out)
{
    return sighand.sigprocmask(how, in, out);
}

void
RSIM_Thread::signal_clear_pending()
{
    sighand.clear_all_pending();
}

int
RSIM_Thread::signal_accept(const RSIM_SignalHandling::siginfo_32 &_info)
{
    RSIM_SignalHandling::siginfo_32 info = _info; /* non-const copy because callbacks may modify it */
    RSIM_SignalHandling::sigset_32 mask;
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
RSIM_Thread::signal_dequeue(RSIM_SignalHandling::siginfo_32 *info/*out*/)
{
    int signo = sighand.dequeue(info);
    if (0==signo) {
        RSIM_SignalHandling::sigset_32 mask;
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
    if (p->get_tracing_file())
        fflush(p->get_tracing_file());
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
    policy.set_ninsns(0);       /* restart instruction counter for trace output */

    /* Redirect tracing output for new process */
    p->open_tracing_file();
    p->btrace_close();
    reopen_trace_facilities(p->get_tracing_file());

    /* FIXME: According to the Linux 2.6.32 man page for pthread_atfork(), all mutexes must be reinitialized with
     * pthread_mutex_init. (FIXME: There may be mutexes in other parts of ROSE that need to be reinitialized--we need to make
     * sure that appropriate atfork callbacks were registered by those layers to reinitialize their mutexes).
     * [RPM 2012-01-19] */
}

void
RSIM_Thread::report_progress_maybe()
{
    RTS_Message *mesg = tracing(TRACE_PROGRESS);
    if (mesg->get_file()) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double report_delta = (now.tv_sec - last_report.tv_sec) + 1e-6 * (now.tv_usec - last_report.tv_usec);
        if (report_delta > report_interval) {
            const struct timeval &ctime = get_process()->get_ctime();
            double elapsed = (now.tv_sec - ctime.tv_sec) + 1e-6 * (now.tv_usec - ctime.tv_usec);
            double insn_rate = elapsed>0.0 ? get_ninsns() / elapsed : 0;
            mesg->mesg("processed %zu insns in %d sec (%d insns/sec)\n", get_ninsns(), (int)(elapsed+0.5), (int)(insn_rate+0.5));
            last_report = now;
        }
    }
}

void
RSIM_Thread::report_stack_frames(RTS_Message *mesg, const std::string &title/*=""*/, bool bp_not_saved/*=false*/)
{
    if (!mesg || !mesg->get_file())
        return;
    RTS_WRITE(get_process()->rwlock()) {
        if (title.empty()) {
            mesg->multipart("stack", "stack frames:");
        } else {
            mesg->multipart("stack", "%s", title.c_str());
        }

        uint32_t bp = policy.readRegister<32>(policy.reg_ebp).known_value();
        uint32_t ip = policy.readRegister<32>(policy.reg_eip).known_value();

        for (int i=0; i<32; i++) {
            mesg->more("\n  #%d: bp=0x%08"PRIx32" ip=0x%08"PRIx32, i, bp, ip);
            SgAsmInstruction *insn = NULL;
            try {
                insn = process->get_instruction(ip);
            } catch (const Disassembler::Exception&) {
                /* IP is probably pointing to non-executable memory or a bad address. */
            }
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
            if (func && !func->get_name().empty() && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS)) {
                mesg->more(" in function %s", func->get_name().c_str());
            } else if (process->get_memory().exists(ip)) {
                const MemoryMap::Segment &sgmt = process->get_memory().at(ip).second;
                if (!sgmt.get_name().empty())
                    mesg->more(" in memory region %s", sgmt.get_name().c_str());
            }

            if (bp_not_saved) {
                /* Presumably being called after a CALL but before EBP is saved on the stack.  In this case, the return address
                 * of the inner-most function should be at ss:[esp], and containing functions have set up their stack
                 * frames. */
                uint32_t sp = policy.readRegister<32>(policy.reg_esp).known_value();
                if (4!=process->mem_read(&ip, sp, 4))
                    break;
                bp_not_saved = false;
                mesg->more(" [no stack frame]");
            } else {
                /* This function has stored its incoming EBP on the stack at ss:[ebp].  This is usually accomplished by the
                 * instructions PUSH EBP; MOV EBP, ESP. */
                if (4!=process->mem_read(&ip, bp+4, 4))
                    break;
                if (4!=process->mem_read(&bp, bp, 4))
                    break;
            }
        }
        mesg->multipart_end();
    } RTS_WRITE_END;
}

void
RSIM_Thread::syscall_return(const RSIM_SEMANTICS_VTYPE<32> &retval)
{
    policy.writeRegister(policy.reg_eax, retval);
}

void
RSIM_Thread::syscall_return(int retval)
{
    policy.writeRegister(policy.reg_eax, policy.number<32>(retval));
}

/* Executed by a real thread to simulate a specimen's thread. */
void *
RSIM_Thread::main()
{
    RSIM_Process *process = get_process();

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    while (true) {
        pthread_testcancel();
        report_progress_maybe();
        try {
            /* Returned from signal handler? This code simulates the sigframe_32 or rt_sigframe_32 "retcode" */
            if (policy.readRegister<32>(policy.reg_eip).known_value()==SIGHANDLER_RETURN) {
                policy.pop();
                policy.writeRegister<32>(policy.reg_eax, RSIM_SEMANTICS_VTYPE<32>(119));
                sys_sigreturn();
                continue;
            } else if (policy.readRegister<32>(policy.reg_eip).known_value()==SIGHANDLER_RT_RETURN) {
                policy.writeRegister<32>(policy.reg_eax, RSIM_SEMANTICS_VTYPE<32>(173));
                sys_rt_sigreturn();
                continue;
            }

            /* Handle a signal if we have any pending that aren't masked */
            RSIM_SignalHandling::siginfo_32 info;
            process->signal_dispatch(); /* assign process signals to threads */
            if (signal_dequeue(&info)>0)
                signal_deliver(info);

            /* Find the instruction.  Callbacks might change the value of the EIP register, in which case we should re-fetch
             * the instruction. The pre-instruction callbacks will be invoked for each re-fetched instruction, but the
             * post-instruction callback is only invoked for the final instruction. */
            SgAsmx86Instruction *insn = NULL;
            bool cb_status;
            do {
                insn = current_insn();
                cb_status = callbacks.call_insn_callbacks(RSIM_Callbacks::BEFORE, this, insn, true);
            } while (insn->get_address()!=policy.readRegister<32>(policy.reg_eip).known_value());

            /* Simulate an instruction.  In order to make our simulated instructions atomic (at least among the simulators) we
             * use a shared semaphore that was created in RSIM_Thread::ctor(). */
            if (cb_status) {
                process->binary_trace_add(this, insn);
                int status = TEMP_FAILURE_RETRY(sem_wait(process->get_simulator()->get_semaphore()));
                assert(0==status);
                insn_semaphore_posted = false;
                semantics.processInstruction(insn);             // blocking syscalls will post, and set insn_semaphore_posted
                if (!insn_semaphore_posted) {
                    status = sem_post(process->get_simulator()->get_semaphore());
                    assert(0==status);
                    insn_semaphore_posted = true;
                }
            }
            callbacks.call_insn_callbacks(RSIM_Callbacks::AFTER, this, insn, cb_status);

            RTS_Message *mesg = tracing(TRACE_STATE);
            if (mesg->get_file())
                policy.dump_registers(mesg);
        } catch (const Disassembler::Exception &e) {
            if (!insn_semaphore_posted) {
                int status __attribute__((unused)) = sem_post(process->get_simulator()->get_semaphore());
                assert(0==status);
                insn_semaphore_posted = true;
            }
            std::ostringstream s;
            s <<e;
            tracing(TRACE_MISC)->mesg("caught Disassembler::Exception: %s\n", s.str().c_str());
            tracing(TRACE_MISC)->mesg("dumping core...\n");
            process->dump_core(SIGSEGV);
            report_stack_frames(tracing(TRACE_MISC));
            abort();
        } catch (const RSIM_Semantics::Dispatcher::Exception &e) {
            /* Thrown for instructions whose semantics are not implemented yet. */
            if (!insn_semaphore_posted) {
                int status __attribute__((unused)) = sem_post(process->get_simulator()->get_semaphore());
                assert(0==status);
                insn_semaphore_posted = true;
            }
            std::ostringstream s;
            s <<e;
            tracing(TRACE_MISC)->mesg("caught RSIM_Semantics::Dispatcher::Exception: %s\n", s.str().c_str());
#ifdef X86SIM_STRICT_EMULATION
            tracing(TRACE_MISC)->mesg("dumping core...\n");
            process->dump_core(SIGILL);
            report_stack_frames(tracing(TRACE_MISC));
            abort();
#else
            report_stack_frames(tracing(TRACE_MISC));
            tracing(TRACE_MISC)->mesg("exception ignored; continuing with a corrupt state...\n");
#endif
        } catch (const RSIM_SEMANTICS_POLICY::Exception &e) {
            if (!insn_semaphore_posted) {
                int status __attribute__((unused)) = sem_post(process->get_simulator()->get_semaphore());
                assert(0==status);
                insn_semaphore_posted = true;
            }
            std::ostringstream s;
            s <<e;
            tracing(TRACE_MISC)->mesg("caught semantic policy exception: %s\n", s.str().c_str());
            tracing(TRACE_MISC)->mesg("dumping core...\n");
            process->dump_core(SIGILL);
            report_stack_frames(tracing(TRACE_MISC));
            abort();
        } catch (const RSIM_Process::Exit &e) {
            if (!insn_semaphore_posted) {
                int status __attribute__((unused)) = sem_post(process->get_simulator()->get_semaphore());
                assert(0==status);
                insn_semaphore_posted = true;
            }
            sys_exit(e);
            return NULL;
        } catch (RSIM_SignalHandling::siginfo_32 &e) {
            if (!insn_semaphore_posted) {
                int status __attribute__((unused)) = sem_post(process->get_simulator()->get_semaphore());
                assert(0==status);
                insn_semaphore_posted = true;
            }
            if (e.si_signo) {
                bool cb_status = get_callbacks().call_signal_callbacks(RSIM_Callbacks::BEFORE, this, e.si_signo, &e,
                                                                       RSIM_Callbacks::SignalCallback::ARRIVAL, true);
                if (cb_status)
                    sighand.generate(e, process, tracing(TRACE_SIGNAL));
                get_callbacks().call_signal_callbacks(RSIM_Callbacks::AFTER, this, e.si_signo, &e,
                                                      RSIM_Callbacks::SignalCallback::ARRIVAL, cb_status);
            }
        }
    }
}

pt_regs_32
RSIM_Thread::get_regs() const
{
    pt_regs_32 regs;
    memset(&regs, 0, sizeof regs);
    regs.ip = policy.readRegister<32>(policy.reg_eip).known_value();
    regs.ax = policy.readRegister<32>(policy.reg_eax).known_value();
    regs.bx = policy.readRegister<32>(policy.reg_ebx).known_value();
    regs.cx = policy.readRegister<32>(policy.reg_ecx).known_value();
    regs.dx = policy.readRegister<32>(policy.reg_edx).known_value();
    regs.si = policy.readRegister<32>(policy.reg_esi).known_value();
    regs.di = policy.readRegister<32>(policy.reg_edi).known_value();
    regs.bp = policy.readRegister<32>(policy.reg_ebp).known_value();
    regs.sp = policy.readRegister<32>(policy.reg_esp).known_value();
    regs.cs = policy.readRegister<16>(policy.reg_cs).known_value();
    regs.ds = policy.readRegister<16>(policy.reg_ds).known_value();
    regs.es = policy.readRegister<16>(policy.reg_es).known_value();
    regs.fs = policy.readRegister<16>(policy.reg_fs).known_value();
    regs.gs = policy.readRegister<16>(policy.reg_gs).known_value();
    regs.ss = policy.readRegister<16>(policy.reg_ss).known_value();
    regs.flags = policy.readRegister<32>(policy.reg_eflags).known_value();
    return regs;
}

void
RSIM_Thread::init_regs(const pt_regs_32 &regs)
{
    policy.writeRegister(policy.reg_eip,    RSIM_SEMANTICS_VTYPE<32>(regs.ip));
    policy.writeRegister(policy.reg_eax,    RSIM_SEMANTICS_VTYPE<32>(regs.ax));
    policy.writeRegister(policy.reg_ebx,    RSIM_SEMANTICS_VTYPE<32>(regs.bx));
    policy.writeRegister(policy.reg_ecx,    RSIM_SEMANTICS_VTYPE<32>(regs.cx));
    policy.writeRegister(policy.reg_edx,    RSIM_SEMANTICS_VTYPE<32>(regs.dx));
    policy.writeRegister(policy.reg_esi,    RSIM_SEMANTICS_VTYPE<32>(regs.si));
    policy.writeRegister(policy.reg_edi,    RSIM_SEMANTICS_VTYPE<32>(regs.di));
    policy.writeRegister(policy.reg_ebp,    RSIM_SEMANTICS_VTYPE<32>(regs.bp));
    policy.writeRegister(policy.reg_esp,    RSIM_SEMANTICS_VTYPE<32>(regs.sp));
    policy.writeRegister(policy.reg_cs,     RSIM_SEMANTICS_VTYPE<16>(regs.cs));
    policy.writeRegister(policy.reg_ds,     RSIM_SEMANTICS_VTYPE<16>(regs.ds));
    policy.writeRegister(policy.reg_es,     RSIM_SEMANTICS_VTYPE<16>(regs.es));
    policy.writeRegister(policy.reg_fs,     RSIM_SEMANTICS_VTYPE<16>(regs.fs));
    policy.writeRegister(policy.reg_gs,     RSIM_SEMANTICS_VTYPE<16>(regs.gs));
    policy.writeRegister(policy.reg_ss,     RSIM_SEMANTICS_VTYPE<16>(regs.ss));
    policy.writeRegister(policy.reg_eflags, RSIM_SEMANTICS_VTYPE<32>(regs.flags));
}

int
RSIM_Thread::set_gdt(const user_desc_32 *ud)
{
    user_desc_32 *entry = gdt_entry(ud->entry_number);
    *entry = *ud;

    /* Make sure all affected shadow registers are reloaded. */
    policy.writeRegister(policy.reg_cs, policy.readRegister<16>(policy.reg_cs));
    policy.writeRegister(policy.reg_ds, policy.readRegister<16>(policy.reg_ds));
    policy.writeRegister(policy.reg_es, policy.readRegister<16>(policy.reg_es));
    policy.writeRegister(policy.reg_fs, policy.readRegister<16>(policy.reg_fs));
    policy.writeRegister(policy.reg_gs, policy.readRegister<16>(policy.reg_gs));
    policy.writeRegister(policy.reg_ss, policy.readRegister<16>(policy.reg_ss));

    return ud->entry_number;
}

user_desc_32 *
RSIM_Thread::gdt_entry(int idx)
{
    if (idx >= RSIM_Process::GDT_ENTRY_TLS_MIN &&
        idx <= RSIM_Process::GDT_ENTRY_TLS_MAX) {
        return tls_array + idx - RSIM_Process::GDT_ENTRY_TLS_MIN;
    }
    return get_process()->gdt_entry(idx);
}

int
RSIM_Thread::get_free_tls() const
{
    static user_desc_32 zero;
    for (int idx=0; idx<RSIM_Process::GDT_ENTRY_TLS_ENTRIES; idx++) {
        if (0==memcmp(tls_array+idx, &zero, sizeof zero))
            return idx + RSIM_Process::GDT_ENTRY_TLS_MIN;
    }
    return -ESRCH;
}

int
RSIM_Thread::set_thread_area(user_desc_32 *info, bool can_allocate)
{
    int idx = info->entry_number;

    if (-1==idx && can_allocate) {
        idx = get_free_tls();
        assert(idx<0x7fffffffLL);
        if (idx < 0)
            return idx;
        info->entry_number = idx;
        tracing(TRACE_SYSCALL)->brief("entry #%d", idx);
    }

    if (idx<(int)RSIM_Process::GDT_ENTRY_TLS_MIN || idx>(int)RSIM_Process::GDT_ENTRY_TLS_MAX)
        return -EINVAL;

    set_gdt(info);
    return idx;
}

int
RSIM_Thread::handle_futex_death(uint32_t futex_va, RTS_Message *trace)
{
    uint32_t futex;
    trace->more("\n  handling death for futex at 0x%08"PRIx32"\n", futex_va);

    if (4!=get_process()->mem_read(&futex, futex_va, 4)) {
        trace->more("    failed to read futex at 0x%08"PRIx32"\n", futex_va);
        return -EFAULT;
    }

    /* If this thread owns the futex then set the FUTEX_OWNER_DIED and signal the futex. */
    if (get_tid()==(int)(futex & 0x1fffffff)) {
        /* Set the FUTEX_OWNER_DIED bit */
        trace->more("    setting FUTEX_OWNER_DIED bit\n");
        futex |= 0x40000000;
        if (4!=get_process()->mem_write(&futex, futex_va, 4)) {
            trace->more("      failed to set FUTEX_OWNER_DIED for futex at 0x%08"PRIx32"\n", futex_va);
            return -EFAULT;
        }

        /* Wake another thread there's one waiting */
        if (futex & 0x80000000) {
            trace->more("    waking futex 0x%08"PRIx32"\n", futex_va);
            int result = futex_wake(futex_va, 1);
            if (result<0) {
                trace->more("      wake failed for futex at 0x%08"PRIu32"\n", futex_va);
                return result;
            }
        }
    } else {
        trace->more("    futex is not owned by this thread; skipping\n");
    }
    return 0;
}
    
int
RSIM_Thread::exit_robust_list()
{
    int retval = 0;

    if (0==robust_list_head_va)
        return 0;

    RTS_Message *trace = tracing(TRACE_THREAD);
    trace->multipart("futex_death", "exit_robust_list()...");

    robust_list_head_32 head;
    if (sizeof(head)!=get_process()->mem_read(&head, robust_list_head_va, sizeof head)) {
        trace->more(" <failed to read robust list head at 0x%08"PRIx32">", robust_list_head_va);
        retval = -EFAULT;
    } else {
        static const size_t max_locks = 1000000;
        size_t nlocks = 0;
        uint32_t lock_entry_va = head.next_va;
        while (lock_entry_va != robust_list_head_va && nlocks++ < max_locks) {
            /* Don't process futex if it's pending; we'll catch it at the end instead. */
            if (lock_entry_va != head.pending_va) {
                uint32_t futex_va = lock_entry_va + head.futex_offset;
                if ((retval = handle_futex_death(futex_va, trace))<0)
                    break;
            }
        
            /* Advance lock_entry_va to next item in the list. */
            if (4!=get_process()->mem_read(&lock_entry_va, lock_entry_va, 4)) {
                trace->more(" <list pointer read failed at 0x%08"PRIx32">", lock_entry_va);
                retval = -EFAULT;
                break;
            }
        }
        if (head.pending_va)
            retval = handle_futex_death(head.pending_va, trace);
    }

    trace->more(" done.\n");
    trace->multipart_end();
    return retval;
}

void
RSIM_Thread::do_clear_child_tid()
{
    if (clear_child_tid) {
        tracing(TRACE_SYSCALL)->mesg("clearing child tid...");
        uint32_t zero = 0;
        size_t n = get_process()->mem_write(&zero, clear_child_tid, sizeof zero);
        if (n!=sizeof zero) {
            tracing(TRACE_SYSCALL)->mesg("cannot write clear_child_tid address 0x%08"PRIx32, clear_child_tid);
        } else {
            tracing(TRACE_SYSCALL)->mesg("waking futex 0x%08"PRIx32, clear_child_tid);
            int nwoke = futex_wake(clear_child_tid, INT_MAX);
            if (nwoke<0)
                tracing(TRACE_SYSCALL)->mesg("wake futex 0x%08"PRIx32" failed with %d\n", clear_child_tid, nwoke);
        }
    }
}

int
RSIM_Thread::sys_exit(const RSIM_Process::Exit &e)
{
    RSIM_Process *process = get_process(); /* while we still have a chance */

    tracing(TRACE_THREAD)->mesg("this thread is terminating%s", e.exit_process?" (for entire process)":"");

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
    
    RSIM_SignalHandling::siginfo_32 info = RSIM_SignalHandling::mk_rt(signo, SI_TKILL);

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
RSIM_Thread::sys_kill(pid_t pid, const RSIM_SignalHandling::siginfo_32 &info)
{
    RSIM_Process *process = get_process();
    assert(process!=NULL);
    return process->sys_kill(pid, info);
}

int
RSIM_Thread::sys_sigpending(RSIM_SignalHandling::sigset_32 *result)
{
    RSIM_SignalHandling::sigset_32 p1, p2;
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
RSIM_Thread::sys_sigsuspend(const RSIM_SignalHandling::sigset_32 *mask) {
    int signo = sighand.sigsuspend(mask, this);
    return signo;
}

int
RSIM_Thread::sys_sigaltstack(const stack_32 *in, stack_32 *out)
{
    uint32_t sp = policy.readRegister<32>(policy.reg_esp).known_value();
    return sighand.sigaltstack(in, out, sp);
}

void
RSIM_Thread::emulate_syscall()
{
    /* Post the instruction semphore because some system calls can block indefinitely. */
    int status __attribute__((unused)) = sem_post(get_process()->get_simulator()->get_semaphore());
    assert(0==status);
    insn_semaphore_posted = true;

    unsigned callno = policy.readRegister<32>(policy.reg_eax).known_value();
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
            tracing(TRACE_MISC)->multipart(name, "syscall_%u(", callno);
            for (int i=0; i<6; i++)
                tracing(TRACE_MISC)->more("%s0x%08"PRIx32, i?", ":"", syscall_arg(i));
            tracing(TRACE_MISC)->more(") is not implemented yet");
            tracing(TRACE_MISC)->multipart_end();

            tracing(TRACE_MISC)->mesg("dumping core...\n");
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
    RTS_Message *trace = tracing(TRACE_FUTEX);

    /* Futexes must be word aligned */
    if (va & 3)
        return 0;

    /* Find the simulator address. */
    *val_ptr = (uint32_t*)process->my_addr(va, 4);
    rose_addr_t addr = (rose_addr_t)*val_ptr;
    trace->mesg("futex: specimen va 0x%08"PRIx64" is at simulator address 0x%08"PRIx64, va, addr);

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

        bool readable=false, writable=false, executable=false, shared=true;
        if (*s++ == 'r')
            readable = true;
        if (*s++ == 'w')
            writable = true;
        if (*s++ == 'x')
            executable = true;
        if (*s++ == 'p')
            shared = false;
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
                trace->mesg("futex: cannot stat file \"%s\"", filename);
                break;
            }

            rose_addr_t addr_in_file = (addr - lo_addr) + map_offset;
            trace->mesg("futex: simulator address 0x%08"PRIx64" is at %s+0x%08"PRIx64, addr, filename, addr_in_file);
            addr = - ((map_offset<<16) + addr_in_file);
            addr ^= sb.st_ino;
            addr &= 0xffffffff;
            addr |= 1; // so as not to conflict with simulator addresses, which are most likely word aligned.
            assert(0!=addr);
            trace->mesg("futex: using generated address 0x%08"PRIx64, addr);
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
    RTS_Message *trace = tracing(TRACE_FUTEX);

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
        trace->mesg("futex wait: futex value %"PRIu32" but need %"PRIu32, *futex_ptr, oldval);
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
        trace->mesg("futex wait: about to block...");
        status = process->get_futexes()->wait(futex_number);
        if (-EINTR==status) {
            trace->mesg("futex wait: interrupted by signal");
            retval = -EINTR;
        } else {
            assert(0==status);
            trace->mesg("futex wait: resumed");
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
    RTS_Message *trace = tracing(TRACE_FUTEX);

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
        trace->mesg("futex wake: failed with error %d", retval);
    } else {
        trace->mesg("futex wake: %d proc%s have been signaled", retval, 1==retval?"":"s");
    }

    /* Release the lock */
    status = sem_post(process->get_simulator()->get_semaphore());
    assert(0==status);

    return retval;
}

#endif /* ROSE_ENABLE_SIMULATOR */
