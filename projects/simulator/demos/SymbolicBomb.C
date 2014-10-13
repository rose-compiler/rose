// Example showing how to use multi-domain semantics to discover when expressions in the symbolic domain become too complex.
// Run this program like:
//   $ SymbolicBomb [RSIM_SWITCHES] EXECUTABLE
//
// The output will be a list of execution paths where complexity exceeds a certain threshold.  If the "--debug" switch is
// specified then output will also include a listing of all analyzed paths and the complexity at each instruction of each
// path.

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "MultiSemantics.h"
#include "SymbolicSemantics.h"
#include "PartialSymbolicSemantics.h"
#include "InsnSemanticsExpr.h"
#include "threadSupport.h"

using namespace rose;
using namespace rose::BinaryAnalysis;

// Everything about detecting symbolic complexity is encapsulated in this single class.  The class' instruction callback is
// invoked for each instruction and when it hits the @p when address an analysis is triggered (only once, and only by the first
// thread to reach the address).  The analysis disassembles the entire process memory and then analyzes each function
// individually.  When all is done, we allow the simulator to continue as normal.
class SymbolicBombDetector: public RSIM_Callbacks::InsnCallback {
protected:
    // These data members are protected by a mutex
    RTS_mutex_t mutex;          // to protect the following data members
    rose_addr_t when;           // when to trigger
    bool triggered;             // have we been triggered yet?
    SgAsmBlock *gblock;         // disassembled instrutions

    // These data members are not protected by the mutex...
    const char *cb_name;        // callback name to print in diagnostics
    size_t max_expr_size;       // maximum allowed total size of expressions in all registers
    size_t max_path_size;       // maximum length of execution path

public:
    // Define a multi-semantics domain incorporating two subdomains.
    typedef BinaryAnalysis::InstructionSemantics::MultiSemantics<
        BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::ValueType,
        BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::State,
        BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::Policy,
        BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType,
        BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State,
        BinaryAnalysis::InstructionSemantics::SymbolicSemantics::Policy
        > MultiSemantics;

    // Define the instruction decoder in terms of the multi-domain semantics
    typedef BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<
        MultiSemantics::Policy<MultiSemantics::State, MultiSemantics::ValueType>,
        MultiSemantics::ValueType
        > DecoderX86;

    // More convenient names for our two sub-policies.
    const MultiSemantics::SP0 SP_VMS;        // the PartialSymbolicSemantics sub-policy
    const MultiSemantics::SP1 SP_SYM;        // the SymbolicSemantics sub-policy


    // Constructor says when to trigger the analysis.  One will usually want to trigger it either at the very first
    // instruction, or just after ld-linux.so executes, but you can cause it to trigger (once) any time you like.  If you need
    // it to trigger more than once, then set "triggered" back to false each time.
    SymbolicBombDetector(rose_addr_t when)
        : when(when), triggered(false), gblock(NULL), cb_name("SymbolicBombDetector"),
          max_expr_size(3000), max_path_size(1000),
          SP_VMS(), SP_SYM() {
        RTS_mutex_init(&mutex, RTS_LAYER_USER_MIN, NULL);
    }

    // Share one callback among all threads. */
    virtual SymbolicBombDetector *clone() { return this; }

    // The instruction callback.  This can be called concurrently by many threads, so we need to protect the test/set of the
    // "triggered" data member.  Besides, we probably want all the other threads to block during the analysis anyway.
    virtual bool operator()(bool enabled, const Args &args) {
        RTS_MUTEX(mutex) {
            if (enabled && !triggered && args.insn->get_address()==when) {
                triggered = true;
                RTS_Message *m = args.thread->tracing(TRACE_MISC);
                m->multipart(cb_name, "%s: triggered\n", cb_name);
                if (!gblock) {
                    m->more("%s: disassembling entire process (may take a while)...\n", cb_name);
                    gblock = args.thread->get_process()->disassemble();
                }
                std::vector<SgAsmFunction*> funcs = SageInterface::querySubTree<SgAsmFunction>(gblock);
                for (size_t i=0; i<funcs.size(); ++i)
                    analyze_function(args.thread, funcs[i], m);
                m->multipart_end();
            }
        } RTS_MUTEX_END;
        return enabled;
    }

