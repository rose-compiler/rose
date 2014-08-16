#include "Partitioner2.h"

#include "SymbolicSemantics2.h"
#include "DispatcherM68k.h"
#include "Diagnostics.h"

#if 1 // DEBUGGING [Robb P. Matzke 2014-08-02]
#include "AsmUnparser_compat.h"
#endif

#include <boost/foreach.hpp>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stack.h>

// Defining this will cause the partitioner to continuously very that the CFG and AUM are consistent.  Doing so will impose a
// substantial slow-down.  Defining this has little effect if NDEBUG or SAWYER_NDEBUG is also defined.
#undef ROSE_PARTITIONER_EXPENSIVE_CHECKS

using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Sawyer::Message::Facility Partitioner::mlog("Partitioner2");

// class method
void Partitioner::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog.initStreams(Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Sorting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
sortBasicBlocksByAddress(const Partitioner::BasicBlock::Ptr &a, const Partitioner::BasicBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

static bool
sortDataBlocksByAddress(const Partitioner::DataBlock::Ptr &a, const Partitioner::DataBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a->address() != b->address())
        return a->address() < b->address();
    if (a->size() < b->size())
        return true;
    return false;
}

static bool
sortFunctionsByAddress(const Partitioner::Function::Ptr &a, const Partitioner::Function::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

// Sort by the successor expressions. If both expressions are concrete then disregard their widths and treat them as unsigned
// when comparing.
static bool
sortByExpression(const Partitioner::BasicBlock::Successor &a, const Partitioner::BasicBlock::Successor &b) {
    if (a.expr()->is_number() && b.expr()->is_number())
        return a.expr()->get_number() < b.expr()->get_number();
    if (a.expr()->is_number() || b.expr()->is_number())
        return a.expr()->is_number();                   // concrete values are less than abstract expressions
    return a.expr()->get_expression()->structural_compare(b.expr()->get_expression()) < 0;
}

static bool
sortVerticesByAddress(const Partitioner::ControlFlowGraph::ConstVertexNodeIterator &a,
                      const Partitioner::ControlFlowGraph::ConstVertexNodeIterator &b) {
    const Partitioner::CfgVertex &av = a->value();
    const Partitioner::CfgVertex &bv = b->value();
    if (av.type() != bv.type() || av.type() != Partitioner::V_BASICBLOCK)
        return av.type() < bv.type();
    return av.address() < bv.address();
}

static bool
sortEdgesBySrc(const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &a,
               const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &b) {
    return sortVerticesByAddress(a->source(), b->source());
}

static bool
sortEdgesByDst(const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &a,
               const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &b) {
    return sortVerticesByAddress(a->target(), b->target());
}

static bool
sortBlocksForAst(SgAsmBlock *a, SgAsmBlock *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a->get_address() != b->get_address())
        return a->get_address() < b->get_address();
    if (a->get_statementList().size()>0 && b->get_statementList().size()>0) {
        // Sort so basic blocks come before data blocks when they start at the same address, regardless of size
        bool a_isBasicBlock = NULL!=isSgAsmInstruction(a->get_statementList()[0]);
        bool b_isBasicBlock = NULL!=isSgAsmInstruction(b->get_statementList()[0]);
        if (a_isBasicBlock != b_isBasicBlock)
            return a_isBasicBlock;
    }
    return false;
}

template<class Container, class Comparator>
static bool
isSorted(const Container &container, Comparator sorted, bool distinct=true) {
    typename Container::const_iterator current = container.begin();
    if (current!=container.end()) {
        typename Container::const_iterator next = current;
        while (++next != container.end()) {
            if ((distinct && !sorted(*current, *next)) || sorted(*next, *current))
                return false;
            ++current;
        }
    }
    return true;
}

template<class Container>
static void
sortedInsert(Container &sortedFunctions /*in,out*/, const Partitioner::Function::Ptr &function) {
    ASSERT_not_null(function);
    ASSERT_require(isSorted(sortedFunctions, sortFunctionsByAddress, true));
    typename Container::iterator lb = std::lower_bound(sortedFunctions.begin(), sortedFunctions.end(), function,
                                                       sortFunctionsByAddress);
    if (lb==sortedFunctions.end() || (*lb)->address()!=function->address())
        sortedFunctions.insert(lb, function);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Function descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::Function::insertDataBlock(const DataBlock::Ptr &dblock) {
    if (isFrozen_)
        throw Exception(printableName() + " is frozen or attached to the CFG/AUM");
    if (dblock==NULL)
        return;

    std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock,
                                                                sortDataBlocksByAddress);
    if (lb != dblocks_.end() && (*lb)->address()==dblock->address()) {
        *lb = dblock;
    } else {
        dblocks_.push_back(dblock);
    }
    ASSERT_require(isSorted(dblocks_, sortDataBlocksByAddress, true));
}

void
Partitioner::Function::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock) {
        std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock,
                                                                    sortDataBlocksByAddress);
        if (lb!=dblocks_.end() && (*lb)==dblock)
            dblocks_.erase(lb);
    }
}

std::string
Partitioner::Function::printableName() const {
    return "function " + StringUtility::addrToString(address());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::BasicBlock::init(const Partitioner *partitioner) {
    operators_ = partitioner->newOperators();
#if 1
    // Try to use our own semantics
    if (usingDispatcher_) {
        if (dispatcher_ = partitioner->newDispatcher(operators_)) {
            usingDispatcher_ = true;
            initialState_ = dispatcher_->get_operators()->get_state(); // points to dispatcher's state
            BaseSemantics::RegisterStateGeneric::promote(initialState_->get_register_state())->initialize_large();
            initialState_ = initialState_->clone(); // make a copy so processInstruction doesn't change it
            optionalPenultimateState_ = initialState_->clone(); // one level of undo information
        } else {
            usingDispatcher_ = false;
        }
    }
#else
    // Rely on other methods to get basic block characteristics
    usingDispatcher_ = false;
#endif
}

void
Partitioner::BasicBlock::insertSuccessor(const BaseSemantics::SValuePtr &successor_, EdgeType type) {
    if (successor_ != NULL) {
        Semantics::SValuePtr successor = Semantics::SValue::promote(successor_);
        if (!successors_.isCached()) {
            Successors successors;
            successors.push_back(Successor(successor, type));
            successors_ = successors;
        } else {
#if 1 // DEBUGGING [Robb P. Matzke 2014-08-16]
            Successors successors = successors_.get();
#else
            Successors &successors = successors_.get();
#endif
            bool found = false;
            BOOST_FOREACH (const Successor &exists, successors) {
                if (exists.expr()->get_expression()->equivalent_to(successor->get_expression())) {
                    ASSERT_require2(exists.type()==type, "changing successor types is not implemented yet");
                    found = true;
                    break;
                }
            }
            if (!found) {
                successors.push_back(Successor(successor, type));
#if 1 // DEBUGGING [Robb P. Matzke 2014-08-16]
                successors_ = successors;
#endif
            }
        }
    }
}

void
Partitioner::BasicBlock::insertSuccessor(rose_addr_t va, size_t nBits, EdgeType type) {
    return insertSuccessor(operators_->number_(nBits, va), type);
}

SgAsmInstruction*
Partitioner::BasicBlock::instructionExists(rose_addr_t startVa) const {
    BOOST_FOREACH (SgAsmInstruction *insn, insns_) {
        if (insn->get_address() == startVa)
            return insn;
    }
    return NULL;
}

Sawyer::Optional<size_t>
Partitioner::BasicBlock::instructionExists(SgAsmInstruction *toFind) const {
    for (size_t i=0; i<insns_.size(); ++i) {
        if (insns_[i]==toFind)
            return i;
    }
    return Sawyer::Nothing();
}

BaseSemantics::StatePtr
Partitioner::BasicBlock::finalState() {
    if (usingDispatcher_ && dispatcher_!=NULL)
        return dispatcher_->get_operators()->get_state();
    return BaseSemantics::StatePtr();
}

void
Partitioner::BasicBlock::append(SgAsmInstruction *insn) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to append instruction");
    ASSERT_not_null(insn);
    ASSERT_require2(!insns_.empty() || insn->get_address()==startVa_,
                    "address of first instruction (" + StringUtility::addrToString(insn->get_address()) + ") "
                    "must match block address (" + StringUtility::addrToString(startVa_) + ")");
    ASSERT_require2(std::find(insns_.begin(), insns_.end(), insn) == insns_.end(),
                    "instruction can only occur once in a basic block");

    // Process the instruction to create a new state
    optionalPenultimateState_ = usingDispatcher_ ?
                                dispatcher_->get_operators()->get_state()->clone() :
                                BaseSemantics::StatePtr();
    insns_.push_back(insn);
    if (usingDispatcher_) {
        try {
            dispatcher_->processInstruction(insn);
        } catch (...) {
            usingDispatcher_ = false;                   // an error turns off semantics for the remainder of the basic block
        }
    }
    clearCache();
}

void
Partitioner::BasicBlock::pop() {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to pop an instruction");
    ASSERT_forbid2(insns_.empty(), "basic block must have at least one instruction to pop");
    ASSERT_require2(optionalPenultimateState_, "only one level of undo is possible");
    insns_.pop_back();

    if (BaseSemantics::StatePtr ps = *optionalPenultimateState_) {
        // If we didn't save a previous state it means that we didn't call processInstruction during the append, and therefore
        // we don't need to update the dispatcher (it's already out of date anyway).  Otherwise the dispatcher state needs to
        // be re-initialized by transferring ownership of the previous state into the partitioner.
        dispatcher_->get_operators()->set_state(ps);
        optionalPenultimateState_ = Sawyer::Nothing();
    }
    clearCache();
}

rose_addr_t
Partitioner::BasicBlock::fallthroughVa() const {
    ASSERT_require(!insns_.empty());
    return insns_.back()->get_address() + insns_.back()->get_size();
}

std::string
Partitioner::BasicBlock::printableName() const {
    return "basic block " + StringUtility::addrToString(address());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DataBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
Partitioner::DataBlock::printableName() const {
    return "data block " + StringUtility::addrToString(address());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      OwnedDataBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Partitioner::OwnedDataBlock::insert(const Function::Ptr &function) {
    std::vector<Function::Ptr>::iterator lb = std::lower_bound(owners_.begin(), owners_.end(), function,
                                                               sortFunctionsByAddress);
    if (lb!=owners_.end() && function->address()==(*lb)->address()) {
        ASSERT_require2(function==(*lb), "two functions with the same address must be the same function");
    } else {
        owners_.insert(lb, function);
        ASSERT_require(isSorted(owners_, sortFunctionsByAddress, true));
    }
    return owners_.size();
}

size_t
Partitioner::OwnedDataBlock::erase(const Function::Ptr &function) {
    if (function) {
        std::vector<Function::Ptr>::iterator lb = std::lower_bound(owners_.begin(), owners_.end(), function,
                                                                   sortFunctionsByAddress);
        if (lb!=owners_.end())
            owners_.erase(lb);
    }
    return owners_.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::AddressUser::print(std::ostream &out) const {
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

Partitioner::BasicBlock::Ptr
Partitioner::AddressUsers::instructionExists(SgAsmInstruction *insn) const {
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

Partitioner::DataBlock::Ptr
Partitioner::AddressUsers::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (dblock==NULL)
        return DataBlock::Ptr();
    AddressUser needle(dblock);
    ASSERT_require(isConsistent());
    std::vector<AddressUser>::const_iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
    if (lb==users_.end() || lb->dblock()==NULL || lb->dblock()->address()!=dblock->address())
        return DataBlock::Ptr();
    return lb->dblock();
}

Sawyer::Optional<Partitioner::AddressUser>
Partitioner::AddressUsers::instructionExists(rose_addr_t startVa) const {
    // This could be a binary search, but since instructions seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn() && user.insn()->get_address() == startVa)
            return user;
    }
    return Sawyer::Nothing();
}

Sawyer::Optional<Partitioner::AddressUser>
Partitioner::AddressUsers::dataBlockExists(rose_addr_t startVa) const {
    // This could be a binary search, but since data blocks seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.dblock() && user.dblock()->address() == startVa)
            return user;
    }
    return Sawyer::Nothing();
}

Partitioner::AddressUsers&
Partitioner::AddressUsers::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
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

Partitioner::AddressUsers&
Partitioner::AddressUsers::insertDataBlock(const DataBlock::Ptr &dblock) {
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

Partitioner::AddressUsers&
Partitioner::AddressUsers::eraseInstruction(SgAsmInstruction *insn) {
    if (insn!=NULL) {
        ASSERT_require(isConsistent());
        AddressUser needle(insn, BasicBlock::Ptr());
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->insn()==insn)
            users_.erase(lb);
    }
    return *this;
}

Partitioner::AddressUsers&
Partitioner::AddressUsers::eraseDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock!=NULL) {
        ASSERT_require(isConsistent());
        AddressUser needle(dblock);
        std::vector<AddressUser>::iterator lb = std::lower_bound(users_.begin(), users_.end(), needle);
        if (lb!=users_.end() && lb->dblock() && lb->dblock()->address()==dblock->address())
            users_.erase(lb);
    }
    return *this;
}

Partitioner::AddressUsers
Partitioner::AddressUsers::instructionUsers() const {
    AddressUsers insns;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.insn())
            insns.users_.push_back(user);
    }
    return insns;
}

Partitioner::AddressUsers
Partitioner::AddressUsers::dataBlockUsers() const {
    AddressUsers dblocks;
    BOOST_FOREACH (const AddressUser &user, users_) {
        if (user.dblock())
            dblocks.users_.push_back(user);
    }
    return dblocks;
}

