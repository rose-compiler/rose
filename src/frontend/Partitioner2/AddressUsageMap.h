#ifndef ROSE_Partitioner2_AddressUsageMap_H
#define ROSE_Partitioner2_AddressUsageMap_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/OwnedDataBlock.h>

#include <sawyer/IntervalMap.h>
#include <sawyer/IntervalSet.h>
#include <sawyer/Optional.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Address usage item.
 *
 *  This struct represents one user for an address interval.  The user can be either an instruction with a valid basic block
 *  (since every instruction in the CFG belongs to exactly one basic block in the CFG), or a data block.  Address usage items
 *  are usually ordered by their starting address. */
class AddressUser {
    SgAsmInstruction *insn_;
    BasicBlock::Ptr bblock_;
    OwnedDataBlock odblock_;
public:
    AddressUser(): insn_(NULL) {}                       // needed by std::vector<AddressUser>, but otherwise unused

    /** Constructs new user which is an instruction and its basic block. The instruction must not be the null pointer, but the
     *  basic block may. A null basic block is generally only useful when searching for a particular instruction in an
     *  AddressUsers object. */
    AddressUser(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock): insn_(insn), bblock_(bblock) {
        ASSERT_not_null(insn_);
    }

    /** Constructs a new user which is a data block. The data block must not be the null pointer. */
    AddressUser(const OwnedDataBlock &odblock): insn_(NULL), odblock_(odblock) {}

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

    /** Returns the basic block.
     *
     *  Returns a non-null basic block if this is an instruction owner, otherwise returns the null pointer.  All instructions
     *  in the AUM belong to a basic block and therefore have a non-null basic block pointer; non-instruction address owners
     *  don't have basic blocks. */
    BasicBlock::Ptr basicBlock() const {
        return bblock_;
    }

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

    /** Determines if this user is a first instruction of a basic block. */
    bool isBlockEntry() const {
        return insn_ && bblock_ && insn_->get_address() == bblock_->address();
    }

    /** Compare two users for equality.  Two pairs are equal if and only if they point to the same instruction and the same
     *  basic block, or they point to the same data block. */
    bool operator==(const AddressUser &other) const {
        return insn_==other.insn_ && bblock_==other.bblock_ && odblock_.dataBlock()==other.odblock_.dataBlock();
    }

    /** Compare two users for sorting.  Two users are compared according to their starting addresses.  If two instruction users
     *  have the same starting address then they are necessarily the same instruction (i.e., instruction pointers are equal),
     *  and they necessarily belong to the same basic block (basic block pointers are equal).  However, one or both of the
     *  basic block pointers may be null, which happens when performing a binary search for an instruction when its basic block
     *  is unknown. Data block ownership records compare only their data blocks and not ownership lists. */
    bool operator<(const AddressUser&) const;

    /** Print the pair on one line. */
    void print(std::ostream&) const;
};



/** List of virtual address users.
 *
 *  This is a list of users of virtual addresses.  A user is either an instruction (and basic block owner) or a data block (and
 *  function and basic block owners). The list is maintained in a sorted order.  The class ensures that all users in the list
 *  have valid pointers and that the list contains no duplicates. */
class AddressUsers {
    std::vector<AddressUser> users_;                    // sorted
public:
    /** Constructs an empty list. */
    AddressUsers() {}

    /** Constructs a list having one instruction user. */
    explicit AddressUsers(SgAsmInstruction *insn, const BasicBlock::Ptr &bb) { insertInstruction(insn, bb); }

    /** Constructs a list having one data block user. */
    explicit AddressUsers(const OwnedDataBlock &odb) { insertDataBlock(odb); }


    /** Determines if an instruction exists in the list.
     *
     *  If the instruciton exists then its basic block pointer is returned, otherwise null. */
    BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

    /** Determines if an instruction exists in the list.
     *
     *  If an instruction with the specified starting address exists in the list then the address user information is returned,
     *  otherwise nothing is returned. */
    Sawyer::Optional<AddressUser> instructionExists(rose_addr_t insnStart) const;

    /** Determines if a data block exists in the list.
     *
     *  If the specified data block exists then its ownership information is returned, otherwise nothing is returned. */
    Sawyer::Optional<OwnedDataBlock> dataBlockExists(const DataBlock::Ptr&) const;

    /** Determines if a data block exists in the list.
     *
     *  If a data block with the specified starting address exists in the list then its ownership information is returned,
     *  otherwise nothing is returned. It is possible that multiple data blocks can exist at a particular address, in which
     *  case an arbitrary one is returned. */
    Sawyer::Optional<OwnedDataBlock> dataBlockExists(rose_addr_t dbStart) const;

    /** Insert an instruction/basic block pair.
     *
     *  Neither the instruction nor the basic block may be null.  The instruction must not already exist in the list. */
    void insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    /** Insert a data block.
     *
     *  The specified data block ownership information (which must be valid) is inserted into this list of users.  If this list
     *  already contains an ownership record for the data block, then the specified record is merged into the existing
     *  record. */
    void insertDataBlock(const OwnedDataBlock&);

    /** Inser a user.
     *
     *  The specified user information is inserted into this list of users.  If this list already contains an equal record then
     *  the specified record is not inserted. */
    void insert(const AddressUsers&);

    /** Erase an instruction.
     *
     *  Erases the specified instruction from the list.  If the instruction is null or the list does not contain the
     *  instruction then this is a no-op. */
    void eraseInstruction(SgAsmInstruction*);

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
    std::vector<BasicBlock::Ptr> basicBlocks() const;

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
    bool operator==(const AddressUsers &other) const {
        return users_.size()==other.users_.size() && std::equal(users_.begin(), users_.end(), other.users_.begin());
    }

    /** Prints pairs space separated on a single line. */
    void print(std::ostream&) const;

protected:
    /** Checks whether the list satisfies all invariants.  This is used in pre- and post-conditions. */
    bool isConsistent() const;
};



/** Address usage map.
 *
 *  Keeps track of which instructions and data span each virtual address and are represented by the control flow graph.  The
 *  user never modifies this data structure directly (and can't since the partitioner never returns a non-const reference), but
 *  only modifies it through the partitioner's API. */
class AddressUsageMap {
    typedef Sawyer::Container::IntervalMap<AddressInterval, AddressUsers> Map;
    Map map_;
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

    /** Determines whether an instruction exists in the map.
     *
     *  If the instruction exists in the map then a pointer to its basic block is returned, otherwise a null pointer is
     *  returned. */
    BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

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
     *  If the specified data block exists in the map then return its ownership record, otherwise return a default-constructed
     *  ownership record whose isValid method will return false. */
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
private:
    friend class Partitioner;

    // Insert an instruction/block pair into the map.  The instruction must not already be present in the map.
    void insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    // Insert a data block into the map.  The data block must not be a null pointer and must not already exist in the map. A
    // data pointer is always inserted along with ownership information--which functions and basic blocks own this data block.
    void insertDataBlock(const OwnedDataBlock&);

    // Remove an instruction from the map. The specified instruction is removed from the map.  If the pointer is null or the
    // instruction does not exist in the map, then this is a no-op.
    void eraseInstruction(SgAsmInstruction*);

    // Remove a data block from the map.  The specified data block is removed from the map.  If the pointer is null or the data
    // block does not exist in the map, then this is a no-op.
    void eraseDataBlock(const DataBlock::Ptr&);

};

} // namespace
} // namespace
} // namespace

#endif
