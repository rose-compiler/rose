/* Simulates a Windows executable (PE) using WINE on Linux */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "RSIM_Adapter.h"

/* Simulate RDTSC instruction by prividing values obtained from debugging. */
class Rdtsc: public RSIM_Callbacks::InsnCallback {
public:
    virtual Rdtsc *clone() { return this; }
    /* The TSC is based on the number of instructions simulated and a manual adjustment. */
    static const uint64_t start = 0xf95654a57da20ull;   // arbitrary
    size_t index;
    std::vector<int> adjustment;
    Rdtsc(): index(0) {
        adjustment.push_back(0);
        adjustment.push_back(0);        adjustment.push_back(0);
        adjustment.push_back(0);        adjustment.push_back(-5);
        adjustment.push_back(0);        adjustment.push_back(-10);
        adjustment.push_back(0);        adjustment.push_back(-15);
        adjustment.push_back(0);        adjustment.push_back(0);
    }
    uint64_t next_value(const Args& args) {
        return start + 1234 * args.thread->policy.get_ninsns() + (index < adjustment.size() ? adjustment[index++] : 0);
    }

    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        if (insn && x86_rdtsc==insn->get_kind()) {
            uint32_t newip = insn->get_address() + insn->get_raw_bytes().size();
            uint64_t value = next_value(args);
            uint32_t eax = value;
            uint32_t edx = value>>32;
            args.thread->policy.writeGPR(x86_gpr_ax, args.thread->policy.number<32>(eax));
            args.thread->policy.writeGPR(x86_gpr_dx, args.thread->policy.number<32>(edx));
            args.thread->policy.writeIP(args.thread->policy.number<32>(newip));
            enabled = false;
            args.thread->tracing(TRACE_MISC)->mesg("RDTSC adjustment; returning 0x%016"PRIx64, value);
        }
        return enabled;
    }
};

/** Provides implementations for functions not in ROSE.
 *
 *  These few functions are sometimes encountered in ld-linux.so and are important for its correct operation. */
class UnhandledInstruction: public RSIM_Callbacks::InsnCallback {
public:
    struct MmxValue {
        VirtualMachineSemantics::ValueType<32> lo, hi;
    };

    MmxValue mmx[8];                    // MMX registers 0-7

    virtual UnhandledInstruction *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        if (enabled && insn) {
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            uint32_t newip_va = insn->get_address() + insn->get_raw_bytes().size();
            VirtualMachineSemantics::ValueType<32> newip = args.thread->policy.number<32>(newip_va);
            switch (insn->get_kind()) {
                case x86_movd: {
                    assert(2==operands.size());
                    SgAsmRegisterReferenceExpression *mre = isSgAsmRegisterReferenceExpression(operands[0]);
                    if (mre && mre->get_descriptor().get_major()==x86_regclass_xmm) {
                        int mmx_number = mre->get_descriptor().get_minor();
                        m->mesg("UnhandledInstruction triggered for %s\n", unparseInstruction(insn).c_str());
                        mmx[mmx_number].lo = args.thread->semantics.read32(operands[1]);
                        mmx[mmx_number].hi = args.thread->policy.number<32>(0);
                        args.thread->policy.writeIP(newip);
                        enabled = false;
                    }
                    break;
                }

                case x86_movq: {
                    assert(2==operands.size());
                    SgAsmRegisterReferenceExpression *mre = isSgAsmRegisterReferenceExpression(operands[1]);
                    if (mre && mre->get_descriptor().get_major()==x86_regclass_xmm) {
                        int mmx_number = mre->get_descriptor().get_minor();
                        m->mesg("UnhandledInstruction triggered for %s\n", unparseInstruction(insn).c_str());
                        VirtualMachineSemantics::ValueType<32> addr = args.thread->semantics.readEffectiveAddress(operands[0]);
                        args.thread->policy.writeMemory(x86_segreg_ss, addr, mmx[mmx_number].lo, args.thread->policy.true_());
                        addr = args.thread->policy.add<32>(addr, args.thread->policy.number<32>(4));
                        args.thread->policy.writeMemory(x86_segreg_ss, addr, mmx[mmx_number].hi, args.thread->policy.true_());
                        args.thread->policy.writeIP(newip);
                        enabled = false;
                    }
                    break;
                }

                default:                // to shut up warnings about the zillion instructions we don't handle here
                    break;
            }
        }
        return enabled;
    }
};

/** wine-pthread tries to open /proc/self/maps.  This won't work because that file describes the simulator rather than the
 * specimen.  Therefore, when this occurs, we create a temporary file and initialize it with information from the specimen's
 * MemoryMap and open that instead.  NOTE: This appears to only happen during error reporting. */