std::vector<Partitioner::BasicBlock::Ptr>
Partitioner::AddressUsers::basicBlocks() const {
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

Partitioner::AddressUsers
Partitioner::AddressUsers::intersection(const AddressUsers &other) const {
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

Partitioner::AddressUsers
Partitioner::AddressUsers::union_(const AddressUsers &other) const {
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
Partitioner::AddressUsers::isConsistent() const {
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
Partitioner::AddressUsers::print(std::ostream &out) const {
    BOOST_FOREACH (const AddressUser &addressUser, users_)
        out <<" " <<addressUser;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUsageMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::AddressUsageMap::insertInstruction(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock) {
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
Partitioner::AddressUsageMap::insertDataBlock(const DataBlock::Ptr &dblock) {
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
Partitioner::AddressUsageMap::eraseInstruction(SgAsmInstruction *insn) {
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
Partitioner::AddressUsageMap::eraseDataBlock(const DataBlock::Ptr &dblock) {
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

Partitioner::BasicBlock::Ptr
Partitioner::AddressUsageMap::instructionExists(SgAsmInstruction *insn) const {
    const AddressUsers noUsers;
    return insn ? map_.getOptional(insn->get_address()).orElse(noUsers).instructionExists(insn) : BasicBlock::Ptr();
}

Sawyer::Optional<Partitioner::AddressUser>
Partitioner::AddressUsageMap::instructionExists(rose_addr_t startVa) const {
    const AddressUsers noUsers;
    if (Sawyer::Optional<AddressUser> found = map_.getOptional(startVa).orElse(noUsers).instructionExists(startVa)) {
        if (found->insn()->get_address() == startVa)
            return found;
    }
    return Sawyer::Nothing();
}

Partitioner::BasicBlock::Ptr
Partitioner::AddressUsageMap::basicBlockExists(rose_addr_t startVa) const {
    if (Sawyer::Optional<AddressUser> found = instructionExists(startVa)) {
        if (found->bblock()->address() == startVa)
            return found->bblock();
    }
    return BasicBlock::Ptr();
}

Partitioner::DataBlock::Ptr
Partitioner::AddressUsageMap::dataBlockExists(const DataBlock::Ptr &dblock) const {
    const AddressUsers noUsers;
    return dblock ? map_.getOptional(dblock->address()).orElse(noUsers).dataBlockExists(dblock) : DataBlock::Ptr();
}

Sawyer::Optional<Partitioner::AddressUser>
Partitioner::AddressUsageMap::dataBlockExists(rose_addr_t startVa) const {
    const AddressUsers noUsers;
    if (Sawyer::Optional<AddressUser> found = map_.getOptional(startVa).orElse(noUsers).dataBlockExists(startVa)) {
        if (found->dblock()->address() == startVa)
            return found;
    }
    return Sawyer::Nothing();
}

Sawyer::Container::IntervalSet<AddressInterval>
Partitioner::AddressUsageMap::extent() const {
    Sawyer::Container::IntervalSet<AddressInterval> retval;
    BOOST_FOREACH (const Map::Interval &interval, map_.keys())
        retval.insert(interval);
    return retval;
}

Sawyer::Container::IntervalSet<AddressInterval>
Partitioner::AddressUsageMap::unusedExtent(size_t nBits) const {
    ASSERT_require(nBits>0 && nBits<=8*sizeof(rose_addr_t));
    AddressInterval vaSpace = AddressInterval::hull(0, IntegerOps::genMask<rose_addr_t>(nBits));
    return unusedExtent(vaSpace);
}

Sawyer::Container::IntervalSet<AddressInterval>
Partitioner::AddressUsageMap::unusedExtent(const AddressInterval &vaSpace) const {
    Sawyer::Container::IntervalSet<AddressInterval> retval = extent();
    retval.invert(vaSpace);
    return retval;
}

Partitioner::AddressUsers
Partitioner::AddressUsageMap::spanning(const AddressInterval &interval) const {
    AddressUsers retval;
    size_t nIters = 0;
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        retval = 0==nIters++ ? node.value() : retval.intersection(node.value());
        if (retval.isEmpty())
            break;
    }
    return retval;
}

Partitioner::AddressUsers
Partitioner::AddressUsageMap::overlapping(const AddressInterval &interval) const {
    AddressUsers retval;
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval))
        retval = retval.union_(node.value());
    return retval;
}

void
Partitioner::AddressUsageMap::print(std::ostream &out, const std::string &prefix) const {
    using namespace StringUtility;
    BOOST_FOREACH (const Map::Node &node, map_.nodes())
        out <<prefix <<"[" <<addrToString(node.key().least()) <<"," <<addrToString(node.key().greatest())
            <<"] =" <<node.value() <<"\n";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::init() {
    undiscoveredVertex_ = cfg_.insertVertex(CfgVertex(V_UNDISCOVERED));
    indeterminateVertex_ = cfg_.insertVertex(CfgVertex(V_INDETERMINATE));
    nonexistingVertex_ = cfg_.insertVertex(CfgVertex(V_NONEXISTING));
}

// Label the progress report and also show some other statistics.  It is okay for this to be slightly expensive since its only
// called when a progress report is actually emitted.
class ProgressBarSuffix {
    const Partitioner *partitioner_;
public:
    ProgressBarSuffix(): partitioner_(NULL) {}
    explicit ProgressBarSuffix(const Partitioner *p): partitioner_(p) {}
    void print(std::ostream &out) const {
        ASSERT_not_null(partitioner_);
        out <<" bytes"                                  // units for progress report
            <<" " <<StringUtility::plural(partitioner_->nPlaceholders(), "blks")
            <<" " <<StringUtility::plural(partitioner_->nFunctions(), "funcs");
    }
};

static std::ostream&
operator<<(std::ostream &out, const ProgressBarSuffix &x) {
    x.print(out);
    return out;
};

void
Partitioner::reportProgress() const {
    // All partitioners share a single progress bar.
    static Sawyer::ProgressBar<size_t, ProgressBarSuffix> *bar = NULL;
    if (!bar) {
        if (0==progressTotal_) {
            BOOST_FOREACH (const MemoryMap::Segments::Node &node, memoryMap_.segments().nodes()) {
                if (0 != (node.value().get_mapperms() & MemoryMap::MM_PROT_EXEC))
                    progressTotal_ += node.key().size();
            }
        }
        bar = new Sawyer::ProgressBar<size_t, ProgressBarSuffix>(progressTotal_, mlog[INFO], "cfg");
    }
    if (progressTotal_) {
        // If multiple partitioners are sharing the progress bar then also make sure that the lower and upper limits are
        // appropriate for THIS partitioner.  However, changing the limits is a configuration change, which also immediately
        // updates the progress bar (we don't want that, so update only if necessary).
        bar->suffix(ProgressBarSuffix(this));
        if (bar->domain().first!=0 || bar->domain().second!=progressTotal_) {
            bar->value(0, nBytes(), progressTotal_);
        } else {
            bar->value(nBytes());
        }
    }
}

size_t
Partitioner::nInstructions() const {
    size_t nInsns = 0;
    BOOST_FOREACH (const CfgVertex &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASICBLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock())
                nInsns += bb->nInsns();
        }
    }
    return nInsns;
}

size_t
Partitioner::nPlaceholders() const {
    ASSERT_require(cfg_.nVertices() >= nSpecialVertices);
    return cfg_.nVertices() - nSpecialVertices;
}

bool
Partitioner::placeholderExists(rose_addr_t startVa) const {
    return vertexIndex_.exists(startVa);
}

Partitioner::BasicBlock::Ptr
Partitioner::erasePlaceholder(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    BasicBlock::Ptr bblock;
    if (placeholder!=cfg_.vertices().end() && placeholder->value().type()==V_BASICBLOCK) {
        rose_addr_t startVa = placeholder->value().address();
        if (bblock = placeholder->value().bblock())
            detachBasicBlock(placeholder);              // removes self edges, notifies subclasses of CFG changes
        if (placeholder->nInEdges()!=0) {
            throw PlaceholderError(startVa, "cannot erase placeholder " + StringUtility::addrToString(startVa) +
                                   " that has " + StringUtility::plural(placeholder->nInEdges(), "incoming edges"));
        }
        vertexIndex_.erase(placeholder->value().address());
        cfg_.eraseVertex(placeholder);
        bblockDetached(startVa, BasicBlock::Ptr());     // null bblock indicates placeholder erasure
    }
    return bblock;
}

size_t
Partitioner::nBasicBlocks() const {
    size_t nBasicBlocks = 0;
    BOOST_FOREACH (const CfgVertex &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASICBLOCK && vertex.bblock())
            ++nBasicBlocks;
    }
    return nBasicBlocks;
}

Partitioner::BasicBlock::Ptr
Partitioner::basicBlockExists(rose_addr_t startVa) const {
    ControlFlowGraph::ConstVertexNodeIterator vertex = findPlaceholder(startVa);
    if (vertex!=cfg_.vertices().end())
        return vertex->value().bblock();
    return BasicBlock::Ptr();
}

Partitioner::BasicBlock::Ptr
Partitioner::basicBlockExists(const BasicBlock::Ptr &bblock) const {
    return bblock==NULL ? BasicBlock::Ptr() : basicBlockExists(bblock->address());
}

BaseSemantics::RiscOperatorsPtr
Partitioner::newOperators() const {
    return Semantics::RiscOperators::instance(instructionProvider_.registerDictionary(), solver_);
}

BaseSemantics::DispatcherPtr
Partitioner::newDispatcher(const BaseSemantics::RiscOperatorsPtr &ops) const {
    ASSERT_not_null(ops);
    if (instructionProvider_.dispatcher() == NULL)
        return BaseSemantics::DispatcherPtr();          // instruction semantics are not implemented for this architecture
    return instructionProvider_.dispatcher()->create(ops);
}

Partitioner::BasicBlock::Ptr
Partitioner::detachBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    BasicBlock::Ptr bblock;
    if (placeholder != cfg_.vertices().end() && placeholder->value().type()==V_BASICBLOCK) {
        bblock = placeholder->value().bblock();
        placeholder->value().nullify();
        adjustPlaceholderEdges(placeholder);
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
            aum_.eraseInstruction(insn);
        bblock->thaw();
        bblockDetached(bblock->address(), bblock);
    }
    return bblock;
}

Partitioner::BasicBlock::Ptr
Partitioner::detachBasicBlock(const BasicBlock::Ptr &bblock) {
    if (bblock!=NULL) {
        ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(bblock->address());
        if (placeholder!=cfg_.vertices().end() && placeholder->value().bblock()==bblock)
            return detachBasicBlock(placeholder);
    }
    return BasicBlock::Ptr();
}

Partitioner::BasicBlock::Ptr
Partitioner::detachBasicBlock(rose_addr_t startVa) {
    return detachBasicBlock(findPlaceholder(startVa));
}

Partitioner::ControlFlowGraph::EdgeNodeIterator
Partitioner::adjustPlaceholderEdges(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    ASSERT_require(placeholder!=cfg_.vertices().end());
    ASSERT_require2(NULL==placeholder->value().bblock(), "vertex must be strictly a placeholder");
    cfg_.clearOutEdges(placeholder);
    return cfg_.insertEdge(placeholder, undiscoveredVertex_);
}

Partitioner::ControlFlowGraph::EdgeNodeIterator
Partitioner::adjustNonexistingEdges(const ControlFlowGraph::VertexNodeIterator &vertex) {
    ASSERT_require(vertex!=cfg_.vertices().end());
    ASSERT_not_null2(vertex->value().bblock(), "vertex must have been discovered");
    ASSERT_require2(vertex->value().bblock()->isEmpty(), "vertex must be non-existing");
    cfg_.clearOutEdges(vertex);
    return cfg_.insertEdge(vertex, nonexistingVertex_);
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlock(const ControlFlowGraph::ConstVertexNodeIterator &placeholder) const {
    ASSERT_require2(placeholder != cfg_.vertices().end(), "invalid basic block placeholder");
    BasicBlock::Ptr bb = placeholder->value().bblock();
    return bb!=NULL ? bb : discoverBasicBlockInternal(placeholder->value().address());
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlock(rose_addr_t startVa) const {
    ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(startVa);
    return placeholder==cfg_.vertices().end() ? discoverBasicBlockInternal(startVa) : discoverBasicBlock(placeholder);
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlockInternal(rose_addr_t startVa) const {
    // If the first instruction of this basic block already exists (in the middle of) some other basic block then the other
    // basic block is called a "conflicting block".  This only applies for the first instruction of this block, but is used in
    // the termination conditions below.
    AddressUser conflict;
    if (instructionExists(startVa).assignTo(conflict))
        ASSERT_forbid(conflict.insn()->get_address() == conflict.bblock()->address());// handled in discoverBasicBlock

    // Keep adding instructions until we reach a termination condition.  The termination conditions are enumerated in detail in
    // the doxygen documentation for this function. READ IT AND KEEP IT UP TO DATE!!!
    BasicBlock::Ptr retval = BasicBlock::instance(startVa, this);
    rose_addr_t va = startVa;
    while (1) {
        SgAsmInstruction *insn = discoverInstruction(va);
        if (insn==NULL)                                                 // case: no instruction available
            goto done;
        retval->append(insn);
        if (insn->is_unknown())                                         // case: "unknown" instruction
            goto done;

        // Give user chance to adjust basic block successors and/or pre-compute cached analysis results
        successorCallbacks_.apply(true, SuccessorCallback::Args(this, retval));

        BOOST_FOREACH (rose_addr_t successorVa, basicBlockConcreteSuccessors(retval)) {
            if (successorVa!=startVa && retval->instructionExists(successorVa)) { // case: successor is inside our own block
                retval->pop();
                goto done;
            }
        }
        
        if (basicBlockIsFunctionCall(retval))                           // case: bb looks like a function call
            break;
        BasicBlock::Successors successors = basicBlockSuccessors(retval);

        if (successors.size()!=1)                                       // case: not exactly one successor
            break;
        SValuePtr successorExpr = successors.front().expr();

        if (!successorExpr->is_number())                                // case: successor is indeterminate
            break;
        rose_addr_t successorVa = successorExpr->get_number();

        if (successorVa == startVa)                                     // case: successor is our own basic block
            goto done;

        if (findPlaceholder(successorVa)!=cfg_.vertices().end())      // case: successor is an existing block
            goto done;

        AddressUser addressUser;
        if (instructionExists(successorVa).assignTo(addressUser)) {     // case: successor is inside an existing block
            if (addressUser.bblock() != conflict.bblock())
                goto done;
        }

        va = successorVa;
    }
done:
    retval->freeze();
    return retval;
}

Partitioner::ControlFlowGraph::VertexNodeIterator
Partitioner::truncateBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, SgAsmInstruction *insn) {
    ASSERT_require(placeholder != cfg_.vertices().end());
    ASSERT_not_null(insn);
    BasicBlock::Ptr bblock = placeholder->value().bblock();
    if (bblock==NULL) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " has no basic block for truncation");
    }
    if (insn==bblock->instructions().front())
        throw BasicBlockError(bblock, basicBlockName(bblock) + " cannot be truncated at its initial instruction");
    if (!bblock->instructionExists(insn)) {
        throw BasicBlockError(bblock, basicBlockName(bblock) +
                              " does not contain instruction \"" + unparseInstructionWithAddress(insn) + "\""
                              " for truncation");
    }

    // For now we do a niave approach; this could be faster [Robb P. Matzke 2014-08-02]
    detachBasicBlock(placeholder);                      // throw away the original block
    ControlFlowGraph::VertexNodeIterator newPlaceholder = insertPlaceholder(insn->get_address());
    BasicBlock::Ptr newBlock = discoverBasicBlock(placeholder); // rediscover original block, but terminate at newPlaceholder
    attachBasicBlock(placeholder, newBlock);            // insert new block at original placeholder and insert successor edge
    return newPlaceholder;
}

Partitioner::ControlFlowGraph::VertexNodeIterator
Partitioner::insertPlaceholder(rose_addr_t startVa) {
    ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(startVa);
    if (placeholder == cfg_.vertices().end()) {
        AddressUser addressUser;
        if (instructionExists(startVa).assignTo(addressUser)) {
            ControlFlowGraph::VertexNodeIterator conflictBlock = findPlaceholder(addressUser.bblock()->address());
            placeholder = truncateBasicBlock(conflictBlock, addressUser.insn());
            ASSERT_require(placeholder->value().address() == startVa);
        } else {
            placeholder = cfg_.insertVertex(CfgVertex(startVa));
            vertexIndex_.insert(startVa, placeholder);
            adjustPlaceholderEdges(placeholder);
            bblockAttached(placeholder);
        }
    }
    return placeholder;
}

void
Partitioner::attachBasicBlock(const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);
    ControlFlowGraph::VertexNodeIterator placeholder = insertPlaceholder(bblock->address()); // insert or find existing
    attachBasicBlock(placeholder, bblock);
}

void
Partitioner::attachBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, const BasicBlock::Ptr &bblock) {
    ASSERT_require(placeholder != cfg_.vertices().end());
    ASSERT_require(placeholder->value().type() == V_BASICBLOCK);
    ASSERT_not_null(bblock);

    if (placeholder->value().address() != bblock->address()) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " cannot hold " + basicBlockName(bblock));
    }
    if (placeholder->value().bblock() == bblock)
        return;                                         // nothing to do since basic block is already in the CFG
    if (placeholder->value().bblock() != NULL) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " already holds a different basic block");
    }

    bblock->freeze();
    bool isFunctionCall = basicBlockIsFunctionCall(bblock);

    // Make sure placeholders exist for the concrete successors
    bool hadIndeterminate = false;
    typedef std::pair<ControlFlowGraph::VertexNodeIterator, CfgEdge> VertexEdgePair;
    std::vector<VertexEdgePair> successors;
    BOOST_FOREACH (const BasicBlock::Successor &successor, basicBlockSuccessors(bblock)) {
        CfgEdge edge(isFunctionCall ? E_FCALL : E_NORMAL);
        if (successor.expr()->is_number()) {
            successors.push_back(VertexEdgePair(insertPlaceholder(successor.expr()->get_number()), edge));
        } else if (!hadIndeterminate) {
            successors.push_back(VertexEdgePair(indeterminateVertex_, edge));
            hadIndeterminate = true;
        }
    }

    // Function calls get an additional return edge because we assume they return to the fall-through address
    if (isFunctionCall) {
        successors.push_back(VertexEdgePair(insertPlaceholder(bblock->fallthroughVa()), CfgEdge(E_FRET)));
    }

    // Make CFG edges
    cfg_.clearOutEdges(placeholder);
    BOOST_FOREACH (const VertexEdgePair &pair, successors)
        cfg_.insertEdge(placeholder, pair.first, pair.second);

    // Insert the basicblock
    placeholder->value().bblock(bblock);
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        aum_.insertInstruction(insn, bblock);
    }
    if (bblock->isEmpty())
        adjustNonexistingEdges(placeholder);

    bblockAttached(placeholder);
}

