#ifndef ROSE_BinaryAnalysis_RegisterDictionary_H
#define ROSE_BinaryAnalysis_RegisterDictionary_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/BinaryAnalysis/RegisterParts.h>

#include <Sawyer/SharedPointer.h>
#include <Sawyer/SharedObject.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <map>
#include <ostream>
#include <queue>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

/** Defines registers available for a particular architecture.
 *
 *  The dictionary maps each register name to a RegisterDescriptor.  The RegisterDescriptor describes how a register name maps
 *  to (part of) a physical CPU register by providing a major number, minor number, bit offset, and number of bits.  The major
 *  number is typically a register class number and the minor number is typically an offset within the class.  For instance,
 *  for x86 the major numbers might indicate whether a register is a general purpose register, an 80-bit floating point
 *  register, a 128-bit MMX register, etc.
 *
 *  Users should not assume that the values of a RegisterDescriptor correspond to actual values found in the machine
 *  instructions that were disassembled.  What they can assume is that two unrelated registers (such as "eax" and "ebx") do not
 *  overlap in the RegisterDescriptor address space (major, minor, offset, size). They can also assume that two related
 *  registers (such as "eax" and "rax", the 32- and 64-bit versions of a single CPU register) do, in fact, overlap in the
 *  RegisterDescriptor address space and that the overlap indicates how the registers are related.
 *
 *  Users should not assume that RegisterDescriptor entries from two separate dictionaries are compatible, although the
 *  dictionaries created by the ROSE library do use compatible descriptors across each family. Looking up the "eax" register in
 *  one dictionary may return a different descriptor than "eax" looked up in a different dictionary (but not in ROSE-created
 *  dictinaries).  Components of the ROSE binary support that generate RegisterDescriptors provide a mechanism for obtaining
 *  (and possibly setting) the register dictionary.  For instance, the @ref Disassembler::Base class has get_registers() and
 *  set_registers() methods. */
class RegisterDictionary: public Sawyer::SharedObject {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reference counting pointer. */
    using Ptr = RegisterDictionaryPtr;

    /** List of registers in dictionary. */
    using Entries = std::map<std::string/*name*/, RegisterDescriptor>;

    /** List of register descriptors in dictionary. */
    using RegisterDescriptors = Rose::BinaryAnalysis::RegisterDescriptors;

private:
    // a descriptor can have more than one name
    using Reverse = std::map<RegisterDescriptor, std::vector<std::string>>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    std::string name_;                                  // name of the dictionary, usually an architecture name like 'i386'
    Entries forward_;                                   // N:1 forward lookup from name to descriptor
    Reverse reverse_;                                   // 1:N reverse lookup from descriptor to names

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(forward_);
        s & BOOST_SERIALIZATION_NVP(reverse_);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors and destructor
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    RegisterDictionary();

    RegisterDictionary(const std::string &name);

    RegisterDictionary(const RegisterDictionary& other);

public:
    /** Allocating constructor for an empty dictionary. */
    static Ptr instance(const std::string &name);

    /** Mostly empty dictionary for the null ISA. */
    static Ptr instanceNull();

    /** Intel 8086 registers.
     *
     *  The Intel 8086 has fourteen 16-bit registers. Four of them (AX, BX, CX, DX) are general registers (although each may
     *  have an additional purpose; for example only CX can be used as a counter with the loop instruction). Each can be
     *  accessed as two separate bytes (thus BX's high byte can be accessed as BH and low byte as BL). Four segment registers
     *  (CS, DS, SS and ES) are used to form a memory address. There are two pointer registers. SP points to the bottom of the
     *  stack and BP which is used to point at some other place in the stack or the memory(Offset).  Two registers (SI and DI)
     *  are for array indexing. The FLAGS register contains flags such as carry flag, overflow flag and zero flag. Finally, the
     *  instruction pointer (IP) points to the next instruction that will be fetched from memory and then executed. */
    static Ptr instanceI8086();

    /** Intel 8088 registers.
     *
     *  Intel 8088 has the same set of registers as Intel 8086. */
    static Ptr instanceI8088();

    /** Intel 80286 registers.
     *
     *  The 80286 has the same registers as the 8086 but adds two new flags to the "flags" register. */
    static Ptr instanceI286();

    /** Intel 80386 registers.
     *
     *  The 80386 has the same registers as the 80286 but extends the general-purpose registers, base registers, index
     *  registers, instruction pointer, and flags register to 32 bits.  Register names from the 80286 refer to the same offsets
     *  and sizes while the full 32 bits are accessed by names prefixed with "e" as in "eax" (the "e" means "extended"). Two
     *  new segment registers (FS and GS) were added and all segment registers remain 16 bits. */
    static Ptr instanceI386();

