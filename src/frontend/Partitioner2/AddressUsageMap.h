#ifndef ROSE_Partitioner2_AddressUsageMap_H
#define ROSE_Partitioner2_AddressUsageMap_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/OwnedDataBlock.h>

#include <Sawyer/IntervalMap.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Optional.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include <algorithm>
#include <boost/foreach.hpp>
#include <ostream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressUser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Address usage item.
 *
 *  This struct represents one user for an address interval.  The user can be either an instruction with at least one valid
 *  basic block (since every instruction in the CFG belongs to at least one basic block in the CFG), or a data block.  Address
 *  usage items are usually ordered by their starting address. */
class AddressUser {
    SgAsmInstruction *insn_;
    std::vector<BasicBlock::Ptr> bblocks_;
    OwnedDataBlock odblock_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(insn_);
        s & BOOST_SERIALIZATION_NVP(bblocks_);
        s & BOOST_SERIALIZATION_NVP(odblock_);
    }
#endif

public:
    AddressUser(): insn_(NULL) {}                       // needed by std::vector<AddressUser>, but otherwise unused

    /** Constructs new user which is an instruction and its basic block.
     *
     *  The instruction must not be the null pointer, but the basic block may as long as a null block user is not inserted into
     *  the AUM. A null basic block is generally only useful when searching for a particular instruction in an AddressUsers
     *  object. */
    AddressUser(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock): insn_(insn) {
        ASSERT_not_null(insn_);
        if (bblock)
            bblocks_.push_back(bblock);
    }

    /** Constructs a new user which is a data block. The data block must not be the null pointer. */
    AddressUser(const OwnedDataBlock &odblock): insn_(NULL), odblock_(odblock) {}

    /** Address of user.
     *
     *  Returns the address of the instruction or the address of the data block, depending on which of @ref isBasicBlock or
     *  @ref isDataBlock returns true. */
    rose_addr_t address() const;

    /** Predicate returning true if user is a basic block or instruction. */
    bool isBasicBlock() const { return insn_!=NULL; }

    /** Predicate returning true if user is a data block. */
    bool isDataBlock() const { return odblock_.dataBlock()!=NULL; }

    /** Return the instruction.
     *
     *  Returns the non-null instruction if this is an instruction address owner, otherwise returns the null pointer. */
    SgAsmInstruction* insn() const {
        return insn_;
    }

    /** Returns an arbitrary basic block.
     *
     *  Returns a non-null basic block if this is an instruction owner, otherwise returns the null pointer.  All instructions
     *  in the AUM belong to at least one basic block and therefore have a non-null basic block pointer; non-instruction
     *  address owners don't have basic blocks. */
    BasicBlock::Ptr firstBasicBlock() const {
        return bblocks_.empty() ? BasicBlock::Ptr() : bblocks_[0];
    }

    /** Returns all basic blocks to which this instruction belongs.
     *
     *  Returns a non-empty vector if this is an instruction owner, otherwise returns an empty vector. All instructions in the
     *  AUM belong to at least one basic block and therefore return a non-empty vector; non-instruction owners don't have basic
     *  blocks. */
    const std::vector<BasicBlock::Ptr>& basicBlocks() const {
        return bblocks_;
    }

    /** Add another basic block to the set of basic blocks. */
    void insertBasicBlock(const BasicBlock::Ptr &bblock);

    /** Remove a basic block from the set of basic blocks. */
    void eraseBasicBlock(const BasicBlock::Ptr &bblock);
    
    /** Returns the data block.
     *
     *  Returns a non-null data block if this is a data block address owner, otherwise returns the null pointer. */
    DataBlock::Ptr dataBlock() const {
        return odblock_.dataBlock();
    }

    /** Return the data block ownership information.
     *
     *  Returns ownership information for a data block.  If this owner is not a data block address owner then the return value
     *  will be a default constructed ownership record for which @ref OwnedDataBlock::isValid returns false.
     *
     *  @{ */
    const OwnedDataBlock& dataBlockOwnership() const {
        return odblock_;
    }
    OwnedDataBlock& dataBlockOwnership() {
        return odblock_;
    }
    /** @} */

    /** Determines if this user is a first instruction of a basic block.
     *
     *  If this user is the first instruction of some basic block then a pointer to that block is returned. An instruction can
     *  only be the first instruction of a single basic block, although that instruction may appear internally in other basic
     *  blocks as well. */
    BasicBlock::Ptr isBlockEntry() const;

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
     *  block(s). The basic blocks are not considered in the comparison. Data block ownership records compare only their data
     *  blocks and not ownership lists. */
    bool operator<(const AddressUser&) const;

    /** Print the pair on one line. */
    void print(std::ostream&) const;

    /** Perform logic consistency checks.
     *
     *  Ensures that this object is logically consistent. If assertions are enabled this asserts, otherwise it returns false. */
    bool isConsistent() const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressUsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** List of virtual address users.
 *
 *  This is a list of users of virtual addresses.  A user is either an instruction (and basic block owners) or a data block
 *  (and function and basic block owners). The list is maintained in a sorted order according to instruction or data block
 *  starting address.  The class ensures that all users in the list have valid pointers and that the list contains no
 *  duplicates. */
