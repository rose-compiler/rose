#include "sage3basic.h"

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/foreach.hpp>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

void
BasicBlock::init(const Partitioner *partitioner) {
    operators_ = partitioner->newOperators();
    if (usingDispatcher_ && partitioner->usingSymbolicSemantics()) {
        dispatcher_ = partitioner->newDispatcher(operators_);
        undropSemantics();
    } else {
        // Rely on other methods to get basic block characteristics
        usingDispatcher_ = false;
    }
}

void
BasicBlock::clearSuccessors() {
    successors_.clear();
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
                if (exists.type()==type && exists.expr()->get_expression()->equivalent_to(successor->get_expression())) {
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
    return insertSuccessor(operators_->number_(nBits, va), type, confidence);
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

BaseSemantics::StatePtr
BasicBlock::finalState() {
    if (usingDispatcher_ && dispatcher_!=NULL)
        return dispatcher_->get_operators()->get_state();
    return BaseSemantics::StatePtr();
}

// Reset semantics back to a state similar to  after calling init() followed by append() with a failed dispatch, except also
// discard the initial and penultimate state.
void
BasicBlock::dropSemantics() {
    if (operators_)
        operators_->get_state()->clear();
    initialState_ = BaseSemantics::StatePtr();
    optionalPenultimateState_ = Sawyer::Nothing();
    usingDispatcher_ = false;
    ASSERT_require(!dispatcher_ || isSemanticsDropped());
}

void
BasicBlock::undropSemantics() {
    if (!initialState_) {
        if (dispatcher_) {
            initialState_ = dispatcher_->get_operators()->get_state();
            BaseSemantics::RegisterStateGeneric::promote(initialState_->get_register_state())->initialize_large();
            initialState_ = initialState_->clone();     // make a copy so process Instruction doesn't change it
            optionalPenultimateState_ = initialState_->clone(); // one level of undo information
            usingDispatcher_ = true;

            BOOST_FOREACH (SgAsmInstruction *insn, instructions()) {
                ASSERT_require(usingDispatcher_);
                optionalPenultimateState_ = dispatcher_->get_operators()->get_state()->clone();
                try {
                    dispatcher_->processInstruction(insn);
                } catch (...) {
                    usingDispatcher_ = false;           // an error turns off semantics for remainder of basic block
                    break;
                }
            }
        } else {
            usingDispatcher_ = false;
        }
    }
    ASSERT_forbid(isSemanticsDropped());
}
        
void
BasicBlock::append(SgAsmInstruction *insn) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to append instruction");
    ASSERT_not_null(insn);
    ASSERT_require2(!insns_.empty() || insn->get_address()==startVa_,
                    "address of first instruction (" + StringUtility::addrToString(insn->get_address()) + ") "
                    "must match block address (" + StringUtility::addrToString(startVa_) + ")");
    ASSERT_require2(std::find(insns_.begin(), insns_.end(), insn) == insns_.end(),
                    "instruction can only occur once in a basic block");

    if (isSemanticsDropped()) {
#if 0 // [Robb P. Matzke 2015-01-05]
        mlog[WARN] <<"recomputing semantics for " <<printableName <<"\n";
#endif
        undropSemantics();
    }

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
    optionalPenultimateState_ = usingDispatcher_ ?
                                dispatcher_->get_operators()->get_state()->clone() :
                                BaseSemantics::StatePtr();
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
BasicBlock::pop() {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to pop an instruction");
    ASSERT_forbid2(insns_.empty(), "basic block must have at least one instruction to pop");
    ASSERT_require2(optionalPenultimateState_, "only one level of undo is possible");

    if (insns_.size() > bigBlock_) {
        insnAddrMap_.erase(insns_.back()->get_address());
        ASSERT_require(insnAddrMap_.size() + 1 == insns_.size());
    } else if (insns_.size() == bigBlock_) {
        insnAddrMap_.clear();
    }
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
BasicBlock::fallthroughVa() const {
    ASSERT_require(!insns_.empty());
    return insns_.back()->get_address() + insns_.back()->get_size();
}

std::string
BasicBlock::printableName() const {
    return "basic block " + StringUtility::addrToString(address());
}

DataBlock::Ptr
BasicBlock::dataBlockExists(const DataBlock::Ptr &dblock) const {
    return dblock!=NULL && existsUnique(dblocks_, dblock, sortDataBlocks) ? dblock : DataBlock::Ptr();
}

bool
BasicBlock::insertDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to insert data block");
    ASSERT_not_null(dblock);
    return insertUnique(dblocks_, dblock, sortDataBlocks);
}

} // namespace
} // namespace
} // namespace