Partitioner::BasicBlock::Successors
Partitioner::basicBlockSuccessors(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    BasicBlock::Successors successors;

    if (bb->isEmpty() || bb->successors().getOptional().assignTo(successors))
        return successors;

    SgAsmInstruction *lastInsn = bb->instructions().back();
    RegisterDescriptor REG_IP = instructionProvider_.instructionPointerRegister();

    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // Use our own semantics if we have them.
        ASSERT_not_null(bb->dispatcher());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        std::vector<Semantics::SValuePtr> worklist(1, Semantics::SValue::promote(ops->readRegister(REG_IP)));
        while (!worklist.empty()) {
            Semantics::SValuePtr pc = worklist.back();
            worklist.pop_back();

            // Special handling for if-then-else expressions
            if (InsnSemanticsExpr::InternalNodePtr ifNode = pc->get_expression()->isInternalNode()) {
                if (ifNode->get_operator()==InsnSemanticsExpr::OP_ITE) {
                    Semantics::SValuePtr expr = Semantics::SValue::promote(ops->undefined_(ifNode->get_nbits()));
                    expr->set_expression(ifNode->child(1));
                    worklist.push_back(expr);
                    expr = Semantics::SValue::promote(ops->undefined_(ifNode->get_nbits()));
                    expr->set_expression(ifNode->child(2));
                    worklist.push_back(expr);
                    continue;
                }
            }

            successors.push_back(BasicBlock::Successor(pc));
        }

    } else {
        // We don't have semantics, so delegate to the SgAsmInstruction subclass (which might try some other semantics).
        bool complete = true;
#if 0 // [Robb P. Matzke 2014-08-16]
        // Look at the entire basic block to try to figure out the successors.  We already did something very similar above, so
        // if our try failed then this one probably will too.  In fact, this one will be even slower because it must reprocess
        // the entire basic block each time it's called because it is stateless, whereas ours above only needed to process each
        // instruction as it was appended to the block.
        std::set<rose_addr_t> successorVas = lastInsn->get_successors(bb->instructions(), &complete, &memoryMap_);
#else
        // Look only at the final instruction of the basic block.  This is probably quite fast compared to looking at a whole
        // basic block.
        std::set<rose_addr_t> successorVas = lastInsn->get_successors(&complete);
#endif
        BOOST_FOREACH (rose_addr_t va, successorVas)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::instance(REG_IP.get_nbits(), va)));
        if (!complete)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::instance(REG_IP.get_nbits())));
    }

    // We don't want parallel edges in the CFG, so remove duplicates.
    std::sort(successors.begin(), successors.end(), sortByExpression);
    for (size_t i=1; i<successors.size(); /*void*/) {
        if (successors[i-1].expr()->get_expression()->equivalent_to(successors[i].expr()->get_expression())) {
            ASSERT_require(successors[i-1].type() == successors[i].type());
            successors.erase(successors.begin()+i);
        } else {
            ++i;
        }
    }
    bb->successors() = successors;
    return successors;
}

std::set<rose_addr_t>
Partitioner::basicBlockGhostSuccessors(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    std::set<rose_addr_t> ghosts;

    if (bb->isEmpty() || bb->ghostSuccessors().getOptional().assignTo(ghosts))
        return ghosts;
    
    std::set<rose_addr_t> insnVas;
    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
        insnVas.insert(insn->get_address());
    
    const BasicBlock::Successors &successors = basicBlockSuccessors(bb);
    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
        bool complete = true;
        BOOST_FOREACH (rose_addr_t naiveVa, insn->get_successors(&complete)) {
            if (insnVas.find(naiveVa)==insnVas.end()) {
                bool found = false;
                BOOST_FOREACH (const BasicBlock::Successor &successor, successors) {
                    if (successor.expr()->is_number() && successor.expr()->get_number()==naiveVa) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    ghosts.insert(naiveVa);
            }
        }
    }
    bb->ghostSuccessors() = ghosts;
    return ghosts;
}

std::vector<rose_addr_t>
Partitioner::basicBlockConcreteSuccessors(const BasicBlock::Ptr &bb) const {
    std::vector<rose_addr_t> retval;
    BOOST_FOREACH (const BasicBlock::Successor &successor, basicBlockSuccessors(bb)) {
        if (successor.expr()->is_number())
            retval.push_back(successor.expr()->get_number());
    }
    return retval;
}

bool
Partitioner::basicBlockIsFunctionCall(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    bool retval = false;

    if (bb->isEmpty() || bb->isFunctionCall().getOptional().assignTo(retval))
        return retval;

    SgAsmInstruction *lastInsn = bb->instructions().back();

    // Use our own semantics if we have them.
    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // Is the block fall-through address equal to the value on the top of the stack?
        ASSERT_not_null(bb->dispatcher());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        RegisterDescriptor REG_IP = instructionProvider_.instructionPointerRegister();
        RegisterDescriptor REG_SP = instructionProvider_.stackPointerRegister();
        RegisterDescriptor REG_SS = instructionProvider_.stackSegmentRegister();
        rose_addr_t returnVa = bb->fallthroughVa();
        BaseSemantics::SValuePtr returnExpr = ops->number_(REG_IP.get_nbits(), returnVa);
        BaseSemantics::SValuePtr sp = ops->readRegister(REG_SP);
        BaseSemantics::SValuePtr topOfStack = ops->undefined_(REG_IP.get_nbits());
        topOfStack = ops->readMemory(REG_SS, sp, topOfStack, ops->boolean_(true));
        BaseSemantics::SValuePtr z = ops->equalToZero(ops->add(returnExpr, ops->negate(topOfStack)));
        bool isRetAddrOnTopOfStack = z->is_number() ? (z->get_number()!=0) : false;
        if (!isRetAddrOnTopOfStack) {
            bb->isFunctionCall() = false;
            return false;
        }
        
        // If the only successor is also the fall-through address then this isn't a function call.  This case handles code that
        // obtains the code address in position independent code. For example, x86 "A: CALL B; B: POP EAX" where A and B are
        // consecutive instruction addresses.
        BasicBlock::Successors successors = basicBlockSuccessors(bb);
        if (1==successors.size() && successors[0].expr()->is_number() && successors[0].expr()->get_number()==returnVa) {
            bb->isFunctionCall() = false;
            return false;
        }

        // This appears to be a function call
        bb->isFunctionCall() = true;
        return true;
    }

    // We don't have semantics, so delegate to the SgAsmInstruction subclass (which might try some other semantics).
    retval = lastInsn->is_function_call(bb->instructions(), NULL, NULL);
    bb->isFunctionCall() = retval;
    return retval;
}

