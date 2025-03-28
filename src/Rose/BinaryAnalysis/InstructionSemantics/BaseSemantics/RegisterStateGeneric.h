#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_RegisterStateGeneric_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_RegisterStateGeneric_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/AddressSet.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.h>
#include <Rose/Exception.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#endif

#include <Sawyer/IntervalSetMap.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

/** A RegisterState for any architecture.
 *
 *  This state stores a list of non-overlapping registers and their values, typically only for the registers that have been
 *  accessed.  The state automatically switches between different representations when accessing a register that overlaps with
 *  one or more stored registers (see the @ref accessModifiesExistingLocations and @ref accessCreatesLocations properties).
 *  For instance, if the state stores 64-bit registers and the specimen suddently switches to 32-bit mode, this state will
 *  split the 64-bit registers into 32-bit pieces.  If the analysis later returns to 64-bit mode, the 32-bit pieces are
 *  concatenated back to 64-bit values. This splitting and concatenation occurs on a per-register basis at the time the
 *  register is read or written.
 *
 *  The register state also stores optional information about writers for each register. Writer information (addresses of
 *  instructions that wrote to the register) are stored as sets defined at each bit of the register. This allows a wide
 *  register, like x86 RAX, to be written to in parts by different instructions, like x86 AL.  The register state itself
 *  doesn't update this information automatically--it only provides the API by which a higher software layer can manipulate the
 *  information.  This design allows the writer data structure to alternatively be used for things other than addresses of
 *  writing instructions.  For instance, the @ref SymbolicSemantics::RiscOperators has a setting that enables tracking
 *  writers. */
class RegisterStateGeneric: public RegisterState {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Base type. */
    using Super = RegisterState;

    /** Shared-ownership pointer. */
    using Ptr = RegisterStateGenericPtr;

    /** Exception when register storage is not present.
     *
     *  If the @ref accessCreatesLocations property is clear and a caller attempts to access a register (or part) that is not
     *  stored in the state, then an exception of this type is thrown. */
    class RegisterNotPresent: public Rose::Exception {
        RegisterDescriptor desc_;
    public:
        explicit RegisterNotPresent(RegisterDescriptor);
    };

    /** A range of bits indexes.
     *
     *  Represents of contiguous interval of bit indexes, such as all bits numbered zero through 15, inclusive. */
    using BitRange = Sawyer::Container::Interval<size_t>;

    /** Register map keys.
     *
     *  This class stores a register major and minor pair that is suitable for using as a key in an std::map or similar. These
     *  objects are implicitly constructed from @ref RegisterDescriptor. */
    struct RegStore {
        unsigned majr, minr;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(majr);
            s & BOOST_SERIALIZATION_NVP(minr);
        }
#endif

    public:
        RegStore();                                     // for serialization
        RegStore(RegisterDescriptor);
        bool operator<(const RegStore&) const;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Types for storing values
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** A register descriptor and its value. */
    struct RegPair {
        RegisterDescriptor desc;
        SValuePtr value;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(desc);
            s & BOOST_SERIALIZATION_NVP(value);
        }
#endif

    public:
        ~RegPair();
        RegPair();                                      // for serialization

    public:
        RegPair(RegisterDescriptor, const SValuePtr&);
        BitRange location() const;
        SValuePtr get(RegisterDescriptor, RiscOperators*) const;
    };

    /** Vector of register/value pairs. */
    using RegPairs = std::vector<RegPair>;

    /** Values for all registers. */
    using Registers = Sawyer::Container::Map<RegStore, RegPairs>;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Types for Boolean properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Boolean properties per bit.
     *
     *  This container stores properties per bit of a major/minor register pair.  For instance, the x86 16-bit AX register
     *  might have different sets of properties for its different subregisters, AL and AH.  This container stores those sets
     *  per bit. */
    using BitProperties = Sawyer::Container::IntervalSetMap<BitRange, InputOutputPropertySet>;

    /** Boolean properties for all registers.
     *
     *  This container is indexed by register major/minor pair, then by a bit number, and stores a set of properties. */
    using RegisterProperties = Sawyer::Container::Map<RegStore, BitProperties>;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Types for storing addresses
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Virtual addresses per bit. */
    using BitAddressSet = Sawyer::Container::IntervalSetMap<BitRange, AddressSet>;

    /** Virtual addresses for all registers. */
    using RegisterAddressSet = Sawyer::Container::Map<RegStore, BitAddressSet>;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    RegisterProperties properties_;                     // Boolean properties for each bit of each register.
    RegisterAddressSet writers_;                        // Writing instruction address set for each bit of each register
    bool accessModifiesExistingLocations_;              // Can read/write modify existing locations?
    bool accessCreatesLocations_;                       // Can new locations be created?

