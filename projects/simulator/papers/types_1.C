#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "SymbolicSemantics.h"
#include "YicesSolver.h"

using namespace BinaryAnalysis::InstructionSemantics;

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

// This is a stub and is here so that if we need to add something to the values we can do so quite easily.  It also allows us
// to define conversions between ValueTypes defined for different instruction semantics name spaces.
template<size_t nBits>
class ValueType: public SymbolicSemantics::ValueType<nBits> {
public:
    ValueType(std::string comment="")
        : SymbolicSemantics::ValueType<nBits>(comment) {}
    ValueType(const SymbolicSemantics::ValueType<nBits> &other)
        : SymbolicSemantics::ValueType<nBits>(other) {}
    ValueType(uint64_t n, std::string comment="")
        : SymbolicSemantics::ValueType<nBits>(n, comment) {}
    explicit ValueType(const SymbolicSemantics::TreeNodePtr &node)
        : SymbolicSemantics::ValueType<nBits>(node) {}
    ValueType(const PartialSymbolicSemantics::ValueType<nBits> &other, std::string comment="") {
        set_expression(other.is_known() ?
                       InsnSemanticsExpr::LeafNode::create_integer(nBits, other.known_value(), comment) :
                       InsnSemanticsExpr::LeafNode::create_variable(nBits, comment));
    }
};


/******************************************************************************************************************************
 *                                              Pointer Type Analysis
 ******************************************************************************************************************************/


/** Info passed from analyzer to semantics. */
struct PtrInfo {
    enum PtrType {UNKNOWN_PTR=0x000, DATA_PTR=0x0001, CODE_PTR=0x0002}; // bits
    struct Pointer {
        Pointer(const ValueType<32> &address, unsigned type): address(address), type(type) {}
        ValueType<32> address;                  /**< Address of the pointer variable. */
        unsigned type;                          /**< Type(s) of pointer; bit vector of PtrType enum constants. */
    };
    typedef std::vector<Pointer> Pointers;
    typedef std::map<SgAsmInstruction*, unsigned/*PtrType bits*/> AddressCalc; /**< Info about where addresses are calculated. */
    int pass;                                   /**< Analysis pass number (0 or 1). */
    RSIM_Thread *thread;                        /**< Thread doing the analysis. */
    RTS_Message *m;                             /**< Message facility for analysis debugging. */
    rose_addr_t analysis_addr;                  /**< Address where analysis starts. */
    Verbosity verbosity;                        /**< How verbose to be with messages. */
    AddressCalc ptr_insns;                      /**< Instructions computing pointer addresses. */
    Pointers pointers;                          /**< List of pointers we found. */
    ExtentMap addrspc;                          /**< If not empty, analysis stays within this address space. */

    PtrInfo(RSIM_Thread *thread, RTS_Message *m, rose_addr_t analysis_addr, Verbosity verbosity, const ExtentMap &addrspc)
        : pass(0), thread(thread), m(m), analysis_addr(analysis_addr), verbosity(verbosity), addrspc(addrspc) {}

    /** Used to iteratively search through a list of Pointer objects. */
    class Comparator {
    public:
        const ValueType<32> &addr;
        SMTSolver *solver;
        Comparator(const ValueType<32> &addr, SMTSolver *solver=NULL): addr(addr), solver(solver) {}
        bool operator()(const Pointer &elmt) {
            return elmt.address.get_expression()->equal_to(addr.get_expression(), solver);
        }
    };

    /** Add another pointer to the list of pointers.  The new pointer is only added if its address is different than all otherr
     *  pointers already in the list.  The comparison is done with an SMT solver if one is given, otherwise we compare the
     *  addresses naively.  In any case, the supplied pointer type bits (@p pointer_types) are OR'd into the list whether the
     *  pointer is already in the list or was newly inserted. */
    void insert_pointer(const ValueType<32> &addr, unsigned pointer_types, SMTSolver *solver=NULL) {
        Pointers::iterator found = find_if(pointers.begin(), pointers.end(), Comparator(addr, solver));
        if (found==pointers.end()) {
            pointers.push_back(Pointer(addr, pointer_types));
        } else {
            found->type |= pointer_types;
        }
    }

