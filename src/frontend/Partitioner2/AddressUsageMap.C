#include "sage3basic.h"
#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/Utility.h>
#include "AsmUnparser_compat.h"
#include <boost/foreach.hpp>
#include <integerOps.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rose_addr_t
AddressUser::address() const {
    if (insn_)
        return insn_->get_address();
    ASSERT_require(odblock_.isValid());
    return odblock_.dataBlock()->address();
}

void
AddressUser::insertBasicBlock(const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(insn_);
    ASSERT_not_null(bblock);
    insertUnique(bblocks_, bblock, sortBasicBlocksByAddress);
}

void
AddressUser::eraseBasicBlock(const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);
    eraseUnique(bblocks_, bblock, sortBasicBlocksByAddress);
}

BasicBlock::Ptr
AddressUser::isBlockEntry() const {
    if (insn_) {
        BOOST_FOREACH (const BasicBlock::Ptr &bb, bblocks_) {
            if (insn_->get_address() == bb->address())
                return bb;
        }
    }
    return BasicBlock::Ptr();
}

bool
AddressUser::operator==(const AddressUser &other) const {
    if (insn_ != other.insn_)
        return false;
    if (bblocks_.size() != other.bblocks_.size() || !std::equal(bblocks_.begin(), bblocks_.end(), other.bblocks_.begin()))
        return false;
    if (odblock_.dataBlock() != other.odblock_.dataBlock()) {
        // Data blocks are not same object, so compare by using their sort predicate. If a < b || b < a then a != b
        if (sortDataBlocks(odblock_.dataBlock(), other.odblock_.dataBlock()) ||
            sortDataBlocks(other.odblock_.dataBlock(), odblock_.dataBlock()))
            return false;
    }

    return true;
}

bool
AddressUser::operator<(const AddressUser &other) const {
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || other.isConsistent());
    if (insn_!=NULL && other.insn_!=NULL) {
        // Both users have instructions, so sort by instruction address
        ASSERT_require((insn_!=other.insn_) ^ (insn_->get_address()==other.insn_->get_address()));
        return insn_->get_address() < other.insn_->get_address();
    } else if (insn_!=NULL || other.insn_!=NULL) {
        // Exactly one user lacks an instruction.
        return insn_==NULL;                         // data blocks come before instructions
    } else {
        // Neither user has instructions, therefore both must have data blocks. Sort by the data blocks.
        ASSERT_not_null(odblock_.dataBlock());
        ASSERT_not_null(other.odblock_.dataBlock());
        return sortDataBlocks(odblock_.dataBlock(), other.odblock_.dataBlock());
    }
}

void
AddressUser::print(std::ostream &out) const {
    if (insn_!=NULL) {
        out <<"{";
        if (bblocks_.empty()) {
            out <<"{B-none ";
        } else {
            BOOST_FOREACH (const BasicBlock::Ptr &bb, bblocks_)
                out <<"B-" <<StringUtility::addrToString(bb->address()) <<" ";
        }
        out <<insn_->toString() <<"}";
    } else {
        ASSERT_require(odblock_.isValid());
        out <<"{D-" <<StringUtility::addrToString(odblock_.dataBlock()->address())
            <<"+" <<odblock_.dataBlock()->size()
            <<" " <<StringUtility::plural(odblock_.nOwners(), "owners") <<"}";
    }
}

bool
AddressUser::isConsistent() const {
    const char *error = NULL;
    if (insn_) {
        if (bblocks_.empty()) {
            error = "insn owner must have at least one bblock owner";
        } else if (!isSorted(bblocks_, sortBasicBlocksByAddress, true)) {
            error = "bblock are not sorted by address or do not have unique addresses";
        } else if (odblock_.isValid()) {
            error = "address user cannot be a instruction and data block at the same time";
        }
    } else if (odblock_.isValid()) {
        if (!bblocks_.empty())
            error = "bblocks should not  be present for data block users";
    } else {
        error = "user must be either an instruction or a data block";
    }
    ASSERT_require2(!error, error);
    return !error;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUsers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // [Robb P Matzke 2016-06-30]
BasicBlock::Ptr
AddressUsers::instructionExists(SgAsmInstruction *insn) const {
    if (!insn)
        return BasicBlock::Ptr();
    AddressUser needle(insn, BasicBlock::Ptr());      // basic block is not used for binary search
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb==users_.end() || lb->insn()!=insn)
        return BasicBlock::Ptr();
    ASSERT_not_null(lb->basicBlock());
    return lb->basicBlock();
}
#endif

Sawyer::Optional<OwnedDataBlock>
AddressUsers::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (dblock==NULL)
        return Sawyer::Nothing();
    AddressUser needle = AddressUser(OwnedDataBlock(dblock));
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb == users_.end() || !equalUnique(lb->dataBlock(), dblock, sortDataBlocks))
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
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    AddressUser user(insn, bblock);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    if (lb == users_.end() || lb->insn() != insn) {
        users_.insert(lb, user);
    } else {
        lb->insertBasicBlock(bblock);
    }
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
}

OwnedDataBlock
AddressUsers::insertDataBlock(const OwnedDataBlock &odb) {
    ASSERT_require(odb.isValid());
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    OwnedDataBlock retval;
    AddressUser user(odb);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    if (lb == users_.end() || !equalUnique(lb->dataBlock(), odb.dataBlock(), sortDataBlocks)) {
        // Wasn't present and list doesn't contain an equivalent data block, so add it
        users_.insert(lb, odb);
        retval = odb;
    } else {
        // An equivalent data block was present, so merge new ownership list into existing ownership list
        ASSERT_require(equalUnique(lb->dataBlock(), odb.dataBlock(), sortDataBlocks));
        BOOST_FOREACH (const Function::Ptr &function, odb.owningFunctions())
            lb->dataBlockOwnership().insertOwner(function);
        BOOST_FOREACH (const BasicBlock::Ptr &bblock, odb.owningBasicBlocks())
            lb->dataBlockOwnership().insertOwner(bblock);
        retval = lb->dataBlockOwnership();
    }
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    return retval;
}

#if 0 // [Robb P Matzke 2016-06-30]
void
AddressUsers::eraseInstruction(SgAsmInstruction *insn) {
    if (insn!=NULL) {
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
        AddressUser needle(insn, BasicBlock::Ptr());
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->insn()==insn)
            users_.erase(lb);
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    }
}
#else
void
AddressUsers::eraseInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bb) {
    if (insn != NULL) {
        ASSERT_not_null(bb);
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
        AddressUser needle(insn, bb);
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb != users_.end() && lb->insn() == insn) {
            lb->eraseBasicBlock(bb);
            if (lb->basicBlocks().empty())
                users_.erase(lb);
        }
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    }
}
#endif

void
AddressUsers::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock!=NULL) {
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
        AddressUser needle = AddressUser(OwnedDataBlock(dblock));
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->dataBlock()==dblock)
            users_.erase(lb);
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    }
}

std::vector<SgAsmInstruction*>
AddressUsers::instructions() const {
    std::vector<SgAsmInstruction*> insns;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (SgAsmInstruction *insn = user.insn())
            insns.push_back(insn);
    }
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isSorted(insns, sortInstructionsByAddress, true));
    return insns;
}

std::vector<BasicBlock::Ptr>
AddressUsers::instructionOwners() const {
    std::vector<BasicBlock::Ptr> bblocks;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn()) {
            BOOST_FOREACH (const BasicBlock::Ptr &bb, user.basicBlocks()) {
                ASSERT_not_null(bb);
                insertUnique(bblocks, bb, sortBasicBlocksByAddress);
            }
        }
    }
    return bblocks;
}

BasicBlock::Ptr
AddressUsers::findBasicBlock(rose_addr_t bbVa) const {
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn()) {
            BOOST_FOREACH (const BasicBlock::Ptr &bb, user.basicBlocks()) {
                if (bb->address() == bbVa)
                    return bb;
            }
        }
    }
    return BasicBlock::Ptr();
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
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || other.isConsistent());
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
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || retval.isConsistent());
    return retval;
}

AddressUsers
AddressUsers::union_(const AddressUsers &other) const {
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || other.isConsistent());
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
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || retval.isConsistent());
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
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
}

bool
AddressUsers::operator==(const AddressUsers &other) const {
    return users_.size()==other.users_.size() && std::equal(users_.begin(), users_.end(), other.users_.begin());
}

bool
AddressUsers::isConsistent() const {
    if (!users_.empty()) {
        std::vector<AddressUser>::const_iterator current = users_.begin();
        std::vector<AddressUser>::const_iterator next = current;
        while (current != users_.end()) {
            ASSERT_require2(current->isConsistent(), boost::lexical_cast<std::string>(*this));
            if (++next == users_.end()) {
                break;
            } else if (*current < *next) {
                ASSERT_forbid2(*next < *current, boost::lexical_cast<std::string>(*this));
                ASSERT_forbid2(*current == *next, boost::lexical_cast<std::string>(*this));
            } else if (*next < *current) {
                ASSERT_forbid2(*current == *next, boost::lexical_cast<std::string>(*this));
                ASSERT_not_reachable("list is not sorted");
            } else {
                ASSERT_require2(*current == *next, boost::lexical_cast<std::string>(*this));
                if (current->insn()) {
                    ASSERT_not_reachable("list is not sorted");
                } else {
                    // Multiple data blocks can can exist at the same address and have the same size, but we can't
                    // allow the exact same data block (by pointers) to appear multiple times in the list.
                    ASSERT_forbid2(current->dataBlock()==next->dataBlock(), "list contains a duplicate");
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

OwnedDataBlock
AddressUsageMap::insertDataBlock(const OwnedDataBlock &odb) {
    ASSERT_require(odb.isValid());
    Sawyer::Optional<OwnedDataBlock> retval;
    AddressInterval interval = AddressInterval::baseSize(odb.dataBlock()->address(), odb.dataBlock()->size());

    // Either the data block is present in the AUM or it isn't. If it is present, then the AUM contains the entire block's
    // interval, otherwise it might contain only parts of the interval. Keep track of which parts of the interval are missing
    // so we can add them after the loop.
    AddressIntervalSet missingParts;
    missingParts.insert(interval);

    // Update the existing parts of the interval by merging this odb's ownership info into the existing odb. Even if the map
    // contains multiple overlapping parts, they should all the OwnedDataBlock values corresponding to the specified dblock
    // will have the same data block pointer and ownership lists.
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        missingParts.erase(node.key());
        AddressUsers newUsers = node.value();
        retval = newUsers.insertDataBlock(odb);
    }

    // Add the missing parts of the interval.
    BOOST_FOREACH (const AddressInterval &i, missingParts.intervals())
        map_.insert(i, AddressUsers(odb));

    // Either there was at least one matching OwnedDataBlock that was updated (and copied into retval), or we simply inserted
    // the specified odb argument.
    return retval.orElse(odb);
}

void
AddressUsageMap::eraseInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    if (insn) {
        ASSERT_not_null(bblock);
        AddressInterval interval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
        Map adjustment;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
            AddressUsers newUsers = node.value();
            newUsers.eraseInstruction(insn, bblock);
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

bool
AddressUsageMap::anyExists(const AddressInterval &where) const {
    return map_.findFirstOverlap(where) != map_.nodes().end();
}

bool
AddressUsageMap::anyExists(const AddressIntervalSet &where) const {
    // FIXME[Robb P Matzke 2016-06-28]: this could be even faster by using iterators and lowerBound.
    BOOST_FOREACH (const AddressInterval &interval, where.intervals()) {
        if (anyExists(interval))
            return true;
    }
    return false;
}

bool
AddressUsageMap::instructionExists(SgAsmInstruction *insn) const {
    const AddressUsers noUsers;
    return insn && map_.getOptional(insn->get_address()).orElse(noUsers).instructionExists(insn->get_address());
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
    const AddressUsers noUsers;
    return map_.getOptional(startVa).orElse(noUsers).findBasicBlock(startVa);
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

void
AddressUsageMap::checkConsistency() const {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug <<"checking AUM consistency...\n";

    // Find all distinct data block objects
    std::set<DataBlock::Ptr> allDataBlocks;
    BOOST_FOREACH (const Map::Node &node, map_.nodes()) {
        const AddressUsers &users = node.value();
        BOOST_FOREACH (const AddressUser &user, users.addressUsers()) {
            if (DataBlock::Ptr dblock = user.dataBlock())
                allDataBlocks.insert(dblock);
        }
    }

    // Of all the data block objects, no two objects should have the same identification. Data blocks are identified by their
    // starting address and size.
    size_t nErrors = 0;
    std::vector<DataBlock::Ptr> dblocks(allDataBlocks.begin(), allDataBlocks.end());
    for (size_t i=1; i < dblocks.size(); ++i) {
        if (dblocks[i]->address() == dblocks[i-1]->address() && dblocks[i]->size() == dblocks[i-1]->size()) {
            debug <<"  duplicate data blocks detected: idx = " <<(i-1) <<" and " <<i <<"\n";
            ++nErrors;
        }
    }

    ASSERT_always_require2(0 == nErrors, StringUtility::plural(nErrors, "errors"));
}

} // namespace
} // namespace
} // namespace
