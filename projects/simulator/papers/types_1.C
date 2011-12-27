#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "SymbolicSemantics.h"
#include "YicesSolver.h"

// If this symbol is undefined then the simulator will not forward its own signals to the specimen, making it easier to kill
// the specimen in some cases.  See its use in main().
#undef PRODUCTION

/** Stack-like list.  Defines push and pop, but the push only pushes if the value is not already on the stack. */
class WorkList {
private:
    std::vector<rose_addr_t> stack;
    std::set<rose_addr_t> present;
public:
    bool empty() const {
        return stack.empty();
    }

    void push(rose_addr_t val) {
        if (present.find(val)==present.end()) {
            stack.push_back(val);
            present.insert(val);
        }
    }

    rose_addr_t pop() {
        assert(!empty());
        rose_addr_t val = stack.back();
        stack.pop_back();
        present.erase(val);
        return val;
    }
};

/** Values used by analysis semantics.  This is a stub and is here so that if we need to add something to the values we can do
 *  so quite easily. */
template<size_t nBits>
class MyValueType: public SymbolicSemantics::ValueType<nBits> {
public:
    MyValueType(): SymbolicSemantics::ValueType<nBits>() {}
    MyValueType(const SymbolicSemantics::ValueType<nBits> &other): SymbolicSemantics::ValueType<nBits>(other) {}
    MyValueType(uint64_t n): SymbolicSemantics::ValueType<nBits>(n) {}
    explicit MyValueType(SymbolicSemantics::TreeNode *node): SymbolicSemantics::ValueType<nBits>(node) {}
};

/** Info passed from analyzer to semantics. */
struct AnalysisInfo {
    typedef std::vector<MyValueType<32> > PointerAddresses;
    AnalysisInfo(RSIM_Thread *thread, RTS_Message *m): pass(0), thread(thread), m(m) {}
    int pass;                                   /** Analysis pass number (0 or 1). */
    RSIM_Thread *thread;                        /** Thread doing the analysis. */
    RTS_Message *m;                             /** Message facility for analysis debugging. */
    SymbolicSemantics::InsnSet ptr_insns;       /** Instructions computing pointer R-values. */
    PointerAddresses ptr_addrs;                 /** Memory addresses storing pointers. */

    struct Comparator {
        const MyValueType<32> &addr;
        SMTSolver *solver;
        Comparator(const MyValueType<32> &addr, SMTSolver *solver=NULL): addr(addr), solver(solver) {}
        bool operator()(const MyValueType<32> &val) {
            return val.expr->equal_to(addr.expr, solver);
        }
    };

    /** Add address to list of pointer addresses.  Only added if not already present.  Comparison uses an SMT solver if
     *  possible. */
    void insert_addr(const MyValueType<32> &addr, SMTSolver *solver) {
        std::ostringstream ss; ss <<"      pointer addr: " <<addr;
        m->more("%s\n", ss.str().c_str());
        AnalysisInfo::PointerAddresses::iterator ai = find_if(ptr_addrs.begin(), ptr_addrs.end(), Comparator(addr, solver));
        if (ai==ptr_addrs.end())
            ptr_addrs.push_back(addr);
    }
};
    
/** Analysis state.  This class adds the ability to merge two states, which is used when two control flow paths meet. */
template<template <size_t> class T>
class MyState: public SymbolicSemantics::State<T> {
public:
    /** Merge other policy into this one and return true if this one changed.  We merge the list of instructions that define
     *  each register's value, but we don't merge the values themselves.  We also don't currently do anything for memory. */
    bool merge(const MyState &other) {
        size_t old_size = this->ip.defs.size();
        this->ip.defs.insert(other.ip.defs.begin(), other.ip.defs.end());
        bool changed = old_size!=this->ip.defs.size();
        for (size_t i=0; i<this->n_gprs; ++i) {
            old_size = this->gpr[i].defs.size();
            this->gpr[i].defs.insert(other.gpr[i].defs.begin(), other.gpr[i].defs.end());
            changed = changed || old_size!=this->gpr[i].defs.size();
        }
        for (size_t i=0; i<this->n_segregs; ++i) {
            old_size = this->segreg[i].defs.size();
            this->segreg[i].defs.insert(other.segreg[i].defs.begin(), other.segreg[i].defs.end());
            changed = changed || old_size!=this->segreg[i].defs.size();
        }
        for (size_t i=0; i<this->n_flags; ++i) {
            old_size = this->flag[i].defs.size();
            this->flag[i].defs.insert(other.flag[i].defs.begin(), other.flag[i].defs.end());
            changed = changed || old_size!=this->flag[i].defs.size();
        }
        return changed;
    }
};

