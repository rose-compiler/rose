#ifndef Rose_SemanticState_H
#define Rose_SemanticState_H

#include "Registers.h"

// Documented elsewhere
namespace BinaryAnalysis {

    /** Binary instruction semantics.
     *
     *  Entities in this namespace deal with the semantics of machine instructions, and with the process of "executing" a
     *  machine instruction in a particular semantic domain.  Instruction "execution" is a very broad term and can refer to
     *  execution in the tranditional sense where each instruction modifies the machine state (registers and memory) in a
     *  particular domain (concrete, interval, sign, symbolic, user-defined).  But it can also refer to any kind of analysis
     *  that depends on semantics of individual machine instructions (def-use, taint, etc).  It can even refer to the
     *  transformation of machine instructions in ROSE internal representation to some other representation (e.g., ROSE RISC or
     *  LLVM) where the other representation is built by "executing" the instruction.
     *
     * @section IS1 Components of instruction semantics
     *
     *  ROSE's binary semantics framework has two major components: the dispatchers and the semantic domains. The
     *  instruction dispatcher "executes" a machine instruction by translating it into a sequence of RISC-like operations, and
     *  the semantics domain defines what the RISC-operators do (e.g., change a concrete machine state, produce an output
     *  listing of RISC operations, build an LLVM representation).
     *
     *  The @em dispatcher is a class template and ROSE defines one per machine architecture. In this respect, the dispatcher
     *  is akin to the microcontroller for a CISC architecture, such as the x86 microcontroller within an x86 CPU. The
     *  dispatcher implicitly defines the interface for the RISC operations, which are documented in the
     *  BinaryAnalysis::InstructionSemantics::NullSemantics::Policy class. Users can subclass the dispatcher if they need to
     *  override how a machine instruction is translated into RISC operations. (Note: ROSE might move away from the huge
     *  "switch" statement implementation to a table-driven approach, in which case subclassing of the entire dispatcher will
     *  likely be replaced by either modifying the dispatch table or by subclassing individual machine instruction classes.)
     *
     *  The <em>semantic domain</em> is a loose term that refers to at least three parts taken as a whole: a value type, a
     *  machine state type, and a semantic policy.  Semantic domains have names like "concrete domain", "interval domain",
     *  "sign domain", "symbolic domain", etc.  The term is used loosely since one could have different implementations of,
     *  say, a "concrete domain" by instantiating slightly different versions of the dispatcher class template. For instance,
     *  one concrete domain might use the PartialSymbolicSemantics classes in a concrete way, while another might use custom
     *  classes tuned for higher performance.  ROSE defines a set of semantic domains--each defined by grouping its three
     *  components (value type, machine state type, and semantic policy) into a single name space or class.
     *
     *  The <em>value type</em> of a semantic domain is a type (usually a class) that holds values for that domain.  For
     *  instance, a concrete domain's value type would likely hold bit vectors of varying sizes.  Instantiations of the value
     *  type are used for register contents, memory contents, memory addresses, and temporary values that exist during
     *  execution of a machine instruction. Value types are templates whose single parameter is the value width in bits.  For
     *  instance, an x86 architecture needs values that are 1, 5, 8, 16, 32, and 64 bits wide (the 1-bit values are for
     *  Booleans in the EFLAGS register; the five-bit values are shift counts on a 32-bit architecutre; the 64-bit values are
     *  needed for integer multiply on a 32-bit architecture; this list is likely not exhaustive).
     *
     *  As instructions execute they use inputs and generate outputs, which are read from and written to a <em>machine
     *  state</em>. The machine state consists of registers and memory, each of which holds a value which is instantiated from
     *  the domain's value type.  Furthermore, memory addresses are also described by instances of the domain's value type
     *  (although internally, they can use a different type as long as a translation is provided to and from that type).  The
     *  names and inter-relationships of the architectures registers are contained in a RegisterDictionary while the state
     *  itself contains the values stored in those registers.  The organization of registers and memory within the state is
     *  defined by the state.  The state can choose to either provide an API to access the registers and memory, or it can be
     *  closely integrated with the semantic policy described next...
     *
     *  The <em>semantic policy</em> class provides the implementations for the RISC operators and holds the machine's state
     *  upon which those operators operate.  The policy class is provided as an argument to the dispatcher class template in
     *  order to instantiate a dispatcher with the specified RISC semantics.  It is possible (and normal) to define multiple
     *  versions of a dispatcher each with a different semantic policy and each tuned for a specific kind of analysis.
     *
     *  @section IS2 Nomenclature choice
     *
     *  The dispatcher class templates currently have names X86InstructionSemantics.  We may change "Semantics" to "Dispatcher"
     *  since "semantics" is already present in the fully qualified name, and since "dispatcher" is a nounified verb that
     *  follows our naming convention yet hints at the action it intends to implement.
     *
     *  The term "policy" will likely remain as is.  The central idiom in policy-based design is a class template (called the
     *  "host class"; i.e., our dispatcher classes), taking several type parameters as input, which are instantiated with types
     *  selected by the user (called "policy classes"), each implementing a particular implicit interface (called a "policy"),
     *  and encapsulating some orthogonal (or mostly orthogonal) aspect of the behavior of the instantiated class. By supplying
     *  a host class combined with a set of different, canned implementations for each policy, a library or module can support
     *  an exponential number of different behavior combinations, resolved at compile time, and selected by mixing and matching
     *  the different supplied policy classes in the instantiation of the host class template. Additionally, by writing a
     *  custom implementation of a given policy, a policy-based library can be used in situations requiring behaviors
     *  unforeseen by the library implementor. Unlike mixins, policies often contain state variables (machine states in our
     *  case) and nested types (the value type in our case); unlike callbacks, policies typically contain several related
     *  functions (the RISC operators in our case).
     *
     *  @section IS3 Specialization
     *
     *  The instruction semantics architecture is designed to allow users to specialize nearly every part of it.  Most of the
     *  components are class templates whose arguments are types that provide mostly orthogonal parts of the templatized
     *  class.  ROSE defines triplets (value type, state type, semantic policy) that are designed to work together to implement
     *  a particular semantic domain, but users are free to replace or subclass any of those components to build customized
     *  semantic domains.  For example, the x86 simulator (in "projects/simulator") subclasses the PartialSymbolicSemantics
     *  state in order to use memory mapped via ROSE's MemoryMap class, and its policy in order to handle system calls (among
     *  other things).
     *
     *  @section IS4 Future work
     *
     *  <em>Table-driven dispatch.</em> The current dispatchers are implemented with a huge switch statement selecting for each
     *  possible machine instruction.  This design doesn't lend itself well to users being able to augment/override individual
     *  instructions, users adding new instructions, enabling large groups of instructions individually (e.g., SSE) to handle
     *  variations in machine architecture since each modification requires a subclass.  The plan is to replace the large
     *  "switch" statement with a dispatch table and replace each "case" with a functor specific to that machine instruction.
     *  Users will be able to modify individual table entries, modify related groups of table entries, subclass functors for
     *  individual instructions, or define new functors.  The functors will likely be class templates that take arguments
     *  similar to the dispatcher's class template.
     *
     *  <em>Floating-point instructions.</em> Floating point registers are defined in the various RegisterDictionary objects
     *  but none of the semantic states actually define space for them, and we haven't defined any floating-point RISC
     *  operations for policies to implement.  As for existing machine instructions, the dispatchers will translate machine
     *  floating point instructions to RISC operations, and the specifics of those operations will be defined by the various
     *  semantic policies.  For instance, the policy for a concrete semantic domain might use the host machine's native IEEE
     *  floating point to emulate the target machine's floating-point operations.
     *
     *  @section IS5 Example
     *
     *  See actual source code for examples since this interface is an active area of ROSE development (Dec-2012). In order to
     *  use one of ROSE's predefined semantic domains you'll likely need to define some types and variables, something along
     *  these lines:
     *
     *  @code
     *   using namespace BinaryAnalysis::InstructionSemantics;
     *   typedef SymbolicSemantics::Policy<> Policy;
     *   typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Dispatcher;
     *   Policy policy;
     *   Dispatcher dispatcher(policy);
     *  @endcode
     *
     *  In order to analyze a sequence of instructions, one calls the dispatcher's processInstruction() method one instruction
     *  at a time.  The dispatcher breaks the instruction down into a sequence of RISC-like operations and invokes those
     *  operations in the policy.  The policy's operations produce domain-specific result values and/or update the state
     *  (registers, memory, etc) associated with the policy.  Each policy provides methods by which the user can inspect and/or
     *  modify the state.  In fact, in order to follow flow-of-control from one instruction to another, it is customary to read
     *  the x86 EIP (instruction pointer register) value to get the address for the next instruction fetch.
     *
     *  One can find actual uses of instruction semantics in ROSE by searching for X86InstructionSemantics.  Also, the
     *  simulator project (in projects/simulator) has many examples how to use instruction semantics--in fact, the simulator
     *  defines its own concrete domain by subclassing PartialSymbolicSemantics in order to execute specimen programs.
     */
    namespace InstructionSemantics {

