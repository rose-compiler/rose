/* Demo for how to collect various statistics about running a specimen:
 *   - number of instructions executed
 *   - number of unique instructions executed (based on mnemonic)
 *   - number of system calls made
 *   - number of unique system calls made (based on syscall number) */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include <set>
#include <string>

class InsnStats: public RSIM_Callbacks::InsnCallback {
private:
    size_t ninsns;                      // number of instructions executed
    std::set<std::string> insns;        // instruction mnemonics encountered

public:
    InsnStats(): ninsns(0) {}

    virtual InsnStats *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        assert(insn);
        ninsns++;
        insns.insert(stringifyX86InstructionKind(insn->get_kind(), "x86_"));
        return enabled;
    }
    
    size_t total_insns() const {
        return ninsns;
    }

    size_t unique_insns() const {
        return insns.size();
    }
};


class SyscallStats: public RSIM_Callbacks::SyscallCallback {
private:
    size_t ncalls;
    std::set<int> calls;

public:
    SyscallStats(): ncalls(0) {}

    virtual SyscallStats *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        ncalls++;
        calls.insert(args.callno);
        return enabled;
    }

    size_t total_calls() const {
        return ncalls;
    }

    size_t unique_calls() const {
        return calls.size();
    }
};


int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    InsnStats insn_stats;
    SyscallStats call_stats;
    sim.install_callback(&insn_stats);
    sim.install_callback(&call_stats);
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);

    sim.exec(argc-n, argv+n);
    sim.activate();
    sim.main_loop();
    sim.deactivate();

    std::cerr <<"Number of instructions executed: " <<insn_stats.total_insns() <<" (" <<insn_stats.unique_insns() <<" unique)\n";
    std::cerr <<"Number of system calls:          " <<call_stats.total_calls() <<" (" <<call_stats.unique_calls() <<" unique)\n";

    return 0;
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif
