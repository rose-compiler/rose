#ifndef ROSE_Partitioner2_AddressUsageMap_H
#define ROSE_Partitioner2_AddressUsageMap_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>

#include <sawyer/IntervalMap.h>
#include <sawyer/IntervalSet.h>
#include <sawyer/Optional.h>

#include <algorithm>
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
    DataBlock::Ptr dblock_;
public:
    AddressUser(): insn_(NULL) {}                       // needed by std::vector<AddressUser>, but otherwise unused

    /** Constructs new user which is an instruction and its basic block. The instruction must not be the null pointer, but the
     *  basic block may. A null basic block is generally only useful when searching for a particular instruction in an
     *  AddressUsers object. */
    AddressUser(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock): insn_(insn), bblock_(bblock) {
        ASSERT_not_null(insn_);
    }

    /** Constructs a new user which is a data block. The data block must not be the null pointer. */
    AddressUser(const DataBlock::Ptr &dblock): insn_(NULL), dblock_(dblock) {
        ASSERT_not_null(dblock);
    }

    /** Return the non-null pointer to the instruction. */
    SgAsmInstruction* insn() const {
        return insn_;
    }

    /** Change the instruction pointer.  The new pointer cannot be null. */
    void insn(SgAsmInstruction *insn) {
        ASSERT_not_null2(insn_, "this is not an instruction address user");
        ASSERT_not_null(insn);
        insn_ = insn;
    }

    /** Return the non-null pointer to the basic block. */
    BasicBlock::Ptr bblock() const {
        return bblock_;
    }

    /** Change the basic block pointer.  The new pointer cannot be null. */
    void bblock(const BasicBlock::Ptr &bblock) {
        ASSERT_not_null2(insn_, "this is not an instruction address user");
        ASSERT_not_null(bblock);
        bblock_ = bblock;
    }

    /** Return the non-null pointer to the data block. */
    DataBlock::Ptr dblock() const {
        return dblock_;
    }

    /** Change the data block pointer. The new pointer cannot be null. */
    void dblock(const DataBlock::Ptr &dblock) {
        ASSERT_not_null2(dblock_, "this is not a data block address user");
        ASSERT_not_null(dblock);
        dblock_ = dblock;
    }

    /** Determines if this user is a first instruction of a basic block. */
    bool isBlockEntry() const {
        return insn_ && bblock_ && insn_->get_address() == bblock_->address();
    }

    /** Compare two users for equality.  Two pairs are equal if and only if they point to the same instruction and the same
     *  basic block, or they point to the same data block. */
    bool operator==(const AddressUser &other) const {
        return insn_==other.insn_ && bblock_==other.bblock_ && dblock_==other.dblock_;
    }

    /** Compare two users for sorting.  Two users are compared according to their starting addresses.  If two instruction users
     *  have the same starting address then they are necessarily the same instruction (i.e., instruction pointers are equal),
     *  and they necessarily belong to the same basic block (basic block pointers are equal).  However, one or both of the
     *  basic block pointers may be null, which happens when performing a binary search for an instruction when its basic block
     *  is unknown. */
    bool operator<(const AddressUser &other) const {    // hot
        if (insn_!=NULL && other.insn_!=NULL) {
            ASSERT_require((insn_!=other.insn_) ^ (insn_->get_address()==other.insn_->get_address()));
            ASSERT_require(insn_!=other.insn_ || bblock_==NULL || other.bblock_==NULL || bblock_==other.bblock_);
            return insn_->get_address() < other.insn_->get_address();
        } else if (insn_!=NULL || other.insn_!=NULL) {
            return insn_==NULL;                         // instructions come before data blocks
        } else {
            ASSERT_not_null(dblock_);
            ASSERT_not_null(other.dblock_);
            return dblock_->address() < other.dblock_->address();
        }
    }

    /** Print the pair on one line. */
    void print(std::ostream&) const;
};




/** List of virtual address users.
 *
 *  This is a list of users of virtual addresses.  A user is either an instruction/block pair or a data block pointer. The list
 *  is maintained in a sorted order by increasing instruction and/or data block starting address.  The class ensures that all
 *  users in the list have valid pointers and that the list contains no duplicates. */
class AddressUsers {
    std::vector<AddressUser> users_;
public:
    /** Constructs an empty list. */
    AddressUsers() {}

    /** Constructs a list having one instruction user. */
    explicit AddressUsers(SgAsmInstruction *insn, const BasicBlock::Ptr &bb) { insertInstruction(insn, bb); }

    /** Constructs a list having one data block user. */
    explicit AddressUsers(const DataBlock::Ptr &db) { insertDataBlock(db); }

    /** Determines if an instruction exists in the list.
     *
     *  If the instruciton exists then its basic block pointer is returned, otherwise null. */
    BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

    /** Determines if an instruction exists in the list.
     *
     *  If an instruction with the specified starting address exists in the list then the address user information is returned,
     *  otherwise nothing is returned. */
    Sawyer::Optional<AddressUser> instructionExists(rose_addr_t insnStart) const;

    /** Insert an instruction/basic block pair.
     *
     *  Neither the instruction nor the basic block may be null.  The instruction must not already exist in the list.  Returns
     *  a reference to @p this so that the method call can be chained. */
    AddressUsers& insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    /** Erase an instruction user.
     *
     *  Erases the specified instruction from the list.  If the instruction is null or the list does not contain the
     *  instruction then this is a no-op. */
    AddressUsers& eraseInstruction(SgAsmInstruction*);

    /** Determines if a data block exists in the list.
     *
     *  Returns true if the specified data block, or a data block with the same starting address, exists in the list. If one
     *  exists then its pointer is returned, otherwise a null pointer is returned. */
    DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

    /** Determines if a data block exists in the list.
     *
     *  If a data block with the specified starting address exists in the list then the address user information is returned,
     *  otherwise nothing is returned. */
    Sawyer::Optional<AddressUser> dataBlockExists(rose_addr_t dbStart) const;

    /** Insert a new data block.
     *
     *  The data block must not be null and must not already exist in the list.  Returns a reference to @p this so that the
     *  method can be chained. */
    AddressUsers& insertDataBlock(const DataBlock::Ptr&);

    /** Erase a data block user.
     *
     *  Erases the specified data block from the list.  If the data block is null or the list does not contain the data block
     *  then this is a no-op. */
    AddressUsers& eraseDataBlock(const DataBlock::Ptr&);

    /** Return all address users.
     *
     *  Returns all address users as a vector sorted by starting address. */
    const std::vector<AddressUser>& addressUsers() const { return users_; }

    /** Returns all instruction users.
     *
     *  Returns a new list of address users that contains only the instruction users from this list. */
    AddressUsers instructionUsers() const;

    /** Returns all data block users.
     *
     *  Returns a new list of address users that contains only the data block users from this list. */
    AddressUsers dataBlockUsers() const;

    /** Returns all basic blocks.
     *
     *  Returns a list of pointers to distinct basic blocks sorted by starting address.  The return value is not an
     *  AddressUsers because it is more useful to have a list of distinct basic blocks, and because the @ref instructionUsers
     *  method returns the other information already. */
    std::vector<BasicBlock::Ptr> basicBlocks() const;

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
 *  Keeps track of which instructions and data span each virtual address and are represented by the control flow graph. */
class AddressUsageMap {
    typedef Sawyer::Container::IntervalMap<AddressInterval, AddressUsers> Map;
    Map map_;
public:
    /** Determines whether a map is empty.
     *
     *  Returns true if the map contains no instructions or data, false if it contains at least one instruction or at least one
     *  data block. */
    bool isEmpty() const { return map_.isEmpty(); }

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
    Sawyer::Container::IntervalSet<AddressInterval> extent() const;

    /** Addresses not represented.
     *
     *  Returns the set of addresses that are not represented.  The nBits argument is the number of bits in the virtual address
     *  space, usually 32 or 64, and must be between 1 and 64, inclusive; or an interval can be supplied.
     *
     *  @{ */
    Sawyer::Container::IntervalSet<AddressInterval> unusedExtent(size_t nBits) const;
    Sawyer::Container::IntervalSet<AddressInterval> unusedExtent(const AddressInterval&) const;
    /** @} */

    /** Insert an instruction/block pair into the map.
     *
     *  The specified instruction/block pair is added to the map. The instruction must not already be present in the map. */
    void insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

    /** Remove an instruction from the map.
     *
     *  The specified instruction is removed from the map.  If the pointer is null or the instruction does not exist in the
     *  map, then this is a no-op. */
    void eraseInstruction(SgAsmInstruction*);

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

    /** Insert a data block into the map.
     *
     *  The data block must not be a null pointer and must not already exist in the map. */
    void insertDataBlock(const DataBlock::Ptr&);

    /** Remove a data block from the map.
     *
     *  The specified data block is removed from the map.  If the pointer is null or the data block does not exist in the map,
     *  then this is a no-op. */
    void eraseDataBlock(const DataBlock::Ptr&);

    /** Determines whether a data block exists in the map.
     *
     *  If a data block exists in the map, or a data block at the same address exists, then returns a pointer to the existing
     *  data block, otherwise returns null. */
    DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

    /** Determines if an address is the start of a data block.
     *
     *  If the specified address is the starting address of a data block then the address user information is returned,
     *  otherwise nothing is returned. */
    Sawyer::Optional<AddressUser> dataBlockExists(rose_addr_t startOfBlock) const;

    /** Users that span the entire interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user starts at or before the beginning of the interval and ends at or after the end of the interval. */
    AddressUsers spanning(const AddressInterval&) const;

    /** Users that overlap the interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user overlaps with the interval.  That is, at least one byte of the instruction or data block came from the specified
     *  interval of byte addresses. */
    AddressUsers overlapping(const AddressInterval&) const;

    /** Users that are fully contained in the interval.
     *
     *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where each
     *  user is fully contained within the specified interval.  That is, each user starts at or after the beginning of the
     *  interval and ends at or before the end of the interval. */
    AddressUsers containedIn(const AddressInterval&) const;

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
};

} // namespace
} // namespace
} // namespace

#endif