        /** Base classes for instruction semantics.  Basically, anything that is common to two or more instruction semantic
         *  domains will be factored out and placed in this name space. */
        namespace BaseSemantics {
            

            /** Indicates no print helper.  This can be used as the PrintHelper argument for most print methods related to
             *  instruction semantics. */
            class SEMANTIC_NO_PRINT_HELPER {};

            /******************************************************************************************************************
             *                                  MemoryCell
             ******************************************************************************************************************/

            /** Represents one location in memory.
             *
             *  Each memory cell has an address, data, and size in bytes.  It also maintains two boolean values to track
             *  whether a cell has been clobbered by a possibly-aliasing write, and how the cell was initialized.
             *
             *  When a memory state is created, every memory location will be given a unique value. However, it's not practicle
             *  to store a value for every possible memory address, yet we want the following example to work correctly:
             *
             * @code
             *  1: mov eax, ds:[edx]    // first read returns V1
             *  2: mov eax, ds:[edx]    // subsequent reads from same address also return V1
             *  3: mov ds:[ecx], eax    // write to unknown address clobbers all memory
             *  4: mov eax, ds:[edx]    // read from same address as above returns V2
             *  5: mov eax, ds:[edx]    // subsequent reads from same address also return V2
             * @endcode
             *
             *  Furthermore, in some semantic policies the read from ds:[edx] at #1 above, retroactively stores V1 in an
             *  original memory state. That way if it needs to do additional analyses starting from the same initial state it
             *  will be available to use.  In other words, the first read from a memory location causes a memory cell to spring
             *  into existence in both the current state and the initial state.
             *
             *  To summarize: every memory address is given a unique named value. These values are implicit until the memory
             *  location is actually read.
             *
             *  See also readMemory() and writeMemory() methods in the various instruction semantics policies. */
            template<template<size_t> class ValueType>
            class MemoryCell {
            protected:
                ValueType<32> address;                      /**< Address of memory cell. */
                ValueType<32> data;                         /**< The data stored at this memory location. */
                size_t nbytes;                              /**< Number of bytes of data. */
                bool clobbered;                             /**< Clobbered by another write to an address that may alias this
                                                             *   one? */
                bool written;                               /**< Has this cell been written by the policy's writeMemory()
                                                             *   method? */

