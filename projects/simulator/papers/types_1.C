#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "SymbolicSemantics.h"
#include "YicesSolver.h"

// If this symbol is undefined then the simulator will not forward its own signals to the specimen, making it easier to kill
// the specimen in some cases.  See its use in main().
#undef PRODUCTION

// Choose an arbitrary concrete value for the stack pointer register (ESP) in order to simplify memory addresses.  This makes
// it easier for the output from the PtrAnalysis to be used by the OracleAnalysis.
static const rose_addr_t analysis_stack_va = 0xb0000000ul;

enum Verbosity {VB_SILENT, VB_MINIMAL, VB_SOME, VB_LOTS};

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

/******************************************************************************************************************************
 *                                      Values used for Pointer and Oracle Analysis
 ******************************************************************************************************************************/

// This is a stub and is here so that if we need to add something to the values we can do so quite easily.
template<size_t nBits>
class ValueType: public SymbolicSemantics::ValueType<nBits> {
public:
    ValueType(): SymbolicSemantics::ValueType<nBits>() {}
    ValueType(const SymbolicSemantics::ValueType<nBits> &other): SymbolicSemantics::ValueType<nBits>(other) {}
    ValueType(uint64_t n): SymbolicSemantics::ValueType<nBits>(n) {}
    explicit ValueType(SymbolicSemantics::TreeNode *node): SymbolicSemantics::ValueType<nBits>(node) {}
};


/******************************************************************************************************************************
 *                                              Pointer Type Analysis
 ******************************************************************************************************************************/


/** Info passed from analyzer to semantics. */
struct PtrInfo {
    typedef std::vector<ValueType<32> > PointerAddresses;
    int pass;                                   /** Analysis pass number (0 or 1). */
    RSIM_Thread *thread;                        /** Thread doing the analysis. */
    RTS_Message *m;                             /** Message facility for analysis debugging. */
    rose_addr_t analysis_addr;                  /** Address where analysis starts. */
    Verbosity verbosity;                        /** How verbose to be with messages. */
    SymbolicSemantics::InsnSet ptr_insns;       /** Instructions computing pointer R-values. */
    PointerAddresses ptr_addrs;                 /** Memory addresses storing pointers. */
    ExtentMap addrspc;                          /** If not empty, analysis stays within this address space. */

    PtrInfo(RSIM_Thread *thread, RTS_Message *m, rose_addr_t analysis_addr, Verbosity verbosity, const ExtentMap &addrspc)
        : pass(0), thread(thread), m(m), analysis_addr(analysis_addr), verbosity(verbosity), addrspc(addrspc) {}

    struct Comparator {
        const ValueType<32> &addr;
        SMTSolver *solver;
        Comparator(const ValueType<32> &addr, SMTSolver *solver=NULL): addr(addr), solver(solver) {}
        bool operator()(const ValueType<32> &val) {
            return val.expr->equal_to(addr.expr, solver);
        }
    };

    /** Add address to list of pointer addresses.  Only added if not already present.  Comparison uses an SMT solver if
     *  possible. */
    void insert_addr(const ValueType<32> &addr, SMTSolver *solver) {
        if (verbosity>=VB_SOME) {
            std::ostringstream ss; ss <<"      pointer addr: " <<addr;
            m->more("%s\n", ss.str().c_str());
        }
        PtrInfo::PointerAddresses::iterator ai = find_if(ptr_addrs.begin(), ptr_addrs.end(), Comparator(addr, solver));
        if (ai==ptr_addrs.end())
            ptr_addrs.push_back(addr);
    }

    /** Returns true if the specified address is a known pointer address.  The comparison of addresses is naive. */
    bool is_pointer(const ValueType<32> &addr, SMTSolver *solver) const {
        for (PointerAddresses::const_iterator pi=ptr_addrs.begin(); pi!=ptr_addrs.end(); ++pi) {
            if (pi->expr->equal_to(addr.expr, solver))
                return true;
        }
        return false;
    }
};
    
