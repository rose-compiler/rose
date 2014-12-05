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

bool
AddressUser::operator<(const AddressUser &other) const {
    if (insn_!=NULL && other.insn_!=NULL) {
        ASSERT_require((insn_!=other.insn_) ^ (insn_->get_address()==other.insn_->get_address()));
        ASSERT_require(insn_!=other.insn_ || bblock_==NULL || other.bblock_==NULL || bblock_==other.bblock_);
        return insn_->get_address() < other.insn_->get_address();
    } else if (insn_!=NULL || other.insn_!=NULL) {
        return insn_==NULL;                         // instructions come before data blocks
    } else {
        ASSERT_not_null(odblock_.dataBlock());
        ASSERT_not_null(other.odblock_.dataBlock());
        return sortDataBlocks(odblock_.dataBlock(), other.odblock_.dataBlock());
    }
}

void
AddressUser::print(std::ostream &out) const {
    if (insn_!=NULL) {
        if (bblock_ != NULL) {
            out <<"{B-" <<StringUtility::addrToString(bblock_->address()) <<" ";
        } else {
            out <<"{B-none       ";
        }
        out <<unparseInstructionWithAddress(insn_) <<"}";
    } else {
        ASSERT_require(odblock_.isValid());
        out <<"{D-" <<StringUtility::addrToString(odblock_.dataBlock()->address())
            <<"+" <<odblock_.dataBlock()->size()
            <<" " <<StringUtility::plural(odblock_.nOwners(), "owners") <<"}";
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
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb==users_.end() || lb->insn()!=insn)
        return BasicBlock::Ptr();
    ASSERT_not_null(lb->basicBlock());
    return lb->basicBlock();
}

Sawyer::Optional<OwnedDataBlock>
AddressUsers::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (dblock==NULL)
        return Sawyer::Nothing();
    AddressUser needle = AddressUser(OwnedDataBlock(dblock));
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb==users_.end() || lb->dataBlock()!=dblock)
        return Sawyer::Nothing();
    ASSERT_require(lb->dataBlockOwnership().isValid());
    return lb->dataBlockOwnership();
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

Sawyer::Optional<OwnedDataBlock>
AddressUsers::dataBlockExists(rose_addr_t startVa) const {
    // This could be a binary search, but since data blocks seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.dataBlock() && user.dataBlock()->address() == startVa) {
            ASSERT_require(user.dataBlockOwnership().isValid());
            return user.dataBlockOwnership();
        }
    }
    return Sawyer::Nothing();
}

void
AddressUsers::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(insn);
    ASSERT_not_null(bblock);
    ASSERT_forbid(instructionExists(insn));
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
    AddressUser user(insn, bblock);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    ASSERT_require2(lb==users_.end() || lb->insn()!=user.insn(), "instruction already exists in the list");
    users_.insert(lb, user);
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
}

void
AddressUsers::insertDataBlock(const OwnedDataBlock &odb) {
    ASSERT_require(odb.isValid());
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
    AddressUser user(odb);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    if (lb==users_.end() || lb->dataBlock()!=odb.dataBlock()) {
        users_.insert(lb, odb);
    } else {
        // merge new ownership list into existing ownership list
        ASSERT_require(lb->dataBlock()==odb.dataBlock());
        BOOST_FOREACH (const Function::Ptr &function, odb.owningFunctions())
            lb->dataBlockOwnership().insertOwner(function);
        BOOST_FOREACH (const BasicBlock::Ptr &bblock, odb.owningBasicBlocks())
            lb->dataBlockOwnership().insertOwner(bblock);
    }
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
}

void
AddressUsers::eraseInstruction(SgAsmInstruction *insn) {
    if (insn!=NULL) {
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
        ASSERT_require(isConsistent());
#endif
        AddressUser needle(insn, BasicBlock::Ptr());
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->insn()==insn)
            users_.erase(lb);
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
        ASSERT_require(isConsistent());
#endif
    }
}

void
AddressUsers::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock!=NULL) {
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
        ASSERT_require(isConsistent());
#endif
        AddressUser needle = AddressUser(OwnedDataBlock(dblock));
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->dataBlock()==dblock)
            users_.erase(lb);
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
        ASSERT_require(isConsistent());
#endif
    }
}

std::vector<SgAsmInstruction*>
AddressUsers::instructions() const {
    std::vector<SgAsmInstruction*> insns;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (SgAsmInstruction *insn = user.insn())
            insns.push_back(insn);
    }
    ASSERT_require(isSorted(insns, sortInstructionsByAddress, true));
    return insns;
}

std::vector<BasicBlock::Ptr>
AddressUsers::basicBlocks() const {
    std::vector<BasicBlock::Ptr> bblocks;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn()) {
            BasicBlock::Ptr bblock = user.basicBlock();
            ASSERT_not_null(bblock);
            insertUnique(bblocks, bblock, sortBasicBlocksByAddress);
        }
    }
    return bblocks;
}

std::vector<DataBlock::Ptr>
AddressUsers::dataBlocks() const {
    std::vector<DataBlock::Ptr> dblocks;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (DataBlock::Ptr dblock = user.dataBlock())
            insertUnique(dblocks, dblock, sortDataBlocks);
    }
    return dblocks;
}

AddressUsers
AddressUsers::intersection(const AddressUsers &other) const {
    AddressUsers retval;
    size_t i=0, j=0;
    while (i<size() && j<other.size()) {
        while (i<size() && users_[i] < other.users_[j])
            ++i;
        if (i>=size())
            break; 
        while (j<other.size() && other.users_[j] < users_[i])
            ++j;
        if (j<other.size() && users_[i]==other.users_[j]) {
            retval.users_.push_back(users_[i]);
            ++i;
            ++j;
        }
    }
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(retval.isConsistent());
#endif
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
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(retval.isConsistent());
#endif
    return retval;
}

void
AddressUsers::insert(const AddressUsers &other) {
    if (users_.empty()) {
        users_ = other.users_;
    } else {
        BOOST_FOREACH (const AddressUser &user, other.users_) {
            std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
            if (lb==users_.end() || !(*lb==user))
                users_.insert(lb, user);
        }
    }
#ifdef ROSE_PARTITIONER_EXPENSIVE_CHECKS
    ASSERT_require(isConsistent());
#endif
}

bool
AddressUsers::isConsistent() const {
    if (!users_.empty()) {
        std::vector<AddressUser>::const_iterator current = users_.begin();
        std::vector<AddressUser>::const_iterator next = current;
        while (current != users_.end()) {
            if (current->insn()!=NULL) {
                // instruction user
                if (current->dataBlock()!=NULL) {
                    ASSERT_require2(current->dataBlock()==NULL, "user cannot have both instruction and data block");
                    return false;
                }
                if (current->basicBlock()==NULL) {
                    ASSERT_not_null2(current->basicBlock(), "instruction user must belong to a basic block");
                    return false;
                }
                if (++next != users_.end()) {
                    if (!(*current < *next)) {
                        ASSERT_forbid2(*next < *current, "list is not sorted");
                        ASSERT_require2(*current < *next, "list contains a duplicate");
                        return false;
                    }
                }
            } else {
                // data block user
                if (current->insn()!=NULL) {
                    ASSERT_require2(current->insn()==NULL, "user cannot have both instruction and data block");
                    return false;
                }
                if (current->basicBlock()!=NULL) {
                    ASSERT_require2(current->basicBlock()==NULL, "user cannot have both basic block and data block");
                    return false;
                }
                if (++next != users_.end()) {
                    if (*next < *current) {
                        ASSERT_forbid2(*next < *current, "list is not sorted");
                        // Multiple data blocks can can exist at the same address and have the same size, but we can't
                        // allow the exact same data block (by pointers) to appear multiple times in the list.
                        ASSERT_forbid2(current->dataBlock()==next->dataBlock(), "list contains a duplicate");
                        return false;
                    }
                }
            }
            ++current;
        }
    }
    return true;
}

void
AddressUsers::print(std::ostream &out) const {
    size_t nItems = 0;
    BOOST_FOREACH (const AddressUser &addressUser, users_)
        out <<(1==++nItems?"":", ") <<addressUser;
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
AddressUsageMap::insertDataBlock(const OwnedDataBlock &odb) {
    ASSERT_require(odb.isValid());
    ASSERT_forbid2(dataBlockExists(odb.dataBlock()).isValid(), "data block must not already exist in the AUM");
    AddressInterval interval = AddressInterval::baseSize(odb.dataBlock()->address(), odb.dataBlock()->size());
    Map adjustment;
    adjustment.insert(interval, AddressUsers(odb));
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        AddressUsers newUsers = node.value();
        newUsers.insertDataBlock(odb);
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
        if (found->basicBlock()->address() == startVa)
            return found->basicBlock();
    }
    return BasicBlock::Ptr();
}

OwnedDataBlock
AddressUsageMap::dataBlockExists(rose_addr_t startVa) const {
    const AddressUsers noUsers;
    if (Sawyer::Optional<OwnedDataBlock> odb = map_.getOptional(startVa).orElse(noUsers).dataBlockExists(startVa)) {
        if (odb->dataBlock()->address() == startVa) {
            ASSERT_require(odb->isValid());
            return *odb;
        }
    }
    return OwnedDataBlock();
}

OwnedDataBlock
AddressUsageMap::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (dblock!=NULL) {
        const AddressUsers noUsers;
        if (Sawyer::Optional<OwnedDataBlock> odb = map_.getOptional(dblock->address()).orElse(noUsers).dataBlockExists(dblock)) {
            ASSERT_require(odb->isValid());
            return *odb;
        }
    }
    return OwnedDataBlock();
}

AddressIntervalSet
AddressUsageMap::extent() const {
    AddressIntervalSet retval;
    BOOST_FOREACH (const Map::Interval &interval, map_.intervals())
        retval.insert(interval);
    return retval;
}

AddressIntervalSet
AddressUsageMap::unusedExtent(size_t nBits) const {
    ASSERT_require(nBits>0 && nBits<=8*sizeof(rose_addr_t));
    AddressInterval vaSpace = AddressInterval::hull(0, IntegerOps::genMask<rose_addr_t>(nBits));
    return unusedExtent(vaSpace);
}

AddressIntervalSet
AddressUsageMap::unusedExtent(const AddressInterval &space) const {
    AddressIntervalSet retval = extent();
    retval.invert(space);
    return retval;
}

AddressIntervalSet
AddressUsageMap::unusedExtent(const AddressIntervalSet &space) const {
    return space - extent();
}

AddressInterval
AddressUsageMap::nextUnused(rose_addr_t minVa) const {
    return map_.firstUnmapped(minVa);
}

void
AddressUsageMap::print(std::ostream &out, const std::string &prefix) const {
    using namespace StringUtility;
    BOOST_FOREACH (const Map::Node &node, map_.nodes())
        out <<prefix <<"[" <<addrToString(node.key().least()) <<"," <<addrToString(node.key().greatest())
            <<"] " <<StringUtility::plural(node.key().size(), "bytes") << ": " <<node.value() <<"\n";
}

} // namespace
} // namespace
} // namespace
