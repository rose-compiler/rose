#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include <Partitioner2/BasicBlock.h>

#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/foreach.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// called only during construction
void
BasicBlock::init(const Partitioner &partitioner) {
    semantics_.operators = partitioner.newOperators();
    if (semantics_.usingDispatcher && partitioner.usingSymbolicSemantics()) {
        semantics_.dispatcher = partitioner.newDispatcher(semantics_.operators);
        undropSemantics(partitioner);
    } else {
        // Rely on other methods to get basic block characteristics
        semantics_.usingDispatcher = false;
    }
}

void
BasicBlock::clearSuccessors() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    successors_.clear();
}

void
BasicBlock::successors(const Successors &successors) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    successors_ = successors;
}

void
BasicBlock::insertSuccessor(const BaseSemantics::SValuePtr &successor_, EdgeType type, Confidence confidence) {
    if (successor_ != NULL) {
        Semantics::SValuePtr successor = Semantics::SValue::promote(successor_);
        if (!successors_.isCached()) {
            Successors successors;
            successors.push_back(Successor(successor, type, confidence));
            successors_ = successors;
        } else {
            Successors successors = successors_.get();
            bool found = false;
            BOOST_FOREACH (const Successor &exists, successors) {
                if (exists.type()==type && exists.expr()->get_expression()->isEquivalentTo(successor->get_expression())) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                successors.push_back(Successor(successor, type, confidence));
                successors_ = successors;
            }
        }
    }
}

void
BasicBlock::insertSuccessor(rose_addr_t va, size_t nBits, EdgeType type, Confidence confidence) {
    BaseSemantics::RiscOperatorsPtr ops = semantics_.operators;
    ASSERT_not_null(ops);
    return insertSuccessor(ops->number_(nBits, va), type, confidence);
}

SgAsmInstruction*
BasicBlock::instructionExists(rose_addr_t startVa) const {
    if (insns_.size() >= bigBlock_) {
        // O(log N) search for large blocks
        ASSERT_require(insns_.size() == insnAddrMap_.size());
        size_t idx = 0;
        if (insnAddrMap_.getOptional(startVa).assignTo(idx))
            return insns_[idx];
    } else {
        // O(N) search for small blocks
        BOOST_FOREACH (SgAsmInstruction *insn, insns_) {
            if (insn->get_address() == startVa)
                return insn;
        }
    }
    return NULL;
}

Sawyer::Optional<size_t>
BasicBlock::instructionExists(SgAsmInstruction *toFind) const {
    if (insns_.size() >= bigBlock_) {
        ASSERT_require(insns_.size() == insnAddrMap_.size());
        size_t idx = 0;
        if (insnAddrMap_.getOptional(toFind->get_address()).assignTo(idx) && insns_[idx]==toFind)
            return idx;
    } else {
        for (size_t i=0; i<insns_.size(); ++i) {
            if (insns_[i]==toFind)
                return i;
        }
    }
    return Sawyer::Nothing();
}

// Reset semantics back to a state similar to  after calling init() followed by append() with a failed dispatch, except also
// discard the initial and penultimate state.
void
BasicBlock::dropSemantics(const Partitioner &partitioner) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    semantics_.operators = partitioner.newOperators();
    semantics_.dispatcher = partitioner.newDispatcher(semantics_.operators);
    semantics_.initialState = BaseSemantics::StatePtr();
    semantics_.usingDispatcher = false;
    semantics_.optionalPenultimateState = Sawyer::Nothing();
    ASSERT_require(!semantics_.dispatcher || semantics_.isSemanticsDropped());
}

BasicBlockSemantics
BasicBlock::undropSemantics(const Partitioner &partitioner) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return undropSemanticsNS(partitioner);
}

BasicBlockSemantics
BasicBlock::undropSemanticsNS(const Partitioner &partitioner) {
    bool reconstructed = false;                            // did we reconstruct the semantics?
    if (!semantics_.initialState) {
        if (semantics_.dispatcher) {
            reconstructed = true;
            BasicBlockSemantics sem;
            sem.operators = partitioner.newOperators();
            ASSERT_not_null(sem.operators);
            sem.dispatcher = partitioner.newDispatcher(sem.operators);
            ASSERT_not_null(sem.dispatcher);
            BaseSemantics::StatePtr curState = sem.operators->currentState();
            BaseSemantics::RegisterStateGeneric::promote(curState->registerState())->initialize_large();
            sem.initialState = curState->clone();
            sem.usingDispatcher = true;

            BaseSemantics::StatePtr penultimateState = curState->clone();
            BOOST_FOREACH (SgAsmInstruction *insn, instructions()) {
                penultimateState = sem.operators->currentState()->clone();
                try {
                    sem.dispatcher->processInstruction(insn);
                } catch (...) {
                    sem.usingDispatcher = false;
                    break;
                }
            }

            sem.optionalPenultimateState = penultimateState;
            semantics_ = sem;

        } else {
            semantics_.usingDispatcher = false;
        }
    }
    ASSERT_forbid(semantics_.isSemanticsDropped());
    BasicBlockSemantics retval = semantics_;
    retval.wasDropped = reconstructed;
    return retval;
}

BasicBlockSemantics
BasicBlock::semantics() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return semantics_;
}

