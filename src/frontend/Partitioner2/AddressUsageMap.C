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
    ASSERT_require(dblock_ != NULL);
    return dblock_->address();
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

    if (dblock_ != NULL && other.dblock_ != NULL) {
        if (sortDataBlocks(dblock_, other.dblock_) || sortDataBlocks(other.dblock_, dblock_))
            return false;                               // one sorts is less than the other according to their keys
    } else if ((NULL == dblock_ && NULL != other.dblock_) || (NULL != dblock_ && NULL == other.dblock_)) {
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
        ASSERT_not_null(dblock_);
        ASSERT_not_null(other.dblock_);
        return sortDataBlocks(dblock_, other.dblock_);
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
        ASSERT_not_null(dblock_);
        out <<"{D-" <<StringUtility::addrToString(dblock_->address())
            <<"+" <<dblock_->size()
            <<" " <<StringUtility::plural(dblock_->nAttachedOwners(), "owners") <<"}";
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
        } else if (dblock_ != NULL) {
            error = "address user cannot be a instruction and data block at the same time";
        }
    } else if (dblock_ != NULL) {
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

SgAsmInstruction*
AddressUsers::instructionExists(SgAsmInstruction *insn) const {
    return findInstruction(insn).insn();
}

SgAsmInstruction*
AddressUsers::instructionExists(rose_addr_t va) const {
    return findInstruction(va).insn();
}

BasicBlock::Ptr
AddressUsers::basicBlockExists(const BasicBlock::Ptr &bb) const {
    if (bb) {
        return basicBlockExists(bb->address());
    } else {
        return BasicBlock::Ptr();
    }
}

BasicBlock::Ptr
AddressUsers::basicBlockExists(rose_addr_t va) const {
    const std::vector<BasicBlock::Ptr> &candidates = findBasicBlock(va).basicBlocks();
    BOOST_FOREACH (const BasicBlock::Ptr &bb, candidates) {
        if (bb->address() == va)
            return bb;
    }
    return BasicBlock::Ptr();
}

DataBlock::Ptr
AddressUsers::dataBlockExists(const DataBlock::Ptr &db) const {
    return findDataBlock(db).dataBlock();
}

DataBlock::Ptr
AddressUsers::dataBlockExists(rose_addr_t va, rose_addr_t size) const {
    return findDataBlock(va, size).dataBlock();
}

AddressUser
AddressUsers::findInstruction(SgAsmInstruction *insn) const {
    if (insn) {
        return findInstruction(insn->get_address());
    } else {
        return AddressUser();
    }
}

AddressUser
AddressUsers::findInstruction(rose_addr_t va) const {
    // This could be a binary search, but since instructions seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn() && user.insn()->get_address() == va)
            return user;
    }
    return AddressUser();
}

AddressUser
AddressUsers::findBasicBlock(const BasicBlock::Ptr &bblock) const {
    if (bblock) {
        return findBasicBlock(bblock->address());
    } else {
        return AddressUser();
    }
}

AddressUser
AddressUsers::findBasicBlock(rose_addr_t va) const {
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn()) {
            BOOST_FOREACH (const BasicBlock::Ptr &bb, user.basicBlocks()) {
                if (bb->address() == va)
                    return user;
            }
        }
    }
    return AddressUser();
}

AddressUser
AddressUsers::findDataBlock(const DataBlock::Ptr &dblock) const {
    if (dblock) {
        return findDataBlock(dblock->address(), dblock->size());
    } else {
        return AddressUser();
    }
}

AddressUser
AddressUsers::findDataBlock(rose_addr_t va, rose_addr_t size) const {
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (const DataBlock::Ptr &candidate = user.dataBlock()) {
            if (candidate->address() == va && candidate->size() == size)
                return user;
        }
    }
    return AddressUser();
}

AddressUser
AddressUsers::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(insn);
    ASSERT_not_null(bblock);
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    AddressUser user(insn, bblock);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    if (lb == users_.end() || NULL == lb->insn() || lb->insn()->get_address() != insn->get_address()) {
        lb = users_.insert(lb, user);
    } else {
        lb->insertBasicBlock(bblock);
    }
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    return *lb;
}

AddressUser
AddressUsers::insertDataBlock(const DataBlock::Ptr &db) {
    ASSERT_not_null(db);
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    AddressUser user(db);
    std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
    if (lb == users_.end() || NULL == lb->dataBlock() || !equalUnique(lb->dataBlock(), db, sortDataBlocks)) {
        // Wasn't present and list doesn't contain an equivalent data block, so add it
        lb = users_.insert(lb, AddressUser(db));
    }
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    return *lb;
}

