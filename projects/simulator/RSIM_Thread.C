#include "rose.h"
#include "RSIM_Simulator.h"

#include <errno.h>
#include <stdarg.h>
#include <syscall.h>

#include <sys/user.h>

/* Constructor */
void
RSIM_Thread::ctor()
{
    my_tid = syscall(SYS_gettid);
    assert(my_tid>0);

    memset(&last_report, 0, sizeof last_report);
    memset(signal_action, 0, sizeof signal_action);
    memset(tls_array, 0, sizeof tls_array);
    signal_stack.ss_sp = 0;
    signal_stack.ss_size = 0;
    signal_stack.ss_flags = SS_DISABLE;
}

void
RSIM_SemanticPolicy::ctor()
{
    for (size_t i=0; i<VirtualMachineSemantics::State::n_gprs; i++)
        writeGPR((X86GeneralPurposeRegister)i, 0);
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
        writeFlag((X86Flag)i, 0);
    writeIP(0);
    writeFlag((X86Flag)1, true_());
    writeGPR(x86_gpr_sp, 0xbffff000ul);     /* high end of stack, exclusive */

    writeSegreg(x86_segreg_cs, 0x23);
    writeSegreg(x86_segreg_ds, 0x2b);
    writeSegreg(x86_segreg_es, 0x2b);
    writeSegreg(x86_segreg_ss, 0x2b);
    writeSegreg(x86_segreg_fs, 0x2b);
    writeSegreg(x86_segreg_gs, 0x2b);
}