/** Analysis policy.  When reading from memory, if the value is not found in the current state, then we consult the memory for
 *  the simulated specimen to obtain a concrete value if possible. */
template<
    template<template<size_t> class T> class S,
    template<size_t> class T>
class MyPolicy: public SymbolicSemantics::Policy<S, T>
{
public:
    typedef typename S<T>::Memory Memory;
    AnalysisInfo *info;

    // This is the set of all instructions that define addresses that are used to dereference memory through the DS register.
    // New instructions are added to this set as we discover them.
    SymbolicSemantics::InsnSet *addr_definers;

    MyPolicy(AnalysisInfo *info): info(info) {}

    bool merge(const MyPolicy &other) {
        info->m->more(" merging... ");
        bool changed = this->get_state().merge(other.get_state());
        info->m->more(changed ? "(changed)" : "(no change)");
        return changed;
    }

    template<size_t Len>
    T<Len> readMemory(X86SegmentRegister segreg, const T<32> &addr, const T<1> &cond) {
        SymbolicSemantics::MemoryCell<T> new_cell(addr, T<32>(), Len/8, NULL/*no defining instruction*/);
        bool aliased = false; /*is new_cell aliased by any existing writes?*/

        if (0==info->pass && segreg==x86_segreg_ds) {
            SymbolicSemantics::InsnSet defs; // defining instructions, excluding self
            for (SymbolicSemantics::InsnSet::const_iterator di=addr.defs.begin(); di!=addr.defs.end(); ++di) {
                if (*di!=this->cur_insn)
                    defs.insert(*di);
            }
#if 1
            if (!defs.empty()) {
                info->m->more("    DS read depends on insn%s", 1==defs.size()?"":"s");
                for (SymbolicSemantics::InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di)
                    info->m->more(" 0x%08"PRIx64, (*di)->get_address());
                info->m->more("\n");
            }
#endif
            info->ptr_insns.insert(defs.begin(), defs.end());
        } else if (1==info->pass && info->ptr_insns.find(this->cur_insn)!=info->ptr_insns.end()) {
            info->m->more("    Pointer L-value found.\n");
            info->insert_addr(addr, this->get_solver());
        }

        for (typename Memory::iterator mi=this->cur_state.mem.begin(); mi!=this->cur_state.mem.end(); ++mi) {
            if (new_cell.must_alias(*mi, this->solver)) {
                if ((*mi).clobbered) {
                    (*mi).clobbered = false;
                    (*mi).data = new_cell.data;
                    return this->template unsignedExtend<32, Len>(new_cell.data).defined_by(NULL, &new_cell.data.defs);
                } else {
                    return this->template unsignedExtend<32, Len>((*mi).data).defined_by(NULL, &(*mi).data.defs);
                }
            } else if ((*mi).written && new_cell.may_alias(*mi, this->solver)) {
                aliased = true;
            }
        }

        if (!aliased && addr.is_known()) {
            /* We didn't find the memory cell in the current state and it's not aliased to any writes in that state.
             * Therefore use the value from the initial (concrete) memory state, creating it in the current state. */
            uint32_t val_concrete = 0;
            uint32_t addr_concrete = addr.known_value();
            if (Len/8==info->thread->get_process()->mem_read(&val_concrete, addr_concrete, Len/8))
                new_cell.data = val_concrete;  // FIXME: Assuming guest and host architectures are both little endian
        }

        /* Create the cell in the current state. */
        this->cur_state.mem.push_back(new_cell);
        return this->template unsignedExtend<32,Len>(new_cell.data); // no defining instruction
    }
            
};

