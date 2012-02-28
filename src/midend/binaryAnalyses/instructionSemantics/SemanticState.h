#ifndef Rose_SemanticState_H
#define Rose_SemanticState_H

/** Indicates no print helper.  This can be used as the PrintHelper argument for most print methods related to instruction
 *  semantics. */
class SEMANTIC_NO_PRINT_HELPER {};

/** Represents one location in memory.  Each memory cell has an address, data, and size in bytes.  It also maintains two
 *  boolean values to track whether a cell has been clobbered by a possibly-aliasing write, and how the cell was initialized.
 *
 *  When a memory state is created, every memory location will be given a unique value. However, it's not practicle to store
 *  a value for every possible memory address, yet we want the following example to work correctly:
 *
 * @code
 *  1: mov eax, ds:[edx]    // first read returns V1
 *  2: mov eax, ds:[edx]    // subsequent reads from same address also return V1
 *  3: mov ds:[ecx], eax    // write to unknown address clobbers all memory
 *  4: mov eax, ds:[edx]    // read from same address as above returns V2
 *  5: mov eax, ds:[edx]    // subsequent reads from same address also return V2
 * @endcode
 *
 *  Furthermore, in some semantic policies the read from ds:[edx] at #1 above, retroactively stores V1 in an original memory
 *  state. That way if it needs to do additional analyses starting from the same initial state it will be available to use.  In
 *  other words, the first read from a memory location causes a memory cell to spring into existence in both the current state
 *  and the initial state.
 *
 *  To summarize: every memory address is given a unique named value. These values are implicit until the memory location is
 *  actually read.
 *
 *  See also readMemory() and writeMemory() methods in the various instruction semantics policies. */
template<template<size_t> class ValueType>
class SemanticMemoryCell {
protected:
    ValueType<32> address;                      /**< Address of memory cell. */
    ValueType<32> data;                         /**< The data stored at this memory location. */
    size_t nbytes;                              /**< Number of bytes of data. */
    bool clobbered;                             /**< Clobbered by another write to an address that may alias this one? */
    bool written;                               /**< Has this cell been written by the policy's writeMemory() method? */

public:
    /** Constructor. Creates a new memory cell object with specified address, data, and size.  The @p clobbered and @p written
     *  properties are both initialized to false. */
    template <size_t Len>
    SemanticMemoryCell(const ValueType<32> &address, const ValueType<Len> &data, size_t nbytes)
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

    /** Accessor for whether a cell has been clobbered.  A cell should be marked as clobbered when a new cell is created and
     *  the address of the new cell could be equal to the address of this cell.
     * @{ */
    bool is_clobbered() const { return clobbered; }
    void set_clobbered(bool b=true) { clobbered=b; }
    void clear_clobbered() { clobbered=false; }
    /** @}*/

    /** Accessor for whether a cell has been written.  A cell that is written to with writeMemory() should be marked as such.
     *  This is to make a distinction between cells that have sprung insto existence by virtue of reading from a previously
     *  unknown cell and cells that have been created as a result of a memoryWrite operation.
     * @{ */
    bool is_written() const { return written; }
    void set_written(bool b=true) { written=b; }
    void clear_written() { written=false; }
    /** @}*/

    /** Prints a memory cell.  The output (at least for the SemanticMemoryCell implementation) is three lines, one each for the
     *  address, the value, and various flags. Each line is prefixed with the specified @p prefix string.  The print() method
     *  of the underlying ValueType should accept two arguments: the std::ostream and a pointer to an optional templatized
     *  PrintHelper which is not interpreted in any way by this method. */
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

    /** Prints a memory cell.  This is the same as calling the SemanticMemoryCell::print() method with an empty prefix and a
     *  null PrintHelper. */
    friend std::ostream& operator<<(std::ostream &o, const SemanticMemoryCell &mc) {
        mc.print<SEMANTIC_NO_PRINT_HELPER>(o);
        return o;
    }
        
};