class AddressUsers {
    std::vector<AddressUser> users_;                    // sorted

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(users_);
    }
#endif

public:
    /** Constructs an empty list. */
    AddressUsers() {}

    /** Constructs a list having one instruction user. */
    explicit AddressUsers(SgAsmInstruction *insn, const BasicBlock::Ptr &bb) { insertInstruction(insn, bb); }

    /** Constructs a list having one data block user. */
    explicit AddressUsers(const OwnedDataBlock &odb) { insertDataBlock(odb); }

#if 0 // [Robb P Matzke 2016-06-30]: insn can be owned by multiple blocks now; use instructionExists(rose_addr_t) instead.
    /** Determines if an instruction exists in the list.
     *
     *  If the instruciton exists then its basic block pointer is returned, otherwise null. */
    BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;
#endif

    /** Determines if an instruction exists in the list.
     *
     *  If an instruction with the specified starting address exists in the list then the address user information is returned,
     *  otherwise nothing is returned. */
    Sawyer::Optional<AddressUser> instructionExists(rose_addr_t insnStart) const;

    /** Determines if a data block exists in the list.
     *
     *  If the specified data block or an equivalent data block exists then its ownership information is returned, otherwise
     *  nothing is returned. */
    Sawyer::Optional<OwnedDataBlock> dataBlockExists(const DataBlock::Ptr&) const;

    /** Determines if a data block exists in the list.
     *
     *  If a data block with the specified starting addres exists in the list then its ownership information is
     *  returned, otherwise nothing is returned. It is possible that multiple data blocks can exist at a particular address, in
     *  which case an arbitrary one is returned. */
    Sawyer::Optional<OwnedDataBlock> dataBlockExists(rose_addr_t dbStart) const;

    /** Find the basic block that starts at this address.
     *
     *  If this user is an instruction, then scan through its basic block owners (usually just one) and return a pointer to any
     *  basic block that starts at this address.  Basic blocks in the AUM have unique starting addresses, thus this function
     *  returns either the pointer to such a block or the null pointer. */
    BasicBlock::Ptr findBasicBlock(rose_addr_t bbVa) const;

    /** Insert an instruction/basic block pair.
     *
     *  Neither the instruction nor the basic block may be null.  If this list already contains the specified instruction, then
     *  the specified basic block is merged into its owner list. */
    void insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    /** Insert a data block.
     *
     *  The specified data block ownership information (which must be valid) is inserted into this list of users.  If this list
     *  already contains an ownership record for the data block (or an equivalent data block), then the specified record is
     *  merged into the existing record.
     *
     *  Returns the data block ownership information, which contains the information specified in the argument merged with the
     *  information that might have been already present in this object. */
    OwnedDataBlock insertDataBlock(const OwnedDataBlock&);

    /** Inser a user.
     *
     *  The specified user information is inserted into this list of users.  If this list already contains an equal record then
     *  the specified record is not inserted. */
    void insert(const AddressUsers&);

    /** Erase an instruction/basic block pair from the list.
     *
     *  Finds the specified instruction in the list and removes the specified basic block owner. If this results in the
     *  instruction not being owned by any blocks then the instruction is removed. */
    void eraseInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    /** Erase a data block user.
     *
     *  Erases the specified data block from the list.  If the data block is null or the list does not contain the data block
     *  then this is a no-op. */
    void eraseDataBlock(const DataBlock::Ptr&);

    /** Selector to select all users.
     *
     *  This selector is the default for methods like @ref AddressUsageMap::overlapping, and causes all users to be selected. */
    static bool selectAllUsers(const AddressUser&) { return true; }

    /** Selector to select instructions and basic blocks.
     *
     *  This selector can be passed as the argument to the @ref select method, or to methods like @ref
     *  AddressUsageMap::overlapping to select only those users that are instructions and basic blocks. */
    static bool selectBasicBlocks(const AddressUser &user) { return user.isBasicBlock(); }

    /** Selector to select data blocks.
     *
     *  This selector can be passed as the argument to the @ref select method, or to methods like @ref
     *  AddressUsageMap::overlapping to select only those users that are data blocks. */
    static bool selectDataBlocks(const AddressUser &user) { return user.isDataBlock(); }

    /** Selects certain users from a list.
     *
     *  Returns a new address users list containing only those users for which the predicate returns true. */
    template<class UserPredicate>
    AddressUsers select(UserPredicate predicate) const {
        AddressUsers retval;
        BOOST_FOREACH (const AddressUser &user, users_) {
            if (predicate(user))
                retval.users_.push_back(user);
        }
        return retval;
    }

    /** Return all address users.
     *
     *  Returns all address users as a vector sorted by starting address. */
    const std::vector<AddressUser>& addressUsers() const { return users_; }

    /** Returns all instruction users.
     *
     *  Returns a new list of address users that contains only the instruction users from this list. */
    AddressUsers instructionUsers() const { return select(selectBasicBlocks); }

    /** Returns all data block users.
     *
     *  Returns a new list of address users that contains only the data block users from this list. */
    AddressUsers dataBlockUsers() const { return select(selectDataBlocks); }

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
    std::vector<BasicBlock::Ptr> instructionOwners() const;

    /** Returns all data blocks.
     *
     *  Returns a list of pointers to distinct data blocks sorted by starting address. */
    std::vector<DataBlock::Ptr> dataBlocks() const;

    /** Number of address users. */
    size_t size() const { return users_.size(); }

    /** Determines whether this address user list is empty.
     *
     *  Returns true if empty, false otherwise. */
    bool isEmpty() const { return users_.empty(); }

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