    /** Intel 80386 with 80387 math co-processor. */
    static Ptr instanceI386Math();

    /** Intel 80486 registers.
     *
     *  The 80486 has the same registers as the 80386 with '387 co-processor but adds a new flag to the "eflags" register. */
    static Ptr instanceI486();

    /** Intel Pentium registers.
     *
     *  The Pentium has the same registers as the 80486 but adds a few flags to the "eflags" register and MMX registers. */
    static Ptr instancePentium();

    /** Intel Pentium III registers.
     *
     *  The Pentium III has the same register set as the Pentium but adds the xmm0 through xmm7 registers for the SSE
     *  instruction set. */
    static Ptr instancePentiumiii();

    /** Intel Pentium 4 registers. */
    static Ptr instancePentium4();

    /** Amd64 registers.
     *
     *  The AMD64 architecture increases the size of the general purpose registers, base registers, index registers,
     *  instruction pointer, and flags register to 64-bits.  Most register names from the Pentium architecture still exist and
     *  refer to 32-bit quantities, while the AMD64 adds new names that start with "r" rather than "e" (such as "rax" for the
     *  64-bit register and "eax" for the 32 low-order bits of the same register).  It also adds eight additional 64-bit
     *  general purpose registers named "r8" through "r15" along with "b", "w", and "d" suffixes for the low-order 8, 16, and
     *  32 bits, respectively.
     *
     *  The only registers that are not retained are the control registers cr0-cr4, which are replaced by 64-bit registers of
     *  the same name, and debug registers dr0-dr7, which are also replaced by 64-bit registers of the same name. */
    static Ptr instanceAmd64();

#ifdef ROSE_ENABLE_ASM_AARCH64
    /** AArch64 registers.
     *
     *  These are the registers for the ARM AArch64 architecture.
     *
     *  They are documented in "Arm Instruction Set Version 1.0 Reference Guide" copyright 2018 Arm Limited. */
    static Ptr instanceAarch64();
#endif

#ifdef ROSE_ENABLE_ASM_AARCH32
    /** AArch32 registers.
     *
     *  These are the registers for the ARM AArch32 architecture. */
    static Ptr instanceAarch32();
#endif

    /** PowerPC-32 registers. */
    static Ptr instancePowerpc32();

    /** PowerPC-64 registers. */
    static Ptr instancePowerpc64();

    /** MIPS32 Release 1.
     *
     * Release 1 of MIPS32 supports only a 32-bit FPU (support for 64-bit FPU was added in MIPS32 Release 2). */
    static Ptr instanceMips32();

    /** MIPS32 Release 1 with special registers.
     *
     * This is the same dictionary as dictionary_mips32(), except additional names are supplied for the general purpose
     * registers (e.g., "zero" for r0, "at" for r1, "gp" for r28, "sp" for r29, "fp" for r30, "ra" for r31, etc.).  This is
     * intended mostly for the AsmUnparser; any layer that looks up registers by name should probably use the standard names
     * rather than relying on these alternate names.  */
    static Ptr instanceMips32AlternateNames();

    /** Motorola M68330 register names */
    static Ptr instanceM68000();

    /** Motorola M68330 alternate registers. */
    static Ptr instanceM68000AlternateNames();

    /** FreeScale ColdFire generic hardware registers. */
    static Ptr instanceColdfire();

    /** Registers for FreeScale ColdFire CPUs with EMAC (extended multiply-accumulate) unit. */
    static Ptr instanceColdfireEmac();

    /** CIL register names */
    static Ptr instanceCil();

    /** JVM register names */
    static Ptr instanceJvm();