BaseSemantics::SValuePtr
Partitioner::basicBlockStackDelta(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);

    BaseSemantics::SValuePtr delta;
    if (bb->stackDelta().getOptional().assignTo(delta))
        return delta;

    if (bb->finalState() != NULL) {
        ASSERT_not_null(bb->initialState());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        ASSERT_not_null(ops);
        RegisterDescriptor REG_SP = instructionProvider_.stackPointerRegister();
        BaseSemantics::SValuePtr sp0 = bb->initialState()->readRegister(REG_SP, ops.get());
        BaseSemantics::SValuePtr spN = bb->finalState()->readRegister(REG_SP, ops.get());
        delta = ops->add(spN, ops->negate(sp0));
    }
    bb->stackDelta() = delta;
    return delta;
}

SgAsmInstruction *
Partitioner::discoverInstruction(rose_addr_t startVa) const {
    return instructionProvider_[startVa];
}

// FIXME[Robb P. Matzke 2014-08-12]: nBytes to be replaced by a data type
void
Partitioner::attachFunctionDataBlock(const Function::Ptr &function, rose_addr_t startVa, size_t nBytes) {
    ASSERT_not_null(function);
    ASSERT_require(nBytes>0);

    if (functionExists(function)) {
        DataBlocks::NodeIterator dbi = dblocks_.find(startVa);
        if (dbi==dblocks_.nodes().end()) {
            // No data block starts at this address, so create a new one
            DataBlock::Ptr dblock = DataBlock::instance(startVa, nBytes);
            dblocks_.insert(startVa, OwnedDataBlock(dblock, function));
            aum_.insertDataBlock(dblock);
            function->thaw();
            function->insertDataBlock(dblock);
            function->freeze();
        } else {
            DataBlock::Ptr dblock = dbi->value().dblock();
            if (dblock->size() < nBytes) {
                // An existing data block, but it's too small. We can make it larger.
                aum_.eraseDataBlock(dblock);            // remove the small version from the AUM
                dblock->thaw();
                dblock->size(nBytes);
                dblock->freeze();
                aum_.insertDataBlock(dblock);           // insert the larger version into the AUM
                dbi->value().insert(function);          // ensure this function is an owner
                function->thaw();
                function->insertDataBlock(dblock);
                function->freeze();
            } else {
                // An existing data block that is large enough
                dbi->value().insert(function);          // ensure this function is an owner
                function->thaw();
                function->insertDataBlock(dblock);
                function->freeze();
            }
        }
    } else {
        ASSERT_not_implemented("[Robb P. Matzke 2014-08-12]");
    }
}

Partitioner::Function::Ptr
Partitioner::findFunctionOwningBasicBlock(rose_addr_t bblockVa) const {
    ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(bblockVa);
    return placeholder != cfg_.vertices().end() ? placeholder->value().function() : Function::Ptr();
}

Partitioner::Function::Ptr
Partitioner::findFunctionOwningBasicBlock(const BasicBlock::Ptr &bblock) const {
    return bblock==NULL ? Function::Ptr() : findFunctionOwningBasicBlock(bblock->address());
}

std::vector<Partitioner::Function::Ptr>
Partitioner::findFunctionsOwningBasicBlocks(const std::vector<rose_addr_t> &bblockVas) const {
    typedef std::vector<Partitioner::Function::Ptr> Functions;
    Functions functions;
    BOOST_FOREACH (rose_addr_t blockVa, bblockVas) {
        if (Function::Ptr function = findFunctionOwningBasicBlock(blockVa)) {
            Functions::iterator lb = std::lower_bound(functions.begin(), functions.end(), function, sortFunctionsByAddress);
            if (lb==functions.end() || (*lb)->address()!=function->address())
                functions.insert(lb, function);
        }
    }
    return functions;
}

std::vector<Partitioner::Function::Ptr>
Partitioner::findFunctionsOwningBasicBlocks(const std::vector<BasicBlock::Ptr> &bblocks) const {
    std::vector<rose_addr_t> bblockVas;
    BOOST_FOREACH (const BasicBlock::Ptr &bblock, bblocks) {
        if (bblock!=NULL)
            bblockVas.push_back(bblock->address());
    }
    return findFunctionsOwningBasicBlocks(bblockVas);
}

std::vector<Partitioner::Function::Ptr>
Partitioner::functionsOverlapping(const AddressInterval &interval) const {
    std::vector<Function::Ptr> functions;
    AddressUsers users = aum_.overlapping(interval);
    BOOST_FOREACH (const AddressUser &user, users.addressUsers()) {
        if (BasicBlock::Ptr bb = user.bblock()) {
            if (Function::Ptr function = findFunctionOwningBasicBlock(bb))
                sortedInsert(functions, function);
        } else if (DataBlock::Ptr db = user.dblock()) {
            OwnedDataBlock odb = *dataBlockExists(db->address());
            BOOST_FOREACH (const Function::Ptr &function, odb.owningFunctions())
                sortedInsert(functions, function);
        }
    }
    return functions;
}

void
Partitioner::bblockAttached(const ControlFlowGraph::VertexNodeIterator &newVertex) {
    ASSERT_require(newVertex!=cfg_.vertices().end());
    ASSERT_require(newVertex->value().type() == V_BASICBLOCK);
    if (isReportingProgress_)
        reportProgress();
    rose_addr_t startVa = newVertex->value().address();
    BasicBlock::Ptr bblock = newVertex->value().bblock();

    Stream debug(mlog[DEBUG]);
    if (debug) {
        if (bblock) {
            if (bblock->isEmpty()) {
                debug <<"attached empty basic block at " <<StringUtility::addrToString(startVa) <<"\n";
            } else {
                debug <<"attached basic block:\n";
                BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
                    debug <<"  + " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        } else {
            debug <<"inserted basic block placeholder at " <<StringUtility::addrToString(startVa) <<"\n";
        }
    }

#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::AttachedBasicBlock(this, startVa, bblock));
#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
}

void
Partitioner::bblockDetached(rose_addr_t startVa, const BasicBlock::Ptr &bblock) {
    Stream debug(mlog[DEBUG]);
    if (debug) {
        if (bblock) {
            if (bblock->isEmpty()) {
                debug <<"detached empty basic block at " <<StringUtility::addrToString(startVa) <<"\n";
            } else {
                debug <<"detached basic block:\n";
                BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
                    debug <<"  - " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        } else {
            debug <<"erased basic block placeholder at " <<StringUtility::addrToString(startVa) <<"\n";
        }
    }

#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::DetachedBasicBlock(this, startVa, bblock));
#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
}

void
Partitioner::checkConsistency() const {
#ifndef NDEBUG
    static const bool extraDebuggingOutput = false;
    using namespace StringUtility;
    Stream debug(mlog[DEBUG]);
    if (extraDebuggingOutput)
        debug <<"checking partitioner consistency...\n";
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASICBLOCK) {
            if (extraDebuggingOutput && debug) {
                debug <<"  basic block" <<(vertex.value().bblock()?"":" placeholder")
                      <<" " <<addrToString(vertex.value().address()) <<"\n";
                debug <<"    outgoing edges:";
                BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex.outEdges()) {
                    const ControlFlowGraph::VertexNode &target = *edge.target();
                    switch (target.value().type()) {
                        case V_BASICBLOCK:
                            debug <<" " <<addrToString(target.value().address());
                            break;
                        case V_INDETERMINATE:
                            debug <<" indeterminate";
                            break;
                        case V_NONEXISTING:
                            debug <<" nonexisting";
                            break;
                        case V_UNDISCOVERED:
                            debug <<" undiscovered";
                            break;
                    }
                }
                debug <<"\n";
            }

            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                ASSERT_require(bb->isFrozen());
                ASSERT_require(bb->address() == vertex.value().address());
                if (bb->isEmpty()) {
                    // Non-existing basic block
                    ASSERT_require2(vertex.nOutEdges()==1,
                                    "nonexisting block " + addrToString(bb->address()) + " must have one outgoing edge");
                    ControlFlowGraph::ConstEdgeNodeIterator edge = vertex.outEdges().begin();
                    ASSERT_require2(edge->target() == nonexistingVertex_,
                                    "nonexisting block " + addrToString(bb->address()) + " edges must go to a special vertex");
                } else {
                    // Existing basic block
                    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                        BasicBlock::Ptr bb2 = aum_.instructionExists(insn);
                        ASSERT_not_null2(bb2,
                                         "instruction " + addrToString(insn->get_address()) + " in block " +
                                         addrToString(bb->address()) + " must be present in the AUM");
                        ASSERT_require2(bb2 == bb,
                                        "instruction " + addrToString(insn->get_address()) + " in block " +
                                        addrToString(bb->address()) + " must belong to correct basic block");
                        AddressInterval insnInterval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
                        AddressUsers addressUsers = aum_.spanning(insnInterval);
                        ASSERT_require2(addressUsers.instructionExists(insn),
                                        "instruction " + addrToString(insn->get_address()) + " in block " +
                                        addrToString(bb->address()) + " must span its own address interval in the AUM");
                    }
                }
            } else {
                // Basic block placeholder
                ASSERT_require2(vertex.nOutEdges() == 1,
                                "placeholder " + addrToString(vertex.value().address()) +
                                " must have exactly one outgoing edge");
                ControlFlowGraph::ConstEdgeNodeIterator edge = vertex.outEdges().begin();
                ASSERT_require2(edge->target() == undiscoveredVertex_,
                                "placeholder " + addrToString(vertex.value().address()) +
                                " edge must go to a special vertex");
            }

            ASSERT_require2(vertexIndex_.exists(vertex.value().address()),
                            "bb/placeholder " + addrToString(vertex.value().address()) +
                            " must exist in the vertex index");

        } else {
            // Special vertices
            ASSERT_require2(vertex.nOutEdges()==0,
                            "special vertices must have no outgoing edges");
        }
    }