protected:
    /** Values for registers that have been accessed.
     *
     *  This is a map whose keys are major/minor pairs and whose values are IntervalSetMaps that associate a value with
     *  non-overlapping ranges of bits. When reading or writing a register, the register being accessed is guaranteed to
     *  overlap only with those registers on the matching major-minor list, if it overlaps at all.  The lists are typically
     *  short (e.g., one list might refer to all the parts of the x86 RAX register, but the RBX parts would be on a different
     *  list. None of the registers stored on a particular list overlap with any other register on that same list; when adding
     *  new register that would overlap, the registers with which it overlaps must be removed first. */
    Registers registers_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(RegisterState);
        s & BOOST_SERIALIZATION_NVP(properties_);
        s & BOOST_SERIALIZATION_NVP(writers_);
        s & BOOST_SERIALIZATION_NVP(accessModifiesExistingLocations_);
        s & BOOST_SERIALIZATION_NVP(accessCreatesLocations_);
        s & BOOST_SERIALIZATION_NVP(registers_);
    }
#endif
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Normal constructors
    //
    // These are protected because objects of this class are reference counted and always allocated on the heap.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    ~RegisterStateGeneric();

protected:
    RegisterStateGeneric();                             // for serialization


    RegisterStateGeneric(const SValuePtr &protoval, const RegisterDictionaryPtr&);

    RegisterStateGeneric(const RegisterStateGeneric&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Static allocating constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Instantiate a new register state. The @p protoval argument must be a non-null pointer to a semantic value which will be
     *  used only to create additional instances of the value via its virtual constructors.  The prototypical value is normally
     *  of the same type for all parts of a semantic analysis: its state and operator classes.
     *
     *  The register dictionary, @p regdict, describes the registers that can be stored by this register state, and should be
     *  compatible with the register dictionary used for other parts of binary analysis. */
    static RegisterStateGenericPtr instance(const SValuePtr &protoval, const RegisterDictionaryPtr&);

    /** Instantiate a new copy of an existing register state. */
    static RegisterStateGenericPtr instance(const RegisterStateGenericPtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Virtual constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionaryPtr&) const override;
    virtual AddressSpacePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Dynamic pointer casts
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Run-time promotion of a base address space pointer to a RegisterStateGeneric pointer. This is a checked conversion--it will
     *  fail if @p from does not point to a RegisterStateGeneric object. */
    static RegisterStateGenericPtr promote(const AddressSpacePtr &from);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Object properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Whether stored registers are adapted to access patterns.
     *
     *  When accessing an existing register for read or write, the register state can adapt the list of existing storage
     *  locations to match the access pattern.  For instance, accessing the middle of a register could split the register into
     *  three storage locations or leave it as one. Similarly accessing a register that spans two or more storage locations
     *  could either concatenate them into one location or leave them separate.
     *
     *  When this property is true then existing storage locations can be modified, otherwise extra steps are taken to preserve
     *  the list of storage locations.
     *
     *  This property applies only to @ref readRegister and @ref writeRegister and not to those methods that are not typically
     *  called as part of processing instruction semantics.
     *
     * @{ */
    bool accessModifiesExistingLocations() const /*final*/;
    virtual void accessModifiesExistingLocations(bool);
    /** @} */

    /** Guards whether access can change set of existing locations.
     *
     *  This guard temporarily enables or disables the @ref accessModifiesExistingLocations property, restoring the property to
     *  its original value when the guard is destroyed. */
    class AccessModifiesExistingLocationsGuard {
        RegisterStateGeneric *rstate_;
        bool savedValue_;
    public:
        AccessModifiesExistingLocationsGuard(RegisterStateGeneric *rstate, bool newValue);
        ~AccessModifiesExistingLocationsGuard();
    };

    /** Property: Whether access can create new locations.
     *
     *  This property controls what happens if some part of a register is accessed that isn't stored in the state. If the
     *  property is true then that part of the register springs into existence, otherwise an @ref RegisterNotPresent exception
     *  is thrown.
     *
     * @{ */
    bool accessCreatesLocations() const /*final*/;
    virtual void accessCreatesLocations(bool);
    /** @} */

    /** Guards whether access is able to create new locations.
     *
     *  This guard temporarily enables or disables the @ref accessCreatesLocations property, restoring the property to its
     *  original value when the guard is destroyed. */
    class AccessCreatesLocationsGuard {
        RegisterStateGeneric *rstate_;
        bool savedValue_;
    public:
        AccessCreatesLocationsGuard(RegisterStateGeneric *rstate, bool newValue);
        ~AccessCreatesLocationsGuard();
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Inherited non-constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual void clear() override;
    virtual void zero() override;
    virtual SValuePtr readRegister(RegisterDescriptor, const SValuePtr &dflt, RiscOperators*) override;
    virtual SValuePtr peekRegister(RegisterDescriptor, const SValuePtr &dflt, RiscOperators*) override;
    virtual void writeRegister(RegisterDescriptor, const SValuePtr &value, RiscOperators*) override;
    virtual void print(std::ostream&, Formatter&) const override;
    virtual bool merge(const AddressSpacePtr &other, RiscOperators *addrOps, RiscOperators *valOps) override;
    virtual void hash(Combinatorics::Hasher&, RiscOperators *addrOps, RiscOperators *valOps) const override;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Initialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Initialize all registers of the dictionary.  When the dictionary contains overlapping registers, only the largest
     *  registers are initialized. For example, on a 32-bit x86 architecture EAX would be initialized but not AX, AH, or AL;
     *  requesting AX, AH, or AL will return part of the initial EAX value. */
    virtual void initialize_large();

    /** Initialize all registers of the dictionary.  When the dictionary contains overlapping registers, only the smallest
     *  registers are initialized. For example, on a 32-bit x86 architecture, AX, AH, AL and the non-named high-order 16 bits
     *  of AX are inititialized, but EAX isn't explicitly initialized.  Requesting the value of EAX will return a value
     *  constructed from the various smaller parts. */
    virtual void initialize_small();

    /** Initialize the specified registers of the dictionary.  Each register in the list must not overlap with any other
     *  register in the list, or strange things will happen.  If @p initialize_to_zero is set then the specified registers are
     *  initialized to zero, otherwise they're initialized with the prototypical value's constructor that takes only a size
     *  parameter. This method is somewhat low level and doesn't do much error checking. */
    void initialize_nonoverlapping(const std::vector<RegisterDescriptor>&, bool initialize_to_zero);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Value storage queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the list of all registers and their values.
     *
     *  The returned registers are guaranteed to be non-overlapping, although they might not correspond to actual named machine
     *  registers.  For instance, if a 32-bit value was written to the x86 EFLAGS register then the return value will contain a
     *  register/value pair for EFLAGS but no pairs for individual flags.  If one subsequently writes a 1-bit value to the ZF
     *  flag (bit 6 of EFLAGS) then the return value will contain a register/value pair for ZF, and also a pair for bits 0-5,
     *  and a pair for bits 7-31, neither of which correspond to actual register names in x86 (there is no name for bits 0-5 as
     *  a whole). The @ref readRegister and @ref writeRegister methods can be used to re-cast the various pairs into other
     *  groupings; @ref get_stored_registers is a lower-level interface. */
    virtual RegPairs get_stored_registers() const;

    /** Determines if some of the specified register is stored in the state. Returns true even if only part of the requested
     *  register is in the state (as when one asks about EAX and the state only stores AX). This is slightly more efficient
     *  than calling stored_parts():
     *
     * @code
     *  RegisterStateGenericPtr rstate = ...;
     *  RegisterDescriptor reg = ...;
     *  assert(rstate->partly_exists(reg) == !parts_exist(reg).empty());
     * @endcode
     */
    virtual bool is_partly_stored(RegisterDescriptor) const;

    /** Determines if the specified register is wholly stored in the state. Returns if the state contains data for the entire
     *  register, even if that data is split among several smaller parts or exists as a subset of a larger part. */
    virtual bool is_wholly_stored(RegisterDescriptor) const;

    /** Determines if the specified register is stored exactly in the state. Returns true only if the specified register wholly
     *  exists and a value can be returned without extracting or concatenating values from larger or smaller stored parts. Note
     *  that a value can also be returned without extracting or conctenating if the state contains no data for the specified
     *  register, as indicated by is_partly_stored() returning false. */
    virtual bool is_exactly_stored(RegisterDescriptor) const;

    /** Returns a description of which bits of a register are stored.
     *
     *  The return value is an ExtentMap that contains the bits that are stored in the state. This does not return the value of
     *  any parts of stored registers--one gets that with @ref readRegister. The return value does not contain any bits that
     *  are not part of the specified register. */
    virtual ExtentMap stored_parts(RegisterDescriptor) const;

    /** Find stored registers overlapping with specified register.
     *
     *  Returns all stored registers that overlap with the specified register.  The registers in the returned vector will never
     *  overlap with each other, but they will all overlap with the specified register. */
    virtual RegPairs overlappingRegisters(RegisterDescriptor) const;

    /** Cause a register to not be stored.  Erases all record of the specified register. The RiscOperators pointer is used for
     *  its extract operation if the specified register is not exactly stored in the state, such as if the state
     *  stores RIP and one wants to erase only the 32-bits overlapping with EIP. */
    virtual void erase_register(RegisterDescriptor, RiscOperators*);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Traversals
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Functors for traversing register values in a register state. */
    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual SValuePtr operator()(RegisterDescriptor, const SValuePtr&) = 0;
    };

    /** Traverse register/value pairs.  Traverses all the (non-overlapping) registers and their values, calling the specified
     *  functor for each register/value pair. If the functor returns a new SValue then the return value becomes the new value
     *  for that register.  The new value must have the same width as the register.
     *
     *  For example, the following code performs a symbolic substitution across all the registers:
     *
     *  @code
     *   struct Substitution: BaseSemantics::RegisterStateGeneric::Visitor {
     *       SymbolicSemantics::SValuePtr from, to;
     *
     *       Substitution(const SymbolicSemantics::SValuePtr &from, const SymbolicSemantics::SValuePtr &to)
     *           : from(from), to(to) {}
     *
     *       BaseSemantics::SValuePtr operator()(RegisterDescriptor reg, const BaseSemantics::SValuePtr &val_) {
     *           SymbolicSemantics::SValuePtr val = SymbolicSemantics::SValue::promote(val_);
     *           return val->substitute(from, to);
     *       }
     *   };
     *
     *   SymbolicSemantics::SValuePtr original_esp = ...;
     *   SymbolicSemantics::SValuePtr fp = ...; // the frame pointer in terms of original_esp
     *   Substitution subst(original_esp, fp);
     *   RegisterStateGenericPtr regs = ...;
     *   std::cerr <<*regs; // register values before substitution
     *   regs->traverse(subst);
     *   std::cerr <<*regs; // all original_esp have been replaced by fp
     *  @endcode
     *
     * As with most ROSE and STL traversals, the Visitor is not allowed to modify the structure of the object over which it is
     * traversing.  In other words, it's permissible to change the values pointed to by the state, but it is not permissible to
     * perform any operation that might change the list of register parts by adding, removing, or combining parts.  This
     * includes calling @ref readRegister and @ref writeRegister except when the register being read or written is already
     * exactly stored in the state as indicated by @ref is_exactly_stored. */
    virtual void traverse(Visitor&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Writer addresses
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Whether a register has writers.
     *
     *  Returns true if the specified register has writers. The "Any" version returns true if any bits of the register have
     *  writers, and the "All" version returns true if all bits of the register have writers (but not necessarily all the same
     *  writer).
     *
     * @{ */
    virtual bool hasWritersAny(RegisterDescriptor) const;
    virtual bool hasWritersAll(RegisterDescriptor) const;
    /** @} */

    /** Get writer information.
     *
     *  Returns all instruction addresses that have written to at least part of the specified register.  For instance, if
     *  instruction 0x1234 and 0x4321 wrote to AL and instruction 0x5678 wrote to AH then this method would return the set
     *  {0x1234, 0x4321, 0x5678} as the writers of AX. */
    virtual AddressSet getWritersUnion(RegisterDescriptor) const;

    /** Get writer information.
     *
     *  Returns the set of instruction addresses that have written to the entire specified register.  For instance, if
     *  instruction 0x1234 and 0x4321 wrote to AL and instructions 0x1234 and 0x5678 wrote to AH then this method will return
     *  the set {0x1234} as the writers of AX. */
    virtual AddressSet getWritersIntersection(RegisterDescriptor) const;

    /** Insert writer information.
     *
     *  Adds the specified instruction addresses as writers of the specified register.  Any previously existing writer
     *  addresses are not affected. Returns true if any addresses were inserted, false if they all already existed.  A single
     *  writer address can also be specified due to the AddressSet implicit constructor. */
    virtual bool insertWriters(RegisterDescriptor, const AddressSet &writerVas);

    /** Erase specified writers.
     *
     *  Removes the specified addresses from the set of writers for the register without affecting other addresses that might
     *  also be present. Returns true if none of the writer addresses existed, false if any were removed.  A single writer
     *  address can also be specified due to the AddressSet implicit constructor. */
    virtual void eraseWriters(RegisterDescriptor, const AddressSet &writerVas);

    /** Set writer information.
     *
     *  Changes the writer information to be exactly the specified address or set of addresses.  A single writer address can
     *  also be specified due to the AddressSet implicit constructor. */
    virtual void setWriters(RegisterDescriptor, const AddressSet &writers);

    /** Erase all writers.
     *
     *  If a register descriptor is provided then all writers are removed for that register only.  Otherwise all writers are
     *  removed for all registers.
     *
     * @{ */
    virtual void eraseWriters(RegisterDescriptor);
    virtual void eraseWriters();
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Per-register bit Boolean properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Whether a register has the specified property.
     *
     *  Returns true if the register has the specified property. The "Any" version returns true if any bits of the register
     *  have the property, while the "All" version returns true if all bits of the register have the property.
     *
     * @{ */
    virtual bool hasPropertyAny(RegisterDescriptor, InputOutputProperty) const;
    virtual bool hasPropertyAll(RegisterDescriptor, InputOutputProperty) const;
    /** @} */

    /** Get properties.
     *
     *  Returns the Boolean properties associated with the specified register.  The "Union" version returns the union of the
     *  properties across all bits of the register, while the "Intersection" version returns the set of properties that are
     *  defined for all bits of the register.
     *
     * @{ */
    virtual InputOutputPropertySet getPropertiesUnion(RegisterDescriptor) const;
    virtual InputOutputPropertySet getPropertiesIntersection(RegisterDescriptor) const;
    /** @} */

    /** Insert Boolean properties.
     *
     *  Inserts the specified properties for all bits of the specified register without affecting any other properties.
     *  Returns true if a property was inserted anywhere, false if all specified properties already existed everywhere in the
     *  specified register.  A single property can also be specified due to the RegisterProperties implicit constructor. */
    virtual bool insertProperties(RegisterDescriptor, const InputOutputPropertySet&);

    /** Erase Boolean properties.
     *
     *  Removes the speciied properties from the specified register.  Returns true if any of the properties were erased, false
     *  if none of them already existed. A single property can also be specified due to the RegisterProperties implicit
     *  constructor. */
    virtual bool eraseProperties(RegisterDescriptor, const InputOutputPropertySet&);

    /** Assign property set.
     *
     *  Assigns the specified property set (or single property) to the specified register. The register will then contain only
     *  those specified properties. */
    virtual void setProperties(RegisterDescriptor, const InputOutputPropertySet&);

    /** Erase all Boolean properties.
     *
     *  Removes all properties from the specified register (or all registers).
     *
     * @{ */
    virtual void eraseProperties(RegisterDescriptor);
    virtual void eraseProperties();
    /** @} */

    /** Get registers having certain properties.
     *
     *  Return a list of registers that have the @p required properties and lack the @p prohibited properties.  The returned
     *  list contains the largest registers that satisfy the conditions. */
    virtual std::vector<RegisterDescriptor>
    findProperties(const InputOutputPropertySet &required,
                   const InputOutputPropertySet &prohibited = InputOutputPropertySet()) const;

    // Documented in super class
    virtual void updateWriteProperties(RegisterDescriptor, InputOutputProperty) override;
    virtual void updateReadProperties(RegisterDescriptor) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Non-public APIs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    void deep_copy_values();

    // Given a register descriptor return information about what's stored in the state. The two return values are:
    //
    //     accessedParts represent the parts of the reigster (matching major and minor numbers) that are present in the
    //     state and overlap with the specified register.
    //
    //     preservedParts represent the parts of the register that are present in the state but don't overlap with the
    //     specified register.
    void scanAccessedLocations(RegisterDescriptor reg, RiscOperators *ops,
                               RegPairs &accessedParts /*out*/, RegPairs &preservedParts /*out*/) const;

    // Given a register descriptor, zero out all the stored parts of the same register (by matching major and minor numbers)
    // if the stored part overlaps with the specified register.
    void clearOverlappingLocations(RegisterDescriptor);

    void assertStorageConditions(const std::string &where, RegisterDescriptor what) const;
};

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterStateGeneric);
#endif

#endif
#endif
