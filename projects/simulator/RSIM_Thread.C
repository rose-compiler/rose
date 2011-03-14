#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <errno.h>
#include <stdarg.h>
#include <syscall.h>

#include <sys/user.h>

size_t RSIM_Thread::next_sequence_number = 0;

/* Constructor */
void
RSIM_Thread::ctor()
{
    my_tid = syscall(SYS_gettid);
    assert(my_tid>0);
    my_seq = next_sequence_number++;

    memset(&last_report, 0, sizeof last_report);
    memset(tls_array, 0, sizeof tls_array);

    for (int tf=0; tf<TRACE_NFACILITIES; tf++) {
        if ((process->get_tracing_flags() & tracingFacilityBit((TracingFacility)tf))) {
            trace_mesg[tf] = new RTS_Message(process->get_tracing_file(), &mesg_prefix);
        } else {
            trace_mesg[tf] = new RTS_Message(NULL, NULL);
        }
    }
}

std::string
RSIM_Thread::id()
{
    static struct timeval start;
    struct timeval now;
    gettimeofday(&now, NULL);
    if (0==start.tv_sec)
        start = now;
    double elapsed = (now.tv_sec - start.tv_sec) + 1e-6 * ((double)now.tv_usec - start.tv_usec);
    char buf1[32];
    sprintf(buf1, "%1.3f", elapsed);

    char buf2[32];
    sprintf(buf2, "0x%08"PRIx64, policy.readIP().known_value());

    return (StringUtility::numberToString(getpid()) + ":" + StringUtility::numberToString(my_seq) +
            " " + buf2 +
            " " + buf1 +
            ": ");
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
    rose_addr_t ip = policy.readIP().known_value();
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
            info->struct_buf = new uint8_t[advertised_size];
            info->struct_nread = get_process()->mem_read(info->struct_buf, info->val, advertised_size);
            info->struct_size = 64; /* max print width, measured in columns of output */
            break;
        }
        case 'P': {     /*ptr to a struct*/
            info->struct_size = va_arg(*ap, size_t);
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
RSIM_Thread::syscall_leave(const char *format, ...) 
{
    va_list ap;
    va_start(ap, format);

    ROSE_ASSERT(strlen(format)>=1);
    RTS_Message *mesg = tracing(TRACE_SYSCALL);
    if (mesg->get_file()) {
        /* System calls return an integer (negative error numbers, non-negative success) */
        ArgInfo info;
        uint32_t value = policy.readGPR(x86_gpr_ax).known_value();
        syscall_arginfo(format[0], value, &info, &ap);

        RTS_MESSAGE(*mesg) {
            mesg->more(" = ");
            print_leave(mesg, format[0], &info);

            /* Additionally, output any other buffer values that were filled in by a successful system call. */
            int result = (int)(uint32_t)(syscall_arg(-1));
            if (format[0]!='d' || (result<-1024 || result>=0) || -EINTR==result) {
                for (size_t i=1; format[i]; i++) {
                    if ('-'!=format[i]) {
                        syscall_arginfo(format[i], syscall_arg(i-1), &info, &ap);
                        if ('P'!=format[i] || 0!=syscall_arg(i-1)) { /* no need to show null pointers */
                            mesg->more("    result arg%zu = ", i-1);
                            print_single(mesg, format[i], &info);
                            mesg->more("\n");
                        }
                    }
                }
            }
            mesg->multipart_end();
        } RTS_MESSAGE_END(true);
    }

    va_end(ap);
}

uint32_t
RSIM_Thread::syscall_arg(int idx)
{
    switch (idx) {
        case -1: return policy.readGPR(x86_gpr_ax).known_value();      /* syscall return value */
        case 0: return policy.readGPR(x86_gpr_bx).known_value();
        case 1: return policy.readGPR(x86_gpr_cx).known_value();
        case 2: return policy.readGPR(x86_gpr_dx).known_value();
        case 3: return policy.readGPR(x86_gpr_si).known_value();
        case 4: return policy.readGPR(x86_gpr_di).known_value();
        case 5: return policy.readGPR(x86_gpr_bp).known_value();
        default: assert(!"invalid argument number"); abort();
    }
}

/* Deliver the specified signal. The signal is not removed from the signal_pending vector, nor is it added if it's masked. */
void
RSIM_Thread::signal_deliver(int signo)
{
    ROSE_ASSERT(signo>0 && signo<=64);

    RTS_Message *mesg = tracing(TRACE_SIGNAL);
    sigaction_32 sa;
    int status = get_process()->sys_sigaction(signo, NULL, &sa);
    assert(status>=0);

    if (sa.handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        /* The signal action may have changed since the signal was generated, so we need to check this again. */
        mesg->mesg("delivering %s(%d) ignored", flags_to_str(signal_names, signo).c_str(), signo);
    } else if (sa.handler_va==(uint32_t)(uint64_t)SIG_DFL) {
        mesg->mesg("delivering %s(%d) default", flags_to_str(signal_names, signo).c_str(), signo);
        switch (signo) {
            case SIGFPE:
            case SIGILL:
            case SIGSEGV:
            case SIGBUS:
            case SIGABRT:
            case SIGTRAP:
            case SIGSYS:
                /* Exit process with core dump */
                tracing(TRACE_MISC)->mesg("dumping core\n");
                get_process()->dump_core(signo);
                throw Exit((signo & 0x7f) | __WCOREFLAG, true);
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
                throw Exit(signo & 0x7f, true);
            case SIGIO:
            case SIGURG:
            case SIGCHLD:
            case SIGCONT:
            case SIGSTOP:
            case SIGTTIN:
            case SIGTTOU:
            case SIGWINCH:
                /* Signal is ignored by default */
                return;
            default:
                /* Exit without a core dump */
                throw Exit(signo & 0x7f, true);
        }

    } else {
        mesg->mesg("delivering %s(%d) to 0x%08"PRIx32,
                    flags_to_str(signal_names, signo).c_str(), signo, sa.handler_va);

        uint32_t signal_return = policy.readIP().known_value();
        policy.push(signal_return);

        /* Switch to the alternate stack? */
        uint32_t old_sp = policy.readGPR(x86_gpr_sp).known_value();
        stack_32 new_stack;
        int status = sighand.sigaltstack(NULL, &new_stack, old_sp);
        assert(status>=0);
        if (0==(new_stack.ss_flags & SS_ONSTACK) && 0!=(sa.flags & SA_ONSTACK))
            policy.writeGPR(x86_gpr_sp, policy.number<32>(new_stack.ss_sp + new_stack.ss_size));

        /* Push stuff that will be needed by the simulated sigreturn() syscall */
        policy.push(old_sp);
        RSIM_SignalHandling::sigset_32 signal_mask;
        status = sighand.sigprocmask(0, NULL, &signal_mask);
        assert(status>=0);

        policy.push(signal_mask >> 32);
        policy.push(signal_mask & 0xffffffff);

        /* Caller-saved registers */
        policy.push(policy.readGPR(x86_gpr_ax));
        policy.push(policy.readGPR(x86_gpr_bx));
        policy.push(policy.readGPR(x86_gpr_cx));
        policy.push(policy.readGPR(x86_gpr_dx));
        policy.push(policy.readGPR(x86_gpr_si));
        policy.push(policy.readGPR(x86_gpr_di));
        policy.push(policy.readGPR(x86_gpr_bp));

        /* New signal mask */
        signal_mask |= sa.mask;
        signal_mask |= (uint64_t)1 << (signo-1);
        sighand.sigprocmask(SIG_SETMASK, &signal_mask, NULL);

        /* Signal handler arguments */
        policy.push(policy.readIP());
        policy.push(policy.number<32>(signo));

        /* Invoke signal handler */
        policy.push(policy.number<32>(SIGHANDLER_RETURN)); /* fake return address to trigger signal_cleanup() call */
        policy.writeIP(policy.number<32>(sa.handler_va));
    }
}

/* Note: if the specimen's signal handler never returns then this function is never invoked.  The specimen may do a longjmp()
 * or siglongjmp(), in which case the original stack, etc are restored anyway. Additionally, siglongjmp() may do a system call
 * to set the signal mask back to the value saved by sigsetjmp(), if any. */
void
RSIM_Thread::signal_return()
{
    /* Discard handler arguments */
    int signo = policy.pop().known_value(); /* signal number */
    policy.pop(); /* signal address */

    RTS_Message *mesg = tracing(TRACE_SIGNAL);
    if (mesg->get_file()) {
        mesg->multipart("", "returning from ");
        print_enum(mesg, signal_names, signo);
        mesg->more(" handler");
        mesg->multipart_end();
    }

    /* Restore caller-saved registers */
    policy.writeGPR(x86_gpr_bp, policy.pop());
    policy.writeGPR(x86_gpr_di, policy.pop());
    policy.writeGPR(x86_gpr_si, policy.pop());
    policy.writeGPR(x86_gpr_dx, policy.pop());
    policy.writeGPR(x86_gpr_cx, policy.pop());
    policy.writeGPR(x86_gpr_bx, policy.pop());
    syscall_return(policy.pop());

    /* Simulate the sigreturn system call (#119), the stack frame of which was set up by signal_deliver() */
    RSIM_SignalHandling::sigset_32 old_sigmask = policy.pop().known_value(); /* low bits */
    old_sigmask |= (RSIM_SignalHandling::sigset_32)policy.pop().known_value() << 32; /* hi bits */
    int status = sighand.sigprocmask(SIG_SETMASK, &old_sigmask, NULL);
    assert(status>=0);

    /* Simulate return from sigreturn */
    policy.writeGPR(x86_gpr_sp, policy.pop());        /* restore stack pointer */
    policy.writeIP(policy.pop());                     /* RET instruction */
}

int
RSIM_Thread::signal_accept(int signo)
{
    RSIM_SignalHandling::sigset_32 mask;
    int status = sighand.sigprocmask(0, NULL, &mask);
    if (status<0)
        return status;

    if ((sighand.mask_of(signo) & mask))
        return -EAGAIN;

    return sighand.generate(signo, get_process(), tracing(TRACE_SIGNAL));
}

int
RSIM_Thread::signal_dequeue()
{
    int signo = sighand.dequeue();
    if (0==signo) {
        RSIM_SignalHandling::sigset_32 mask;
        int status = sighand.sigprocmask(0, NULL, &mask);
        assert(status>=0);
        signo = get_process()->sighand.dequeue(&mask);
    }
    return signo;
}

void
RSIM_Thread::report_progress_maybe()
{
    RTS_Message *mesg = tracing(TRACE_PROGRESS);
    if (mesg->get_file()) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double delta = (now.tv_sec - last_report.tv_sec) + 1e-1 * (now.tv_usec - last_report.tv_usec);
        if (delta > report_interval) {
            double insn_rate = delta>0 ? get_ninsns() / delta : 0;
            mesg->mesg("processed %zu insns in %d sec (%d insns/sec)\n", get_ninsns(), (int)(delta+0.5), (int)(insn_rate+0.5));
        }
        last_report = now;
    }
}

