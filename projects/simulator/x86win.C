/* Simulates a Windows executable (PE) using WINE on Linux */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "RSIM_Adapter.h"

class StackInitializer: public MemoryInitializer {
public:
    rose_addr_t newsp;
    StackInitializer(const std::string &filename, rose_addr_t memaddr, rose_addr_t when, rose_addr_t newsp)
        : MemoryInitializer(filename, memaddr, when), newsp(newsp) {}
    virtual StackInitializer *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        if (!triggered) {
            enabled = MemoryInitializer::operator()(enabled, args);
            if (triggered)
                args.thread->policy.writeGPR(x86_gpr_sp, args.thread->policy.number<32>(newsp));
        }
        return enabled;
    }
};

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

/***************************************************************************************************************************
 * The ld-linux.so.2 on Robb's machine executes a couple of instructions that aren't yet handled by ROSE.  So we handle
 * them here.  It turns out that we don't actually need to handle them in order for the linker to still work.
 *
 * 0x40014cd0: movd   mmx0, DWORD PTR ds:[eax + 0x04]
 *     10673:1 0.000 0x00014cd5[957]:     stack frames:
 *     10673:1 0.000 0x00014cd5[957]:       #0: bp=0xbfffe0a0 ip=0x00014cd5 in memory region ld-linux.so.2(LOAD#0)
 *     10673:1 0.000 0x00014cd5[957]:       #1: bp=0xbfffe0e8 ip=0x00001238 in memory region ld-linux.so.2(LOAD#0)
 *     10673:1 0.000 0x00014cd5[957]:       #2: bp=0x00000000 ip=0x00000857 in memory region ld-linux.so.2(LOAD#0)
 * 0x40014cd8: movq   QWORD PTR ss:[ebp + 0xd4<-0x2c>], mmx0]
 *     10673:1 0.000 0x00014cdd[959]:     stack frames:
 *     10673:1 0.000 0x00014cdd[959]:       #0: bp=0xbfffe0a0 ip=0x00014cdd in memory region ld-linux.so.2(LOAD#0)
 *     10673:1 0.000 0x00014cdd[959]:       #1: bp=0xbfffe0e8 ip=0x00001238 in memory region ld-linux.so.2(LOAD#0)
 *     10673:1 0.000 0x00014cdd[959]:       #2: bp=0x00000000 ip=0x00000857 in memory region ld-linux.so.2(LOAD#0)
 */
class LinkerFixups: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t base_va;
    VirtualMachineSemantics::ValueType<32> v1;

    LinkerFixups(rose_addr_t base_va)
        : base_va(base_va) {}

    virtual LinkerFixups *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        if (enabled && insn) {
            rose_addr_t newip = insn->get_address() + insn->get_raw_bytes().size();
            const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            if (insn->get_address()==base_va+0x00014cd0 && x86_movd==insn->get_kind()) {
                std::cerr <<"LinkerFixups: handling " <<unparseInstructionWithAddress(insn) <<"\n";
                v1 = args.thread->semantics.read32(operands[1]);
                args.thread->policy.writeIP(args.thread->policy.number<32>(newip));
                enabled = false; // skip this instruction
            } else if (insn->get_address()==base_va+0x00014cd8 && x86_movq==insn->get_kind()) {
                std::cerr <<"LinkerFixups: handling " <<unparseInstructionWithAddress(insn) <<"\n";
                VirtualMachineSemantics::ValueType<32> addr = args.thread->semantics.readEffectiveAddress(operands[0]);
                args.thread->policy.writeMemory(x86_segreg_ss, addr, v1, args.thread->policy.true_());
                addr = args.thread->policy.add<32>(addr, args.thread->policy.number<32>(4));
                args.thread->policy.writeMemory(x86_segreg_ss, addr, args.thread->policy.number<32>(0),
                                                args.thread->policy.true_());
                args.thread->policy.writeIP(args.thread->policy.number<32>(newip));
                enabled = false; // skip this instruction
            }
        }
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;

    /* Disassemble when we hit the dynamic linker at 0x68000850 */
    MemoryDisassembler disassembler(0x68000850, false);
    sim.install_callback(&disassembler);

    /* Initialize the stack when we hit the program entry address (filled in below). */
    StackInitializer stack_initializer("x-real-initial-stack", 0xbffeb000, 0, 0xbfffef70);
    sim.install_callback(&stack_initializer);
    sim.install_callback(new FunctionReporter);

    /* Emulate instructions that ROSE doesn't handle yet. */
    sim.install_callback(new LinkerFixups(0x68000000));

#if 0
    static uint8_t entry[] = {0x10, 0x93, 0x04, 0x08};
    sim.install_callback(new MemoryInitializer(entry, sizeof entry, 0xbfffee88, 0x6800252b));
#endif

#if 0
    /* Initializing memory stuff */
    rose_addr_t bp = 0x68000850;
    MemoryInitializer dd32data("x-real-dd32-data", 0x08054000, bp);
    sim.install_callback(&dd32data);
    MemoryInitializer ldsodata("x-real-ldso-data", 0x6801c000, bp);
    sim.install_callback(&ldsodata);
    MemoryInitializer preloaddata("x-real-preload-data", 0x7c403000, bp);
    sim.install_callback(&preloaddata);
    MemoryInitializer stackdata("x-real-stack", 0xbffeb000, bp);
    sim.install_callback(&stackdata);
#endif

    /* Make adjustments for RDTSC instruction after the instruction executes. */
    sim.get_callbacks().add_insn_callback(RSIM_Callbacks::AFTER, new Rdtsc);

    /* Load the specimen */
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);

    /* Finish initialization */
    rose_addr_t oep = sim.get_process()->get_ep_orig_va();
    stack_initializer.when = oep;
    
    /* Run the simulator */
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