void
AddressUsers::insert(const AddressUsers &other) {
    if (users_.empty()) {
        users_ = other.users_;
    } else {
        BOOST_FOREACH (const AddressUser &user, other.users_) {
            std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), user);
            if (lb == users_.end() || !(*lb == user))
                users_.insert(lb, user);
        }
    }
    ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
}

SgAsmInstruction*
AddressUsers::eraseInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bb) {
    SgAsmInstruction *retval = NULL;
    if (insn != NULL) {
        ASSERT_not_null(bb);
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
        AddressUser needle(insn, bb);
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb != users_.end() && lb->insn() != NULL && lb->insn()->get_address() == insn->get_address()) {
            retval = lb->insn();
            lb->eraseBasicBlock(bb);
            if (lb->basicBlocks().empty())
                users_.erase(lb);
        }
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    }
    return retval;
}

DataBlock::Ptr
AddressUsers::eraseDataBlock(const DataBlock::Ptr &dblock) {
    DataBlock::Ptr retval;
    if (dblock!=NULL) {
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
        AddressUser needle = AddressUser(dblock);
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb != users_.end() && lb->dataBlock() != NULL && equalUnique(lb->dataBlock(), dblock, sortDataBlocks)) {
            retval = lb->dataBlock();
            users_.erase(lb);
        }
        ASSERT_require(!ROSE_PARTITIONER_EXPENSIVE_CHECKS || isConsistent());
    }
    return retval;
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

AddressIntervalSet
AddressUsageMap::extent() const {
    AddressIntervalSet retval;
    BOOST_FOREACH (const Map::Interval &interval, map_.intervals())
        retval.insert(interval);
    return retval;
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

SgAsmInstruction*
AddressUsageMap::instructionExists(SgAsmInstruction *insn) const {
    if (insn) {
        return instructionExists(insn->get_address());
    } else {
        return NULL;
    }
}

SgAsmInstruction*
AddressUsageMap::instructionExists(rose_addr_t va) const {
    return map_.getOptional(va).orDefault().instructionExists(va);
}

BasicBlock::Ptr
AddressUsageMap::basicBlockExists(const BasicBlock::Ptr &bb) const {
    if (bb) {
        return basicBlockExists(bb->address());
    } else {
        return BasicBlock::Ptr();
    }
}

BasicBlock::Ptr
AddressUsageMap::basicBlockExists(rose_addr_t va) const {
    return map_.getOptional(va).orDefault().basicBlockExists(va);
}

DataBlock::Ptr
AddressUsageMap::dataBlockExists(const DataBlock::Ptr &db) const {
    if (db) {
        return dataBlockExists(db->address(), db->size());
    } else {
        return DataBlock::Ptr();
    }
}

DataBlock::Ptr
AddressUsageMap::dataBlockExists(rose_addr_t va, rose_addr_t size) const {
    return map_.getOptional(va).orDefault().dataBlockExists(va, size);
}

AddressUser
AddressUsageMap::findInstruction(SgAsmInstruction *insn) const {
    if (insn) {
        return findInstruction(insn->get_address());
    } else {
        return AddressUser();
    }
}

AddressUser
AddressUsageMap::findInstruction(rose_addr_t va) const {
    return map_.getOptional(va).orDefault().findInstruction(va);
}

AddressUser
AddressUsageMap::findBasicBlock(const BasicBlock::Ptr &bb) const {
    if (bb) {
        return findBasicBlock(bb->address());
    } else {
        return AddressUser();
    }
}

AddressUser
AddressUsageMap::findBasicBlock(rose_addr_t va) const {
    return map_.getOptional(va).orDefault().findBasicBlock(va);
}

AddressUser
AddressUsageMap::findDataBlock(const DataBlock::Ptr &db) const {
    if (db) {
        return findDataBlock(db->address(), db->size());
    } else {
        return AddressUser();
    }
}

AddressUser
AddressUsageMap::findDataBlock(rose_addr_t va, rose_addr_t size) const {
    return map_.getOptional(va).orDefault().findDataBlock(va, size);
}

AddressUser
AddressUsageMap::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(insn);
    ASSERT_not_null(bblock);
    AddressUser retval(insn, bblock);

    AddressInterval interval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
    Map adjustment;
    adjustment.insert(interval, AddressUsers(insn, bblock));
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        AddressUsers newUsers = node.value();
        retval = newUsers.insertInstruction(insn, bblock); // all should be the same
        adjustment.insert(interval.intersection(node.key()), newUsers);
    }
    map_.insertMultiple(adjustment);
    return retval;
}