    /** Returns true if the specified address is a known pointer address.  The comparison of addresses is naive unless an SMT
     *  solver is specified. */
    bool is_pointer(const ValueType<32> &addr, SMTSolver *solver) const {
        return find_if(pointers.begin(), pointers.end(), Comparator(addr, solver)) != pointers.end();
    }

    /** Output information about the pointers that were detected.  If a pointer is marked as both "code" and "data", indicate
     *  "code" in the output, otherwise indicate "data".  Display the only the value expression part of the addresses, not
     *  the addresses of the value's defining instructions.  This makes the output more suitable for automated comparison
     *  against precomputed answers, a feature used by the makefiles when testing. */
    void show_pointers(std::ostream &o) {
        if (pointers.empty()) {
            o <<"    no pointers\n";
        } else {
            for (PtrInfo::Pointers::const_iterator pi=pointers.begin(); pi!=pointers.end(); ++pi) {
                o <<"    " <<(0!=(pi->type & PtrInfo::CODE_PTR) ? "code pointer" : "data pointer");
                if (pi->address.is_known()) {
                    if (pi->address.known_value()>=analysis_stack_va &&
                        pi->address.known_value()<analysis_stack_va + 0x8000) { // arbitrary limit
                        o <<", function argument at " <<pi->address.get_expression() <<"\n";
                    } else if (pi->address.known_value()<analysis_stack_va &&
                               pi->address.known_value()>=analysis_stack_va - 0x8000) { // arbitrary limit
                        o <<", auto variable at " <<pi->address.get_expression() <<"\n";
                    } else {
                        o <<", non-stack, constant address\n";
                    }
                } else {
                    o <<", symbolic address\n";
                }
            }
        }
    }
};
    
/** Analysis state.  This class adds the ability to merge two states, which is used when two control flow paths meet. */
template<template <size_t> class T>
class PtrState: public SymbolicSemantics::State<T> {
public:
    typedef SymbolicSemantics::State<T> Super;
    typedef SymbolicSemantics::MemoryCell<T> MemoryCell;

    class MemoryCellComparator {
    public:
        T<32> address;
        SMTSolver *solver;
        MemoryCellComparator(const T<32> &address, SMTSolver *solver=NULL): address(address), solver(solver) {}
        bool operator()(const MemoryCell &elmt) {
            return elmt.address().get_expression()->equal_to(address.get_expression(), solver);
        }
    };

    /** Initialize the pointer analysis state from a pure symbolic semantics state. */
    PtrState& operator=(const SymbolicSemantics::State<T> &other) {
        *(dynamic_cast<SymbolicSemantics::State<T>*>(this)) = other;
        return *this;
    }

    /** Merge other policy into this one and return true if this one changed.  We currently merge the list of defining
     *  instructions, and this determines whether the function returns true or false.  Also, if the two value expressions are
     *  unequal, we replace the value expression with a new, unknown value.  A chance in the value expression does not affect
     *  the return value of this method, otherwise the data flow equation may not converge on a solution. */
    bool merge(const PtrState &other, SMTSolver *smt_solver) {

        // Merge registers
        size_t nchanges = this->registers.ip.add_defining_instructions(other.registers.ip.get_defining_instructions());
        for (size_t i=0; i<this->registers.n_gprs; ++i) {
            nchanges += this->registers.gpr[i].add_defining_instructions(other.registers.gpr[i].get_defining_instructions());
            if (!this->registers.gpr[i].get_expression()->equal_to(other.registers.gpr[i].get_expression(), smt_solver))
                this->registers.gpr[i].set_expression(T<32>()); // new, unknown value
        }
        for (size_t i=0; i<this->registers.n_segregs; ++i) {
            nchanges += this->registers.segreg[i].add_defining_instructions(other.registers.gpr[i].get_defining_instructions());
            if (!this->registers.segreg[i].get_expression()->equal_to(other.registers.segreg[i].get_expression(), smt_solver))
                this->registers.segreg[i].set_expression(T<16>()); // new, unknown value
        }
        for (size_t i=0; i<this->registers.n_flags; ++i) {
            nchanges += this->registers.flag[i].add_defining_instructions(other.registers.gpr[i].get_defining_instructions());
            if (!this->registers.flag[i].get_expression()->equal_to(other.registers.flag[i].get_expression(), smt_solver))
                this->registers.flag[i].set_expression(T<1>()); // new, unknown value
        }

        // Merge memory
        for (typename Super::Memory::CellList::const_iterator mi=other.memory.cell_list.begin();
             mi!=other.memory.cell_list.end();
             ++mi) {
            typename Super::Memory::CellList::iterator found = std::find_if(this->memory.cell_list.begin(),
                                                                            this->memory.cell_list.end(),
                                                                            MemoryCellComparator(mi->address(), smt_solver));
            if (found==this->memory.cell_list.end()) {
                this->memory.cell_list.push_back(*mi);
                ++nchanges;
            } else {
                /* The address */
                nchanges += found->address().add_defining_instructions(mi->address().get_defining_instructions());
                if (!found->address().get_expression()->equal_to(mi->address().get_expression(), smt_solver))
                    found->address().set_expression(T<32>()); // new, unknown value

                /* The data */
                nchanges += found->value().add_defining_instructions(mi->value().get_defining_instructions());
                if (!found->value().get_expression()->equal_to(mi->value().get_expression(), smt_solver))
                    found->value().set_expression(T<8>()); // new, unknown value
            }
        }

        return nchanges!=0;
    }
};

