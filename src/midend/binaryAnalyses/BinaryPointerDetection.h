#ifndef ROSE_BinaryPointerDetection_H
#define ROSE_BinaryPointerDetection_H

#include "SymbolicSemantics.h"
#include "WorkList.h"
#include "YicesSolver.h"

// documented elsewhere
namespace BinaryAnalysis {

/** Binary pointer analyses. Various analyses for pointer variables in binary specimens. */
namespace PointerAnalysis {

/** Type of pointer. These bits represent various knowledge about pointer variables. */
enum PointerType {UNKNOWN_PTR=0x000,            /**< Pointer variable is of unknown type. */
                  DATA_PTR=0x0001,              /**< Pointer variable points to data. */
                  CODE_PTR=0x0002               /**< Pointer variable points to code. */
};


// Machine state used internally by PointerDetection analysis. This class is not for public consumption.  We would make this a
// private member of the PointerDetection class, but we need to have the correct template signature in order to use it with the
// existing instruction semantics framework.
template <
    template <size_t> class ValueType
    >
class PointerDetectionState: public BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State<ValueType> {
public:
    typedef BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State<ValueType> Super;
    typedef BinaryAnalysis::InstructionSemantics::SymbolicSemantics::MemoryCell<ValueType> MemoryCell;

    PointerDetectionState() {}

    class MemoryCellComparator {
    public:
        ValueType<32> address;
        SMTSolver *solver;
        MemoryCellComparator(const ValueType<32> &address, SMTSolver *solver=NULL): address(address), solver(solver) {}
        bool operator()(const MemoryCell &elmt) {
            return elmt.address().get_expression()->equal_to(address.get_expression(), solver);
        }
    };

    // Initialize the pointer analysis state from a pure symbolic semantics state.
    PointerDetectionState& operator=(const Super &other) {
        *(dynamic_cast<Super*>(this)) = other;
        return *this;
    }
    
    // Merge other policy into this one and return true if this one changed.  We currently merge the list of defining
    // instructions, and this determines whether the function returns true or false.  Also, if the two value expressions are
    // unequal, we replace the value expression with a new, unknown value.  A change in the value expression does not affect
    // the return value of this method, otherwise the data flow equation may not converge on a solution.
    bool merge(const PointerDetectionState &other, SMTSolver *smt_solver) {

        // Merge registers
        size_t nchanges = this->registers.ip.add_defining_instructions(other.registers.ip.get_defining_instructions());
        for (size_t i=0; i<this->registers.n_gprs; ++i) {
            nchanges += this->registers.gpr[i].add_defining_instructions(other.registers.gpr[i].get_defining_instructions());
            nchanges += merge_value(this->registers.gpr[i], other.registers.gpr[i], smt_solver);
        }
        for (size_t i=0; i<this->registers.n_segregs; ++i) {
            nchanges += this->registers.segreg[i].add_defining_instructions(other.registers.gpr[i].get_defining_instructions());
            nchanges += merge_value(this->registers.segreg[i], other.registers.segreg[i], smt_solver);
        }
        for (size_t i=0; i<this->registers.n_flags; ++i) {
            nchanges += this->registers.flag[i].add_defining_instructions(other.registers.gpr[i].get_defining_instructions());
            nchanges += merge_value(this->registers.flag[i], other.registers.flag[i], smt_solver);
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
                ValueType<32> addr = found->address();
                nchanges += addr.add_defining_instructions(mi->address().get_defining_instructions());
                nchanges += merge_value(addr, mi->address(), smt_solver);
                found->address(addr);

                /* The data */
                ValueType<8> val = found->value();
                nchanges += val.add_defining_instructions(mi->value().get_defining_instructions());
                nchanges += merge_value(val, mi->value(), smt_solver);
                found->value(val);
            }
        }

        return nchanges!=0;
    }

