#ifndef ROSE_BinaryAnalysis_Partitioner2_AddressUsageMap_H
#define ROSE_BinaryAnalysis_Partitioner2_AddressUsageMap_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/BinaryAnalysis/AddressIntervalSet.h>

#include <Sawyer/IntervalMap.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Optional.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

#include <algorithm>
#include <ostream>
#include <string>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressUser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Address usage item.
 *
 *  This struct represents one user for an address interval.  The user can be either an instruction with at least one valid
 *  basic block (since every instruction in the CFG belongs to at least one basic block in the CFG), or a data block, but not
 *  both. */
class AddressUser {
    SgAsmInstruction *insn_;
    std::vector<BasicBlockPtr> bblocks_;                // sorted and unique
    DataBlockPtr dblock_;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;
    template<class S> void serialize(S&, const unsigned version);
#endif

public:
    /** Default constructed user is empty. */
    AddressUser();

    /** Constructs new user which is an instruction and its basic block.
     *
     *  The instruction must not be the null pointer, but the basic block may as long as a null block user is not inserted into
     *  the AUM. A null basic block is generally only useful when searching for a particular instruction in an AddressUsers
     *  object. */
    AddressUser(SgAsmInstruction*, const BasicBlockPtr&);

    /** Constructs a new user which is a data block. The data block must not be the null pointer. */
    explicit AddressUser(const DataBlockPtr&);

    ~AddressUser();

    /** Address of user.
     *
     *  Returns the address of the instruction or the address of the data block, depending on which of @ref isBasicBlock or
     *  @ref isDataBlock returns true. */
    rose_addr_t address() const;

    /** Predicate returning true if user is a basic block or instruction. */
    bool isBasicBlock() const;

    /** Predicate returning true if user is a data block. */
    bool isDataBlock() const;

    /** True if this object was default constructed.
     *
     *  Returns true if this @ref AddressUser doesn't point to anything. */
    bool isEmpty() const;

    /** Return the instruction.
     *
     *  Returns the non-null instruction if this is an instruction address owner, otherwise returns the null pointer. */
    SgAsmInstruction* insn() const;

    /** Returns an arbitrary basic block.
     *
     *  Returns a non-null basic block if this object points to an instruction, otherwise returns the null pointer.  All
     *  instructions in the AUM belong to at least one basic block and therefore have a non-null basic block pointer. */
    BasicBlockPtr firstBasicBlock() const;

    /** Returns all basic blocks to which this instruction belongs.
     *
     *  Returns a non-empty vector if this is an instruction owner, otherwise returns an empty vector. All instructions in the
     *  AUM belong to at least one basic block and therefore return a non-empty vector; non-instruction owners don't have basic
     *  blocks. */
    const std::vector<BasicBlockPtr>& basicBlocks() const;

    /** Add another basic block to the set of basic blocks. */
    void insertBasicBlock(const BasicBlockPtr &bblock);

    /** Remove a basic block from the set of basic blocks. */
    void eraseBasicBlock(const BasicBlockPtr &bblock);
    
    /** Returns the data block.
     *
     *  Returns a non-null data block if this is a data block address owner, otherwise returns the null pointer. */
    DataBlockPtr dataBlock() const;

    /** Determines if this user is a first instruction of a basic block.
     *
     *  If this user is the first instruction of some basic block then a pointer to that block is returned. An instruction can
     *  only be the first instruction of a single basic block, although that instruction may appear internally in other basic
     *  blocks as well. */
    BasicBlockPtr isBlockEntry() const;

    /** Compare two users for equality.
     *
     *  Two users are equal if and only if they point to the same instruction (or both null) having the same basic block owners
     *  and they point to the same data block (or both null). This is used by the @ref Sawyer::Container::IntervalMap to decide
     *  whether it's possible to join adjacent values. */
    bool operator==(const AddressUser &other) const;

    /** Compare two users for sorting.
     *
     *  Two users are compared according to their starting addresses.  If two instruction users have the same starting address
     *  then they are necessarily the same instruction (i.e., instruction pointers are equal) and thus belong to the same basic
     *  block(s). The basic blocks are not considered in the comparison. Data block ownership records compare based on their
     *  keys, which is their starting address and size. */
    bool operator<(const AddressUser&) const;

    /** Print the pair on one line. */
    void print(std::ostream&) const;

    /** Perform logic consistency checks.
     *
     *  Ensures that this object is logically consistent. If assertions are enabled this asserts, otherwise it returns false. */
    bool isConsistent() const;