void
RSIM_Thread::syscall_return(const RSIM_SEMANTIC_VTYPE<32> &retval)
{
    policy.writeGPR(x86_gpr_ax, retval);
}

void
RSIM_Thread::syscall_return(int retval)
{
    policy.writeGPR(x86_gpr_ax, policy.number<32>(retval));
}

/* Executed by a real thread to simulate a specimen's thread. */
void *
RSIM_Thread::main()
{
    RSIM_Process *process = get_process();

    while (true) {
        report_progress_maybe();
        try {
            /* Returned from signal handler? */
            if (policy.readIP().known_value()==SIGHANDLER_RETURN) {
                signal_return();
                continue;
            }

            /* Simulate an instruction */
            SgAsmx86Instruction *insn = current_insn();
            process->binary_trace_add(this, insn);
            semantics.processInstruction(insn);
            RTS_Message *mesg = tracing(TRACE_STATE);
            if (mesg->get_file()) {
                RTS_MESSAGE(*mesg) {
                    mesg->mesg("Machine state after instruction:\n");
                    policy.dump_registers(mesg->get_file());
                } RTS_MESSAGE_END(true);
            }

            /* Handle a signal if we have any pending that aren't masked */
            process->signal_dispatch(); /* assign process signals to threads */
            int signo = signal_dequeue();
            if (signo>0)
                signal_deliver(signo);

        } catch (const Disassembler::Exception &e) {
            std::ostringstream s;
            s <<e;
            tracing(TRACE_MISC)->mesg("caught Disassembler::Exception: %s\n", s.str().c_str());
            tracing(TRACE_MISC)->mesg("dumping core\n");
            process->dump_core(SIGSEGV);
            abort();
        } catch (const RSIM_Semantics::Exception &e) {
            /* Thrown for instructions whose semantics are not implemented yet. */
            std::ostringstream s;
            s <<e;
            tracing(TRACE_MISC)->mesg("caught RSIM_Semantics::Exception: %s\n", s.str().c_str());
#ifdef X86SIM_STRICT_EMULATION
            tracing(TRACE_MISC)->mesg("dumping core\n");
            process->dump_core(SIGILL);
            abort();
#else
            tracing(TRACE_MISC)->mesg("exception ignored; continuing with a corrupt state...\n");
#endif
        } catch (const RSIM_SEMANTIC_POLICY::Exception &e) {
            std::ostringstream s;
            s <<e;
            tracing(TRACE_MISC)->mesg("caught semantic policy exception: %s\n", s.str().c_str());
            tracing(TRACE_MISC)->mesg("dumping core\n");
            process->dump_core(SIGILL);
            abort();
        } catch (const RSIM_Thread::Exit &e) {
            sys_exit(e);
            return NULL;
        } catch (const RSIM_SemanticPolicy::Signal &e) {
            sighand.generate(e.signo, process, tracing(TRACE_SIGNAL));
        } catch (...) {
            tracing(TRACE_MISC)->mesg("caught an unhandled exception\n");
            tracing(TRACE_MISC)->mesg("dumping core\n");
            process->dump_core(SIGILL);
            abort();
        }
    }
}

