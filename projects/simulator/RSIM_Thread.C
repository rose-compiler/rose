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
    memset(&last_report, 0, sizeof last_report);
    memset(signal_action, 0, sizeof signal_action);
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
        double elapsed = (this_call.tv_sec - first_call.tv_sec) + (this_call.tv_usec - first_call.tv_usec)/1e6;
        fprintf(tracing(TRACE_SYSCALL), "[pid %d] 0x%08"PRIx64" %8.4f: ", getpid(), policy.readIP().known_value(), elapsed);
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
        if (format[0]!='d' || -1!=result || -EINTR==result) {
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
                /* Exit with core dump */
                get_process()->dump_core(signo);
                throw Exit((signo & 0x7f) | __WCOREFLAG);
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
                throw Exit(signo & 0x7f);
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
                throw Exit(signo & 0x7f);
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
            fprintf(tracing(TRACE_PROGRESS), "RSIM_Thread: thread %d: processed %zu insns in %d sec (%d insns/sec)\n",
                    getpid(), get_ninsns(), (int)(delta+0.5), (int)(insn_rate+0.5));
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