#endif
}

// class method
std::string
Partitioner::basicBlockName(const BasicBlock::Ptr &bblock) {
    return bblock==NULL ? "no basic block" : bblock->printableName();
}

// class method
std::string
Partitioner::dataBlockName(const DataBlock::Ptr &dblock) {
    return dblock==NULL ? "no data block" : dblock->printableName();
}

// class method
std::string
Partitioner::functionName(const Function::Ptr &function) {
    return function==NULL ? "no function" : function->printableName();
}

// class method
std::string
Partitioner::vertexName(const ControlFlowGraph::VertexNode &vertex) {
    switch (vertex.value().type()) {
        case V_BASICBLOCK: {
            std::string retval = StringUtility::addrToString(vertex.value().address()) +
                                 "<" + StringUtility::numberToString(vertex.id());
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                if (bb->isEmpty())
                    retval += ",X";                     // non-existing
            } else {
                retval += ",P";                         // placeholder
            }
            return retval + ">";
        }
        case V_INDETERMINATE:
            return "indeterminate<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_NONEXISTING:
            return "non-existing<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_UNDISCOVERED:
            return "undiscovered<" + StringUtility::numberToString(vertex.id()) + ">";
    }
}

// class method
std::string
Partitioner::vertexNameEnd(const ControlFlowGraph::VertexNode &vertex) {
    if (vertex.value().type() == V_BASICBLOCK) {
        if (BasicBlock::Ptr bb = vertex.value().bblock()) {
            if (!bb->isEmpty()) {
                return vertexName(vertex) + ":" + StringUtility::addrToString(bb->instructions().back()->get_address());
            }
        }
    }
    return vertexName(vertex);
}

// class method
std::string
Partitioner::edgeNameDst(const ControlFlowGraph::EdgeNode &edge) {
    std::string retval;
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FCALL:
            retval += "<fcall>";
            break;
        case E_FRET:
            retval += "<fret>";
            break;
    }
    return retval + vertexName(*edge.target());
}

// class method
std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::EdgeNode &edge) {
    const ControlFlowGraph::VertexNode &source = *edge.source();
    std::string retval = vertexNameEnd(source);
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FCALL:
            retval += "<fcall>";
            break;
        case E_FRET:
            retval += "<fret>";
            break;
    }
    return retval;
}

// class method
std::string
Partitioner::edgeName(const ControlFlowGraph::EdgeNode &edge) {
    std::string retval = vertexNameEnd(*edge.source()) + " ";
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FCALL:
            retval += "(fcall)";
            break;
        case E_FRET:
            retval += "(fret)";
            break;
    }
    return retval + "-> " + vertexName(*edge.target());
}

void
Partitioner::dumpCfg(std::ostream &out, const std::string &prefix, bool showBlocks) const {
    AsmUnparser unparser;
    const std::string insnPrefix = prefix + "    ";
    unparser.insnRawBytes.fmt.prefix = insnPrefix.c_str();
    unparser.set_registers(instructionProvider_.registerDictionary());

    // Sort the vertices according to basic block starting address.
    std::vector<ControlFlowGraph::ConstVertexNodeIterator> sortedVertices;
    for (ControlFlowGraph::ConstVertexNodeIterator vi=cfg_.vertices().begin(); vi!=cfg_.vertices().end(); ++vi) {
        if (vi->value().type() == V_BASICBLOCK)
            sortedVertices.push_back(vi);
    }
    std::sort(sortedVertices.begin(), sortedVertices.end(), sortVerticesByAddress);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexNodeIterator &vertex, sortedVertices) {
        out <<prefix <<"basic block " <<vertexName(*vertex);
        if (vertex->value().function()) {
            if (vertex->value().function()->address() == vertex->value().address()) {
                out <<" entry block for " <<functionName(vertex->value().function());
            } else {
                out <<" owned by " <<functionName(vertex->value().function());
            }
        }
        out <<"\n";

        // Sort incoming edges according to source (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeNodeIterator> sortedInEdges;
        for (ControlFlowGraph::ConstEdgeNodeIterator ei=vertex->inEdges().begin(); ei!=vertex->inEdges().end(); ++ei)
            sortedInEdges.push_back(ei);
        std::sort(sortedInEdges.begin(), sortedInEdges.end(), sortEdgesBySrc);
        out <<prefix <<"  predecessors:";
        if (sortedInEdges.empty()) {
            out <<" none\n";
        } else {
            BOOST_FOREACH (const ControlFlowGraph::ConstEdgeNodeIterator &edge, sortedInEdges)
                out <<" " <<edgeNameSrc(*edge);
            out <<"\n";
        }

        // Show instructions in execution order
        if (showBlocks) {
            if (BasicBlock::Ptr bb = vertex->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                    out <<insnPrefix;                   // hexdump does not prefix the first line
                    unparser.unparse(out, insn);
                }
            }
        }

        // Warn if instruction semantics failed
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (bb->finalState()==NULL)
                out <<prefix <<"  semantics failed\n";
        }

        // Stack delta
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            BaseSemantics::SValuePtr delta = basicBlockStackDelta(bb);
            if (delta == NULL) {
                out <<prefix <<"  stack delta: not available\n";
            } else {
                out <<prefix <<"  stack delta: " <<*delta <<"\n";
            }
        }
        
        // Sort outgoing edges according to destination (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeNodeIterator> sortedOutEdges;
        for (ControlFlowGraph::ConstEdgeNodeIterator ei=vertex->outEdges().begin(); ei!=vertex->outEdges().end(); ++ei)
            sortedOutEdges.push_back(ei);
        std::sort(sortedOutEdges.begin(), sortedOutEdges.end(), sortEdgesByDst);
        out <<prefix <<"  successors:";
        if (sortedOutEdges.empty()) {
            out <<" none\n";
        } else {
            BOOST_FOREACH (const ControlFlowGraph::ConstEdgeNodeIterator &edge, sortedOutEdges)
                out <<" " <<edgeNameDst(*edge);
            out <<"\n";
        }

        // Ghost edges
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            std::set<rose_addr_t> ghosts = basicBlockGhostSuccessors(bb);
            if (!ghosts.empty()) {
                out <<prefix <<"  ghost successors:";
                BOOST_FOREACH (rose_addr_t ghost, ghosts)
                    out <<" " <<StringUtility::addrToString(ghost);
                out <<"\n";
            }
        }
    }
}

Partitioner::Function::Ptr
Partitioner::nextFunctionPrologue(rose_addr_t startVa) {
    while (memoryMap_.next(startVa, MemoryMap::MM_PROT_EXEC).assignTo(startVa)) {
        Sawyer::Optional<rose_addr_t> unmappedVa = aum_.leastUnmapped(startVa);
        if (!unmappedVa)
            return Function::Ptr();                   // no higher unused address
        if (startVa == *unmappedVa) {
            BOOST_FOREACH (const FunctionPrologueMatcher::Ptr &matcher, functionPrologueMatchers_) {
                if (matcher->match(this, startVa)) {
                    if (Function::Ptr function = matcher->function())
                        return function;
                }
            }
            ++startVa;
        } else {
            startVa = *unmappedVa;
        }
    }
    return Function::Ptr();
}

size_t
Partitioner::attachFunctions(const Functions &functions) {
    size_t nNewBlocks = 0;
    BOOST_FOREACH (const Function::Ptr function, functions.values())
        nNewBlocks += attachFunction(function);
    return nNewBlocks;
}

size_t
Partitioner::attachFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);

    size_t nNewBlocks = 0;
    Function::Ptr exists;
    if (functions_.getOptional(function->address()).assignTo(exists)) {
        if (exists != function)
            throw FunctionError(function, functionName(function) + " is already attached with a different function pointer");
        ASSERT_require(function->isFrozen());
    } else {
        // Insert function into the table, and make sure all its basic blocks see that they're owned by the function.
        functions_.insert(function->address(), function);
        nNewBlocks = attachFunctionBasicBlocks(function);

        // Insert function data blocks into the data block table and AUM.  Data blocks can be owned by multiple functions, but
        // there can be only one data block per starting address.
        BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks()) {
            ASSERT_not_null(dblock);
            dblock->freeze();                           // make sure it doesn't change while in the AUM
            OwnedDataBlock &odb = dblocks_.insertMaybe(dblock->address(), OwnedDataBlock(dblock));
            if (odb.dblock() != dblock)
                throw DataBlockError(odb.dblock(),
                                     dataBlockName(odb.dblock()) + " is already attached with a different data block pointer");
            bool newlyCreated = odb.nOwners()==0;
            odb.insert(function);
            if (newlyCreated)
                aum_.insertDataBlock(dblock);
        }

        // Prevent the function connectivity from changing while the function is in the CFG.  Non-frozen functions
        // can have basic blocks added and erased willy nilly because the basic blocks don't need to know that they're owned by
        // a function.  But we can't have that when the function is one that's part of the CFG.
        function->freeze();
    }
    return nNewBlocks;
}

size_t
Partitioner::attachFunctionBasicBlocks(const Functions &functions) {
    size_t nNewBlocks = 0;
    BOOST_FOREACH (const Function::Ptr function, functions.values())
        nNewBlocks += attachFunctionBasicBlocks(function);
    return nNewBlocks;
}