    /** Class method to choose an appropriate register dictionary for an instruction set architecture.
     *
     *  Returns the best available register dictionary for any architecture. Returns the null pointer if no dictionary is
     *  appropriate.
     *
     *  @{ */
    static Ptr instanceForIsa(SgAsmExecutableFileFormat::InsSetArchitecture);
    static Ptr instanceForIsa(SgAsmInterpretation*);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Property: Architecture name.
     *
     * @{ */
    const std::string &name() const;
    void name(const std::string&);
    /** @} */

    /** Returns the list of all register definitions in the dictionary. */
    const Entries& registers() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mutators
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Insert a definition into the dictionary.
     *
     *  If the name already exists in the dictionary then the new RegisterDescriptor will replace the one that already
     *  exists. */
    void insert(const std::string &name, RegisterDescriptor);

    /** Insert a definition into the dictionary.
     *
     *  If the name already exists in the dictionary then the new RegisterDescriptor will replace the one that already
     *  exists. */
    void insert(const std::string &name, unsigned majr, unsigned minr, unsigned offset, unsigned nbits);

    /** Inserts definitions from another dictionary into this dictionary.
     *
     *  Names in the other dictionary that are the same as names in this dictionary will replace the definitions in this
     *  dictionary. */
    void insert(const Ptr&);

    /** Changes the size of a register.
     *
     *  This is a common enough operation that we have a special method to do it.  To change other properties of a register you
     *  would look up the register descriptor, change the property, then re-insert the register into the dictionary using the
     *  new descriptor.  This method does exactly that. */
    void resize(const std::string &name, unsigned new_nbits);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Lookup
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Find a register by name.
     *
     *  Looks up the descriptor for the register having the specified name. If no descriptor exists for that name then a
     *  default constructed invalid descriptor is returned. See also, @ref findOrThrow. */
    RegisterDescriptor find(const std::string &name) const;

    /** Find a register by name.
     *
     *  Looks up the descriptor for the register having the specified name.  If no descriptor exists for that name then a @ref
     *  Rose::Exception is thrown.  See also @ref find. */
    RegisterDescriptor findOrThrow(const std::string &name) const;

    /** Returns a register name for a given descriptor.
     *
     *  If more than one register has the same descriptor then the name added latest is returned.  If no register is found then
     *  either return the empty string (default) or generate a generic name according to the optional supplied
     *  NameGenerator. */
    const std::string& lookup(RegisterDescriptor) const;

    /** Determine if a register descriptor exists.
     *
     *  This is similar to the @ref lookup method that takes a @ref RegisterDescriptor argument, but instead of returning the
     *  name it returns true if found, false if not found. */
    bool exists(RegisterDescriptor) const;

    /** Finds the first largest register with specified major and minor number.
     *
     *  Returns the first register with the largest width and having the specified major and minor numbers. Registers wider
     *  than @p maxWidth (if non-zero) are ignored. If no register is found with the major/minor number then an invalid
     *  (default-constructed) register is returned.
     *
     *  This function takes O(n) time where n is the number of registers defined. */
    RegisterDescriptor findLargestRegister(unsigned major, unsigned minor, size_t maxWidth=0) const;

    /** Returns all register parts.
     *
     *  Returns all parts of all registers in this dictionary without any regard for register boundaries.  For instance, if a
     *  diction contains the x86 AX and AL registers where AX is 16 bits and AL is its low-order eight bits, the return value
     *  will only contain the fact that the 16 bits corresponding to AX are stored, which also happens to contain the eight
     *  bits of AL, but it won't keep track that AX and AL were inserted separately. In other words, erasing AL from the
     *  returned container would also erase the low-order 8 bits of AX. */
    Rose::BinaryAnalysis::RegisterParts getAllParts() const;

    /** Returns the list of all register descriptors.
     *
     *  The returned list may have overlapping register descriptors. The return value is similar to get_registers() except only
     *  the RegisterDescriptor part is returned, not the names. */
    RegisterDescriptors getDescriptors() const;

    /** Returns the first unused major register number. */
    unsigned firstUnusedMajor() const;

    /** Returns the first unused minor register number. */
    unsigned firstUnusedMinor(unsigned majr) const;

    /** Compares number of bits in register descriptors. This comparator is used to sort register descriptors in either
     *  ascending or descending order depending on the number of significant bits in the register. The default constructor
     *  uses a descending sort order.
     *
     *  For instance, to get a list of all descriptors sorted by descending number of significant bits, one could do:
     *
     * @code
     *  RegisterDescriptors regs = dictionary.get_descriptors();
     *  std::sort(regs.begin(), regs.end(), SortBySize());
     * @endcode
     */
    class SortBySize {
    public:
        enum Direction { ASCENDING, DESCENDING };
        explicit SortBySize(Direction d=DESCENDING): direction(d) {}
        bool operator()(RegisterDescriptor a, RegisterDescriptor b) const {
            return ASCENDING==direction ?
                a.nBits() < b.nBits() :
                a.nBits() > b.nBits();
        }
    protected:
        Direction direction;
    };

    /** Returns the list of non-overlapping registers or register parts.
     *
     *  The list of registers are processed in the reverse specified order and each register is added to the return value if
     *  its bits were not already added to the return value by a previous register.  If a register under consideration is only
     *  partially represented in the return value at the time it is considered, then it is either split into smaller parts to
     *  be reconsidered later, or not reconsidered at all. Thus, when @p reconsiderParts is true, the return value might
     *  contain register descriptors that were not part of the input @p reglist, and which might not even have entries/names in
     *  the register dictionary (see get_largest_registers() for more explaination).
     *
     *  For example, to get a list of the largest non-overlapping registers one could do the following:
     * @code
     *  RegisterDictionary::SortBySize largest_to_smallest(RegisterDictionary::SortBySize::DESCENDING);
     *  RegisterDictionary::RegisterDescriptors all_registers = ditionary.get_descriptors();
     *  RegisterDictionary::RegisterDescriptors list = dictionary.filter_nonoverlapping(all_registers, largest_to_smallest, true);
     * @endcode
     *
     *  Filtering largest to smallest and reconsidering parts is the default, so the example can be shortened to:
     * @code
     *  RegisterDictionary::RegisterDescriptors list = dictionary.filter_nonoverlapping(dictionary.get_descriptors());
     * @endcode
     *
     *  In fact, it can be shortened even more since this common operation is encapsulated in get_largest_registers():
     * @code
     *  RegisterDictionary::RegisterDescriptors list = dictionary.get_largest_registers();
     * @endcode
     */
    template<class Compare>
    static RegisterDescriptors filterNonoverlapping(RegisterDescriptors reglist,
                                                    Compare order = SortBySize(),
                                                    bool reconsiderParts = true);

    /** Returns a list of the largest non-overlapping registers.
     *
     *  For instance, for a 32-bit x86 dictionary the return value will contain registers EAX, EBX, etc. but not AX, AH, AL,
     *  BX, BH, BL, etc. Note that some of the returned descriptors might not correspond to actual names in the dictionary;
     *  this can happen when the dictionary contains two registers that partially overlap, but are themselves not subsets of a
     *  larger register, as in:
     *
     * @code
     *  |XXXXXXXXXXXX....| The "X" register occupies the first 12 bits of a 16-bit register
     *  |....YYYYYYYYYYYY| The "Y" register occupies the last 12 bits of a 16-bit register
     * @endcode
     *
     * If the 16-bit register has no name, and the high- and low-order four-bit parts have no name, then the return value
     * might consist of register "X" and the low four bits.  Or it could be register "Y" and the high four bits. */
    RegisterDescriptors getLargestRegisters() const;

    /** Returns a list of the smallest non-overlapping registers.
     *
     *  For instance, for a 32-bit x86 dictionary the return value will contain AX, AH, AL, BX, BH, BL, etc. rather than EAX
     *  and EBX.  It will also return the high 16-bit parts of EAX and EBX even though they're not represented with explicit
     *  definitions in the dictionary.
     *
     *  This is a one-liner in terms of filter_nonoverlapping.  If you don't want the unnamed parts to appear in the return
     *  value (e.g., the high-order 16 bits of EAX), then call filter_nonoverlapping() like this:
     *
     * @code
     *  SortBySize order(SortBySize::ASCENDING);
     *  RegisterDescriptors smallest = dict.filter_nonoverlapping(dict.get_descriptors(), order, false);
     * @endcode
     */
    RegisterDescriptors getSmallestRegisters() const;

    /** Prints the contents of this register dictionary.
     *
     *  The first line of output contains the dictionary name. One additional line of output will be generated for each entry
     *  in the dictionary.
     *
     *  @{ */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const RegisterDictionary&);
    /** @} */