/** Analysis policy to detect memory address of "pointers".  This policy hooks into the memoryRead, memoryWrite, and
 *  registerWrite X86InstructionSemantics callbacks.  The goal is to detect the storage location of things like "arg1", "arg2",
 *  and "var2" in the following C code when it is compiled into a binary:
 *
 * @code
 *  int f1(bool (*arg1)(), int *arg2) {
 *      int *var2 = arg2;
 *      return arg1() ? 1 : *var2;
 *  }
 * @endcode
 *
 *  Depending on how the binary is compiled (e.g., which compiler optimizations are applied), it may or may not be possible to
 *  detect all the pointer variables.  On the other hand, the compiler may generate temporary pointers that don't exist in the
 *  source code.
 *
 *  Since binary files have no explicit type information (except perhaps in debug tables upon which we don't want to depend),
 *  we have to discover that something is a pointer by how it's used.  The property that distinguishes data pointers from
 *  non-pointers is that they're used as addresses when reading from or writing to memory.  We use two passes to find such
 *  pointers: the first pass looks at the address expressions used for memory I/O operations in order to discover which other
 *  instructions were used to define that address.  The second monitors those defining instructions to figure out whether they
 *  read from memory, and if they do, then the address being read is assumed to be the address of a pointer.
 *
 *  Code pointers (e.g., pointers to functions in C), are detected in a similar manner to data pointers.  We first look for
 *  situations where a value is written to the EIP register, obtain the list of instructions that defined that value, and then
 *  in a second pass monitor those instructions for memory reads. */
template<
    template<template<size_t> class T> class S,
    template<size_t> class T>
class PtrPolicy: public SymbolicSemantics::Policy<S, T>
{
public:
    typedef typename SymbolicSemantics::Policy<S, T> Super;
    typedef typename S<T>::Memory Memory;
    PtrInfo *info;

    // This is the set of all instructions that define addresses that are used to dereference memory through the DS register.
    // New instructions are added to this set as we discover them.
    SymbolicSemantics::InsnSet *addr_definers;

    PtrPolicy(PtrInfo *info): info(info) {}
    PtrPolicy(const SymbolicSemantics::Policy<SymbolicSemantics::State, T> &init_policy, PtrInfo *info): info(info) {
        this->cur_state  = init_policy.get_state();
    }

    /** Merge a state from another policy.  Machine states are normally represented by the policies that hold those states (we
     *  do it this way for convenience because the policy contains many of the methods that operate on the state).  This merge
     *  method simply merges the state of the @p other policy into this policy and returns true iff this policy changed as a
     *  result. */
    bool merge(const PtrPolicy &other) {
        if (info->verbosity>=VB_LOTS)
            info->m->more(" merging... ");
        bool changed = this->get_state().merge(other.get_state(), this->get_solver());
        if (info->verbosity>=VB_LOTS)
            info->m->more(changed ? "(changed)" : "(no change)");
        return changed;
    }

    /** Is this access interesting? Returns true if the access looks like a pointer dereference in which we would be
     *  interested. This is called by the first pass of analysis, which is trying to discover when pointer variable's value is
     *  used. */
    bool interesting_access(X86SegmentRegister segreg, const T<32> &addr, const T<1> &cond) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(this->cur_insn);
        if (x86_ret==insn->get_kind()) {
            // memory read for the RET instruction is not interesting.  It is only reading the return address that was pushed
            // onto the stack by the caller.  What's even worse is that the address of the return value was defined by the
            // PUSH and POP instructions.  In the second pass, since POP reads from memory, the addresses it reads from would
            // be interpreted as addresses of pointers.  We don't want that.
            return false;
        }