    /** Return true/false in Boolean context.
     *
     *  Returns false if this AddressUser is empty (default constructed) and true otherwise. */
    operator bool() const {
        return !isEmpty();
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressUsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** List of virtual address users.
 *
 *  This is a list of users of virtual addresses.  A user is either an instruction (and basic block owners) or a data block The
 *  list is maintained in a sorted order according to instruction or data block comparators.  The class ensures that all users
 *  in the list have valid pointers and that the list contains no duplicates. */
class AddressUsers {
    std::vector<AddressUser> users_;                    // sorted

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;
    template<class S> void serialize(S&, const unsigned version);
#endif

public:
    /** Constructs an empty list. */
    AddressUsers();
    ~AddressUsers();

    /** Constructs a list having one instruction user. */
    explicit AddressUsers(SgAsmInstruction *insn, const BasicBlockPtr&);

    /** Constructs a list having one data block user. */
    explicit AddressUsers(const DataBlockPtr&);

    /** Determines whether the specified instruction or an equivalent exists.
     *
     *  Returns a non-null pointer to the instruction contained in this object if the specified instruction or an equivalent
     *  can be found, otherwise returns a null pointer. The search can be done by @ref SgAsmInstruction pointer or a starting
     *  address.
     *
     * @{ */
    SgAsmInstruction* instructionExists(SgAsmInstruction*) const;
    SgAsmInstruction* instructionExists(rose_addr_t va) const;
    /** @} */

    /** Determines whether the specified basic block or an equivalent exists.
     *
     *  Returns a non-null pointer to the basic block contained in this object if the specified basic block or an equivalent
     *  can be found, otherwise returns a null pointer. The search can be done by @ref BasicBlock object or a starting address.
     *  The basic block is found only if its first instruction is present.
     *
     * @{ */
    BasicBlockPtr basicBlockExists(const BasicBlockPtr&) const;
    BasicBlockPtr basicBlockExists(rose_addr_t va) const;
    /** @} */

    /** Determines whether the specified data block or an equivalent exists.
     *
     *  Returns a non-null pointer to the data block contained in this object if the specified data block or an equivalent can
     *  be found, otherwise returns a null pointer. The search can be done by @ref DataBlock object or a starting address and
     *  size.
     *
     * @{ */
    DataBlockPtr dataBlockExists(const DataBlockPtr&) const;
    DataBlockPtr dataBlockExists(rose_addr_t va, rose_addr_t size) const;
    /** @} */

    /** Find an AddressUser record for the specified instruction, or equivalent.
     *
     *  If the specified instruction or an equivalent instruction exists in this object, then its record is returned, otherwise
     *  an empty record is returned. The instruction can be indicated by either an @ref SgAsmInstruction object or a starting
     *  address.
     *
     * @{ */
    AddressUser findInstruction(SgAsmInstruction*) const;
    AddressUser findInstruction(rose_addr_t va) const;
    /** @} */

    /** Find an AddressUser record for the specified basic block, or equivalent.
     *
     *  If the specified basic block or an equivalent basic block (based on starting address) is present in this list, then
     *  return its record, otherwise return an empty record. This looks up the basic block by looking for its first
     *  instruction, therefore returns a record only if the first instruction is present.  The basic block can be specified by
     *  either a @ref BasicBlock object or a starting address.
     *
     * @{ */
    AddressUser findBasicBlock(const BasicBlockPtr&) const;
    AddressUser findBasicBlock(rose_addr_t va) const;
    /** @} */

    /** Find an AddressUser record for the specified data block, or equivalent.
     *
     *  If the specified data block or an equivalent data block exists in this object, then its record is returned, otherwise
     *  an empty record is returned. The data block can be specified by either a @ref DataBlock object or a starting address
     *  and size.
     *
     * @{ */
    AddressUser findDataBlock(const DataBlockPtr&) const;
    AddressUser findDataBlock(rose_addr_t va, rose_addr_t size) const;
    /** @} */

    /** Insert an instruction/basic block pair.
     *
     *  Neither the instruction nor the basic block may be null.  If this list already contains the specified instruction, then
     *  the specified basic block is merged into its owner list and a reference to that user is returned. */
    AddressUser insertInstruction(SgAsmInstruction*, const BasicBlockPtr&);

    /** Insert a data block.
     *
     *  The specified data block is inserted into this list of users unless it or an equivalent data block is already present. Returns
     *  the record describing the data block. */
    AddressUser insertDataBlock(const DataBlockPtr&);

    /** Insert one set of address users into another. */
    void insert(const AddressUsers&);

    /** Erase an instruction/basic block pair from this list.
     *
     *  Finds the specified instruction or equivalent in the list and removes the specified basic block owner. If this results
     *  in the instruction not being owned by any blocks then the instruction is removed. Returns the affected instruction. */
    SgAsmInstruction* eraseInstruction(SgAsmInstruction*, const BasicBlockPtr&);

    /** Erase a data block from this list.
     *
     *  Erases the specified data block or equivalent from this list.  If the data block is null or the list does not contain
     *  the data block then this is a no-op. Returns the erased data block.*/
    DataBlockPtr eraseDataBlock(const DataBlockPtr&);

    /** Selector to select all users.
     *
     *  This selector is the default for methods like @ref AddressUsageMap::overlapping, and causes all users to be selected. */
    static bool selectAllUsers(const AddressUser&);

    /** Selector to select instructions and basic blocks.
     *
     *  This selector can be passed as the argument to the @ref select method, or to methods like @ref
     *  AddressUsageMap::overlapping to select only those users that are instructions and basic blocks. */
    static bool selectBasicBlocks(const AddressUser&);

    /** Selector to select data blocks.
     *
     *  This selector can be passed as the argument to the @ref select method, or to methods like @ref
     *  AddressUsageMap::overlapping to select only those users that are data blocks. */
    static bool selectDataBlocks(const AddressUser&);

    /** Selects certain users from a list.
     *
     *  Returns a new address users list containing only those users for which the predicate returns true. */
    template<class UserPredicate>
    AddressUsers select(UserPredicate predicate) const {
        AddressUsers retval;
        for (const AddressUser &user: users_) {
            if (predicate(user))
                retval.users_.push_back(user);
        }
        return retval;
    }

    /** Return all address users.
     *
     *  Returns all address users as a vector sorted by starting address. */
    const std::vector<AddressUser>& addressUsers() const;

    /** Returns all instruction users.
     *
     *  Returns a new list of address users that contains only the instruction users from this list. */
    AddressUsers instructionUsers() const;

    /** Returns all data block users.
     *
     *  Returns a new list of address users that contains only the data block users from this list. */
    AddressUsers dataBlockUsers() const;

    /** Returns all instructions.
     *
     *  Returns a list of distinct instructions sorted by starting address.  The return value is not an AddressUsers because it
     *  is more useful to have a list of distinct instructions, and because the @ref instructionUsers method returns the other
     *  information already. */
    std::vector<SgAsmInstruction*> instructions() const;

    /** Returns all basic blocks.
     *
     *  Returns a list of pointers to distinct basic blocks sorted by starting address.  The return value is not an
     *  AddressUsers because it is more useful to have a list of distinct basic blocks, and because the @ref instructionUsers
     *  method returns the other information already. */
    std::vector<BasicBlockPtr> instructionOwners() const;

    /** Returns all data blocks.
     *
     *  Returns a list of pointers to distinct data blocks sorted by starting address. */
    std::vector<DataBlockPtr> dataBlocks() const;

    /** Number of address users. */
    size_t size() const;

    /** Determines whether this address user list is empty.
     *
     *  Returns true if empty, false otherwise. */
    bool isEmpty() const;

    /** Computes the intersection of this list with another. */
    AddressUsers intersection(const AddressUsers&) const;

    /** Computes the union of this list with another. */
    AddressUsers union_(const AddressUsers&) const;

    /** True if two lists are equal. */
    bool operator==(const AddressUsers &other) const;

    /** Prints pairs space separated on a single line. */
    void print(std::ostream&) const;

    /** Check logical consistency.
     *
     *  Ensures that this object is logically consistent. If assertions are enabled this asserts, otherwise it returns false. */
    bool isConsistent() const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressUsageMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Address usage map.
 *
 *  Keeps track of which instructions and data span each virtual address. The AUM that's part of the partitioner is never modified
 *  directly by the user, and represents the instructions and basic blocks that are in the control flow graph as well as any data
 *  blocks they own. */
class AddressUsageMap {
    using Map = Sawyer::Container::IntervalMap<AddressInterval, AddressUsers>;
    Map map_;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;
    template<class S> void serialize(S&, const unsigned version);
#endif

public:
    AddressUsageMap();
    ~AddressUsageMap();

public:
    /** Determines whether a map is empty.
     *
     *  Returns true if the map contains no instructions or data, false if it contains at least one instruction or at least one
     *  data block. */
    bool isEmpty() const;

    /** Reset map to initial empty state. */
    void clear();

    /** Number of addresses represented by the map.
     *
     *  Returns the number of addresses that have at least one user.  This is a constant-time operation. */
    size_t size() const;

    /** Minimum and maximum used addresses.
     *
     *  Returns minimum and maximum addresses that exist in this address usage map.  If the map is empty then the returned
     *  interval is empty, containing neither a minimum nor maximum address. */
    AddressInterval hull() const;

    /** Addresses represented.
     *
     *  Returns the set of addresses that are represented. */
    AddressIntervalSet extent() const;

    /** Predicate to determine whether an address is used.
     *
     *  Returns true if the specified address belongs to any instruction, basic block, or data block. This is a O(log N)
     *  operation where N is the number of contiguous intervals in this address usage map.  It may be slightly faster than some
     *  of the other methods since it doesn't need to construct a non-POD return value. */
    bool exists(rose_addr_t) const;

    /** Predicate to determine whether any of the specified addresses are used.
     *
     *  Returns true if any address in the specified set or interval belongs to any instruction, basic block, or data
     *  block. This may be slightly faster than some of the other predicates because it doesn't need to construct a non-POD
     *  return value.
     *
     *  @{ */
    bool anyExists(const AddressInterval&) const;
    bool anyExists(const AddressIntervalSet&) const;
    /** @} */

    /** Addresses not represented.
     *
     *  Returns the set of addresses that are not represented.  The nBits argument is the number of bits in the virtual address
     *  space, usually 32 or 64, and must be between 1 and 64, inclusive. Alternatively, an interval or interval set can be
     *  supplied to limit the return value.
     *
     *  @{ */
    AddressIntervalSet unusedExtent(size_t nBits) const;
    AddressIntervalSet unusedExtent(const AddressInterval&) const;
    AddressIntervalSet unusedExtent(const AddressIntervalSet&) const;
    /** @} */

    /** Next unused address interval.
     *
     *  Returns the next address interval that begins at or after the specified address and which does not correspond to any
     *  instruction, basic block, data block, or function. The largest such interval is returned, but it will not contain any
     *  values less than @p minVa. Returns an empty interval if no such interval exists. */
    AddressInterval nextUnused(rose_addr_t minVa) const;

    /** Determines whether the specified instruction or an equivalent exists.
     *
     *  Returns the instruction or equivalent that exists, otherwise null.
     *
     * @{ */
    SgAsmInstruction* instructionExists(SgAsmInstruction*) const;
    SgAsmInstruction* instructionExists(rose_addr_t va) const;
    /** @} */

    /** Determine if a basic block exists.
     *
     *  If the specified basic block's starting address exists in this AUM and is the starting instruction of this basic block
     *  or an equivalent then returns a pointer to the existing basic block, otherwise the null pointer is returned.  A basic
     *  block exists only when it has at least one instruction; this is contrary to the CFG, where a basic block can be
     *  represented by a placeholder with no instructions.
     *
     * @{ */
    BasicBlockPtr basicBlockExists(const BasicBlockPtr&) const;
    BasicBlockPtr basicBlockExists(rose_addr_t startOfBlock) const;
    /** @} */

    /** Determines if a data block exists.
     *
     *  If the specified data block or an equivalent exists in the map then a pointer to the exsiting data block is returned,
     *  otherwise a null pointer is returned.
     *
     * @{ */
    DataBlockPtr dataBlockExists(const DataBlockPtr&) const;
    DataBlockPtr dataBlockExists(rose_addr_t va, rose_addr_t size) const;
    /** @} */

    /** Find an AddressUser record for the specified instruction, or equivalent.
     *
     *  If the specified instruction or an equivalent instruction exists in this object, then its record is returned, otherwise
     *  an empty record is returned. The instruction can be indicated by either an @ref SgAsmInstruction object or a starting
     *  address.
     *
     * @{ */
    AddressUser findInstruction(SgAsmInstruction*) const;
    AddressUser findInstruction(rose_addr_t va) const;
    /** @} */

    /** Find an AddressUser record for the specified basic block, or equivalent.
     *
     *  If the specified basic block or an equivalent basic block (based on starting address) is present in this list, then
     *  return its record, otherwise return an empty record. This looks up the basic block by looking for its first
     *  instruction, therefore returns a record only if the first instruction is present.  The basic block can be specified by
     *  either a @ref BasicBlock object or a starting address.
     *
     * @{ */
    AddressUser findBasicBlock(const BasicBlockPtr&) const;
    AddressUser findBasicBlock(rose_addr_t va) const;
    /** @} */

    /** Find an AddressUser record for the specified data block, or equivalent.
     *
     *  If the specified data block or an equivalent data block exists in this object, then its record is returned, otherwise
     *  an empty record is returned. The data block can be specified by either a @ref DataBlock object or a starting address
     *  and size.
     *
     * @{ */
    AddressUser findDataBlock(const DataBlockPtr&) const;
    AddressUser findDataBlock(rose_addr_t va, rose_addr_t size) const;
    /** @} */

    /** Insert the instruction along with an owning basic block.
     *
     *  Inserts the specified instruction and its owning basic block if the information is not already present in this AUM.
     *  Returns the relavent address user information since this method might substitute existing equivalent instruction and
     *  owner. */
    AddressUser insertInstruction(SgAsmInstruction*, const BasicBlockPtr&);

    /** Insert the data block.
     *
     *  Inserts the specified data block if an equivalent data block is not already present. Returns the relevant address user
     *  information since this method might substitute an existing equivalent data block. */
    AddressUser insertDataBlock(const DataBlockPtr&);

    /** Remove the specified instruction/basic block pair.
     *
     *  If the specified instruction or equivalent is found in this AUM then the specified basic block is removed as one of its
     *  owners. If this leaves the instruction with no owning basic blocks, then the instruction itself is also
     *  removed. Returns the affected instruction. */
    SgAsmInstruction* eraseInstruction(SgAsmInstruction*, const BasicBlockPtr&);

    /** Remove the specified data block.
     *
     *  Removes the specified data block or an equivalent from this AUM. Returns the data block that was erased. */
    DataBlockPtr eraseDataBlock(const DataBlockPtr&);

    /** Find address users that span the entire interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user starts at or before the beginning of the interval and ends at or after the end of the interval. The specified
     *  predicate is used to select which users are inserted into the result and should be a functor that takes an AddressUser
     *  as an argument and returns true to select that user for inclusion in the result.
     *
     * @{ */
    AddressUsers spanning(const AddressInterval&) const;

    template<class UserPredicate>
    AddressUsers spanning(const AddressInterval &interval, UserPredicate userPredicate) const {
        AddressUsers retval;
        size_t nIters = 0;
        for (const Map::Node &node: map_.findAll(interval)) {
            AddressUsers users = node.value().select(userPredicate);
            retval = 0==nIters++ ? users : retval.intersection(users);
            if (retval.isEmpty())
                break;
        }
        return retval;
    }
    /** @} */

    /** Users that overlap the interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user overlaps with the interval.  That is, at least one byte of the instruction or data block came from the specified
     *  interval of byte addresses. The specified predicate is used to select which users are inserted into the result and
     *  should be a functor that takes an AddressUser as an argument and returns true to select that user for inclusion in the
     *  result.
     *
     * @{ */
    AddressUsers overlapping(const AddressInterval&) const;

    template<class UserPredicate>
    AddressUsers overlapping(const AddressInterval &interval, UserPredicate userPredicate) const {
        AddressUsers retval;
        for (const Map::Node &node: map_.findAll(interval))
            retval.insert(node.value().select(userPredicate));
        return retval;
    }
    /** @} */

    /** Users that are fully contained in the interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user is fully contained within the specified interval.  That is, each user starts at or after the beginning of the
     *  interval and ends at or before the end of the interval. The specified predicate is used to select which users are
     *  inserted into the result and should be a functor that takes an AddressUser as an argument and returns true to select
     *  that user for inclusion in the result.
     *
     * @{ */
    // FIXME[Robb P. Matzke 2014-08-26]: not implemented yet
    AddressUsers containedIn(const AddressInterval&) const;

    //template<class UserPredicate>
    //AddressUsers containedIn(const AddressInterval &interval, UserPredicate userPredicate) const {...}
    /** @} */
        

    /** Returns the least unmapped address with specified lower limit.
     *
     *  Returns the smallest unmapped address that is greater than or equal to @p startVa.  If no such address exists then
     *  nothing is returned. */
    Sawyer::Optional<rose_addr_t> leastUnmapped(rose_addr_t startVa) const;

    /** Dump the contents of this AUM to a stream.
     *
     *  The output contains one entry per line and the last line is terminated with a linefeed. */
    void print(std::ostream&, const std::string &prefix="") const;

    /** Check invariants.
     *
     *  Aborts if invariants are not satisified. */
    void checkConsistency() const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
