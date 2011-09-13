/* Demo for how to collect various statistics about running a specimen:
 *   - number of instructions executed
 *   - number of unique instructions executed (based on mnemonic)
 *   - number of system calls made
 *   - number of unique system calls made (based on syscall number) */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include <map>
#include <set>
#include <string>

class InsnStats: public RSIM_Callbacks::InsnCallback {
private:
    size_t ninsns;                              // total number of instructions executed
    typedef std::map<std::string/*mnemonic*/, size_t/*counter*/> Histogram;
    Histogram insns;                            // number of each kind of instruction executed

public:
    InsnStats(): ninsns(0) {}

    virtual InsnStats *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        assert(insn);
        ninsns++;

        std::string kind = stringifyX86InstructionKind(insn->get_kind(), "x86_");
        std::pair<Histogram::iterator, bool> inserted = insns.insert(std::make_pair<std::string, size_t>(kind, 1));
        if (!inserted.second)
            inserted.first->second++;

        return enabled;
    }
    
    size_t total_insns() const {
        return ninsns;
    }

    size_t unique_insns() const {
        return insns.size();
    }

    void print(std::ostream &o) const {
        o <<"Total instructions executed:\t" <<ninsns <<"\n";
        o <<"By instruction:\n";
        for (Histogram::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            o <<"  " <<ii->first <<":\t" <<ii->second <<"\n";
    }

    friend std::ostream& operator<<(std::ostream &o, const InsnStats &stats) {
        stats.print(o);
        return o;
    }
};


class SyscallStats: public RSIM_Callbacks::SyscallCallback {
private:
    size_t ncalls;
    typedef std::map<int/*callno*/, size_t/*counter*/> Histogram;
    Histogram calls;

public:
    SyscallStats(): ncalls(0) {}

    virtual SyscallStats *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        ncalls++;

        std::pair<Histogram::iterator, bool> inserted = calls.insert(std::make_pair<int, size_t>(args.callno, 1));
        if (!inserted.second)
            inserted.first->second++;
        return enabled;
    }

    size_t total_calls() const {
        return ncalls;
    }

    size_t unique_calls() const {
        return calls.size();
    }

    void print(std::ostream &o) const {
        o <<"Total syscalls called:\t" <<ncalls <<"\n";
        o <<"By call number:\n";
        for (Histogram::const_iterator ci=calls.begin(); ci!=calls.end(); ++ci)
            o <<"  " <<ci->first <<":\t" <<ci->second <<"\n";
    }

    friend std::ostream& operator<<(std::ostream &o, const SyscallStats &stats) {
        stats.print(o);
        return o;
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

#if 0
    std::cerr <<"Number of instructions executed: " <<insn_stats.total_insns() <<" (" <<insn_stats.unique_insns() <<" unique)\n";
    std::cerr <<"Number of system calls:          " <<call_stats.total_calls() <<" (" <<call_stats.unique_calls() <<" unique)\n";
#else
    std::cerr <<insn_stats <<call_stats;
#endif

    return 0;
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif
