#include "sage3basic.h"
#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/Utility.h>
#include "AsmUnparser_compat.h"
#include <boost/foreach.hpp>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AddressUser::print(std::ostream &out) const {
    if (insn_!=NULL) {
        if (bblock_ != NULL) {
            out <<"{" <<unparseInstructionWithAddress(insn_) <<" in " <<StringUtility::addrToString(bblock_->address()) <<"}";
        } else {
            out <<unparseInstructionWithAddress(insn_);
        }
    } else {
        ASSERT_not_null(dblock_);
        out <<"data at " <<StringUtility::addrToString(dblock_->address());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BasicBlock::Ptr
AddressUsers::instructionExists(SgAsmInstruction *insn) const {
    if (!insn)
        return BasicBlock::Ptr();
    AddressUser needle(insn, BasicBlock::Ptr());      // basic block is not used for binary search
    ASSERT_require(isConsistent());
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb==users_.end() || lb->insn()!=insn)
        return BasicBlock::Ptr();
    ASSERT_not_null(lb->bblock());
    return lb->bblock();
}

DataBlock::Ptr
AddressUsers::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (dblock==NULL)
        return DataBlock::Ptr();
    AddressUser needle(dblock);
    ASSERT_require(isConsistent());
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb==users_.end() || lb->dblock()==NULL || lb->dblock()->address()!=dblock->address())
        return DataBlock::Ptr();
    return lb->dblock();
}

Sawyer::Optional<AddressUser>
AddressUsers::instructionExists(rose_addr_t startVa) const {
    // This could be a binary search, but since instructions seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn() && user.insn()->get_address() == startVa)
            return user;
    }
    return Sawyer::Nothing();
}

Sawyer::Optional<AddressUser>
AddressUsers::dataBlockExists(rose_addr_t startVa) const {
    // This could be a binary search, but since data blocks seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.dblock() && user.dblock()->address() == startVa)
            return user;
    }
    return Sawyer::Nothing();
}

AddressUsers&
AddressUsers::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(insn);
    ASSERT_not_null(bblock);
    ASSERT_forbid(instructionExists(insn));
    ASSERT_require(isConsistent());
    AddressUser user(insn, bblock);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    ASSERT_require2(lb==users_.end() || lb->insn()!=user.insn(), "instruction already exists in the list");
    users_.insert(lb, user);
    ASSERT_require(isConsistent());
    return *this;
}

AddressUsers&
AddressUsers::insertDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_not_null(dblock);
    ASSERT_forbid(dataBlockExists(dblock));
    ASSERT_require(isConsistent());
    AddressUser user(dblock);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    ASSERT_require2(lb==users_.end() || lb->dblock()->address()!=user.dblock()->address(),
                    "data blockalready exists in the list");
    users_.insert(lb, user);
    ASSERT_require(isConsistent());
    return *this;
}

AddressUsers&
AddressUsers::eraseInstruction(SgAsmInstruction *insn) {
    if (insn!=NULL) {
        ASSERT_require(isConsistent());
        AddressUser needle(insn, BasicBlock::Ptr());
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->insn()==insn)
            users_.erase(lb);
    }
    return *this;
}

AddressUsers&
AddressUsers::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock!=NULL) {
        ASSERT_require(isConsistent());
        AddressUser needle(dblock);
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->dblock() && lb->dblock()->address()==dblock->address())
            users_.erase(lb);
    }
    return *this;
}

AddressUsers
AddressUsers::instructionUsers() const {
    AddressUsers insns;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn())
            insns.users_.push_back(user);
    }
    return insns;
}

AddressUsers
AddressUsers::dataBlockUsers() const {
    AddressUsers dblocks;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.dblock())
            dblocks.users_.push_back(user);
    }
    return dblocks;
}

std::vector<BasicBlock::Ptr>
AddressUsers::basicBlocks() const {
    std::vector<BasicBlock::Ptr> bblocks;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn()) {
            BasicBlock::Ptr bblock = user.bblock();
            ASSERT_not_null(bblock);
            std::vector<BasicBlock::Ptr>::iterator lb = std::lower_bound(bblocks.begin(), bblocks.end(), bblock,
                                                                         sortBasicBlocksByAddress);
            if (lb==bblocks.end() || (*lb)->address()!=bblock->address())
                bblocks.insert(lb, bblock);
        }
    }
    return bblocks;
}

AddressUsers
AddressUsers::intersection(const AddressUsers &other) const {
    AddressUsers retval;
    size_t i=0, j=0;
    while (i<size() && j<other.size()) {
        while (i<size() && users_[i] < other.users_[j])
            ++i;
        while (j<other.size() && other.users_[j] < users_[i])
            ++j;
        if (i<size() && j<other.size() && users_[i]==other.users_[j]) {
            retval.users_.push_back(users_[i]);
            ++i;
            ++j;
        }
    }
    ASSERT_require(retval.isConsistent());
    return retval;
}

AddressUsers
AddressUsers::union_(const AddressUsers &other) const {
    AddressUsers retval;
    size_t i=0, j=0;
    while (i<size() && j<other.size()) {
        if (users_[i] < other.users_[j]) {
            retval.users_.push_back(users_[i++]);
        } else if (users_[i] == other.users_[j]) {
            retval.users_.push_back(users_[i++]);
            ++j;
        } else {
            retval.users_.push_back(other.users_[j++]);
        }
    }
    ASSERT_require(i>=size() || j>=other.size());
    while (i<size())
        retval.users_.push_back(users_[i++]);
    while (j<other.size())
        retval.users_.push_back(other.users_[j++]);
    ASSERT_require(retval.isConsistent());
    return retval;
}

bool
AddressUsers::isConsistent() const {
    if (!users_.empty()) {
        std::vector<AddressUser>::const_iterator current = users_.begin();
        std::vector<AddressUser>::const_iterator next = current;
        while (current != users_.end()) {
            if (current->insn()!=NULL) {
                // instruction user
                if (current->dblock()!=NULL) {
                    ASSERT_require2(current->dblock()==NULL, "user cannot have both instruction and data block");
                    return false;
                }
                if (current->bblock()==NULL) {
                    ASSERT_not_null2(current->bblock(), "instruction user must belong to a basic block");
                    return false;
                }
            } else {
                // data block user
                if (current->insn()!=NULL) {
                    ASSERT_require2(current->insn()==NULL, "user cannot have both instruction and data block");
                    return false;
                }
                if (current->bblock()!=NULL) {
                    ASSERT_require2(current->bblock()==NULL, "user cannot have both basic block and data block");
                    return false;
                }
            }
            if (++next != users_.end()) {
                if (!(*current < *next)) {
                    ASSERT_forbid2(*next < *current, "list is not sorted");
                    ASSERT_require2(*current < *next, "list contains a duplicate");
                    return false;
                }
            }
            ++current;
        }
    }
    return true;
}

void
AddressUsers::print(std::ostream &out) const {
    BOOST_FOREACH (const AddressUser &addressUser, users_)
        out <<" " <<addressUser;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUsageMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
AddressUsageMap::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(insn);
    ASSERT_not_null(bblock);
    ASSERT_forbid(instructionExists(insn));
    AddressInterval interval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
    Map adjustment;
    adjustment.insert(interval, AddressUsers(insn, bblock));
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        AddressUsers newUsers = node.value();
        newUsers.insertInstruction(insn, bblock);
        adjustment.insert(interval.intersection(node.key()), newUsers);
    }
    map_.insertMultiple(adjustment);
}

void
AddressUsageMap::insertDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_not_null(dblock);
    ASSERT_forbid(dataBlockExists(dblock));
    AddressInterval interval = AddressInterval::baseSize(dblock->address(), dblock->size());
    Map adjustment;
    adjustment.insert(interval, AddressUsers(dblock));
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        AddressUsers newUsers = node.value();
        newUsers.insertDataBlock(dblock);
        adjustment.insert(interval.intersection(node.key()), newUsers);
    }
    map_.insertMultiple(adjustment);
}

void
AddressUsageMap::eraseInstruction(SgAsmInstruction *insn) {
    if (insn) {
        AddressInterval interval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
        Map adjustment;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
            AddressUsers newUsers = node.value();
            newUsers.eraseInstruction(insn);
            if (!newUsers.isEmpty())
                adjustment.insert(interval.intersection(node.key()), newUsers);
        }
        map_.erase(interval);
        map_.insertMultiple(adjustment);
    }
}

void
AddressUsageMap::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock) {
        AddressInterval interval = AddressInterval::baseSize(dblock->address(), dblock->size());
        Map adjustment;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
            AddressUsers newUsers = node.value();
            newUsers.eraseDataBlock(dblock);
            if (!newUsers.isEmpty())
                adjustment.insert(interval.intersection(node.key()), newUsers);
        }
        map_.erase(interval);
        map_.insertMultiple(adjustment);
    }
}

BasicBlock::Ptr
AddressUsageMap::instructionExists(SgAsmInstruction *insn) const {
    const AddressUsers noUsers;
    return insn ? map_.getOptional(insn->get_address()).orElse(noUsers).instructionExists(insn) : BasicBlock::Ptr();
}

Sawyer::Optional<AddressUser>
AddressUsageMap::instructionExists(rose_addr_t startVa) const {
    const AddressUsers noUsers;
    if (Sawyer::Optional<AddressUser> found = map_.getOptional(startVa).orElse(noUsers).instructionExists(startVa)) {
        if (found->insn()->get_address() == startVa)
            return found;
    }
    return Sawyer::Nothing();
}

BasicBlock::Ptr
AddressUsageMap::basicBlockExists(rose_addr_t startVa) const {
    if (Sawyer::Optional<AddressUser> found = instructionExists(startVa)) {
        if (found->bblock()->address() == startVa)
            return found->bblock();
    }
    return BasicBlock::Ptr();
}

DataBlock::Ptr
AddressUsageMap::dataBlockExists(const DataBlock::Ptr &dblock) const {
    const AddressUsers noUsers;
    return dblock ? map_.getOptional(dblock->address()).orElse(noUsers).dataBlockExists(dblock) : DataBlock::Ptr();
}

Sawyer::Optional<AddressUser>
AddressUsageMap::dataBlockExists(rose_addr_t startVa) const {
    const AddressUsers noUsers;
    if (Sawyer::Optional<AddressUser> found = map_.getOptional(startVa).orElse(noUsers).dataBlockExists(startVa)) {
        if (found->dblock()->address() == startVa)
            return found;
    }
    return Sawyer::Nothing();
}

Sawyer::Container::IntervalSet<AddressInterval>
AddressUsageMap::extent() const {
    Sawyer::Container::IntervalSet<AddressInterval> retval;
    BOOST_FOREACH (const Map::Interval &interval, map_.keys())
        retval.insert(interval);
    return retval;
}

Sawyer::Container::IntervalSet<AddressInterval>
AddressUsageMap::unusedExtent(size_t nBits) const {
    ASSERT_require(nBits>0 && nBits<=8*sizeof(rose_addr_t));
    AddressInterval vaSpace = AddressInterval::hull(0, IntegerOps::genMask<rose_addr_t>(nBits));
    return unusedExtent(vaSpace);
}

Sawyer::Container::IntervalSet<AddressInterval>
AddressUsageMap::unusedExtent(const AddressInterval &vaSpace) const {
    Sawyer::Container::IntervalSet<AddressInterval> retval = extent();
    retval.invert(vaSpace);
    return retval;
}

AddressUsers
AddressUsageMap::spanning(const AddressInterval &interval) const {
    AddressUsers retval;
    size_t nIters = 0;
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        retval = 0==nIters++ ? node.value() : retval.intersection(node.value());
        if (retval.isEmpty())
            break;
    }
    return retval;
}

AddressUsers
AddressUsageMap::overlapping(const AddressInterval &interval) const {
    AddressUsers retval;
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval))
        retval = retval.union_(node.value());
    return retval;
}

void
AddressUsageMap::print(std::ostream &out, const std::string &prefix) const {
    using namespace StringUtility;
    BOOST_FOREACH (const Map::Node &node, map_.nodes())
        out <<prefix <<"[" <<addrToString(node.key().least()) <<"," <<addrToString(node.key().greatest())
            <<"] =" <<node.value() <<"\n";
}

} // namespace
} // namespace
} // namespace