            public:
                /** Constructor. Creates a new memory cell object with specified address, data, and size.  The @p clobbered and
                 *  @p written properties are both initialized to false. */
                template <size_t Len>
                MemoryCell(const ValueType<32> &address, const ValueType<Len> &data, size_t nbytes)
                    : address(address), data(data), nbytes(nbytes), clobbered(false), written(false) {}

                /** Accessor for the memory cell address.
                 * @{ */
                const ValueType<32>& get_address() const { return address; }
                ValueType<32>& get_address() { return address; }
                void set_address(const ValueType<32> &addr) { address=addr; }
                /** @}*/

                /** Accessor for the data stored at a memory location.
                 * @{ */
                const ValueType<32>& get_data() const { return data; }
                ValueType<32>& get_data() { return data; }
                void set_data(const ValueType<32> &d) { data=d; }
                /** @}*/

                /** Accessor for the number of signification bytes of data. */
                size_t get_nbytes() const { return nbytes; }
                void set_nbytes(size_t n) { nbytes=n; }
                /** @}*/

                /** Accessor for whether a cell has been clobbered.  A cell should be marked as clobbered when a new cell is
                 *  created and the address of the new cell could be equal to the address of this cell.
                 * @{ */
                bool is_clobbered() const { return clobbered; }
                void set_clobbered(bool b=true) { clobbered=b; }
                void clear_clobbered() { clobbered=false; }
                /** @}*/