    // Merge two symbolic values, storing the result in the first argument.  To simplify things, if the arguments have
    // different values then the merge result is a new, unbound variable (unless it was already a variable, in which case no
    // change is made).  Returns 1 if the merge operation changes the first argument, zero if no change is made.
    template<size_t nBits>
    size_t merge_value(ValueType<nBits> &inout, const ValueType<nBits> &other, SMTSolver *smt_solver) {
        if (inout.get_expression()->equal_to(other.get_expression(), smt_solver))
            return 0;
        InsnSemanticsExpr::LeafNodePtr inout_leaf = inout.get_expression()->isLeafNode();
        if (inout_leaf && inout_leaf->is_variable())
            return 0;
        inout.set_expression(ValueType<nBits>()); // set expression without affecting defining instructions
        return 1;
    }
};

/** Pointer detection analysis.  This analysis attempts to discover which memory addresses store pointer variables and whether
 *  those pointer variables point to code or data.  The goal is to detect the storage location of things like "arg1", "arg2",
 *  and "var2" in the following C code after it is compiled into a binary:
 *
 *  @code
 *   int f1(bool (*arg1)(), int *arg2) {
 *       int *var2 = arg2;
 *       return arg1() ? 1 : *var2;
 *  }
 *  @endcode
 *
 *  Depending on how the binary is compiled (e.g., which compiler optimizations where applied), it may or may not be possible
 *  to detect all the pointer variables.  On the other hand, the compiler may generate temporary pointers that don't exist in
 *  the source code. Since binary files have no explicit type information (except perhaps in debug tables upon which we don't
 *  want to depend), we have to discover that something is a pointer by how it's used.  The property that distinguishes data
 *  pointers from non-pointers is that they're used as addresses when reading from or writing to memory.  We use two passes to
 *  find such pointers: the first pass looks at the address expressions used for memory I/O operations in order to discover
 *  which other instructions were used to define that address.  The second monitors those defining instructions to figure out
 *  whether they read from memory, and if they do, then the address being read is assumed to be the address of a pointer.
 *  Code pointers (e.g., pointers to functions in C), are detected in a similar manner to data pointers: we first look for
 *  situations where a value is written to the EIP register, obtain the list of instructions that defined that value, and then
 *  in a second pass monitor those instructions for memory reads.
 *
 *  This analysis is used by instantiating an analyzer and giving it an InstructionProvidor object that has a get_instruction()
 *  method that takes a single rose_addr_t virtual address and returns  the instruction at that address.  The analysis is run
 *  by calling its analyze() method, after which results can be queried.
 *
 *  The pointer detection normally uses the BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType, but the user
 *  can specify any other compatible value type as a second template argument.
 *
 *  @section Ex1 Example
 *
 *  @code
 *  #include "BinaryPointerDetection.h"
 *  #include "YicesSolver.h"
 *
 *  // RSIM_Process can be any class that provides a get_instruction() method. In this case
 *  // it is a class that comes from the binary simulator.   The thread->get_process() expression
 *  // returns an instance of the RSIM_Process class. 
 *  typedef BinaryAnalysis::PointerAnalysis::PointerDetection<RSIM_Process> PointerDetector;
 *  SMTSolver *smt_solver =  YicesSolver::available_linkage() ? new YicesSolver : NULL;
 *  PointerDetector analyzer(thread->get_process(), smt_solver);
 *
 *  // Analyze a particular binary function.  We could also analyze any snippet of binary code
 *  // by providing a starting address and address RangeMap.
 *  SgAsmFunction *function = ...;
 *  analyzer.analyze(function);
 *
 *  // Obtain the results. In this case, we ask for the list of all pointer variables that were
 *  // detected and then print the symbolic address of each pointer variable.
 *  const PointerDetector::Pointers &pointers = analyzer.get_pointers();
 *  BOOST_FOREACH (const PointerDetector::Pointer &ptr, pointers) {
 *      std::cerr <<ptr.address <<std::endl;
 *  }
 *  @endcode
 */
template <
    class InstructionProvidor,
    template <size_t> class ValueType = BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType
    >
class PointerDetection {
public:

    /** Description for a single pointer variable.  This struct contains the address of the pointer variable and a bit mask
     *  indicating whether the pointer points to code and/or data. */
    struct Pointer {
        Pointer(const ValueType<32> &address, unsigned type): address(address), type(type) {}
        ValueType<32> address;                  /**< Address of the pointer variable. */
        unsigned type;                          /**< Type(s) of pointer; bit vector of PointerType enum constants. */
    };

    /** A vector of pointers.  This is the return type for the get_pointers() method. */
    typedef std::vector<Pointer> Pointers;

protected:
    typedef std::map<SgAsmInstruction*, unsigned/*nbits*/> AddressCalc; // Info about where addresses are calculated.

protected:
    // Pointer detection info used internally.
    struct PointerDetectionInfo {
        size_t pass;                            // Analysis pass number.
        AddressCalc ptr_insns;                  // Instructions computing pointer addresses.
        Pointers pointers;                      // List of pointers we found.

        PointerDetectionInfo(): pass(0) {}

        // Used to iteratively search through a list of Pointer objects.
        class Comparator {
        public:
            const ValueType<32> &addr;
            SMTSolver *solver;
            Comparator(const ValueType<32> &addr, SMTSolver *solver=NULL): addr(addr), solver(solver) {}
            bool operator()(const Pointer &elmt) {
                return elmt.address.get_expression()->equal_to(addr.get_expression(), solver);
            }
        };

        // Add another pointer to the list of pointers.  The new pointer is only added if its address is different than all
        // other pointers already in the list.  The comparison is done with an SMT solver if one is given, otherwise we compare
        // the addresses naively.  In any case, the supplied pointer type bits (@p pointer_types) are OR'd into the list
        // whether the pointer is already in the list or was newly inserted. */
        void insert_pointer(const ValueType<32> &addr, unsigned pointer_types, SMTSolver *solver=NULL) {
            typename Pointers::iterator found = find_if(pointers.begin(), pointers.end(), Comparator(addr, solver));
            if (found==pointers.end()) {
                pointers.push_back(Pointer(addr, pointer_types));
            } else {
                found->type |= pointer_types;
            }
        }

        // Returns true if the specified address is a known pointer address.  The comparison of addresses is naive unless an
        // SMT solver is specified.
        bool is_pointer(const ValueType<32> &addr, SMTSolver *solver) const {
            return find_if(pointers.begin(), pointers.end(), Comparator(addr, solver)) != pointers.end();
        }
    };

protected:
    // Parent class of our own instruction semantics policy
    typedef BinaryAnalysis::InstructionSemantics::SymbolicSemantics::Policy<PointerDetectionState, ValueType> SuperPolicy;

    // Instruction semantics policy used internally by PointerDetection analysis. */
    class PointerDetectionPolicy: public SuperPolicy {
    public:
        typedef typename PointerDetectionState<ValueType>::Memory Memory;
        PointerDetectionInfo *info;
        FILE *debug;

        // This is the set of all instructions that define addresses that are used to dereference memory through the DS register.
        // New instructions are added to this set as we discover them.
        BinaryAnalysis::InstructionSemantics::SymbolicSemantics::InsnSet *addr_definers;

        explicit PointerDetectionPolicy(PointerDetectionInfo *info, SMTSolver *solver=NULL, FILE *debug=NULL)
            : info(info), debug(debug) {
            assert(info);
            this->set_solver(solver);
        }

        // Merge a state from another policy.  Machine states are normally represented by the policies that hold those states
        // (we do it this way for convenience because the policy contains many of the methods that operate on the state).  This
        // merge method simply merges the state of the @p other policy into this policy and returns true iff this policy
        // changed as a result.
        bool merge(const PointerDetectionPolicy &other) {
            bool changed = this->get_state().merge(other.get_state(), this->get_solver());
            return changed;
        }

        // Is this access interesting? Returns true if the access looks like a pointer dereference in which we would be
        // interested. This is called by the first pass of analysis, which is trying to discover when pointer variable's value
        // is used.
        bool interesting_access(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) {
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

        // Remember the defining instructions.  The @p defs are the set of instructions that were used to define an expression
        // that is being dereferenced.  These instructions are added to this policy's list of instructions that define a
        // pointer value.
        void mark_addr_definers(const BinaryAnalysis::InstructionSemantics::SymbolicSemantics::InsnSet &defs, PointerType type) {
            for (BinaryAnalysis::InstructionSemantics::SymbolicSemantics::InsnSet::const_iterator di=defs.begin();
                 di!=defs.end(); ++di) {
                if (debug)
                    fprintf(debug, " 0x%08"PRIx64, (*di)->get_address());
                std::pair<AddressCalc::iterator, bool> inserted = info->ptr_insns.insert(std::make_pair(*di, type));
                if (!inserted.second)
                    inserted.first->second |= type;
            }
            if (debug)
                fprintf(debug, "\n");
        }

        // First pass processing for interesting memory dereferences.  This is called during the first pass when we encountered
        // an interesting case of dereferencing memory, either for reading or writing.  It gets a list of instructions that
        // defined the memory address (excluding the current instruction), and adds those instructions to the policy's list of
        // defining instructions.
        //
        // We exclude the current instruction from the list of defining instructions so that constant address expressions are
        // not considered to have been obtained from a pointer's memory address.  For example, the MOV instruction listed below
        // dereferences a constant address, 0x080c0338, to read a value from memory, and that constant's defining instruction
        // is this very MOV instruction.  If we had entered this MOV instruction into the list of defining instructions, then
        // the second pass of the analysis will assume that this instruction is reading a pointer's value rather than
        // dereferencing the pointer to read the value to which it points.
        //
        //   mov eax, DWORD PTR ds:[0x080c0338]
        void dereference(const ValueType<32> &addr, PointerType type, const std::string &desc) {
            BinaryAnalysis::InstructionSemantics::SymbolicSemantics::InsnSet defs = addr.get_defining_instructions();
            defs.erase(this->cur_insn); // see note above
            if (!defs.empty()) {
                if (debug)
                    fprintf(debug, "PointerDetection:   address for %s depends on insn%s at",
                            desc.c_str(), 1==defs.size()?"":"s");
                mark_addr_definers(defs, type);
                if (debug) {
                    std::ostringstream ss; ss <<addr;
                    fprintf(debug, "PointerDetection:     address is %s\n", ss.str().c_str());
                }
            }
        }

        // Callback to handle memory reads.  This augments the superclass by looking for pointer dereferences (in the first
        // pass) and looking for reads from pointer variable addresses (in the second pass).
        template<size_t Len>
        ValueType<Len> readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) {
            switch (info->pass) {
                case 0: {
                    if (interesting_access(segreg, addr, cond))
                        dereference(addr, DATA_PTR, "memory read");
                    break;
                }
                case 1: {
                    AddressCalc::const_iterator i=info->ptr_insns.find(this->cur_insn);
                    if (i!=info->ptr_insns.end()) {
                        if (debug) {
                            std::ostringstream ss; ss <<addr;
                            fprintf(debug, "PointerDetection:   pointer address found: %s\n", ss.str().c_str());
                        }
                        unsigned pointer_type = i->second; // bit vector of PointerType
                        info->insert_pointer(addr, pointer_type, this->get_solver());
                    }
                    break;
                }
            }

            return SuperPolicy::template readMemory<Len>(segreg, addr, cond);
        }

        // Callback to handle memory writes.  This aguments the superclass by looking for pointer dereferences (in the first
        // pass). It is not necessary to watch for writing to pointer variable addresses in the second pass (the analysis only
        // looks for places where pointers are used and thus only requires that a pointer variable's value has been read).
        template<size_t Len>
        void writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &val,
                         const ValueType<1> &cond) {
            if (0==info->pass && interesting_access(segreg, addr, cond))
                dereference(addr, DATA_PTR, "memory write");
            SuperPolicy::template writeMemory<Len>(segreg, addr, val, cond);
        }

        // Callback to handle writes to registers.  This callback augments the superclass by looking for certain kinds of
        // writes to the EIP register that are indicative of dereferencing a code pointer.
        template<size_t Len>
        void writeRegister(const RegisterDescriptor &reg, const ValueType<Len> &value) {
            if (0==info->pass && !value.is_known() && reg.equal(this->findRegister("eip", 32))) {
                InsnSemanticsExpr::InternalNodePtr inode = value.get_expression()->isInternalNode();
                if (inode!=NULL && InsnSemanticsExpr::OP_ITE==inode->get_operator() &&
                    inode->child(1)->is_known() && inode->child(2)->is_known()) {
                    // We must have processed a branch instruction.  Both directions of the branch are concrete addresses, so
                    // there is no code pointer involved here.
                } else {
                    BinaryAnalysis::InstructionSemantics::SymbolicSemantics::InsnSet defs = value.get_defining_instructions();
                    if (!defs.empty()) {
                        if (debug)
                            fprintf(debug, "PointerDetection:   EIP write depends on insn%s at", 1==defs.size()?"":"s");
                        mark_addr_definers(defs, CODE_PTR);
                    }
                }
            }
            SuperPolicy::template writeRegister<Len>(reg, value);
        }
    };

protected:
    InstructionProvidor *instruction_providor;
    PointerDetectionInfo info;
    size_t max_processed;
    SMTSolver *solver;
    FILE *debug;
    PointerDetectionState<ValueType> start_state;

public:
    /** Binary instruction semantics used by this analysis. */
    typedef BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<PointerDetectionPolicy, ValueType> Semantics;

