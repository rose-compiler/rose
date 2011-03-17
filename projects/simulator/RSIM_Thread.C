#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <errno.h>
#include <stdarg.h>
#include <syscall.h>

#include <sys/user.h>

size_t RSIM_Thread::next_sequence_number = 1;

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
int
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
                return 0;
            default:
                /* Exit without a core dump */
                throw Exit(signo & 0x7f, true);
        }

    } else {
        /* Most of the code here is based on __setup_frame() in Linux arch/x86/kernel/signal.c */
        mesg->mesg("delivering %s(%d) to 0x%08"PRIx32,
                    flags_to_str(signal_names, signo).c_str(), signo, sa.handler_va);

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
            int status = sighand.sigaltstack(NULL, &stack, regs.sp);
            assert(status>=0);
            frame_va = sighand.get_sigframe(&sa, sizeof frame, regs.sp);

            frame.signo = signo;
            frame.pinfo = frame_va + OFFSET_OF_MEMBER(frame, info);
            frame.puc = frame_va + OFFSET_OF_MEMBER(frame, uc);

            /* FIXME: copy siginfo_t into frame.info */
            assert(0);

            frame.uc.uc_flags = 0; /* zero unless cpu_has_xsave; see Linux ia32_setup_rt_frame() */
            frame.uc.uc_link_va = 0;
            frame.uc.uc_stack.ss_sp = stack.ss_sp;
            frame.uc.uc_stack.ss_flags = sighand.on_signal_stack(frame_va) ? SS_ONSTACK : SS_DISABLE;
            frame.uc.uc_stack.ss_size = stack.ss_size;

            sighand.setup_sigcontext(&frame.uc.uc_mcontext, &regs, signal_mask);

            frame.uc.uc_sigmask = signal_mask;

            /* Restorer. If sa_flags 0x04000000 is set, then stack frame "pretcode" is set to the sa_restorer address passed to
             * sigaction. Otherwise pretcode points either to the "retcode" member of the stack frame (eight bytes of x86 code
             * that invoke syscall 119) or to the rt_sigreturn address in the VDSO. */
            if (sa.flags & 0x04000000/*SA_RESTORER, deprecated*/) {
                frame.pretcode = sa.restorer_va;
            } else {
                frame.pretcode = SIGHANDLER_RETURN; /* or could point to frame.retcode */
                //frame.preturn = frame_va + OFFSET_OF_MEMBER(frame, retcode);
                //frame.preturn = VDSO32_SYMBOL(vdso, rt_sigreturn); /* NOT IMPLEMENTED YET */
            }
        
            /* Signal handler return code. For pre-2.6 kernels, this was an eight-byte chunk of x86 code that calls
             * sys_sigreturn().  Newer kernels still push these bytes but never execute them. GDB uses them as a magic number
             * to recognize that it's at a signal stack frame. Instead, pretcode is the address of sigreturn in the VDSO.  For
             * now, we hard code it to a value that will be recognized by RSIM_Thread::main() as a
             * return-from-signal-handler. */
            frame.retcode[0] = 0xb8;    /* b8 00 00 00 77 | mov eax, 119 */
            frame.retcode[1] = 0x00;
            frame.retcode[2] = 0x00;
            frame.retcode[3] = 0x00;
            frame.retcode[4] = 0x77;
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

            sighand.setup_sigcontext(&frame.sc, &regs, signal_mask);
            frame.extramask = signal_mask >> 32;
            if (sa.flags & 0x04000000/*SA_RESTORER, deprecated*/) {
                frame.pretcode = sa.restorer_va;
            } else {
                frame.pretcode = SIGHANDLER_RETURN; /* or could point to frame.retcode */
                //frame.preturn = frame_va + OFFSET_OF_MEMBER(frame, retcode);
                //frame.preturn = VDSO32_SYMBOL(vdso, rt_sigreturn); /* NOT IMPLEMENTED YET */
            }

            frame.retcode[0] = 0x58;    /* 58             | pop eax */
            frame.retcode[1] = 0xb8;    /* b8 00 00 00 77 | mov eax, 119 */
            frame.retcode[2] = 0x00;
            frame.retcode[3] = 0x00;
            frame.retcode[4] = 0x00;
            frame.retcode[5] = 0x77;
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
        policy.writeFlag(x86_flag_df, policy.false_());
        policy.writeFlag(x86_flag_tf, policy.false_());

        /* Set up registers for signal handler */
        policy.writeGPR(x86_gpr_ax, policy.number<32>(signo));
        policy.writeGPR(x86_gpr_dx, policy.number<32>(0));
        policy.writeGPR(x86_gpr_cx, policy.number<32>(0));
        policy.writeSegreg(x86_segreg_ds, 0x2b);        /* see RSIM_SemanticPolicy::ctor() */
        policy.writeSegreg(x86_segreg_es, 0x2b);        /* see RSIM_SemanticPolicy::ctor() */
        policy.writeSegreg(x86_segreg_ss, 0x2b);        /* see RSIM_SemanticPolicy::ctor() */
        policy.writeSegreg(x86_segreg_cs, 0x23);        /* see RSIM_SemanticPolicy::ctor() */
        policy.writeGPR(x86_gpr_sp, policy.number<32>(frame_va));
        policy.writeIP(policy.number<32>(sa.handler_va)); /* we're now executing in the signal handler... */
    }

    return 0;
}