        return true;
    }

    /** Remember the defining instructions.  The @p defs are the set of instructions that were used to define an expression
     *  that is being dereferenced.  These instructions are added to this policy's list of instructions that define a pointer
     *  value. */
    void mark_addr_definers(const SymbolicSemantics::InsnSet &defs, PtrInfo::PtrType type) {
        for (SymbolicSemantics::InsnSet::const_iterator di=defs.begin(); di!=defs.end(); ++di) {
            if (info->verbosity>=VB_SOME)
                info->m->more(" 0x%08"PRIx64, (*di)->get_address());
            std::pair<PtrInfo::AddressCalc::iterator, bool> inserted = info->ptr_insns.insert(std::make_pair(*di, type));
            if (!inserted.second)
                inserted.first->second |= type;
        }
    }

    /** First pass processing for interesting memory dereferences.  This is called during the first pass when we encountered an
     *  interesting case of dereferencing memory, either for reading or writing.  It gets a list of instructions that defined
     *  the memory address (excluding the current instruction), and adds those instructions to the policy's list of defining
     *  instructions.
     *
     *  We exclude the current instruction from the list of defining instructions so that constant address expressions are not
     *  considered to have been obtained from a pointer's memory address.  For example, the MOV instruction listed below
     *  dereferences a constant address, 0x080c0338, to read a value from memory, and that constant's defining instruction is
     *  this very MOV instruction.  If we had entered this MOV instruction into the list of defining instructions, then the
     *  second pass of the analysis will assume that this instruction is reading a pointer's value rather than dereferencing
     *  the pointer to read the value to which it points.
     *
     * @code
     *  mov eax, DWORD PTR ds:[0x080c0338]
     * @endcode
     */
    void dereference(const T<32> &addr, PtrInfo::PtrType type, const std::string &desc) {
        SymbolicSemantics::InsnSet defs = addr.get_defining_instructions();
        defs.erase(this->cur_insn);
        if (!defs.empty()) {
            if (info->verbosity>=VB_SOME)
                info->m->more("    Address for %s depends on insn%s at", desc.c_str(), 1==defs.size()?"":"s");
            mark_addr_definers(defs, type);
            if (info->verbosity>=VB_SOME) {
                std::ostringstream ss; ss<<addr;
                info->m->more("\n        Address is %s\n", ss.str().c_str());
            }
        }
    }

    /** Callback to handle memory reads.  This augments the superclass by looking for pointer dereferences (in the first pass)
     *  and looking for reads from pointer variable addresses (in the second pass). */
    template<size_t Len>
    T<Len> readMemory(X86SegmentRegister segreg, const T<32> &addr, const T<1> &cond) {
        switch (info->pass) {
            case 0: {
                if (interesting_access(segreg, addr, cond))
                    dereference(addr, PtrInfo::DATA_PTR, "memory read");
                break;
            }
            case 1: {
                PtrInfo::AddressCalc::const_iterator i=info->ptr_insns.find(this->cur_insn);
                if (i!=info->ptr_insns.end()) {
                    if (info->verbosity>=VB_SOME) {
                        std::ostringstream ss; ss<<addr;
                        info->m->more("    Pointer address found: %s\n", ss.str().c_str());
                    }
                    unsigned pointer_type = i->second; // bit vector of PtrInfo::PtrType
                    info->insert_pointer(addr, pointer_type, this->get_solver());
                }
                break;
            }
        }

        return Super::template readMemory<Len>(segreg, addr, cond);
    }

    /** Callback to handle memory writes.  This aguments the superclass by looking for pointer dereferences (in the first
     *  pass). It is not necessary to watch for writing to pointer variable addresses in the second pass (the analysis only
     *  looks for places where pointers are used and thus only requires that a pointer variable's value has been read). */
    template<size_t Len>
    void writeMemory(X86SegmentRegister segreg, const T<32> &addr, const T<Len> &val, const T<1> &cond) {
        if (0==info->pass && interesting_access(segreg, addr, cond))
            dereference(addr, PtrInfo::DATA_PTR, "memory write");
        Super::template writeMemory<Len>(segreg, addr, val, cond);
    }

    /** Callback to handle writes to registers.  This callback augments the superclass by looking for certain kinds of writes
     *  to the EIP register that are indicative of dereferencing a code pointer. */
    template<size_t Len>
    void writeRegister(const RegisterDescriptor &reg, const T<Len> &value) {
        if (0==info->pass && !value.is_known() && reg.equal(this->findRegister("eip", 32))) {
            InsnSemanticsExpr::InternalNodePtr inode = value.get_expression()->isInternalNode();
            if (inode!=NULL && InsnSemanticsExpr::OP_ITE==inode->get_operator() &&
                inode->child(1)->is_known() && inode->child(2)->is_known()) {
                // We must have processed a branch instruction.  Both directions of the branch are concrete addresses, so there
                // is no code pointer involved here.
            } else {
                SymbolicSemantics::InsnSet defs = value.get_defining_instructions();
                if (!defs.empty()) {
                    if (info->verbosity>=VB_SOME)
                        info->m->more("    EIP write depends on insn%s at", 1==defs.size()?"":"s");
                    mark_addr_definers(defs, PtrInfo::CODE_PTR);
                    if (info->verbosity>=VB_SOME)
                        info->m->more("\n");
                }
            }
        }
        Super::template writeRegister<Len>(reg, value);
    }
};