/** Analysis state.  This class adds the ability to merge two states, which is used when two control flow paths meet. */
template<template <size_t> class T>
class PtrState: public SymbolicSemantics::State<T> {
public:
    /** Merge other policy into this one and return true if this one changed.  We merge the list of instructions that define
     *  each register's value, but we don't merge the values themselves.  We also don't currently do anything for memory. */
    bool merge(const PtrState &other) {
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
class PtrPolicy: public SymbolicSemantics::Policy<S, T>
{
public:
    typedef typename S<T>::Memory Memory;
    PtrInfo *info;

    // This is the set of all instructions that define addresses that are used to dereference memory through the DS register.
    // New instructions are added to this set as we discover them.
    SymbolicSemantics::InsnSet *addr_definers;

    PtrPolicy(PtrInfo *info): info(info) {}

    bool merge(const PtrPolicy &other) {
        if (info->verbosity>=VB_LOTS)
            info->m->more(" merging... ");
        bool changed = this->get_state().merge(other.get_state());
        if (info->verbosity>=VB_LOTS)
            info->m->more(changed ? "(changed)" : "(no change)");
        return changed;
    }

    bool interesting_access(X86SegmentRegister segreg, const T<32> &addr, const T<1> &cond) {
        // return x86_segreg_ds == segreg;
        return true;
    }
    
    template<size_t Len>
    T<Len> readMemory(X86SegmentRegister segreg, const T<32> &addr, const T<1> &cond) {
        SymbolicSemantics::MemoryCell<T> new_cell(addr, T<32>(), Len/8, NULL/*no defining instruction*/);
        bool aliased = false; /*is new_cell aliased by any existing writes?*/

        if (0==info->pass && interesting_access(segreg, addr, cond)) {
            SymbolicSemantics::InsnSet defs; // defining instructions, excluding self
            for (SymbolicSemantics::InsnSet::const_iterator di=addr.defs.begin(); di!=addr.defs.end(); ++di) {
                if (*di!=this->cur_insn)
                    defs.insert(*di);
            }
            if (!defs.empty() && info->verbosity>=VB_SOME) {
                info->m->more("    Read depends on insn%s", 1==defs.size()?"":"s");
                for (SymbolicSemantics::InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di)
                    info->m->more(" 0x%08"PRIx64, (*di)->get_address());
                info->m->more("\n");
            }
            info->ptr_insns.insert(defs.begin(), defs.end());
        } else if (1==info->pass && info->ptr_insns.find(this->cur_insn)!=info->ptr_insns.end()) {
            if (info->verbosity>=VB_SOME)
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

    template<size_t Len>
    void writeMemory(X86SegmentRegister segreg, const T<32> &addr, const T<Len> &val, const T<1> &cond) {
        if (0==info->pass && interesting_access(segreg, addr, cond)) {
            SymbolicSemantics::InsnSet defs; // defining instructions, excluding self
            for (SymbolicSemantics::InsnSet::const_iterator di=addr.defs.begin(); di!=addr.defs.end(); ++di) {
                if (*di!=this->cur_insn)
                    defs.insert(*di);
            }
            if (!defs.empty() && info->verbosity>=VB_SOME) {
                info->m->more("    Write depends on insn%s", 1==defs.size()?"":"s");
                for (SymbolicSemantics::InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di)
                    info->m->more(" 0x%08"PRIx64, (*di)->get_address());
                info->m->more("\n");
            }
            info->ptr_insns.insert(defs.begin(), defs.end());
        }

        SymbolicSemantics::MemoryCell<T> new_cell(addr, this->template unsignedExtend<Len, 32>(val), Len/8, NULL/*no defng insn*/);
        bool saved = false; // has new_cell been saved to memory?
        for (typename Memory::iterator mi=this->cur_state.mem.begin(); mi!=this->cur_state.mem.end(); ++mi) {
            if (new_cell.must_alias(*mi, this->solver)) {
                *mi = new_cell;
                saved = true;
            } else if (new_cell.may_alias(*mi, this->solver)) {
                (*mi).set_clobbered();
            }
        }
        if (!saved)
            this->cur_state.mem.push_back(new_cell);
    }
};

/** Perform a pointer analysis, looking for memory addresses that contain pointers.  Return value is a PtrInfo object that
 *  contains a list of memory addresses (possibly symbolic) that we think contain pointers. */
class PtrAnalysis {
public:
    typedef PtrPolicy<PtrState, ValueType> Policy;
    typedef X86InstructionSemantics<Policy, ValueType> Semantics;
    typedef std::map<rose_addr_t, PtrPolicy<PtrState, ValueType> > StateMap;

    // Limit the number of instructions we process.  The symbolic expressions can become very large quite quickly.  A better
    // approach might be to bail when the expression complexity reaches a certain point.
    static const size_t max_processed = 200;

    // Do the analysis
    void analyze(PtrInfo &info/*in,out*/) {
        static const char *name = "PtrAnalysis";
        if (info.verbosity>=VB_MINIMAL) {
            info.m->multipart(name, "%s triggered: analysis starts at 0x%08"PRIx64"\n", name, info.analysis_addr);
            if (!info.addrspc.empty()) {
                std::ostringstream ss;
                ss <<info.addrspc;
                info.m->more("%s: analysis restricted to these %zu addresses: %s\n", name, info.addrspc.size(), ss.str().c_str());
            }
        }

        // Create the policy that holds the analysis state which is modified by each instruction.  Then plug the policy
        // into the X86InstructionSemantics to which we'll feed each instruction.  Each time we process an instruction
        // we'll load this this policy object with the instructions initial state, process the instruction, and then obtain
        // the instruction's final state from the same policy object.  (The X86InstructionSemantics keeps a modifiable
        // reference to this policy.)
        Policy policy(&info);
        Semantics semantics(policy);
        const RegisterDescriptor *ESP = policy.get_register_dictionary()->lookup("esp");
        const RegisterDescriptor *EIP = policy.get_register_dictionary()->lookup("eip");

        // Make two passes.  During the first pass we discover what addresses are used to dereference memory and what
        // instructions define those addresses, marking those instructions in the process.  During the second pass, if a
        // marked instruction reads from memory we remember that the memory location holds a pointer.  The policies all
        // point to "info" and update it appropriately.
        for (info.pass=0; info.pass<2; ++info.pass) {
            if (info.verbosity>=VB_SOME)
                info.m->more("%s: pass %d\n", name, info.pass);
            StateMap before, after;
            WorkList worklist;
            size_t nprocessed = 0; // number of instructions processed, including failures

            // Initialize the policy for the first simulated instruction.  We choose a concrete value for the ESP register
            // only because we'll use that same value in another analysis.
            PtrPolicy<PtrState, ValueType> initial_policy(&info);
            initial_policy.writeRegister(*ESP, policy.number<32>(analysis_stack_va)); // arbitrary
            before.insert(std::make_pair(info.analysis_addr, initial_policy));

            worklist.push(info.analysis_addr);
            while (!worklist.empty() && nprocessed++ < max_processed) {
                // Process next instruction on work list by loading it's initial state
                rose_addr_t va = worklist.pop();
                SgAsmx86Instruction *insn = NULL;
                try {
                    insn = isSgAsmx86Instruction(info.thread->get_process()->get_instruction(va));
                } catch (const Disassembler::Exception &e) {
                    if (info.verbosity>=VB_SOME) {
                        std::ostringstream ss; ss <<e;
                        info.m->more("%s: caught disassembler exception: %s\n", name, ss.str().c_str());
                    }
                    continue;
                }

                // Skip the instruction if it's (partially) outside our address limits.
                if (!info.addrspc.empty() && !info.addrspc.contains(Extent(insn->get_address(), insn->get_size()))) {
                    if (info.verbosity>=VB_SOME)
                        info.m->more("  skipping   0x%08"PRIx64": %s (outside address range limit)\n",
                                     insn->get_address(), unparseInstruction(insn).c_str());
                    continue;
                }

                if (info.verbosity>=VB_SOME)
                    info.m->more("  processing 0x%08"PRIx64": %s\n", insn->get_address(), unparseInstruction(insn).c_str());
                StateMap::iterator bi=before.find(va);
                policy = bi==before.end() ? PtrPolicy<PtrState, ValueType>(&info) : bi->second;
                policy.writeRegister(*EIP, policy.number<32>(insn->get_address()));
                if (info.verbosity>=VB_LOTS) {
                    std::ostringstream ss;
                    ss <<"  -- Instruction's initial state --\n" <<policy;
                    info.m->more("%s", ss.str().c_str());
                }
                try {
                    semantics.processInstruction(insn);
                } catch (const Semantics::Exception &e) {
                    if (info.verbosity>=VB_LOTS) {
                        std::ostringstream ss; ss <<e;
                        info.m->more("%s: caught semantics exception: %s", name, ss.str().c_str());
                    }
                } catch (const Policy::Exception &e) {
                    if (info.verbosity>=VB_LOTS) {
                        std::ostringstream ss; ss <<e;
                        info.m->more("%s: caught policy exception: %s", name, ss.str().c_str());
                    }
                }
                if (info.verbosity>=VB_LOTS) {
                    std::ostringstream ss;
                    ss <<"  -- Instruction's final state --\n" <<policy;
                    info.m->more("%s", ss.str().c_str());
                }

                // Save the state resulting from this instruction
                std::pair<StateMap::iterator, bool> insert_result = after.insert(std::make_pair(va, policy));
                if (!insert_result.second)
                    insert_result.first->second = policy;

                // Find control flow successors
                std::set<rose_addr_t> successors;
                ValueType<32> eip_value = policy.readRegister<32>(semantics.REG_EIP);
                InsnSemanticsExpr::InternalNode *inode = dynamic_cast<InsnSemanticsExpr::InternalNode*>(eip_value.expr);
                if (eip_value.is_known()) {
                    successors.insert(eip_value.known_value());
                    // assume all CALLs return since we might not actually traverse the called function
                    if (insn->get_kind()==x86_call)
                        successors.insert(insn->get_address()+insn->get_size());
                } else if (NULL!=inode && InsnSemanticsExpr::OP_ITE==inode->get_operator() &&
                           inode->child(1)->is_known() && inode->child(2)->is_known()) {
                    successors.insert(inode->child(1)->get_value());    // the if-true case
                    successors.insert(inode->child(2)->get_value());    // the if-false case
                } else {
                    std::ostringstream ss; ss<<eip_value;
                    if (info.verbosity>=VB_LOTS)
                        info.m->more("    unknown control flow successors: %s\n", ss.str().c_str());
                }

                // Merge output state of this instruction into input states of successors.
                for (std::set<rose_addr_t>::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
                    if (info.verbosity>=VB_LOTS)
                        info.m->more("    successor 0x%08"PRIx64, *si);
                    std::pair<StateMap::iterator, bool> insert_result = before.insert(std::make_pair(*si, policy));
                    if (insert_result.second || insert_result.first->second.merge(policy))
                        worklist.push(*si);
                    if (info.verbosity>=VB_LOTS)
                        info.m->more("\n");
                }
            }

            if (nprocessed>max_processed && info.verbosity>=VB_MINIMAL)
                info.m->more("  limited processing after %zu instructions\n", max_processed);
        }

        // Final results
        if (info.verbosity>=VB_MINIMAL) {
            info.m->more("%s: pointer addresses:\n", name);
            for (PtrInfo::PointerAddresses::const_iterator ai=info.ptr_addrs.begin(); ai!=info.ptr_addrs.end(); ++ai) {
                std::ostringstream ss;
                ss <<"  " <<*ai <<"\n";
                info.m->more("%s", ss.str().c_str());
            }
            if (info.ptr_addrs.empty())
                info.m->more("  No addresses found.\n");
        }

        // Cleanup
        if (info.verbosity>=VB_MINIMAL)
            info.m->multipart_end();
    }
};



/******************************************************************************************************************************
 *                                              Memory Oracle Analysis
 ******************************************************************************************************************************/

template<template <size_t> class ValueType>
class OracleState: public SymbolicSemantics::State<ValueType> {
public:
};

template<
    template<template<size_t> class ValueType> class State,
    template<size_t> class ValueType>
class OraclePolicy: public SymbolicSemantics::Policy<State, ValueType>
{
public:
    typedef typename       SymbolicSemantics::Policy<State, ValueType>  super;
    typedef typename State<ValueType>::Memory Memory;
    const PtrInfo &info; // memory addresses of known pointer variables

    OraclePolicy(const PtrInfo &info): info(info) {}

    // Provide a memory oracle
    template<size_t nBits>
    ValueType<nBits>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) {
        if (info.verbosity>=VB_SOME) {
            std::ostringstream ss;
            ss <<"    reading from memory address " <<addr <<"\n";
            info.m->more("%s", ss.str().c_str());
        }
        ValueType<nBits> retval = super::template readMemory<nBits>(segreg, addr, cond);
        if (!retval.is_known()) {
            retval = info.is_pointer(addr, this->get_solver()) ? random_pointer<nBits>() : random_integer<nBits>();
            super::template writeMemory<nBits>(segreg, addr, retval, cond);
        }
        return retval;
    }

    // Return a random pointer
    template<size_t nBits>
    ValueType<nBits>
    random_pointer() const {
        ValueType<nBits> retval = this->template number<nBits>(rand() % (1ul << nBits));
        if (info.verbosity>=VB_SOME) {
            std::ostringstream ss;
            ss <<"    providing a random pointer: " <<retval <<"\n";
            info.m->more("%s", ss.str().c_str());
        }
        return retval;
    }

    // Return a random integer
    template<size_t nBits>
    ValueType<nBits>
    random_integer() const {
        static const uint64_t limit = 100; // arbitrary
        ValueType<nBits> retval = this->template number<nBits>(rand() % limit);
        if (info.verbosity>=VB_SOME) {
            std::ostringstream ss;
            ss <<"    providing a random unsigned integer: " <<retval <<"\n";
            info.m->more("%s", ss.str().c_str());
        }
        return retval;
    }
};

class OracleAnalysis {
public:
    typedef OraclePolicy<OracleState, ValueType> Policy;
    typedef X86InstructionSemantics<Policy, ValueType> Semantics;

    // Throws:
    //   Disassembler::Exception when it can't disassemble something, probably because we supplied a bugus value for
    //   the instruction pointer.
    size_t analyze(const PtrInfo &info) {
        static const char *name = "OracleAnalysis";
        if (info.verbosity>=VB_MINIMAL)
            info.m->multipart(name, "%s triggered: analysis starts at 0x%08"PRIx64"\n", name, info.analysis_addr);

        static const size_t limit = 200;        // max number of instructions to process
        size_t ninsns = 0;                      // number of instructions processed
        Policy policy(info);
        Semantics semantics(policy);

        try {
            // Initialize the policy for the first simulated instruction.
            policy.writeRegister(semantics.REG_EIP, policy.number<32>(info.analysis_addr));
            policy.writeRegister(semantics.REG_ESP, policy.number<32>(analysis_stack_va));

            // Simulate some instructions
            for (/*void*/; ninsns<limit && policy.readRegister<32>(semantics.REG_EIP).is_known(); ++ninsns) {
                rose_addr_t va = policy.readRegister<32>(semantics.REG_EIP).known_value();
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(info.thread->get_process()->get_instruction(va));
                if (info.verbosity>=VB_SOME)
                    info.m->more("  processing 0x%08"PRIx64": %s\n", insn->get_address(), unparseInstruction(insn).c_str());
                semantics.processInstruction(insn);
                if (info.verbosity>=VB_LOTS) {
                    std::ostringstream ss;
                    ss <<"  -- State --\n" <<policy;
                    info.m->more("%s", ss.str().c_str());
                }
            }
        } catch (const Disassembler::Exception &e) {
            if (info.verbosity>=VB_MINIMAL) {
                std::ostringstream ss; ss <<e;
                info.m->more("%s: caught disassembler exception: %s\n", name, ss.str().c_str());
            }
        } catch (const Semantics::Exception &e) {
            if (info.verbosity>=VB_MINIMAL) {
                std::ostringstream ss; ss <<e;
                info.m->more("%s: caught semantics exception: %s\n", name, ss.str().c_str());
            }
        } catch (const Policy::Exception &e) {
            if (info.verbosity>=VB_MINIMAL) {
                std::ostringstream ss; ss <<e;
                info.m->more("%s: caught policy exception: %s", name, ss.str().c_str());
            }
        } catch (...) {
            // just rethrow the exception and allow it to be caught (and described) by the simulator as if the simulator
            // had encountered it during normal simulation.
            if (info.verbosity>=VB_MINIMAL) {
                std::ostringstream ss; ss <<policy;
                info.m->more("%s: caught an exception; terminating analysis after processing %zu instruction%s.  Final state:\n%s",
                             name, ninsns, 1==ninsns?"":"s", ss.str().c_str());
                info.m->multipart_end();
            }
            throw;
        }
        
        // Cleanup
        if (info.verbosity>=VB_MINIMAL) {
            std::ostringstream ss; ss <<policy;
            info.m->more("%s finished after processing %zu instruction%s.  Final state:\n%s",
                         name, ninsns, 1==ninsns?"":"s", ss.str().c_str());
            info.m->multipart_end();
        }
        return ninsns;
    }
};


/******************************************************************************************************************************
 *                                              Top-level analysis
 *
 * This callback just waits for the EIP to reach a certain value and then fires off the pointer and oracle analyses.
 ******************************************************************************************************************************/

class AnalysisTrigger: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_addr, analysis_addr;
    size_t randomize, niters;
    Verbosity verbosity;
    ExtentMap limits; // limits for choosing random analysis addresses; empty means no limits

    AnalysisTrigger(rose_addr_t trigger_addr, rose_addr_t analysis_addr, size_t randomize, const ExtentMap &limits,
                    size_t niters, Verbosity verbosity)
        : trigger_addr(trigger_addr), analysis_addr(analysis_addr), randomize(randomize), niters(niters),
          verbosity(verbosity), limits(limits) {}

    // This analysis is intended to run in a single thread, so clone is a no-op.
    virtual AnalysisTrigger *clone() { return this; }

    // Given a function, figure out what part of the address space its instructions occupy.
    ExtentMap function_extent(SgAsmFunction *func) {
        struct: public AstSimpleProcessing {
            ExtentMap extents;
            void visit(SgNode *node) {
                SgAsmInstruction *insn = isSgAsmInstruction(node);
                if (insn)
                    extents.insert(Extent(insn->get_address(), insn->get_size()));
            }
        } t;
        t.traverse(func, preorder);
        return t.extents;
    }

    // The actual analysis, triggered when we reach the specified execution address...
    virtual bool operator()(bool enabled, const Args &args) {
        static const bool limit_to_function = true;
        if (enabled && args.insn->get_address()==trigger_addr) {
            static const char *name = "AnalysisTrigger";
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("%s triggered\n", name);

            // Show a memory map to aid the user in choosing address limits in future runs.
            args.thread->get_process()->mem_showmap(m, "process memory map:\n");

            // Disassemble if necessary.  The only reason we would need to do this is if we need to choose random addresses for
            // the analysis (fast, simple disassembly) and/or if we need to know how instructions are related to functions.
            std::vector<SgAsmInstruction*> insns;
            if (randomize>0 || limit_to_function) {
                m->mesg("%s: disassembling the whole program...", name);
                SgAsmBlock *gblk = args.thread->get_process()->disassemble(!limit_to_function);
                insns = SageInterface::querySubTree<SgAsmInstruction>(gblk);
                m->mesg("%s: disassembled %zu instructions", name, insns.size());
#if 1
                const char *listing_name = "x-prog.lst";
                m->mesg("%s: generating assembly listing in \"%s\"", name, listing_name);
                std::ofstream listing(listing_name);
                AsmUnparser().unparse(listing, gblk);
#endif
                if (!limits.empty()) {
                    std::ostringstream ss; ss<<limits;
                    m->mesg("%s: limiting instructions to addresses: %s\n", name, ss.str().c_str());
                    std::vector<SgAsmInstruction*> tmp = insns;
                    insns.clear();
                    for (size_t i=0; i<tmp.size(); ++i) {
                        if (limits.contains(Extent(tmp[i]->get_address(), tmp[i]->get_size())))
                            insns.push_back(tmp[i]);
                    }
                    m->mesg("%s: %zu instructions left from which to choose\n", name, insns.size());
                }
            }

            for (size_t n=0; n<std::max((size_t)1, randomize); ++n) {

                // Choose an analysis address
                SgAsmInstruction *insn = NULL;
                if (n>0 || (randomize>0 && 0==analysis_addr)) {
                    insn = insns[rand() % insns.size()];
                    analysis_addr = insn->get_address();
                } else {
                    try {
                        insn = args.thread->get_process()->get_instruction(analysis_addr);
                    } catch (...) {
                    }
                }

                // Limit the range of addresses that will be considered during the analysis
                ExtentMap addrspc;
                if (limit_to_function) {
                    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
                    if (func) {
                        addrspc = function_extent(func);
                    } else {
                        m->mesg("%s: cannot limit analysis to a single function at insn at 0x%08"PRIx64, name, analysis_addr);
                    }
                } else if (!limits.empty()) {
                    addrspc = limits;
                }

                // Do the analyses
                PtrInfo ptr_info(args.thread, m, analysis_addr, verbosity, addrspc);
                PtrAnalysis().analyze(ptr_info);
                for (size_t iter=0; iter<niters; ++iter) {
                    m->mesg("%s: MemoryOracle at virtual address 0x%08"PRIx64" (%zu of %zu), iteration %zu",
                            name, analysis_addr, n+1, randomize, iter);
                    size_t ninsns = OracleAnalysis().analyze(ptr_info);
                    m->mesg("%s:   MemoryOracle processed %zu instruction%s", name, ninsns, 1==ninsns?"":"s");
                }
            }
            throw this; // terminate simulation
        }
        return enabled;
    }
};

int main(int argc, char *argv[], char *envp[])
{
    // Parse (and remove) switches intended for the analysis.
    std::string trigger_func, analysis_func;
    rose_addr_t trigger_va=0, analysis_va=0;
    size_t randomize=0, niters=1;
    Verbosity verbosity=VB_MINIMAL;
    ExtentMap limits;

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
        } else if (!strncmp(argv[i], "--random=", 9)) {
            // number of analyses (i.e., number of analysis starting addresses)
            randomize = strtoul(argv[i]+9, NULL, 0);
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--limits=", 9)) {
            // when --random is specified, limit instructions to those which are contained in the specified part of the address
            // range.  The value of this switch should be a comma-separated list of addresses appearing in pairs consisting of
            // the low limit and high limit (inclusive).
            char *s=argv[i]+9, *rest;
            while (*s) {
                rose_addr_t lo=strtoull(s, &rest, 0);
                s = *rest ? rest+1 : rest;
                rose_addr_t hi=strtoull(s, &rest, 0);
                s = *rest ? rest+1 : rest;
                if (hi<lo) {
                    std::cerr <<argv[0] <<": --limits switch should have pairs of inclusive address limits.\n";
                    return 1;
                }
                limits.insert(Extent::inin(lo, hi));
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--analysis=", 11)) {
            // name or address of function to analyze.  If the name is "oep" then trigger on the original entry point.
            char *rest;
            analysis_va = strtoull(argv[i]+11, &rest, 0);
            if (*rest) {
                analysis_va = 0;
                analysis_func = argv[i]+11;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--iterations=", 13)) {
            // number of iterations of each analysis
            niters = strtoul(argv[i]+13, NULL, 0);
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--verbosity=", 12)) {
            if (!strcmp(argv[i]+12, "silent")) {
                verbosity = VB_SILENT;
            } else if (!strcmp(argv[i]+12, "minimal")) {
                verbosity = VB_MINIMAL;
            } else if (!strcmp(argv[i]+12, "some")) {
                verbosity = VB_SOME;
            } else if (!strcmp(argv[i]+12, "lots")) {
                verbosity = VB_LOTS;
            } else {
                std::cerr <<argv[0] <<": --verbosity should be silent, minimal, some, or lots\n";
                return 1;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        }
    }
    if (trigger_func.empty() && 0==trigger_va) {
        std::cerr <<argv[0] <<": --trigger=NAME_OR_ADDR is required\n";
        return 1;
    }
    if (analysis_func.empty() && 0==analysis_va && 0==randomize) {
        std::cerr <<argv[0] <<": --analysis or --random switch is required\n";
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
    if (!trigger_func.empty() && trigger_func!="oep") {
        trigger_va = RSIM_Tools::FunctionFinder().address(project, trigger_func);
        if (0==trigger_va) {
            std::cerr <<argv[0] <<": could not find trigger function: " <<trigger_func <<"\n";
            return 1;
        }
    }
    if (!analysis_func.empty()) {
        analysis_va = RSIM_Tools::FunctionFinder().address(project, analysis_func);
        if (0==analysis_va) {
            std::cerr <<argv[0] <<": could not find analysis function: " <<analysis_func <<"\n";
            return 1;
        }
    }

    // Register the analysis callback.
    AnalysisTrigger trigger(trigger_va, analysis_va, randomize, limits, niters, verbosity);
    sim.install_callback(&trigger);

    // The rest is normal boiler plate to run the simulator, except we'll catch the Analysis to terminate the simulation early
    // if desired.
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
#ifdef PRODUCTION
    sim.activate();
#endif
    if (trigger_func=="oep")
        trigger.trigger_addr = sim.get_process()->get_ep_orig_va();
    try {
        sim.main_loop();
    } catch (AnalysisTrigger*) {
        std::cerr <<"analysis finsished; simulation terminated.\n";
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