FILE *
RSIM_Thread::tracing(unsigned what) const
{
    return get_process()->tracing(what);
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
    static timeval first_call;

    if (tracing(TRACE_SYSCALL)) {
        timeval this_call;
        gettimeofday(&this_call, NULL);
        if (0==first_call.tv_sec)
            first_call = this_call;
        pid_t tid = syscall(SYS_gettid);
        double elapsed = (this_call.tv_sec - first_call.tv_sec) + (this_call.tv_usec - first_call.tv_usec)/1e6;
        fprintf(tracing(TRACE_SYSCALL), "[tid %d] 0x%08"PRIx64" %8.4f: ", tid, policy.readIP().known_value(), elapsed);
        ArgInfo args[6];
        for (size_t i=0; format[i]; i++)
            syscall_arginfo(format[i], values?values[i]:syscall_arg(i), args+i, app);
        print_enter(tracing(TRACE_SYSCALL), name, syscall_arg(-1), format, args);
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
    if (tracing(TRACE_SYSCALL)) {
        /* System calls return an integer (negative error numbers, non-negative success) */
        ArgInfo info;
        uint32_t value = policy.readGPR(x86_gpr_ax).known_value();
        syscall_arginfo(format[0], value, &info, &ap);
        print_leave(tracing(TRACE_SYSCALL), format[0], &info);

        /* Additionally, output any other buffer values that were filled in by a successful system call. */
        int result = (int)(uint32_t)(syscall_arg(-1));
        if (format[0]!='d' || (result<-1024 || result>=0) || -EINTR==result) {
            for (size_t i=1; format[i]; i++) {
                if ('-'!=format[i]) {
                    syscall_arginfo(format[i], syscall_arg(i-1), &info, &ap);
                    if ('P'!=format[i] || 0!=syscall_arg(i-1)) { /* no need to show null pointers */
                        fprintf(tracing(TRACE_SYSCALL), "    arg%zu = ", i-1);
                        print_single(tracing(TRACE_SYSCALL), format[i], &info);
                        fprintf(tracing(TRACE_SYSCALL), "\n");
                    }
                }
            }
        }
    }
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

/* Called asynchronously to make a signal pending. The signal will be dropped if it's action is to ignore. Otherwise it will be
 * made pending by setting the appropriate bit in the RSIM_Thread's signal_pending vector.  And yes, we know that this
 * function is not async signal safe when signal tracing is enabled. */
void
RSIM_Thread::signal_generate(int signo)
{
    ROSE_ASSERT(signo>0 && signo<_NSIG);
    uint64_t sigbit = (uint64_t)1 << (signo-1);
    bool is_masked = (0 != (signal_mask & sigbit));

    if (tracing(TRACE_SIGNAL)) {
        fprintf(tracing(TRACE_SIGNAL), " [generated ");
        print_enum(tracing(TRACE_SIGNAL), signal_names, signo);
        fprintf(tracing(TRACE_SIGNAL), "(%d)", signo);
    }
    
    if (signal_action[signo-1].handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        if (tracing(TRACE_SIGNAL))
            fputs(" ignored]", tracing(TRACE_SIGNAL));
    } else if (is_masked) {
        if (tracing(TRACE_SIGNAL))
            fputs(" masked]", tracing(TRACE_SIGNAL));
        signal_pending |= sigbit;
    } else {
        if (tracing(TRACE_SIGNAL))
            fputs("]", tracing(TRACE_SIGNAL));
        signal_pending |= sigbit;
        signal_reprocess = true;
    }
}

/* Deliver a pending, unmasked signal.  Posix doesn't specify an order, so we'll deliver the one with the lowest number. */
void
RSIM_Thread::signal_deliver_any()
{
    if (signal_reprocess) {
        signal_reprocess = false;
        for (size_t i=0; i<64; i++) {
            uint64_t sigbit = (uint64_t)1 << i;
            if ((signal_pending & sigbit) && 0==(signal_mask & sigbit)) {
                signal_pending &= ~sigbit;
                signal_deliver(i+1); /* bit N is signal N+1 */
                return;
            }
        }
    }
}

/* Deliver the specified signal. The signal is not removed from the signal_pending vector, nor is it added if it's masked. */
void
RSIM_Thread::signal_deliver(int signo)
{
    ROSE_ASSERT(signo>0 && signo<=64);

    if (tracing(TRACE_SIGNAL)) {
        fprintf(tracing(TRACE_SIGNAL), "0x%08"PRIx64": delivering ", policy.readIP().known_value());
        print_enum(tracing(TRACE_SIGNAL), signal_names, signo);
        fprintf(tracing(TRACE_SIGNAL), "(%d)", signo);
    }

    if (signal_action[signo-1].handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        /* The signal action may have changed since the signal was generated, so we need to check this again. */
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " ignored\n");
    } else if (signal_action[signo-1].handler_va==(uint32_t)(uint64_t)SIG_DFL) {
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " default\n");
        switch (signo) {
            case SIGFPE:
            case SIGILL:
            case SIGSEGV:
            case SIGBUS:
            case SIGABRT:
            case SIGTRAP:
            case SIGSYS:
                /* Exit process with core dump */
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

    } else if (signal_mask & ((uint64_t)1 << signo)) {
        /* Masked, but do not adjust signal_pending vector. */
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " masked (discarded)\n");
    } else {
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " to 0x%08"PRIx32"\n", signal_action[signo-1].handler_va);

        uint32_t signal_return = policy.readIP().known_value();
        policy.push(signal_return);

        /* Switch to the alternate stack? */
        uint32_t signal_oldstack = policy.readGPR(x86_gpr_sp).known_value();
        if (0==(signal_stack.ss_flags & SS_ONSTACK) && 0!=(signal_action[signo-1].flags & SA_ONSTACK))
            policy.writeGPR(x86_gpr_sp, policy.number<32>(signal_stack.ss_sp + signal_stack.ss_size));

        /* Push stuff that will be needed by the simulated sigreturn() syscall */
        policy.push(signal_oldstack);
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
        signal_mask |= signal_action[signo-1].mask;
        signal_mask |= (uint64_t)1 << (signo-1);
        // signal_reprocess = true;  -- Not necessary because we're not clearing any */

        /* Signal handler arguments */
        policy.push(policy.readIP());
        policy.push(policy.number<32>(signo));

        /* Invoke signal handler */
        policy.push(policy.number<32>(SIGHANDLER_RETURN)); /* fake return address to trigger signal_cleanup() call */
        policy.writeIP(policy.number<32>(signal_action[signo-1].handler_va));
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

    if (tracing(TRACE_SIGNAL)) {
        fprintf(tracing(TRACE_SIGNAL), "[returning from ");
        print_enum(tracing(TRACE_SIGNAL), signal_names, signo);
        fprintf(tracing(TRACE_SIGNAL), " handler]\n");
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
    uint64_t old_sigmask = policy.pop().known_value(); /* low bits */
    old_sigmask |= (uint64_t)policy.pop().known_value() << 32; /* hi bits */
    if (old_sigmask!=signal_mask)
        signal_reprocess = true;
    signal_mask = old_sigmask;

    /* Simulate return from sigreturn */
    policy.writeGPR(x86_gpr_sp, policy.pop());        /* restore stack pointer */
    policy.writeIP(policy.pop());                     /* RET instruction */
}

/* Suspend execution until a signal arrives. The signal must not be masked, and must either terminate the process or have a
 * signal handler. */
void
RSIM_Thread::signal_pause()
{
    /* Signals that terminate a process by default */
    uint64_t terminating = (uint64_t)(-1);
    terminating &= ~((uint64_t)1 << (SIGIO-1));
    terminating &= ~((uint64_t)1 << (SIGURG-1));
    terminating &= ~((uint64_t)1 << (SIGCHLD-1));
    terminating &= ~((uint64_t)1 << (SIGCONT-1));
    terminating &= ~((uint64_t)1 << (SIGSTOP-1));
    terminating &= ~((uint64_t)1 << (SIGTTIN-1));
    terminating &= ~((uint64_t)1 << (SIGTTOU-1));
    terminating &= ~((uint64_t)1 << (SIGWINCH-1));

    /* What signals would unpause this syscall? */
    uint64_t unpause = 0;
    for (uint64_t i=0; i<64; i++) {
        uint64_t sigbit = (uint64_t)1 << i;
        if (signal_action[i].handler_va==(uint64_t)SIG_DFL && 0!=(sigbit & terminating)) {
            unpause |= sigbit;
        } else if (signal_action[i].handler_va!=0) {
            unpause |= sigbit;
        }
    }

    /* Pause until the simulator receives a signal that should be delivered to the specimen.  We violate the
     * semantics a tiny bit here: the pause() syscall returns before the signal handler is invoked.  I don't
     * think this matters much since the handler will be invoked before the instruction that follows the "INT 80". */
    while (0==(signal_pending & unpause & ~signal_mask))
        pause();
}

void
RSIM_Thread::report_progress_maybe()
{
    if (tracing(TRACE_PROGRESS)) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double delta = (now.tv_sec - last_report.tv_sec) + 1e-1 * (now.tv_usec - last_report.tv_usec);
        if (delta > report_interval) {
            double insn_rate = delta>0 ? get_ninsns() / delta : 0;
            pid_t tid = syscall(SYS_gettid);
            fprintf(tracing(TRACE_PROGRESS), "RSIM_Thread: thread %d: processed %zu insns in %d sec (%d insns/sec)\n",
                    tid, get_ninsns(), (int)(delta+0.5), (int)(insn_rate+0.5));
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
            if (policy.readIP().known_value()==SIGHANDLER_RETURN) {
                signal_return();
                continue;
            }

            SgAsmx86Instruction *insn = current_insn();
            process->binary_trace_add(this, insn);
            semantics.processInstruction(insn);
            if (tracing(TRACE_STATE)) {
                fprintf(tracing(TRACE_STATE), "Machine state after instruction:\n");
                policy.dump_registers(tracing(TRACE_STATE));
            }

            signal_deliver_any();
        } catch (const RSIM_Semantics::Exception &e) {
            /* Thrown for instructions whose semantics are not implemented yet. */
            std::cerr <<e <<"\n\n";
#ifdef X86SIM_STRICT_EMULATION
            process->dump_core(SIGILL);
            abort();
#else
            std::cerr <<"Ignored. Continuing with a corrupt state...\n";
#endif
        } catch (const RSIM_SEMANTIC_POLICY::Exception &e) {
            std::cerr <<e <<"\n\n";
            process->dump_core(SIGILL);
            abort();
        } catch (const RSIM_Thread::Exit &e) {
            sys_exit(e);
            return NULL;
        } catch (const RSIM_SemanticPolicy::Signal &e) {
            signal_generate(e.signo);
        } catch (...) {
            process->dump_core(SIGILL);
            fprintf(stderr, "fatal: simulator thread received an unhandled exception\n");
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
        if (tracing(TRACE_SYSCALL))
            fprintf(tracing(TRACE_SYSCALL), "[entry #%d] ", idx);
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

    if (robust_list_head_va)
        fprintf(stderr, "warning: robust_list not cleaned up\n"); /* FIXME: see set_robust_list() syscall */

    /* Clear and signal child TID if necessary (CLONE_CHILD_CLEARTID) */
    if (clear_child_tid) {
        uint32_t zero = 0;
        size_t n = process->mem_write(&zero, clear_child_tid, sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        if (tracing(TRACE_SYSCALL))
            fprintf(tracing(TRACE_SYSCALL), "[futex wake 0x%08"PRIx32"] ", clear_child_tid);
        int nwoke = futex_wake(clear_child_tid);
        ROSE_ASSERT(nwoke>=0);
    }

    /* Remove the child from the process. */
    process->remove_thread(this);
    this->process = NULL;

    /* Cause the entire process to exit if necesary. */
    if (e.exit_process)
        process->exit(e.status);

    return e.status;
}

void
RSIM_Thread::post_fork()
{
    my_tid = syscall(SYS_gettid);
    assert(my_tid==getpid());
    process->post_fork();
}
