#ifndef ROSE_BinaryAnalysis_RegisterDictionary_H
#define ROSE_BinaryAnalysis_RegisterDictionary_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/BinaryAnalysis/RegisterParts.h>
#include <rose_extent.h>
#include <Map.h>                                        // rose

#include <Sawyer/SharedPointer.h>
#include <Sawyer/SharedObject.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#endif

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
    RegisterDescriptor instructionPointer_;             // optional special instruction pointer register
    RegisterDescriptor stackPointer_;                   // optional special stack pointer register
    RegisterDescriptor stackFrame_;                     // optional special stack frame pointer register
    RegisterDescriptor stackSegment_;                   // optional special stack segment register
    RegisterDescriptor callReturn_;                     // optional special call return address register

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
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
    RegisterDictionary& operator=(const RegisterDictionary&) = default;

public:
    /** Allocating constructor for an empty dictionary. */
    static Ptr instance(const std::string &name);

    /** Mostly empty dictionary for the null ISA. */
    static Ptr instanceNull();

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

    /** Return the number of entries in the dictionary. */
    size_t size() const;

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
     *  return the empty string. */
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Register names
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Name of the register or nothing.
     *
     *  Returns the non-empty name of the register if available, or nothing. */
    Sawyer::Optional<std::string> name(RegisterDescriptor) const;

    /** Name or quad.
     *
     *  Returns the name of the register if available, or else the quad. The quad is generated from the @ref
     *  RegisterDescriptor::toString method. */
    std::string nameOrQuad(RegisterDescriptor) const;

    /** Name and quad.
     *
     *  If the register has a name, then this function returns the name in quotes using C string literal escapes followed by
     *  the quad. Otherwise just the quad is returned. The quad is generated by calling @ref RegisterDescriptor::toString. */
    std::string nameAndQuad(RegisterDescriptor) const;

    /** Quad and name.
     *
     *  Returns a string containing the register quad. If the register has a name then it follows in quotes using C string
     *  literal escapes. The quad is generated by calling @ref RegisterDescriptor::toString. */
    std::string quadAndName(RegisterDescriptor) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Special registers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: The register that points to instructions.
     *
     *  If this architecture has no such register (most do) then this property stores an empty register descriptor.
     *
     * @{ */
    RegisterDescriptor instructionPointerRegister() const;
    void instructionPointerRegister(RegisterDescriptor);
    void instructionPointerRegister(const std::string&);
    /** @} */

    /** Property: The register that points to the stack.
     *
     *  If this architecture has no such register (most do) then this property stores an empty register descriptor.
     *
     * @{ */
    RegisterDescriptor stackPointerRegister() const;
    void stackPointerRegister(RegisterDescriptor);
    void stackPointerRegister(const std::string&);
    /** @} */

    /** Property: The register that ponts to the stack frame.
     *
     *  If this architecture has no such register (most do) then this property stores an empty register descriptor.
     *
     * @{ */
    RegisterDescriptor stackFrameRegister() const;
    void stackFrameRegister(RegisterDescriptor);
    void stackFrameRegister(const std::string&);
    /** @} */

    /** Property: The segment register for accessing the stack.
     *
     *  If this architecture has no such register (most don't) then this property stores an empty register descriptor.
     *
     * @{ */
    RegisterDescriptor stackSegmentRegister() const;
    void stackSegmentRegister(RegisterDescriptor);
    void stackSegmentRegister(const std::string&);
    /** @} */

    /** Property: The register that holds the return address for a function.
     *
     *  If this architecture has no such register (most don't) then this property stores an empty register descriptor.
     *
     * @{ */
    RegisterDescriptor callReturnRegister() const;
    void callReturnRegister(RegisterDescriptor);
    void callReturnRegister(const std::string&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Debugging
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Prints the contents of this register dictionary.
     *
     *  The first line of output contains the dictionary name. One additional line of output will be generated for each entry
     *  in the dictionary.
     *
     *  @{ */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const RegisterDictionary&);
    /** @} */
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