pt_regs_32
RSIM_Thread::get_regs() const
{
    pt_regs_32 regs;
    memset(&regs, 0, sizeof regs);
    regs.ip = policy.readIP().known_value();
    regs.ax = policy.readGPR(x86_gpr_ax).known_value();
    regs.bx = policy.readGPR(x86_gpr_bx).known_value();
    regs.cx = policy.readGPR(x86_gpr_cx).known_value();
    regs.dx = policy.readGPR(x86_gpr_dx).known_value();
    regs.si = policy.readGPR(x86_gpr_si).known_value();
    regs.di = policy.readGPR(x86_gpr_di).known_value();
    regs.bp = policy.readGPR(x86_gpr_bp).known_value();
    regs.sp = policy.readGPR(x86_gpr_sp).known_value();
    regs.cs = policy.readSegreg(x86_segreg_cs).known_value();
    regs.ds = policy.readSegreg(x86_segreg_ds).known_value();
    regs.es = policy.readSegreg(x86_segreg_es).known_value();
    regs.fs = policy.readSegreg(x86_segreg_fs).known_value();
    regs.gs = policy.readSegreg(x86_segreg_gs).known_value();
    regs.ss = policy.readSegreg(x86_segreg_ss).known_value();
    regs.flags = 0;
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
        if (policy.readFlag((X86Flag)i).known_value()) {
            regs.flags |= (1u<<i);
        }
    }
    return regs;
}