                /** Accessor for whether a cell has been written.  A cell that is written to with writeMemory() should be
                 *  marked as such.  This is to make a distinction between cells that have sprung insto existence by virtue of
                 *  reading from a previously unknown cell and cells that have been created as a result of a memoryWrite
                 *  operation.
                 * @{ */
                bool is_written() const { return written; }
                void set_written(bool b=true) { written=b; }
                void clear_written() { written=false; }
                /** @}*/

                /** Prints a memory cell.  The output (at least for the BaseSemantics::MemoryCell implementation) is three
                 *  lines, one each for the address, the value, and various flags. Each line is prefixed with the specified @p
                 *  prefix string.  The print() method of the underlying ValueType should accept two arguments: the
                 *  std::ostream and a pointer to an optional templatized PrintHelper which is not interpreted in any way by
                 *  this method. */
                template<typename PrintHelper>
                void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
                    o <<prefix <<"address = { ";
                    address.print(o, ph);
                    o <<" }\n";

                    o <<prefix <<"  value = { ";
                    data.print(o, ph);
                    o <<" }\n";

                    o <<prefix <<"  flags = { size=" <<nbytes;
                    if (!written) o <<"; read-only";
                    if (clobbered) o <<"; clobbered";
                    o <<" }\n";
                }

                /** Prints a memory cell.  This is the same as calling the BaseSemantics::MemoryCell::print() method with an
                 *  empty prefix and a null PrintHelper. */
                friend std::ostream& operator<<(std::ostream &o, const MemoryCell &mc) {
                    mc.print<SEMANTIC_NO_PRINT_HELPER>(o);
                    return o;
                }

            };

            /******************************************************************************************************************
             *                                  RegisterStateX86
             ******************************************************************************************************************/

            /** The set of all registers and their values. */
            template <template <size_t> class ValueType>
            class RegisterStateX86 {
            public:
                static const size_t n_gprs = 8;             /**< Number of general-purpose registers in this state. */
                static const size_t n_segregs = 6;          /**< Number of segmentation registers in this state. */
                static const size_t n_flags = 32;           /**< Number of flag registers in this state. */

                ValueType<32> ip;                           /**< Instruction pointer. */
                ValueType<32> gpr[n_gprs];                  /**< General-purpose registers */
                ValueType<16> segreg[n_segregs];            /**< Segmentation registers. */
                ValueType<1> flag[n_flags];                 /**< Control/status flags (i.e., FLAG register). */

                void clear() {
                    ip = ValueType<32>();
                    for (size_t i=0; i<n_gprs; ++i)
                        gpr[i] = ValueType<32>();
                    for (size_t i=0; i<n_segregs; ++i)
                        segreg[i] = ValueType<16>();
                    for (size_t i=0; i<n_flags; ++i)
                        flag[i] = ValueType<1>();
                }