/* Note: if the specimen's signal handler never returns then this function is never invoked.  The specimen may do a longjmp()
 * or siglongjmp(), in which case the original stack, etc are restored anyway. Additionally, siglongjmp() may do a system call
 * to set the signal mask back to the value saved by sigsetjmp(), if any. */
int
RSIM_Thread::signal_return()
{
    RTS_Message *mesg = tracing(TRACE_SIGNAL);

    uint32_t sp = policy.readGPR(x86_gpr_sp).known_value();
    uint32_t frame_va = sp - 4; /* because sig handler's RET popped frame's pretcode */

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

    /* Restore hardware context */
    policy.writeSegreg(x86_segreg_gs, policy.number<16>(frame.sc.gs));
    policy.writeSegreg(x86_segreg_fs, policy.number<16>(frame.sc.fs));
    policy.writeSegreg(x86_segreg_es, policy.number<16>(frame.sc.es));
    policy.writeSegreg(x86_segreg_ds, policy.number<16>(frame.sc.ds));
    policy.writeGPR(x86_gpr_di, policy.number<32>(frame.sc.di));
    policy.writeGPR(x86_gpr_si, policy.number<32>(frame.sc.si));
    policy.writeGPR(x86_gpr_bp, policy.number<32>(frame.sc.bp));
    policy.writeGPR(x86_gpr_sp, policy.number<32>(frame.sc.sp));
    policy.writeGPR(x86_gpr_bx, policy.number<32>(frame.sc.bx));
    policy.writeGPR(x86_gpr_dx, policy.number<32>(frame.sc.dx));
    policy.writeGPR(x86_gpr_cx, policy.number<32>(frame.sc.cx));
    policy.writeGPR(x86_gpr_ax, policy.number<32>(frame.sc.ax));
    policy.writeIP(policy.number<32>(frame.sc.ip));
    policy.writeSegreg(x86_segreg_cs, policy.number<16>(frame.sc.cs));
    policy.writeSegreg(x86_segreg_ss, policy.number<16>(frame.sc.ss));

    /* Restore saved flags:  ac, of, df, tf, sf, zf, af, pf, cf, rf */
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
    uint32_t flags = (policy.get_eflags() & ~flag_mask) | (frame.sc.flags & flag_mask);
    policy.set_eflags(flags);

    return 0;
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

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    while (true) {
        pthread_testcancel();
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
                    policy.dump_registers(mesg);
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

    tracing(TRACE_THREAD)->mesg("this thread is terminating%s", e.exit_process?" (for entire process)":"");

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
    
    if (pid<0) {
        retval = -EINVAL;
    } else if (pid==getpid() && tid>=0) {
        RSIM_Thread *thread = process->get_thread(tid);
        if (!thread) {
            retval = -ESRCH;
        } else if (thread==this) {
            retval = sighand.generate(signo, process, tracing(TRACE_SIGNAL));
        } else {
            thread->sighand.generate(signo, process, thread->tracing(TRACE_SIGNAL));
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