/** Data type analysis.  Monitor the calling thread's instruction pointer register (eip) and when it hits a specified value
 *  start running the data type analysis rooted at the specified analysis address.  The analysis address need not be the same
 *  as the trigger address; one might want to trigger the analysis after dynamic libraries are loaded but without actually ever
 *  calling the code to be analyzed. */
class PtrAnalysis: public RSIM_Callbacks::InsnCallback {
public:
    typedef std::map<rose_addr_t, MyPolicy<MyState, MyValueType> > StateMap;
    rose_addr_t trigger_addr, analysis_addr;
    static const bool debug = true;

    PtrAnalysis(rose_addr_t trigger_addr, rose_addr_t analysis_addr)
        : trigger_addr(trigger_addr), analysis_addr(analysis_addr) {}

    // This analysis is intended to run in a single thread, so clone is a no-op.
    virtual PtrAnalysis *clone() { return this; }

    // The actual analysis, triggered when we reach the specified execution address...
    virtual bool operator()(bool enabled, const Args &args) {
        static const char *name = "PtrAnalysis";
        if (enabled && args.insn->get_address()==trigger_addr) {
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->multipart(name, "%s triggered: analyzing function at 0x%08"PRIx64"\n", name, analysis_addr);
            AnalysisInfo info(args.thread, m); // to be passed down to policy operations

            // Create the policy that holds the analysis state which is modified by each instruction.  Then plug the policy
            // into the X86InstructionSemantics to which we'll feed each instruction.  Each time we process an instruction
            // we'll load this this policy object with the instructions initial state, process the instruction, and then obtain
            // the instruction's final state from the same policy object.  (The X86InstructionSemantics keeps a modifiable
            // reference to this policy.)
            MyPolicy<MyState, MyValueType> policy(&info);
            X86InstructionSemantics<MyPolicy<MyState, MyValueType>, MyValueType> semantics(policy);

            // Make two passes.  During the first pass we discover what addresses are used to dereference memory and what
            // instructions define those addresses, marking those instructions in the process.  During the second pass, if a
            // marked instruction reads from memory we remember that the memory location holds a pointer.  The policies all
            // point to "info" and update it appropriately.
            for (info.pass=0; info.pass<2; ++info.pass) {
                m->more((std::string(78, '=') + "\n  PASS " +
                         StringUtility::numberToString(info.pass) + "\n" +
                         std::string(78, '=') + "\n").c_str());
                StateMap before, after;
                WorkList worklist;
                worklist.push(analysis_addr);
                while (!worklist.empty()) {
                    // Process next instruction on work list by loading it's initial state
                    rose_addr_t va = worklist.pop();
                    SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.thread->get_process()->get_instruction(va));
                    m->more("  processing 0x%08"PRIx64": %s\n", insn->get_address(), unparseInstruction(insn).c_str());
                    StateMap::iterator bi=before.find(va);
                    policy = bi==before.end() ? MyPolicy<MyState, MyValueType>(&info) : bi->second;
                    if (debug) {
                        std::ostringstream ss;
                        ss <<"  -- Instruction's initial state --\n" <<policy;
                        m->more("%s", ss.str().c_str());
                    }
                    semantics.processInstruction(insn);
                    if (debug) {
                        std::ostringstream ss;
                        ss <<"  -- Instruction's final state --\n" <<policy;
                        m->more("%s", ss.str().c_str());
                    }
                    
                    // Save the state resulting from this instruction
                    std::pair<StateMap::iterator, bool> insert_result = after.insert(std::make_pair(va, policy));
                    if (!insert_result.second)
                        insert_result.first->second = policy;

                    // Find control flow successors
                    std::set<rose_addr_t> successors;
                    MyValueType<32> eip_value = policy.readRegister<32>(semantics.REG_EIP);
                    InsnSemanticsExpr::InternalNode *inode = dynamic_cast<InsnSemanticsExpr::InternalNode*>(eip_value.expr);
                    if (eip_value.is_known()) {
                        successors.insert(eip_value.known_value());
                    } else if (NULL!=inode && InsnSemanticsExpr::OP_ITE==inode->get_operator() &&
                               inode->child(1)->is_known() && inode->child(2)->is_known()) {
                        successors.insert(inode->child(1)->get_value());    // the if-true case
                        successors.insert(inode->child(2)->get_value());    // the if-false case
                    } else {
                        std::ostringstream ss; ss<<eip_value;
                        m->more("    unknown control flow successors: %s\n", ss.str().c_str());
                    }

                    // Merge output state of this instruction into input states of successors.
                    for (std::set<rose_addr_t>::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
                        m->more("    successor 0x%08"PRIx64, *si);
                        std::pair<StateMap::iterator, bool> insert_result = before.insert(std::make_pair(*si, policy));
                        if (insert_result.second || insert_result.first->second.merge(policy))
                            worklist.push(*si);
                        m->more("\n");
                    }
                }
            }

            // Final results
            m->more((std::string(78, '=') + "\n  POINTER ADDRESSES\n" + std::string(78, '=') + "\n").c_str());
            for (AnalysisInfo::PointerAddresses::const_iterator ai=info.ptr_addrs.begin(); ai!=info.ptr_addrs.end(); ++ai) {
                std::ostringstream ss;
                ss <<"  " <<*ai <<"\n";
                m->more("%s", ss.str().c_str());
            }
            if (info.ptr_addrs.empty())
                m->more("  No addresses found.\n");

            // Cleanup
            m->multipart_end();
            throw this;                 // optional, to terminate simulation
        }
        return enabled;
    }
};