                void zero() {
                    static const uint64_t z = 0;
                    ip = ValueType<32>(z);
                    for (size_t i=0; i<n_gprs; ++i)
                        gpr[i] = ValueType<32>(z);
                    for (size_t i=0; i<n_segregs; ++i)
                        segreg[i] = ValueType<16>(z);
                    for (size_t i=0; i<n_flags; ++i)
                        flag[i] = ValueType<1>(z);
                }

                /** Print the register contents. This emits one line per register and contains the register name and its value.
                 *  The @p ph argument is a templatized PrintHelper that is simply passed as the second argument of the
                 *  underlying print methods for the ValueType. */
                template<typename PrintHelper>
                void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
                    std::ios_base::fmtflags orig_flags = o.flags();
                    try {
                        for (size_t i=0; i<n_gprs; ++i) {
                            o <<prefix <<std::setw(7) <<std::left <<gprToString((X86GeneralPurposeRegister)i) <<" = { ";
                            gpr[i].print(o, ph);
                            o <<" }\n";
                        }
                        for (size_t i=0; i<n_segregs; ++i) {
                            o <<prefix <<std::setw(7) <<std::left <<segregToString((X86SegmentRegister)i) <<" = { ";
                            segreg[i].print(o, ph);
                            o <<" }\n";
                        }
                        for (size_t i=0; i<n_flags; ++i) {
                            o <<prefix <<std::setw(7) <<std::left <<flagToString((X86Flag)i) <<" = { ";
                            flag[i].print(o, ph);
                            o <<" }\n";
                        }
                        o <<prefix <<std::setw(7) <<std::left <<"ip" <<" = { ";
                        ip.print(o, ph);
                        o <<" }\n";
                    } catch (...) {
                        o.flags(orig_flags);
                        throw;
                    }
                    o.flags(orig_flags);
                }
            };

            /******************************************************************************************************************
             *                                  StateX86
             ******************************************************************************************************************/

            /** Base class for x86 virtual machine states.
             *
             *  Binary semantic analysis usually progresses one instruction at a time--one starts with an initial state and the
             *  act of processing an instruction modifies the state.  The StateX86 is the base class class for the semantic
             *  states of various instruction semantic policies.  It contains storage for all the machine registers and a
             *  vector of MemoryCell objects to represent memory. */
            template<
                template <template <size_t> class ValueType> class MemoryCell,
                template <size_t> class ValueType>
            class StateX86 {
            public:
                RegisterStateX86<ValueType> registers;
                typedef std::list<MemoryCell<ValueType> > Memory;
                Memory memory;

                /** Initialize state.  The state is initialized using the ValueType default constructor and the memory vector
                 * is emptied. */
                void clear() {
                    registers.clear();
                    clear_memory();
                }

                /** Initialize all registers to zero.  This is done with the ValueType((uint64_t)0) constructor. Memory is not
                 * affected. */
                void zero_registers() {
                    registers.zero();
                }

                /** Clear all memory locations.  This just empties the memory vector. */
                void clear_memory() {
                    memory.clear();
                }

                /** Print the register contents. This emits one line per register and contains the register name and its value.
                 *  The @p ph argument is a templatized PrintHelper that is simply passed as the second argument of the
                 *  underlying print methods for the ValueType. */
                template<typename PrintHelper>
                void print_registers(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
                    registers.print(o, prefix, ph);
                }

                /** Print memory contents.  This simply calls the MemoryCell::print method for each memory cell. The @p ph
                 * argument is a templatized PrintHelper that's just passed as the second argument to the underlying print
                 * methods for the ValueType. */
                template<typename PrintHelper>
                void print_memory(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
                    std::ios_base::fmtflags orig_flags = o.flags();
                    try {
                        for (typename Memory::const_iterator mi=memory.begin(); mi!=memory.end(); ++mi)
                            mi->print(o, prefix, ph);
                    } catch (...) {
                        o.flags(orig_flags);
                        throw;
                    }
                    o.flags(orig_flags);
                }