void
BasicBlock::append(const Partitioner &partitioner, SgAsmInstruction *insn) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid2(isFrozen_, "basic block must be modifiable to append instruction");
    ASSERT_not_null(insn);
    ASSERT_require2(!insns_.empty() || insn->get_address()==startVa_,
                    "address of first instruction (" + StringUtility::addrToString(insn->get_address()) + ") "
                    "must match block address (" + StringUtility::addrToString(startVa_) + ")");
    ASSERT_require2(std::find(insns_.begin(), insns_.end(), insn) == insns_.end(),
                    "instruction can only occur once in a basic block");

    if (semantics_.isSemanticsDropped())
        undropSemanticsNS(partitioner);

    // Append instruction to block, switching to O(log N) mode if the block becomes big.
    insns_.push_back(insn);
    if (insns_.size() == bigBlock_) {
        ASSERT_require(insnAddrMap_.isEmpty());
        for (size_t i=0; i<insns_.size(); ++i)
            insnAddrMap_.insert(insns_[i]->get_address(), i);
        ASSERT_require(insnAddrMap_.size() == insns_.size());
    } else if (insns_.size() > bigBlock_) {
        insnAddrMap_.insert(insns_.back()->get_address(), insns_.size()-1);
        ASSERT_require(insnAddrMap_.size() == insns_.size());
    }

    // Process the instruction to create a new state
    semantics_.optionalPenultimateState = semantics_.usingDispatcher ?
                                              semantics_.dispatcher->get_operators()->currentState()->clone() :
                                              BaseSemantics::StatePtr();
    if (semantics_.usingDispatcher) {
        try {
            semantics_.dispatcher->processInstruction(insn);
        } catch (...) {
            semantics_.usingDispatcher = false;         // an error turns off semantics for the remainder of the basic block
        }
    }
    clearCacheNS();
}

void
BasicBlock::pop() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid2(isFrozen_, "basic block must be modifiable to pop an instruction");
    ASSERT_forbid2(insns_.empty(), "basic block must have at least one instruction to pop");
    ASSERT_require2(semantics_.optionalPenultimateState, "only one level of undo is possible");

    if (insns_.size() > bigBlock_) {
        insnAddrMap_.erase(insns_.back()->get_address());
        ASSERT_require(insnAddrMap_.size() + 1 == insns_.size());
    } else if (insns_.size() == bigBlock_) {
        insnAddrMap_.clear();
    }
    insns_.pop_back();

    if (BaseSemantics::StatePtr ps = *semantics_.optionalPenultimateState) {
        // If we didn't save a previous state it means that we didn't call processInstruction during the append, and therefore
        // we don't need to update the dispatcher (it's already out of date anyway).  Otherwise the dispatcher state needs to
        // be re-initialized by transferring ownership of the previous state into the partitioner.
        semantics_.dispatcher->get_operators()->currentState(ps);
        semantics_.optionalPenultimateState = Sawyer::Nothing();
    }
    clearCacheNS();
}

AddressIntervalSet
BasicBlock::insnAddresses() const {
    AddressIntervalSet retval;
    BOOST_FOREACH (SgAsmInstruction *insn, insns_)
        retval.insert(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
    return retval;
}

AddressIntervalSet
BasicBlock::dataAddresses() const {
    AddressIntervalSet retval;
    BOOST_FOREACH (const DataBlock::Ptr &db, dblocks_)
        retval.insert(db->extent());
    return retval;
}

rose_addr_t
BasicBlock::fallthroughVa() const {
    ASSERT_require(!insns_.empty());
    return insns_.back()->get_address() + insns_.back()->get_size();
}

std::string
BasicBlock::printableName() const {
    return "basic block " + StringUtility::addrToString(address()) +
        (comment_.empty() ? "" : " \"" + StringUtility::cEscape(comment_) + "\"");
}

DataBlock::Ptr
BasicBlock::dataBlockExists(const DataBlock::Ptr &dblock) const {
    Sawyer::Optional<DataBlock::Ptr> found;
    if (dblock)
        found = getUnique(dblocks_, dblock, sortDataBlocks);
    return found ? *found : DataBlock::Ptr();
}

bool
BasicBlock::insertDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to insert data block");
    ASSERT_not_null(dblock);
    return insertUnique(dblocks_, dblock, sortDataBlocks); // false if equivalent dblock already exists
}

DataBlock::Ptr
BasicBlock::eraseDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to erase data block");
    DataBlock::Ptr retval;
    if (dblock) {
        std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock, sortDataBlocks);
        if (lb != dblocks_.end() && (*lb)->extent() == dblock->extent()) {
            retval = *lb;
            dblocks_.erase(lb);
        }
    }
    return retval;
}

void
BasicBlock::replaceOrInsertDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_not_null(dblock);
    replaceOrInsert(dblocks_, dblock, sortDataBlocks);
}

std::set<rose_addr_t>
BasicBlock::explicitConstants() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    std::set<rose_addr_t> retval;
    BOOST_FOREACH (SgAsmInstruction *insn, insns_) {
        std::set<rose_addr_t> insnConstants = insn->explicitConstants();
        retval.insert(insnConstants.begin(), insnConstants.end());
    }
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