    /** Return the number of entries in the dictionary. */
    size_t size() const;
};

std::ostream&
operator<<(std::ostream&, const RegisterDictionary&);

template<class Compare>
RegisterDictionary::RegisterDescriptors
RegisterDictionary::filterNonoverlapping(RegisterDescriptors desc, Compare order, bool reconsiderParts) {
    RegisterDescriptors retval;
    Map<std::pair<int/*major*/, int/*minor*/>, ExtentMap> have_bits; // the union of the bits of descriptors we will return

    // Process the descriptors in the order specified.
    std::priority_queue<RegisterDescriptor, RegisterDescriptors, Compare> heap(order, desc);
    while (!heap.empty()) {
        const RegisterDescriptor cur_desc = heap.top();
        heap.pop();
        const std::pair<int, int> cur_majmin(cur_desc.majorNumber(), cur_desc.minorNumber());
        const Extent cur_extent(cur_desc.offset(), cur_desc.nBits());
        ExtentMap &have_extents = have_bits[cur_majmin];
        if (have_extents.distinct(cur_extent)) {
            // We're not returning any of these bits yet, so add the whole descriptor
            retval.push_back(cur_desc);
            have_extents.insert(cur_extent);
        } else if (reconsiderParts) {
            // We're already returning some (or all) of these bits. Split the cur_extent into sub-parts by subtracting the
            // stuff we're already returning.
            ExtentMap parts;
            parts.insert(cur_extent);
            parts.erase_ranges(have_extents);
            for (ExtentMap::iterator pi = parts.begin(); pi != parts.end(); ++pi) {
                const Extent &part = pi->first;
                RegisterDescriptor part_desc(cur_desc.majorNumber(), cur_desc.minorNumber(), part.first(), part.size());
                heap.push(part_desc);
            }
        }
    }
    return retval;
}

} // namespace
} // namespace

#endif
#endif