                /** Print the state.  This emits a multi-line string containing the registers and all known memory locations.
                 *  The @p ph argument is a templatized PrintHelper pointer that's simply passed as the second argument to the
                 *  print methods for the ValueType. */
                template<typename PrintHelper>
                void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
                    o <<prefix <<"registers:\n";
                    print_registers(o, prefix+"    ", ph);
                    o <<prefix <<"memory:\n";
                    print_memory(o, prefix+"    ", ph);
                }

                /** Prints a semantic policy state.  This is the same as calling the StateX86's print() method with an empty
                 *  prefix string and a null PrintHelper. */
                friend std::ostream& operator<<(std::ostream &o, const StateX86 &state) {
                    state.print<SEMANTIC_NO_PRINT_HELPER>(o);
                    return o;
                }
            };

            /******************************************************************************************************************
             *                                  Policy
             ******************************************************************************************************************/

            /** Base class for most instruction semantics policies.  Policies can be derived from the base class or they may
             *  implement the same interface.   The policy is responsible for defining the semantics of the RISC-like
             *  operations invoked by the translation object (e.g., X86InstructionSemantics).  We omit the declarations and
             *  definitions of the RISC operations from the base class so that failure to implement them in a subclass is an
             *  error.  Their documentation can be found in the NullSemantics::Policy class. */
            class Policy {
            protected:
                const RegisterDictionary *regdict;                  /**< See set_register_dictionary(). */

            public:
                Policy(): regdict(NULL) {}

                /** Instruction semantic policy exception. */
                struct Exception {
                    Exception(const std::string &mesg): mesg(mesg) {}
                    friend std::ostream& operator<<(std::ostream &o, const Exception &e) {
                        o <<"semantics exception: " <<e.mesg;
                        return o;
                    }
                    std::string mesg;
                };

                /** Access the register dictionary.  The register dictionary defines the (superset) of registers stored in the
                 *  policy's state(s).  This dictionary is used by the semantic translation class to translate register names
                 *  to register descriptors.  For instance, to read from the "eax" register, the semantics will look up "eax"
                 *  in the policy's register dictionary and then pass that descriptor to the policy's readRegister() method.
                 *  Register descriptors are also stored in instructions when the instruction is disassembled, so the
                 *  disassembler and policy should probably be using the same dictionary.
                 *
                 *  The register dictionary should not be changed after a translation object is instantiated because the
                 *  translation object's constructor may query the dictionary and cache the resultant register descriptors.
                 *
                 * @{ */
                const RegisterDictionary *get_register_dictionary() const {
                    return regdict;
                }
                void set_register_dictionary(const RegisterDictionary *regdict) {
                    this->regdict = regdict;
                }
                /** @} */

                /** Lookup a register by name.  This policy's register dictionary is consulted and the specified register is
                 *  located by name.  If a bit width is specified (@p nbits) then it must match the size of register that was
                 *  found.  If a valid register cannot be found then an exception is thrown. */
                const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0) {
                    const RegisterDictionary *regdict = get_register_dictionary();
                    if (!regdict)
                        throw Exception("no register dictionary");

                    const RegisterDescriptor *reg = regdict->lookup(regname);
                    if (!reg) {
                        std::ostringstream ss;
                        ss <<"Invalid register: \"" <<regname <<"\"";
                        throw Exception(ss.str());
                    }

                    if (nbits>0 && reg->get_nbits()!=nbits) {
                        std::ostringstream ss;
                        ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
                           <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
                        throw Exception(ss.str());
                    }
                    return *reg;
                }
            };
        } /*namespace*/
    } /*namespace*/
} /*namespace*/
#endif