/** State of a semantic analysis.
 *
 *  Binary semantic analysis usually progresses one instruction at a time--one starts with an initial state and the act of
 *  processing an instruction modifies the state.  The SemanticStateX86 is the base class class for the semantic states of
 *  various instruction semantic policies.  It contains storage for all the machine registers and a vector of MemoryCell
 *  objects to represent memory. */
template<
    template <template <size_t> class ValueType> class MemoryCell,
    template <size_t> class ValueType>
class SemanticStateX86 {
public:
    typedef std::vector<MemoryCell<ValueType> > Memory;

    static const size_t n_gprs = 8;             /**< Number of general-purpose registers in this state. */
    static const size_t n_segregs = 6;          /**< Number of segmentation registers in this state. */
    static const size_t n_flags = 32;           /**< Number of flag registers in this state. */

    ValueType<32> ip;                           /**< Instruction pointer. */
    ValueType<32> gpr[n_gprs];                  /**< General-purpose registers */
    ValueType<16> segreg[n_segregs];            /**< Segmentation registers. */
    ValueType<1> flag[n_flags];                 /**< Control/status flags (i.e., FLAG register). */
    Memory mem;                                 /**< Core memory. */

    /** Initialize state.  The state is initialized using the ValueType default constructor and the memory vector is emptied. */
    void clear() {
        ip = ValueType<32>();
        for (size_t i=0; i<n_gprs; ++i)
            gpr[i] = ValueType<32>();
        for (size_t i=0; i<n_segregs; ++i)
            segreg[i] = ValueType<16>();
        for (size_t i=0; i<n_flags; ++i)
            flag[i] = ValueType<1>();
        mem.clear();
    }
    
    /** Initialize all registers to zero.  This is done with the ValueType((uint64_t)0) constructor. Memory is not affected. */
    void zero_registers() {
        static const uint64_t z = 0;
        ip = ValueType<32>(z);
        for (size_t i=0; i<n_gprs; ++i)
            gpr[i] = ValueType<32>(z);
        for (size_t i=0; i<n_segregs; ++i)
            segreg[i] = ValueType<16>(z);
        for (size_t i=0; i<n_flags; ++i)
            flag[i] = ValueType<1>(z);
    }

    /** Clear all memory locations.  This just empties the memory vector. */
    void clear_memory() {
        mem.clear();
    }

    /** Print the register contents. This emits one line per register and contains the register name and its value.  The @p ph
     *  argument is a templatized PrintHelper that is simply passed as the second argument of the underlying print methods for
     *  the ValueType. */
    template<typename PrintHelper>
    void print_registers(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
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

    /** Print memory contents.  This simply calls the MemoryCell::print method for each memory cell. The @p ph argument is a
     * templatized PrintHelper that's just passed as the second argument to the underlying print methods for the ValueType. */
    template<typename PrintHelper>
    void print_memory(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
        std::ios_base::fmtflags orig_flags = o.flags();
        try {
            for (typename Memory::const_iterator mi=mem.begin(); mi!=mem.end(); ++mi)
                (*mi).print(o, prefix, ph);
        } catch (...) {
            o.flags(orig_flags);
            throw;
        }
        o.flags(orig_flags);
    }

    /** Print the state.  This emits a multi-line string containing the registers and all known memory locations.  The @p ph
     *  argument is a templatized PrintHelper pointer that's simply passed as the second argument to the print methods for the
     *  ValueType. */
    template<typename PrintHelper>
    void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
        o <<prefix <<"registers:\n";
        print_registers(o, prefix+"    ", ph);
        o <<prefix <<"memory:\n";
        if (mem.empty()) {
            o <<prefix <<"    (empty)\n";
        } else {
            print_memory(o, prefix+"    ", ph);
        }
    }

    /** Prints a semantic policy state.  This is the same as calling the SemanticStateX86's print() method with an empty prefix
     *  string and a null PrintHelper. */
    friend std::ostream& operator<<(std::ostream &o, const SemanticStateX86 &state) {
        state.print<SEMANTIC_NO_PRINT_HELPER>(o);
        return o;
    }
};

#endif