AddressUser
AddressUsageMap::insertDataBlock(const DataBlock::Ptr &db) {
    ASSERT_not_null(db);
    AddressUser retval(db);

    AddressInterval interval = db->extent();
    Map adjustment;
    adjustment.insert(interval, AddressUsers(db));
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        AddressUsers newUsers = node.value();
        retval = newUsers.insertDataBlock(db);          // all should be the same
        adjustment.insert(interval.intersection(node.key()), newUsers);
    }
    map_.insertMultiple(adjustment);
    return retval;
}

SgAsmInstruction*
AddressUsageMap::eraseInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
    SgAsmInstruction *retval = NULL;
    if (insn) {
        ASSERT_not_null(bblock);
        AddressInterval interval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
        Map adjustment;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
            AddressUsers newUsers = node.value();
            retval = newUsers.eraseInstruction(insn, bblock);// all should be the same
            if (!newUsers.isEmpty())
                adjustment.insert(interval.intersection(node.key()), newUsers);
        }
        map_.erase(interval);
        map_.insertMultiple(adjustment);
    }
    return retval;
}

DataBlock::Ptr
AddressUsageMap::eraseDataBlock(const DataBlock::Ptr &db) {
    DataBlock::Ptr retval;
    if (db) {
        AddressInterval interval = db->extent();
        Map adjustment;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
            AddressUsers newUsers = node.value();
            retval = newUsers.eraseDataBlock(db);       // all should be the same
            if (!newUsers.isEmpty())
                adjustment.insert(interval.intersection(node.key()), newUsers);
        }
        map_.erase(interval);
        map_.insertMultiple(adjustment);
    }
    return retval;
}

void
AddressUsageMap::print(std::ostream &out, const std::string &prefix) const {
    using namespace StringUtility;
    if (map_.isEmpty()) {
        out <<prefix <<"empty\n";
    } else {
        BOOST_FOREACH (const Map::Node &node, map_.nodes()) {
            out <<prefix <<"[" <<addrToString(node.key().least()) <<"," <<addrToString(node.key().greatest())
                <<"] " <<StringUtility::plural(node.key().size(), "bytes") << "\n";
            const AddressUsers users = node.value();
            BOOST_FOREACH (const AddressUser &user, users.addressUsers()) {
                if (SgAsmInstruction *insn = user.insn()) {
                    out <<prefix <<"  " <<insn->toString() <<"\n";
                    BOOST_FOREACH (const BasicBlock::Ptr &bb, user.basicBlocks())
                        out <<prefix <<"    owned by " <<bb->printableName()
                            <<" having " <<plural(bb->nInstructions(), "instructions") <<"\n";
                } else {
                    DataBlock::Ptr db = user.dataBlock();
                    ASSERT_not_null(db);
                    out <<prefix <<"  " <<db->printableName()
                        <<" having " <<plural(db->size(), "bytes")<<"\n";
                    BOOST_FOREACH (const BasicBlock::Ptr &bb, db->attachedBasicBlockOwners())
                        out <<prefix <<"    owned by " <<bb->printableName() <<"\n";
                    BOOST_FOREACH (const Function::Ptr &f, db->attachedFunctionOwners())
                        out <<prefix <<"    owned by " <<f->printableName() <<"\n";
                }
            }
        }
    }
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

#if 0 // [Robb Matzke 2019-06-20]
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
    // contains multiple overlapping parts, all the OwnedDataBlock objects containing the specified dblock or equivalent dblock
    // will have the same data block pointer and ownership lists.
    BOOST_FOREACH (Map::Node &node, map_.findAll(interval)) {
        missingParts.erase(node.key());
        AddressUsers &newUsers = node.value();
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
AddressUsageMap::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock) {
        AddressInterval interval = dblock->extent();
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

OwnedDataBlock
AddressUsageMap::eraseDataBlockOwners(const OwnedDataBlock &odb) {
    OwnedDataBlock remaining;
    if (odb.isValid()) {
        AddressInterval interval = odb.dataBlock()->extent();
        BOOST_FOREACH (Map::Node &node, map_.findAll(interval)) {
            AddressUsers &users = node.value();
            remaining = users.eraseDataBlockOwners(odb); // all returns will be the same
        }
    }
    return remaining;
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
#endif

} // namespace
} // namespace
} // namespace