int main(int argc, char *argv[], char *envp[])
{
    // Parse (and remove) switches intended for the analysis.
    std::string trigger_func, analysis_func;
    rose_addr_t trigger_va=0, analysis_va=0;

    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i], "--trigger=", 10)) {
            // name or address of function triggering analysis
            char *rest;
            trigger_va = strtoull(argv[i]+10, &rest, 0);
            if (*rest) {
                trigger_va = 0;
                trigger_func = argv[i]+10;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--analysis-func=", 16)) {
            // name or address of function to analyze
            char *rest;
            analysis_va = strtoull(argv[i]+16, &rest, 0);
            if (*rest) {
                analysis_va = 0;
                analysis_func = argv[i]+16;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        }
    }
    if (trigger_func.empty() && 0==trigger_va) {
        std::cerr <<argv[0] <<": --trigger-func=NAME_OR_ADDR is required\n";
        return 1;
    }
    if (analysis_func.empty() && 0==analysis_va) {
        std::cerr <<argv[0] <<": --analysis-func=NAME_OR_ADDR is required\n";
        return 1;
    }

    // Parse switches intended for the simulator.
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    // Parse the ELF container so we can get to the symbol table.  This is normal ROSE static analysis.
    char *rose_argv[4];
    int rose_argc=0;
    rose_argv[rose_argc++] = argv[0];
    rose_argv[rose_argc++] = strdup("-rose:read_executable_file_format_only");
    rose_argv[rose_argc++] = argv[n];
    rose_argv[rose_argc] = NULL;
    SgProject *project = frontend(rose_argc, rose_argv);

    // Find the address of "main" and analysis functions.
    if (!trigger_func.empty())
        trigger_va = RSIM_Tools::FunctionFinder().address(project, trigger_func);
    assert(trigger_va!=0);
    if (!analysis_func.empty())
        analysis_va = RSIM_Tools::FunctionFinder().address(project, analysis_func);
    assert(analysis_va!=0);

    // Register the analysis callback.
    PtrAnalysis analysis(trigger_va, analysis_va);
    sim.install_callback(&analysis);

    // The rest is normal boiler plate to run the simulator, except we'll catch the Analysis to terminate the simulation early
    // if desired.
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
#ifdef PRODUCTION
    sim.activate();
#endif
    try {
        sim.main_loop();
    } catch (PtrAnalysis*) {
        std::cerr <<"simulation terminated by PtrAnalysis.\n";
    }
#ifdef PRODUCTION
    sim.deactivate();
#endif
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