size_t
Partitioner::attachFunctionBasicBlocks(const Function::Ptr &function) {
    ASSERT_not_null(function);
    size_t nNewBlocks = 0;
    bool functionExists = functions_.exists(function->address());
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(blockVa);
        if (placeholder == cfg_.vertices().end()) {
            placeholder = insertPlaceholder(blockVa);
            ++nNewBlocks;
        }
        if (functionExists) {
            if (placeholder->value().function()!=NULL && placeholder->value().function()!=function) {
                throw FunctionError(function,
                                    functionName(function) + " basic block " + vertexName(*placeholder) +
                                    " is already owned by " + functionName(placeholder->value().function()));
            }
            placeholder->value().function(function);
        }
    }
    return nNewBlocks;
}

void
Partitioner::detachFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);
    if (functionExists(function->address()) != function)
        return;                                         // already detached

    // Unlink basic blocks
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(blockVa);
        ASSERT_require(placeholder != cfg_.vertices().end());
        ASSERT_require(placeholder->value().type() == V_BASICBLOCK);
        ASSERT_require(placeholder->value().function() == function);
        placeholder->value().function(Function::Ptr());
    }

    // Unlink data blocks
    BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks()) {
        ASSERT_not_null(dblock);
        DataBlocks::ValueIterator found = dblocks_.find(dblock->address());
        ASSERT_require(found!=dblocks_.nodes().end());
        if (0==found->erase(function)) {
            // This function was the last attached owner of the data block, so erase the data block from the data block table
            // and the address usage map and thaw it.
            aum_.eraseDataBlock(dblock);
            dblocks_.eraseAt(found);
            dblock->thaw();
        }
    }

    // Unlink the function itself
    functions_.erase(function->address());
    function->thaw();
}

Partitioner::AddressUsageMap
Partitioner::aum(const Function::Ptr &function) const {
    AddressUsageMap retval;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(blockVa);
        if (placeholder != cfg_.vertices().end()) {
            ASSERT_require(placeholder->value().type() == V_BASICBLOCK);
            if (BasicBlock::Ptr bb = placeholder->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    retval.insertInstruction(insn, bb);
            }
        }
    }
    return retval;
}

std::set<rose_addr_t>
Partitioner::ghostSuccessors() const {
    std::set<rose_addr_t> ghosts;
    BOOST_FOREACH (const CfgVertex &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASICBLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock()) {
                BOOST_FOREACH (rose_addr_t ghost, basicBlockGhostSuccessors(bb)) {
                    ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(ghost);
                    if (placeholder == cfg_.vertices().end())
                        ghosts.insert(ghost);
                }
            }
        }
    }
    return ghosts;
}

std::vector<Partitioner::Function::Ptr>
Partitioner::functions() const {
    std::vector<Function::Ptr> functions;
    functions.reserve(functions_.size());
    BOOST_FOREACH (const Function::Ptr &function, functions_.values())
        functions.push_back(function);
    ASSERT_require(isSorted(functions, sortFunctionsByAddress, true));
    return functions;
}

std::vector<Partitioner::Function::Ptr>
Partitioner::discoverFunctionEntryVertices() const {
    std::vector<Function::Ptr> functions;

    // Entry points we already know about. */
    functions.reserve(functions_.size());
    BOOST_FOREACH (const Function::Ptr &knownFunction, functions_.values())
        functions.push_back(knownFunction);
    ASSERT_require(isSorted(functions, sortFunctionsByAddress, true));

    // Find additional function entry points by looking for edges that are marked as function calls.  We process one vertex at
    // a time so we can short-circuit once we find a function edge.
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASICBLOCK && !functions_.exists(vertex.value().address())) {
            BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex.inEdges()) {
                if (edge.value().type() == E_FCALL) {
                    rose_addr_t entryVa = vertex.value().address();
                    sortedInsert(functions, Function::instance(entryVa));
                    break;
                }
            }
        }
    }

    return functions;
}

size_t
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function,
                                         EdgeList *inwardInterFunctionEdges /*out*/,
                                         EdgeList *outwardInterFunctionEdges /*out*/) {
    std::vector<size_t> inwardIds, outwardIds;
    size_t retval = discoverFunctionBasicBlocks(function, inwardIds /*out*/, outwardIds /*out*/);
    if (inwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, inwardIds)
            inwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    if (outwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, outwardIds)
            outwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    return retval;
}

size_t
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function,
                                         ConstEdgeList *inwardInterFunctionEdges /*out*/,
                                         ConstEdgeList *outwardInterFunctionEdges /*out*/) const {
    std::vector<size_t> inwardIds, outwardIds;
    size_t retval = discoverFunctionBasicBlocks(function, inwardIds /*out*/, outwardIds /*out*/);
    if (inwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, inwardIds)
            inwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    if (outwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, outwardIds)
            outwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    return retval;
}

size_t
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function,
                                         std::vector<size_t> &inwardInterFunctionEdges /*out*/,
                                         std::vector<size_t> &outwardInterFunctionEdges /*out*/) const {
    ASSERT_not_null(function);
    if (function->isFrozen())
        throw FunctionError(function, functionName(function) +
                            " is frozen or attached to CFG/AUM when discovering basic blocks");
    
    Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"discoverFunctionBlocks(" <<functionName(function) <<")\n";
    size_t nConflictsOrig = inwardInterFunctionEdges.size() + outwardInterFunctionEdges.size();

    typedef Sawyer::Container::Map<size_t /*vertexId*/, Function::Ownership> VertexOwnership;
    VertexOwnership ownership;                          // contains only OWN_EXPLICIT and OWN_PROVISIONAL entries

    // Find the vertices that this function already owns
    BOOST_FOREACH (rose_addr_t startVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(startVa);
        if (placeholder == cfg_.vertices().end()) {
            throw Exception("block " + StringUtility::addrToString(startVa) + " of " + functionName(function) +
                            " must exist in the CFG");
        }
        ownership.insert(placeholder->id(), Function::OWN_EXPLICIT);
        SAWYER_MESG(debug) <<"  explicitly owns vertex " <<placeholder->id() <<"\n";
    }

    // Find all the unowned vertices we can reach from the previously owned vertices following non-function-call edges and
    // excluding special target vertices.  The loop is guaranteed to never visit the same vertex more than once because, we
    // only add a vertex to the worklist if it is not in the ownership set, adding it to the worklist and ownership set at the
    // same time.  The loop is also therefore guaranteed to not visit an edge more than once.
    Sawyer::Container::Stack<size_t> worklist(ownership.keys());
    while (!worklist.isEmpty()) {
        const ControlFlowGraph::VertexNode &source = *cfg_.findVertex(worklist.pop());
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, source.outEdges()) {
            if (edge.value().type()!=E_FCALL && !edge.isSelfEdge()) {
                const ControlFlowGraph::VertexNode &target = *edge.target();
                if (target.value().type()==V_BASICBLOCK && !ownership.exists(target.id())) {
                    if (target.value().function()) {
                        // Some other function already owns this vertex.  The edge is therefore an inter-function edge which
                        // was not labeled as a function call.  If the edge is to a known function entry block then we'll
                        // assume this should have been a function call edge and not traverse it, otherwise we'll have to let
                        // the user decide what to do.
                        if (!functionExists(target.value().address()))
                            outwardInterFunctionEdges.push_back(edge.id());
                    } else {
                        SAWYER_MESG(debug) <<"  following edge " <<edgeName(edge) <<"\n";
                        ownership.insert(target.id(), Function::OWN_PROVISIONAL);
                        worklist.push(target.id());
                        SAWYER_MESG(debug) <<"    provisionally owns vertex " <<target.id() <<"\n";
                    }
                }
            }
        }
    }

    // For all the provisionally-owned vertices other than this function's entry vertex, check that they have incoming edges
    // only from this same function.  We explicitly do not check the vertices that were previously owned (the user may have
    // wanted them in the function regardless of problems).
    BOOST_FOREACH (const VertexOwnership::Node &node, ownership.nodes()) {
        if (node.value() == Function::OWN_PROVISIONAL) {
            const ControlFlowGraph::VertexNode &target = *cfg_.findVertex(node.key());
            SAWYER_MESG(debug) <<"  testing provisional vertex " <<vertexName(target) <<"\n";
            if (target.value().address() != function->address()) {
                BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, target.inEdges()) {
                    const ControlFlowGraph::VertexNode &source = *edge.source();
                    SAWYER_MESG(debug) <<"    testing edge " <<edgeName(edge);
                    if (!ownership.exists(source.id())) {
                        // This edge crosses a function boundary yet is not labeled as a function call.
                        inwardInterFunctionEdges.push_back(edge.id());
                        if (debug) {
                            if (source.value().function()) {
                                debug <<"; edge source is owned by " <<functionName(source.value().function()) <<"\n";
                            } else {
                                debug <<"; edge is not owned by any function\n";
                            }
                        }
                    } else {
                        SAWYER_MESG(debug) <<"; ok\n";
                    }
                }
            }
        }
    }

    // If there were no conflicts then add all the provisional vertices to this function. This does not modify the CFG.
    if (inwardInterFunctionEdges.empty() && outwardInterFunctionEdges.empty()) {
        BOOST_FOREACH (const VertexOwnership::Node &node, ownership.nodes()) {
            if (node.value() == Function::OWN_PROVISIONAL)
                function->insertBasicBlock(cfg_.findVertex(node.key())->value().address());
        }
    }

    return inwardInterFunctionEdges.size() + outwardInterFunctionEdges.size() - nConflictsOrig;
}

std::set<rose_addr_t>
Partitioner::functionGhostSuccessors(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    std::set<rose_addr_t> ghosts;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        if (BasicBlock::Ptr bb = basicBlockExists(blockVa)) {
            BOOST_FOREACH (rose_addr_t ghost, basicBlockGhostSuccessors(bb)) {
                ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(ghost);
                if (placeholder == cfg_.vertices().end())
                    ghosts.insert(ghost);
            }
        }
    }
    return ghosts;
}