    /** Machine state used by this analysis. */
    typedef std::map<rose_addr_t, PointerDetectionPolicy> StateMap;

    /** Instantiates a new analysis with the specified instruction providor.  The instruction providor type is from the
     *  PointerDetection class template and must provide a get_instruction() method that takes a single rose_addr_t virtual
     *  address and returns the instruction at that address. It may return the null pointer if an instruction cannot be
     *  obtained at the specified address.
     *
     *  The SMT @p solver argument is optional and defaults to the null pointer.  If an SMT solver is provided then it will be
     *  used to solve address aliasing equations, otherwise a naive comparison is used.  The naive comparison can often give
     *  good results and is much faster. */
    explicit PointerDetection(InstructionProvidor *ip, SMTSolver *solver=NULL)
        : instruction_providor(ip), max_processed(256), solver(solver), debug(NULL) {}

    /** Accessor for debugging.  Set a non-null file pointer to get debugging information emitted to that file; clear the file
     *  descriptor to turn off debugging.
     * @{ */
    void set_debug(FILE *f) { debug = f; }
    FILE *get_debug() const { return debug; }
    /** @} */

    /** Access the initial state. This is the state of the machine prior to the start node for each analysis() call.
     * @{ */
    PointerDetectionState<ValueType>& initial_state() { return start_state; }
    const PointerDetectionState<ValueType>& initial_state() const { return start_state; }
    /** @} */

    /** Returns the array of all discovered pointer variables.  This method returns the results from this analyzer's most
     * recent analyze() call. */
    const Pointers& get_pointers() const { return info.pointers; }

    /** Queries whether an address can be a pointer variable.  Returns true if the address can be a pointer variable, by
     *  scanning the list returned by get_pointers() and asking the SMT solver (if available, naive comparison otherwise)
     *  whether the specified address can be equal to any known pointer. */
    bool is_pointer(const ValueType<32> &va) const {
        bool retval = false;
        SMTSolver *solver = YicesSolver::available_linkage() ? new YicesSolver : NULL;
        for (typename Pointers::const_iterator pi=info.pointers.begin(); pi!=info.pointers.end() && !retval; ++pi)
            retval = va.get_expression()->equal_to(pi->address.get_expression(), solver);
        delete solver;
        return retval;
    }

    /** Analyze pointers in a single function.  The analysis will not traverse into other functions called by this function. */ 
    void analyze(SgAsmFunction *func) {
        ExtentMap func_extent;
        func->get_extent(&func_extent);
        analyze(func->get_entry_va(), func_extent);
    }