/** Perform a pointer analysis, looking for memory addresses that contain pointers.  Return value is a PtrInfo object that
 *  contains a list of memory addresses (possibly symbolic) that we think contain pointers. */
class PtrAnalysis {
public:
    typedef PtrPolicy<PtrState, ValueType> Policy;
    typedef X86InstructionSemantics<Policy, ValueType> Semantics;
    typedef std::map<rose_addr_t, PtrPolicy<PtrState, ValueType> > StateMap;
    typedef SymbolicSemantics::Policy<SymbolicSemantics::State, ValueType> InitialPolicy;

    InitialPolicy initial_policy;

    PtrAnalysis(const InitialPolicy &p): initial_policy(p) {}

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
        Policy policy(initial_policy, &info);
        Semantics semantics(policy);
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

            // Initialize the policy for the first simulated instruction.
            {
                PtrPolicy<PtrState, ValueType> p(initial_policy, &info);
                before.insert(std::make_pair(info.analysis_addr, p));
            }

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

                if (info.verbosity>=VB_SOME) {
                    info.m->more("  pass %d: processing 0x%08"PRIx64": %s\n",
                                 info.pass, insn->get_address(), unparseInstruction(insn).c_str());
                }
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
                InsnSemanticsExpr::InternalNodePtr inode = eip_value.get_expression()->isInternalNode();
                if (eip_value.is_known()) {
                    successors.insert(eip_value.known_value());
                    // assume all CALLs return since we might not actually traverse the called function.  If we had done a full
                    // disassembly with partitioning then we could be more precise here by looking to see if the CALL was
                    // indeed a function call, and if so, whether the called function's can_return() property is set.
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
            for (PtrInfo::Pointers::const_iterator pi=info.pointers.begin(); pi!=info.pointers.end(); ++pi) {
                std::ostringstream ss;
                ss <<" "
                   <<(0!=(pi->type & PtrInfo::DATA_PTR) ? " data" : "")
                   <<(0!=(pi->type & PtrInfo::CODE_PTR) ? " code" : "")
                   <<" pointer at " <<pi->address
                   <<"\n";
                info.m->more("%s", ss.str().c_str());
            }
            if (info.pointers.empty())
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
    OracleState& operator=(const SymbolicSemantics::State<ValueType> &other) {
        *dynamic_cast<SymbolicSemantics::State<ValueType>*>(this) = other;
        return *this;
    }
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

    OraclePolicy(const SymbolicSemantics::Policy<SymbolicSemantics::State, ValueType> &init_policy, const PtrInfo &info)
        : info(info) {
        this->cur_state = init_policy.get_state();
    }

    // Provide a memory oracle.  When reading memory we could have two approaches.  A simple approach would be to first do a
    // readMemory() from the superclass and if the obtained value is symbolic, follow it with a writeMemory() to the
    // superclass.  However, this has the problem that a readMemory() is also responsible for updating the initial memory
    // state (it's done this way so we don't have to store an initial value for every possible memory address--we only store
    // the values for memory locations that are read without having first been written.)  The initial state ends up containing
    // symbolic values rather than the concrete values we would have liked.
    //
    // So we use a different approach.  Instead of using the policy's readMemory() interface, we call the
    // SymbolicSemantics::Policy::mem_read() method and supply a default value.  The drawback is that we need to compute the
    // default value for every call to mem_read() since we don't know before hand whether the default value will be needed.
    template<size_t nBits>
    ValueType<nBits>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) {
        if (info.verbosity>=VB_SOME) {
            std::ostringstream ss;
            ss <<"    reading from memory address " <<addr <<"\n";
            info.m->more("%s", ss.str().c_str());
        }
        ValueType<nBits> dflt = info.is_pointer(addr, this->get_solver()) ? random_pointer<nBits>() : random_integer<nBits>();
        return super::template mem_read(this->get_state(), addr, &dflt);
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
    typedef SymbolicSemantics::Policy<SymbolicSemantics::State, ValueType> InitialPolicy;

    InitialPolicy initial_policy;

    OracleAnalysis(const InitialPolicy &p): initial_policy(p) {}

    // Throws:
    //   Disassembler::Exception when it can't disassemble something, probably because we supplied a bugus value for
    //   the instruction pointer.
    size_t analyze(const PtrInfo &info) {
        static const char *name = "OracleAnalysis";
        if (info.verbosity>=VB_MINIMAL)
            info.m->multipart(name, "%s triggered: analysis starts at 0x%08"PRIx64"\n", name, info.analysis_addr);

        static const size_t limit = 200;        // max number of instructions to process
        size_t ninsns = 0;                      // number of instructions processed
        Policy policy(initial_policy, info);
        Semantics semantics(policy);

        try {
            // Initialize the policy for the first simulated instruction.
            policy.writeRegister(semantics.REG_EIP, policy.number<32>(info.analysis_addr));

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
            std::ostringstream ss;
            ss <<name <<": finished after processing " <<ninsns <<" instruction" <<(1==ninsns?"":"s") <<".\n"
               <<name <<": initial state was:\n" <<policy.get_orig_state()
               <<name <<": final state is:\n" <<policy.get_state();
            info.m->more("%s", ss.str().c_str());
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
#if 0
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

            // Create an initial state.  Both PtrAnalysis and OracleAnlaysis accept a SymbolicSemantics::Policy as an
            // initializer.
            typedef X86InstructionSemantics<PtrAnalysis::InitialPolicy, ValueType> InitSemantics;
            PtrAnalysis::InitialPolicy init_policy;
            init_policy.get_state().registers.gpr[x86_gpr_sp] = ValueType<32>(analysis_stack_va, "esp@init");


            // Run the PtrAnalysis and OracleAnalysis repeatedly with different starting addresses
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

                // Do one pointer analysis
                PtrInfo ptr_info(args.thread, m, analysis_addr, verbosity, addrspc);
                PtrAnalysis(init_policy).analyze(ptr_info);
                std::cout <<"Pointers discovered by PtrAnalysis:\n";
                ptr_info.show_pointers(std::cout);

                // Do a bunch of memory oracle analyses
                for (size_t iter=0; iter<niters; ++iter) {

                    // Initialize some registers
                    SymbolicSemantics::State<ValueType> &state = init_policy.get_state();
                    for (size_t i=0; i<state.registers.n_gprs; ++i) {
                        if (i!=x86_gpr_sp && i!=x86_gpr_bp)
                            state.registers.gpr[i] = ValueType<32>((uint64_t)rand() & 0xffffffffull);
                    }
                    for (size_t i=0; i<state.registers.n_segregs; ++i)
                        state.registers.segreg[i] = args.thread->policy.get_concrete_state().registers.segreg[i];
                    for (size_t i=0; i<state.registers.n_flags; ++i)
                        state.registers.flag[i] = ValueType<1>((uint64_t)rand() & 1ull);

                    m->mesg("%s: MemoryOracle at virtual address 0x%08"PRIx64" (%zu of %zu), iteration %zu",
                            name, analysis_addr, n+1, std::max(randomize, (size_t)1), iter);
                    size_t ninsns = OracleAnalysis(init_policy).analyze(ptr_info);
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