class ProcMapsOpen: public RSIM_Simulator::SystemCall::Callback {
public:
    bool operator()(bool enabled, const Args &args) {
        RSIM_Thread *t = args.thread;
        if (enabled) {
            uint32_t filename_va = t->syscall_arg(0);
            bool error;
            std::string filename = t->get_process()->read_string(filename_va, 0, &error);
            if (error) {
                t->syscall_return(-EFAULT);
                return enabled;
            }

            if (filename=="/proc/self/maps") {
                RTS_WRITE(t->get_process()->rwlock()) {
                    FILE *f = fopen("x-maps", "w");
                    assert(f);
                    const std::vector<MemoryMap::MapElement> &me = t->get_process()->get_memory()->get_elements();
                    for (size_t i=0; i<me.size(); i++) {
                        unsigned p = me[i].get_mapperms();
                        fprintf(f, "%08"PRIx64"-%08"PRIx64" %c%c%cp 00000000 00:00 0 %s\n",
                                me[i].get_va(), me[i].get_va()+me[i].get_size(),
                                (p & MemoryMap::MM_PROT_READ)  ? 'r' : '-',
                                (p & MemoryMap::MM_PROT_WRITE) ? 'w' : '-',
                                (p & MemoryMap::MM_PROT_EXEC)  ? 'x' : '-',
                                me[i].get_name().c_str());
                    }
                    fclose(f);
                    filename = "x-maps";
                } RTS_WRITE_END;

                uint32_t flags=t->syscall_arg(1), mode=(flags & O_CREAT)?t->syscall_arg(2):0;
                int fd = open(filename.c_str(), flags, mode);
                if (-1==fd) {
                    t->syscall_return(-errno);
                    return enabled;
                }
                t->syscall_return(fd);
                return false; // this implementation was successful; skip the rest
            }
        }
        return enabled;
    }
};

class MapReporter: public RSIM_Simulator::SystemCall::Callback {
public:
    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled) {
            RSIM_Process *p = args.thread->get_process();
            p->mem_showmap(args.thread->tracing(TRACE_MISC), "MapReporter triggered: current memory map:", "    ");
        }
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    /**************************************************************************************************************************
     * Create simulator, including setting up all necessary callbacks.  The specimen is not actually loaded into memory until
     * later.
     **************************************************************************************************************************/
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

#if 0
    /* Disassemble when we hit the dynamic linker at 0x68000850 */
    MemoryDisassembler disassembler(0x68000850, false);
    sim.install_callback(&disassembler);
    sim.install_callback(new FunctionReporter);
#endif

    /* Emulate instructions that ROSE doesn't handle yet. */
    sim.install_callback(new UnhandledInstruction);

    /* Make adjustments for RDTSC instruction after the instruction executes. */
    sim.get_callbacks().add_insn_callback(RSIM_Callbacks::AFTER, new Rdtsc);

    /* System call handlers */
    sim.syscall_implementation(5/*open*/)->body.prepend(new ProcMapsOpen);
    sim.syscall_implementation(243/*set_thread_area*/)->body.prepend(new MapReporter);

    /**************************************************************************************************************************
     * Load the specimen into memory, create a process and its initial thread.
     **************************************************************************************************************************/
    
    sim.exec(argc-n, argv+n);
    RSIM_Process *proc = sim.get_process();
    assert(proc);
    RSIM_Thread *thread = proc->get_thread(getpid()); /* main thread */
    assert(thread);

    /* Initialize the stack from data saved when the program was run natively. */
    {
        rose_addr_t bottom_of_stack = 0xbffeb000;
        rose_addr_t new_stack_pointer = bottom_of_stack + 0x13f40; /* points to specimen's argc */

        int fd = open("x-real-initial-stack", O_RDONLY);
        if (fd>=0) {
            fprintf(stderr, "Initializing the stack...\n");
            uint8_t buf[4096];
            ssize_t nread;
            while ((nread=read(fd, buf, sizeof buf))>0) {
                size_t nwritten = proc->mem_write(buf, bottom_of_stack, (size_t)nread);
                assert(nwritten==(size_t)nread);
                bottom_of_stack += nwritten;
            }
            close(fd);
        }

        thread->policy.writeGPR(x86_gpr_sp, thread->policy.number<32>(new_stack_pointer));
    }

    /**************************************************************************************************************************
     * Allow the specimen to run
     **************************************************************************************************************************/

    sim.activate();
    sim.main_loop();
    sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // probably doesn't return
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