void
RSIM_Thread::init_regs(const pt_regs_32 &regs)
{
    policy.writeIP(regs.ip);
    policy.writeGPR(x86_gpr_ax, regs.ax);
    policy.writeGPR(x86_gpr_bx, regs.bx);
    policy.writeGPR(x86_gpr_cx, regs.cx);
    policy.writeGPR(x86_gpr_dx, regs.dx);
    policy.writeGPR(x86_gpr_si, regs.si);
    policy.writeGPR(x86_gpr_di, regs.di);
    policy.writeGPR(x86_gpr_bp, regs.bp);
    policy.writeGPR(x86_gpr_sp, regs.sp);
    policy.writeSegreg(x86_segreg_cs, regs.cs);
    policy.writeSegreg(x86_segreg_ds, regs.ds);
    policy.writeSegreg(x86_segreg_es, regs.es);
    policy.writeSegreg(x86_segreg_fs, regs.fs);
    policy.writeSegreg(x86_segreg_gs, regs.gs);
    policy.writeSegreg(x86_segreg_ss, regs.ss);
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
        policy.writeFlag((X86Flag)i, regs.flags & ((uint32_t)1<<i) ? policy.true_() : policy.false_());
    }
}

int
RSIM_Thread::set_gdt(const user_desc_32 *ud)
{
    user_desc_32 *entry = gdt_entry(ud->entry_number);
    *entry = *ud;

    /* Make sure all affected shadow registers are reloaded. */
    for (size_t i=0; i<6; i++)
        policy.writeSegreg((X86SegmentRegister)i, policy.readSegreg((X86SegmentRegister)i));

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
RSIM_Thread::futex_wake(uint32_t va)
{
    /* We need the process-wide read lock so no other thread unmaps the memory while we're signaling the futex. */
    int retval;
    assert(4==sizeof(int));
    RTS_READ(get_process()->rwlock()) {
        int *addr = (int*)get_process()->my_addr(va, sizeof(int));
        if (!addr) {
            retval = -EFAULT;
        } else if (-1 == (retval = syscall(SYS_futex, addr, 1/*FUTEX_WAKE*/))) {
            retval = -errno;
        }
    } RTS_READ_END;
    return retval;
}

int
RSIM_Thread::sys_exit(const Exit &e)
{
    RSIM_Process *process = get_process(); /* while we still have a chance */

    tracing(TRACE_THREAD)->mesg("this thread is terminating");

    if (robust_list_head_va)
        tracing(TRACE_MISC)->mesg("warning: robust_list not cleaned up\n"); /* FIXME: see set_robust_list() syscall */

    /* Clear and signal child TID if necessary (CLONE_CHILD_CLEARTID) */
    if (clear_child_tid) {
        tracing(TRACE_SYSCALL)->brief("waking futex 0x%08"PRIx32, clear_child_tid);
        uint32_t zero = 0;
        size_t n = process->mem_write(&zero, clear_child_tid, sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = futex_wake(clear_child_tid);
        ROSE_ASSERT(nwoke>=0);
    }

    /* Remove the child from the process. */
    process->remove_thread(this);
    this->process = NULL;

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
    
    if (pid<0) {
        retval = -EINVAL;
    } else if (pid==getpid() && tid>=0) {
        RSIM_Thread *thread = process->get_thread(tid);
        if (!thread) {
            retval = -ESRCH;
        } else if (thread==this) {
            retval = sighand.generate(signo, process, tracing(TRACE_SIGNAL));
        } else {
            thread->sighand.generate(signo, process, tracing(TRACE_SIGNAL));
            retval = syscall(SYS_tgkill, pid, tid, RSIM_SignalHandling::SIG_WAKEUP);
        }
    } else {
        retval = sys_kill(pid, signo);
    }
    return retval;
}

int
RSIM_Thread::sys_kill(pid_t pid, int signo)
{
    RSIM_Process *process = get_process();
    assert(process!=NULL);
    return process->sys_kill(pid, signo);
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

void
RSIM_Thread::post_fork()
{
    my_tid = syscall(SYS_gettid);
    assert(my_tid==getpid());
    process->post_fork();
}

#endif /* ROSE_ENABLE_SIMULATOR */