    // Analyze one function.  We do this by executing as many instructions as possible starting from the function entry point
    // and using our multi-domain semantics (defined above).  The virtual machine semantics drives the execution order, and we
    // execute until we don't know where we are or until we encounter an error (via exception).
    void
    analyze_function(RSIM_Thread *thread, SgAsmFunction *func, RTS_Message *m) {
        m->more("%s: analyzing function \"%s\" at 0x%08"PRIx64"\n", cb_name, func->get_name().c_str(), func->get_entry_va());
        RSIM_Process *proc = thread->get_process();
        MultiSemantics::Policy<MultiSemantics::State, MultiSemantics::ValueType> policy;
        DecoderX86 decoder(policy);

        // Execution starts at the function entry address.
        MultiSemantics::ValueType<32> eip;
        eip.set_subvalue(SP_VMS, policy.get_policy(SP_VMS).number<32>(func->get_entry_va()));
        eip.set_subvalue(SP_SYM, policy.get_policy(SP_SYM).number<32>(func->get_entry_va()));
        policy.writeRegister(decoder.REG_EIP, eip);
        std::vector<SgAsmX86Instruction*> execution_path;

        try {
            while (true) {
                // Fetch an instruction
                eip = policy.readRegister<32>(decoder.REG_EIP);
                if (!eip.get_subvalue(SP_VMS).is_known()) {
                    std::ostringstream ss;
                    ss <<eip.get_subvalue(SP_VMS);
                    m->more("%s:   unknown eip: %s\n", cb_name, ss.str().c_str());
                    break;
                }
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(proc->get_instruction(eip.get_subvalue(SP_VMS).known_value()));
                if (!insn)
                    break;

                // Simulate the instruction
                m->more("%s:   %s\n", cb_name, unparseInstructionWithAddress(insn).c_str());
                decoder.processInstruction(insn);
                execution_path.push_back(insn);

                // Measure the state complexity
                size_t c = symbolic_state_complexity(policy);
                m->more("%s:   complexity = %zu\n", cb_name, c);

                // Report bomb. Use stdout rather than the diagnostic stream since this is the main result of the analysis.
                if (c>max_expr_size || execution_path.size()>max_path_size) {
                    std::cerr <<cb_name <<": complexity = " <<c <<"; path length = " <<execution_path.size() <<"\n";
                    for (size_t i=0; i<execution_path.size(); ++i)
                        std::cerr <<"  #" <<i <<"\t" <<unparseInstructionWithAddress(execution_path[i]) <<"\n";
                    std::cerr <<policy.get_policy(SP_SYM).get_state() <<"\n";
                    break;
                }
            }
        } catch (const Disassembler::Exception &e) {
            std::ostringstream ss;
            ss <<e;
            m->more("%s: disassembler exception: %s\n", cb_name, ss.str().c_str());
        } catch (const DecoderX86::Exception &e) {
            std::ostringstream ss;
            ss <<e;
            m->more("%s: decoder exception: %s\n", cb_name, ss.str().c_str());
        } catch (const MultiSemantics::Policy<MultiSemantics::State, MultiSemantics::ValueType>::Exception &e) {
            std::ostringstream ss;
            ss <<e;
            m->more("%s: decoder policy exception: %s\n", cb_name, ss.str().c_str());
        }
    }

    // Calls the node counter for a particular value type.  T must be a MultiSemantic::ValueType<>.
    template<class T>
    size_t symbolic_expr_complexity(const T &multival) {
        return multival.get_subvalue(SP_SYM).get_expression()->nnodes();
    }

    // Counts nodes across all registers.  Note that for x86, some registers have multiple names depending on the part of the
    // physical register being referenced.  Therefore, by counting the complexity of each named register, we're counting most
    // physical registers multiple times.  That's fine, as long as we're consistent.
    size_t symbolic_state_complexity(MultiSemantics::Policy<MultiSemantics::State, MultiSemantics::ValueType> &policy) {
        const RegisterDictionary::Entries &regs = policy.get_register_dictionary()->get_registers();
        size_t nnodes = 0;
        for (RegisterDictionary::Entries::const_iterator ri=regs.begin(); ri!=regs.end(); ++ri) {
            try {
                const RegisterDescriptor &reg = ri->second;
                switch (reg.get_nbits()) { // arghh!  side effect of using templates :-/
                    case 1:  nnodes += symbolic_expr_complexity(policy.readRegister<1>(reg));  break;
                    case 8:  nnodes += symbolic_expr_complexity(policy.readRegister<8>(reg));  break;
                    case 16: nnodes += symbolic_expr_complexity(policy.readRegister<16>(reg)); break;
                    case 32: nnodes += symbolic_expr_complexity(policy.readRegister<32>(reg)); break;
                    default: /*skip these registers*/                                          break;
                }
            } catch (const MultiSemantics::Policy<MultiSemantics::State, MultiSemantics::ValueType>::Exception &e) {
                // register is probably not implemented in the state object, so skip it
            }
        }
        return nnodes;
    }
};

// Main program is almost standard RSIM boilerplate.  The only thing we add is the instantiation and installation of the
// SymbolicBombDetector callback.
int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);
    sim.install_callback(new SymbolicBombDetector(sim.get_process()->get_ep_start_va()), RSIM_Callbacks::BEFORE, true);
    sim.main_loop();
    sim.describe_termination(stderr);
    sim.terminate_self();
    return 0;
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