protected:
    std::vector<AddressUser>::iterator findInstruction(SgAsmInstruction*);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressUsageMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Address usage map.
 *
 *  Keeps track of which instructions and data span each virtual address and are represented by the control flow graph.  The
 *  user never modifies this data structure directly (and can't since the partitioner never returns a non-const reference), but
 *  only modifies it through the partitioner's API. */
class AddressUsageMap {
    typedef Sawyer::Container::IntervalMap<AddressInterval, AddressUsers> Map;
    Map map_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(map_);
    }
#endif

public:
    /** Determines whether a map is empty.
     *
     *  Returns true if the map contains no instructions or data, false if it contains at least one instruction or at least one
     *  data block. */
    bool isEmpty() const { return map_.isEmpty(); }

    /** Reset map to initial empty state. */
    void clear() { map_.clear(); }

    /** Number of addresses represented by the map.
     *
     *  Returns the number of addresses that have at least one user.  This is a constant-time operation. */
    size_t size() const { return map_.size(); }

    /** Minimum and maximum used addresses.
     *
     *  Returns minimum and maximum addresses that exist in this address usage map.  If the map is empty then the returned
     *  interval is empty, containing neither a minimum nor maximum address. */
    AddressInterval hull() const { return map_.hull(); }

    /** Addresses represented.
     *
     *  Returns the set of addresses that are represented. */
    AddressIntervalSet extent() const;

    /** Predicate to determine whether an address is used.
     *
     *  Returns true if the specified address belongs to any instruction, basic block, or data block. This is a O(log N)
     *  operation where N is the number of contiguous intervals in this address usage map.  It may be slightly faster than some
     *  of the other methods since it doesn't need to construct a non-POD return value. */
    bool exists(rose_addr_t va) const { return map_.exists(va); }

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

    /** Determines whether an instruction exists in the map.
     *
     *  If the instruction exists in the map then returns true, otherwise false. */
    bool instructionExists(SgAsmInstruction*) const;

    /** Determines if an address is the start of an instruction.
     *
     *  If the specified address is the starting address of an instruction then the address user information is returned,
     *  otherwise nothing is returned. */
    Sawyer::Optional<AddressUser> instructionExists(rose_addr_t startOfInsn) const;

    /** Determines if an address is the start of a basic block.
     *
     *  If the specified address is the starting address of a basic block then the basic block pointer is returned, otherwise
     *  the null pointer is returned.  A basic block exists only when it has at least one instruction; this is contrary to the
     *  CFG, where a basic block can be represented by a placeholder with no instructions. */
    BasicBlock::Ptr basicBlockExists(rose_addr_t startOfBlock) const;

    /** Determines if an address is the start of a data block.
     *
     *  If the specified data block exists in the map then its ownership information is returned.  Otherwise a
     *  default-constructed data block ownership record is created whose @c isValid method will return false. */
    OwnedDataBlock dataBlockExists(rose_addr_t startOfBlock) const;

    /** Determines whether a data block exists in the map.
     *
     *  If the specified data block or an equivalent data block exists in the map then return its ownership record, otherwise
     *  return a default-constructed ownership record whose isValid method will return false. */
    OwnedDataBlock dataBlockExists(const DataBlock::Ptr&) const;

    /** Find address users that span the entire interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user starts at or before the beginning of the interval and ends at or after the end of the interval. The specified
     *  predicate is used to select which users are inserted into the result and should be a functor that takes an AddressUser
     *  as an argument and returns true to select that user for inclusion in the result.
     *
     * @{ */
    AddressUsers spanning(const AddressInterval &interval) const {
        return spanning(interval, AddressUsers::selectAllUsers);
    }
    
    template<class UserPredicate>
    AddressUsers spanning(const AddressInterval &interval, UserPredicate userPredicate) const {
        AddressUsers retval;
        size_t nIters = 0;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
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
    AddressUsers overlapping(const AddressInterval &interval) const {
        return overlapping(interval, AddressUsers::selectAllUsers);
    }
        
    template<class UserPredicate>
    AddressUsers overlapping(const AddressInterval &interval, UserPredicate userPredicate) const {
        AddressUsers retval;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval))
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
    AddressUsers containedIn(const AddressInterval &interval) const {
        return containedIn(interval, AddressUsers::selectAllUsers);
    }

    template<class UserPredicate>
    AddressUsers containedIn(const AddressInterval &interval, UserPredicate userPredicate) const;
    // FIXME[Robb P. Matzke 2014-08-26]: not implemented yet
    /** @} */
        

    /** Returns the least unmapped address with specified lower limit.
     *
     *  Returns the smallest unmapped address that is greater than or equal to @p startVa.  If no such address exists then
     *  nothing is returned. */
    Sawyer::Optional<rose_addr_t> leastUnmapped(rose_addr_t startVa) const {
        return map_.leastUnmapped(startVa);
    }

    /** Dump the contents of this AUM to a stream.
     *
     *  The output contains one entry per line and the last line is terminated with a linefeed. */
    void print(std::ostream&, const std::string &prefix="") const;

    /** Check invariants.
     *
     *  Aborts if invariants are not satisified. */
    void checkConsistency() const;

private:
    friend class Partitioner;

    // Insert an instruction/block pair into the map.
    void insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    // Insert or merge a data block into the map.  The data block must not be a null pointer. A data pointer is always inserted
    // along with ownership information--which functions and basic blocks own this data block. If the address usage map already
    // has this data block, then the provided ownership information is merged into the existing ownership lists. Returns the
    // data block ownership information, either a copy of the argument or a copy of the updated existing one in the AUM.
    OwnedDataBlock insertDataBlock(const OwnedDataBlock&);

    // Remove an instruction/block pair from the map.  If the instruction exists in the map then the specified block is removed
    // from the instruction's owner list. If this results in an empty owner list then the instruction is also removed.
    // If the pointer is null or the instruction/block pair does not exist in the map, then this is a no-op.
    void eraseInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    // Remove a data block from the map.  The specified data block is removed from the map.  If the pointer is null or the data
    // block does not exist in the map, then this is a no-op.
    void eraseDataBlock(const DataBlock::Ptr&);
};

} // namespace
} // namespace
} // namespace

#endif