SgAsmBlock*
Partitioner::buildBasicBlockAst(const BasicBlock::Ptr &bb, bool relaxed) const {
    ASSERT_not_null(bb);
    if (bb->isEmpty()) {
        if (relaxed) {
            mlog[WARN] <<"creating an empty basic block AST node at " <<StringUtility::addrToString(bb->address()) <<"\n";
        } else {
            return NULL;
        }
    }

    ControlFlowGraph::ConstVertexNodeIterator bblockVertex = findPlaceholder(bb->address());
    SgAsmBlock *ast = SageBuilderAsm::buildBasicBlock(bb->instructions());
    unsigned reasons = 0;

    // Is this block an entry point for a function?
    if (Function::Ptr function = functionExists(bb->address())) {
        if (function->address() == bb->address())
            reasons |= SgAsmBlock::BLK_ENTRY_POINT;
    }

    // Does this block have any predecessors?
    if (bblockVertex != cfg_.vertices().end() && bblockVertex->nInEdges()==0)
        reasons |= SgAsmBlock::BLK_CFGHEAD;

    ast->set_reason(reasons);
    ast->set_code_likelihood(1.0);                      // FIXME[Robb P. Matzke 2014-08-07]

    // Cache the basic block successors in the AST since we've already computed them.  If the basic block is in the CFG then we
    // can use the CFG's edges to initialize the AST successors since they are canonical. Otherwise we'll use the successors
    // from bb. In any case, we fill in the successor SgAsmIntegerValueExpression objects with only the address and not
    // pointers to SgAsmBlock IR nodes since we don't have all the blocks yet; the SgAsmBlock pointers will be initialized
    // higher up on the AST-building stack.
    if (bblockVertex != cfg_.vertices().end()) {
        bool isComplete = true;
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, bblockVertex->outEdges()) {
            const ControlFlowGraph::VertexNode &target = *edge.target();
            if (target.value().type() == V_INDETERMINATE || target.value().type() == V_NONEXISTING) {
                isComplete = false;
            } else {
                ASSERT_require(target.value().type() == V_BASICBLOCK);
                SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(target.value().address());
                succ->set_parent(ast);
                ast->get_successors().push_back(succ);
            }
        }
        ast->set_successors_complete(isComplete);
    } else {
        bool isComplete = true;
        BOOST_FOREACH (const BasicBlock::Successor &successor, basicBlockSuccessors(bb)) {
            if (successor.expr()->is_number()) {
                SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(successor.expr()->get_number());
                succ->set_parent(ast);
                ast->get_successors().push_back(succ);
            } else {
                isComplete = false;
            }
        }
        ast->set_successors_complete(isComplete);
    }
    return ast;
}

SgAsmBlock*
Partitioner::buildDataBlockAst(const DataBlock::Ptr &dblock, bool relaxed) const {
    // Build the static data item
    SgUnsignedCharList rawBytes = memoryMap_.read(dblock->address(), dblock->size(), MemoryMap::MM_PROT_NONE);
    ASSERT_require(rawBytes.size()==dblock->size());
    SgAsmStaticData *datum = SageBuilderAsm::buildStaticData(dblock->address(), rawBytes);

    // Build the data block IR node pointing to the static item
    return SageBuilderAsm::buildDataBlock(datum);
}

SgAsmFunction*
Partitioner::buildFunctionAst(const Function::Ptr &function, bool relaxed) const {
    // Build the child basic block IR nodes
    std::vector<SgAsmBlock*> children;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator vertex = findPlaceholder(blockVa);
        if (vertex == cfg_.vertices().end()) {
            mlog[WARN] <<functionName(function) <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" does not exist in the CFG; no AST node created\n";
        } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (SgAsmBlock *child = buildBasicBlockAst(bb, relaxed))
                children.push_back(child);
        } else {
            mlog[WARN] <<functionName(function) <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" is undiscovered in the CFG; no AST node created\n";
        }
    }
    if (children.empty()) {
        if (relaxed) {
            mlog[WARN] <<"creating an empty AST node for " <<functionName(function) <<"\n";
        } else {
            return NULL;
        }
    }

    // Build the child data block IR nodes.
    BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks()) {
        if (SgAsmBlock *child = buildDataBlockAst(dblock, relaxed))
            children.push_back(child);
    }

    // Sort the children in the order usually used by ROSE
    std::sort(children.begin(), children.end(), sortBlocksForAst);

    unsigned reasons = 0;

    // Is the function part of the CFG?
    ControlFlowGraph::ConstVertexNodeIterator entryVertex = findPlaceholder(function->address());
    if (entryVertex != cfg_.vertices().end())
        reasons |= SgAsmFunction::FUNC_GRAPH;
    
    // Is the function discontiguous?
    if (aum(function).extent().nIntervals() > 1)
        reasons |= SgAsmFunction::FUNC_DISCONT;

    // Is the function the target of a function call?
    if (entryVertex != cfg_.vertices().end()) {
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, entryVertex->inEdges()) {
            if (edge.value().type() == E_FCALL) {
                reasons |= SgAsmFunction::FUNC_CALL_TARGET;
                break;
            }
        }
    }
    
    // Build the AST
    SgAsmFunction *ast = SageBuilderAsm::buildFunction(function->address(), children);
    ast->set_reason(reasons);
    ast->set_name("");                                  // FIXME[Robb P. Matzke 2014-08-07]
    ast->set_may_return(SgAsmFunction::RET_UNKNOWN);    // FIXME[Robb P. Matzke 2014-08-07]
    return ast;
}

SgAsmBlock*
Partitioner::buildGlobalBlockAst(bool relaxed) const {
    // Create the children first
    std::vector<SgAsmFunction*> children;
    BOOST_FOREACH (const Function::Ptr &function, functions_.values()) {
        if (SgAsmFunction *func = buildFunctionAst(function, relaxed)) {
            children.push_back(func);
        }
    }
    if (children.empty()) {
        if (relaxed) {
            mlog[WARN] <<"building an empty global block\n";
        } else {
            return NULL;
        }
    }

    // Build the global block
    SgAsmBlock *global = new SgAsmBlock;
    BOOST_FOREACH (SgAsmFunction *function, children) {
        global->get_statementList().push_back(function);
        function->set_parent(global);
    }
    return global;
}

SgAsmBlock*
Partitioner::buildAst(SgAsmInterpretation *interp/*=NULL*/, bool relaxed) const {
    if (SgAsmBlock *global = buildGlobalBlockAst(relaxed)) {
        fixupAstPointers(global, interp);
        return global;
    }
    return NULL;
}

void
Partitioner::fixupAstPointers(SgNode *ast, SgAsmInterpretation *interp/*=NULL*/) const {
    typedef Sawyer::Container::Map<rose_addr_t, SgAsmNode*> Index;

    // Build various indexes since ASTs have inherently linear search time.  We store just the starting address for all these
    // things because that's all we ever want to point to.
    struct Indexer: AstSimpleProcessing {
        Index insnIndex, bblockIndex, funcIndex;
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                insnIndex.insert(insn->get_address(), insn);
            } else if (SgAsmBlock *block = isSgAsmBlock(node)) {
                if (!block->get_statementList().empty() && isSgAsmInstruction(block->get_statementList().front()))
                    bblockIndex.insert(block->get_address(), block);
            } else if (SgAsmFunction *func = isSgAsmFunction(node)) {
                funcIndex.insert(func->get_entry_va(), func);
            }
        }
    } indexer;
    indexer.traverse(ast, preorder);

    // Build a list of memory-mapped sections in the interpretation.  We'll use this list to select the best-matching section
    // for a particular address via SgAsmGenericFile::best_section_by_va()
    SgAsmGenericSectionPtrList mappedSections;
    Index sectionIndex;
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *header, interp->get_headers()->get_headers()) {
            if (header->is_mapped())
                mappedSections.push_back(header);
            BOOST_FOREACH (SgAsmGenericSection *section, header->get_mapped_sections()) {
                if (!section->get_mapped_xperm())
                    mappedSections.push_back(section);
            }
        }
    }

    struct FixerUpper: AstSimpleProcessing {
        const Index &insnIndex, &bblockIndex, &funcIndex;
        const SgAsmGenericSectionPtrList &mappedSections;
        FixerUpper(const Index &insnIndex, const Index &bblockIndex, const Index &funcIndex,
                   const SgAsmGenericSectionPtrList &mappedSections)
            : insnIndex(insnIndex), bblockIndex(bblockIndex), funcIndex(funcIndex), mappedSections(mappedSections) {}
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                if (ival->get_baseNode()==NULL) {
                    rose_addr_t va = ival->get_absoluteValue();
                    SgAsmNode *base = NULL;
                    if (funcIndex.getOptional(va).assignTo(base) || bblockIndex.getOptional(va).assignTo(base) ||
                        insnIndex.getOptional(va).assignTo(base)) {
                        ival->makeRelativeTo(base);
                    } else if (SgAsmGenericSection *section = SgAsmGenericFile::best_section_by_va(mappedSections, va)) {
                        ival->makeRelativeTo(section);
                    }
                }
            }
        }
    } fixerUpper(indexer.insnIndex, indexer.bblockIndex, indexer.funcIndex, mappedSections);
    fixerUpper.traverse(ast, preorder);
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::AddressUser &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::AddressUsers &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::AddressUsageMap &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::ControlFlowGraph::VertexNode &x) {
    out <<Partitioner::vertexName(x);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::ControlFlowGraph::EdgeNode &x) {
    out <<Partitioner::edgeName(x);
    return out;
}

} // namespace
} // namespace
} // namespace
