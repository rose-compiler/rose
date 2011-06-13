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
#if 0
    /* Method 1: Use value obtained manually from GDB.  The deltas will be much larger than the specimen running natively. */
    size_t index;
    std::vector<uint64_t> values;
    Rdtsc() :index(0) {
        values.push_back(0xf95654a57da20ull);
        values.push_back(0xf9600d6542551ull);
        values.push_back(0xf96847e0d705aull);
        values.push_back(0xf96bda83742f3ull);
        values.push_back(0xf96f157c8ed33ull);
        values.push_back(0xf971a57ec0ae7ull);
        values.push_back(0xf9742fe76523cull);
        values.push_back(0xf9762f9bc171bull);
        values.push_back(0xf977d5c45d33eull);
        values.push_back(0xf979b58ede376ull);
        values.push_back(0xf97b502bb8877ull);
        values.push_back(0xf97e33947c78aull);
        values.push_back(0xf982044019373ull);
        values.push_back(0xf9842e2f190c7ull);
        values.push_back(0xf9869346f023full);
        values.push_back(0xf98904046d465ull);
        values.push_back(0xf98b7e5eb8d24ull);
        values.push_back(0xf98d9cff31ec2ull);
        values.push_back(0xf98fa66a0e1bfull);
    };
    uint64_t next_value(const Args&) {
        return index < values.size() ? values[index++] : values.back();
    }
#elsif 0
    /* Method 2: The TSC is based on the number of instructions simulated. */
    static const uint64_t start = 0xf95654a57da20ull;   // arbitrary
    uint64_t next_value(const Args &args) {
        return start + 1234 * args.thread->policy.get_ninsns();
    }
#else
    /* Method 3: The TSC is based on the number of instructions simulated and a manual adjustment. */
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
#endif
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