    /** Analyze a code snippet to find pointer variables.  The analysis begins at the @p start_va virtual address but does not
     * follow control flow outside @p addrspc.  If @p addrspc is empty then no control flow pruning is performed. */ 
    void analyze(rose_addr_t start_va, ExtentMap &addrspc) {
        if (debug) {
            fprintf(debug, "BinaryAnalysis::PointerAnalysis::PointerDetection (a.k.a., PointerDetection): starting\n");
            fprintf(debug, "PointerDetection: starting at 0x%08"PRIx64"\n", start_va);
            if (!addrspc.empty()) {
                std::ostringstream ss; ss <<addrspc;
                fprintf(debug, "PointerDetection: restricted to these addresses: %s\n", ss.str().c_str());
            }
        }
        
        // Create the policy that holds the analysis state which is modified by each instruction.  Then plug the policy
        // into the X86InstructionSemantics to which we'll feed each instruction.  Each time we process an instruction
        // we'll load this this policy object with the instruction's initial state, process the instruction, and then obtain
        // the instruction's final state from the same policy object.  (The X86InstructionSemantics keeps a modifiable
        // reference to this policy.)
        PointerDetectionPolicy policy(&info, solver, debug);
        Semantics semantics(policy);
        const RegisterDescriptor *EIP = policy.get_register_dictionary()->lookup("eip");

        // Make two passes.  During the first pass we discover what addresses are used to dereference memory and what
        // instructions define those addresses, marking those instructions in the process.  During the second pass, if a
        // marked instruction reads from memory we remember that the memory location holds a pointer.  The policies all
        // point to "info" and update it appropriately.
        for (info.pass=0; info.pass<2; ++info.pass) {
            if (debug) {
                fprintf(debug, "PointerDetection: =============================== starting pass %zu "
                        "================================\n", info.pass);
            }
            StateMap before, after;
            policy.get_state() = start_state;
            before.insert(std::make_pair(start_va, policy));
            size_t nprocessed = 0;
            WorkList<rose_addr_t> worklist;
            worklist.push(start_va);
            while (!worklist.empty() && nprocessed++ < max_processed) {
                // Get the next instruction, but skip it if its (partially) outside our address limits
                if (debug)
                    fprintf(debug, "PointerDetection: %s\n", std::string(80, '-').c_str());
                rose_addr_t va = worklist.pop();
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(instruction_providor->get_instruction(va));
                if (!insn || (!addrspc.empty() && !addrspc.contains(Extent(insn->get_address(), insn->get_size())))) {
                    if (debug)
                        fprintf(debug, "PointerDetection: processing pass %zu 0x%08"PRIx64": %s\n", info.pass, va,
                                (insn?"instruction excluded by ExtentMap":"no x86 instruction here"));
                    continue;
                }
                if (debug)
                    fprintf(debug, "PointerDetection: processing pass %zu 0x%08"PRIx64": %s\n",
                            info.pass, va, unparseInstruction(insn).c_str());

                // Process the instruction
                typename StateMap::iterator bi = before.find(va);
                policy = bi==before.end() ? PointerDetectionPolicy(&info, solver, debug) : bi->second;
                policy.writeRegister(*EIP, policy.template number<32>(insn->get_address()));
                semantics.processInstruction(insn);
                if (debug) {
                    std::ostringstream ss; ss <<policy;
                    fprintf(debug, "PointerDetection: resulting state:\n%s", 
                            StringUtility::prefixLines(ss.str(), "PointerDetection:   ").c_str());
                }

                // Save the state after the instruction
                std::pair<typename StateMap::iterator, bool> insert_result = after.insert(std::make_pair(va, policy));
                if (!insert_result.second)
                    insert_result.first->second = policy;

                // Find control flow successors
                std::set<rose_addr_t> successors;
                ValueType<32> eip_value = policy.template readRegister<32>(semantics.REG_EIP);
                InsnSemanticsExpr::InternalNodePtr inode = eip_value.get_expression()->isInternalNode();
                if (eip_value.is_known()) {
                    successors.insert(eip_value.known_value());
                    // assume all CALLs return since we might not actually traverse the called function.  If we had done a full
                    // disassembly with partitioning then we could be more preceise here by looking to see if the CALL was
                    // indeed a function call, and if so, whether the called function's can_return() property is set.
                    if (insn->get_kind()==x86_call)
                        successors.insert(insn->get_address() + insn->get_size());
                } else if (NULL!=inode && InsnSemanticsExpr::OP_ITE==inode->get_operator() &&
                           inode->child(1)->is_known() && inode->child(2)->is_known()) {
                    successors.insert(inode->child(1)->get_value()); // the if-true case
                    successors.insert(inode->child(2)->get_value()); // the if-false case
                } else {
                    // we don't know the successors, so skip them (FIXME: This could be improved [Robb Matzke 2013-01-07])
                }

                // Merge output state of this instruction into input states of successors
                if (debug)
                    fprintf(debug, "PointerDetection:   merging result state into successor input states...\n");
                for (std::set<rose_addr_t>::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
                    std::pair<typename StateMap::iterator, bool> insert_result = before.insert(std::make_pair(*si, policy));
                    bool changed = insert_result.second || insert_result.first->second.merge(policy);
                    bool pushed = changed && worklist.push(*si);

                    if (debug) {
                        fprintf(debug, "PointerDetection:     successor = 0x%08"PRIx64"\n", *si);
                        if (insert_result.second) {
                            fprintf(debug, "PointerDetection:       successor input state didn't exist; setting it up\n");
                        } else if (!changed) {
                            fprintf(debug, "PointerDetection:       no change to successor input state\n");
                        } else if (!pushed) {
                            fprintf(debug, "PointerDetection:       successor state changed, but already on work list\n");
                        } else {
                            fprintf(debug, "PointerDetection:       successor changed and added to work list\n");
                        }
                    }
                }
            }
        }
    }
};

} // namespace
} // namespace

#endif
