#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <BinaryAnalysis/ModelChecker/BasicBlockUnit.h>

#include <BinaryAnalysis/ModelChecker/ErrorTag.h>
#include <BinaryAnalysis/ModelChecker/Semantics.h>
#include <BinaryAnalysis/ModelChecker/Settings.h>
#include <BinaryAnalysis/ModelChecker/Tag.h>
#include <BinaryAnalysis/ModelChecker/SourceLister.h>
#include <boost/format.hpp>
#include <Partitioner2/BasicBlock.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

BasicBlockUnit::BasicBlockUnit(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bblock)
    : ExecutionUnit(partitioner.sourceLocations().get(bblock->address())), partitioner_(partitioner),
                    bblock_(bblock) {}

BasicBlockUnit::~BasicBlockUnit() {}

BasicBlockUnit::Ptr
BasicBlockUnit::instance(const P2::Partitioner &partitioner, const P2::BasicBlockPtr &bblock) {
    ASSERT_not_null(bblock);
    return Ptr(new BasicBlockUnit(partitioner, bblock));
}

P2::BasicBlock::Ptr
BasicBlockUnit::basicBlock() const {
    // No lock necessary since the basic block pointer cannot be changed after construction.
    ASSERT_not_null(bblock_);
    return bblock_;
}

std::string
BasicBlockUnit::printableName() const {
    // No lock necessary since the basic block pointer cannot be changed after construction.
    std::string retval = bblock_->printableName();
    for (const P2::Function::Ptr &func: partitioner_.functionsOwningBasicBlock(bblock_)) {
        retval += " in " + func->printableName();
        break;
    }
    return retval;
}

void
BasicBlockUnit::printSteps(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix,
                           size_t stepOrigin, size_t maxSteps) const {
    // No lock necessary since the basic block pointer cannot be changed after construction. However, the BasicBlock API itself
    // might not be thread safe.
    ASSERT_not_null(bblock_);
    std::vector<SgAsmInstruction*> insns = bblock_->instructions();
    SourceLocation prevLoc;
    for (size_t i = 0; i < insns.size() && maxSteps > 0; ++i, --maxSteps) {
        SgAsmInstruction *insn = insns[i];
        ASSERT_not_null(insn);

        if (SourceLocation sloc = partitioner_.sourceLocations().get(insn->get_address())) {
            if (sloc != prevLoc) {
                out <<prefix <<"| " <<sloc <<"\n";
                if (settings->sourceLister)
                    settings->sourceLister->list(out, sloc, prefix);
                prevLoc = sloc;
            }
        }

        out <<(boost::format("%s#%-6d %s\n")
               %prefix
               %(stepOrigin + i)
               %(insns[i]->toString()));
    }
}

size_t
BasicBlockUnit::nSteps() const {
    // No lock necessary since the basic block pointer cannot be changed after construction. However, the BasicBlock API itself
    // might not be thread safe.
    ASSERT_not_null(bblock_);
    return bblock_->nInstructions();
}

Sawyer::Optional<rose_addr_t>
BasicBlockUnit::address() const {
    // No lock necessary since the basic block pointer cannot be changed after construction. However, the BasicBlock API itself
    // might not be thread safe.
    ASSERT_not_null(bblock_);
    return bblock_->address();
}

bool
BasicBlockUnit::containsUnknownInsn() const {
    for (SgAsmInstruction *insn: bblock_->instructions()) {
        if (insn->isUnknown())
            return true;
    }
    return false;
}

std::vector<Tag::Ptr>
BasicBlockUnit::execute(const Settings::Ptr &settings, const Semantics::Ptr &semantics, const BS::RiscOperatorsPtr &ops) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    std::vector<Tag::Ptr> tags;
    BS::DispatcherPtr cpu = semantics->createDispatcher(ops);
    BS::Formatter fmt;
    fmt.set_line_prefix("      ");

    SAWYER_MESG_FIRST(mlog[WHERE], mlog[TRACE], mlog[DEBUG]) <<"  executing " <<printableName() <<"\n";
    if (mlog[DEBUG] && settings->showInitialStates)
        mlog[DEBUG] <<"    initial state\n" <<(*ops->currentState() + fmt);

    if (bblock_->nInstructions() == 0) {
        tags.push_back(ErrorTag::instance(0, "empty basic block", "basic block has no instructions", nullptr, bblock_->address()));
    } else {
        for (size_t i = 0; i < bblock_->nInstructions(); ++i) {
            SgAsmInstruction *insn = bblock_->instructions()[i];
            SAWYER_MESG(mlog[DEBUG]) <<"    executing " <<insn->toString() <<"\n";

            if (Tag::Ptr tag = executeInstruction(settings, insn, cpu)) {
                if (i+1 < bblock_->nInstructions())
                    ops->currentState(BS::StatePtr());      // force a semantic failure since we didn't finish the block
                tags.push_back(tag);
            }
            if (!ops->currentState())
                break;

            // Show states
            if (i + 1 < bblock_->nInstructions()) {
                if (settings->showIntermediateStates)
                    SAWYER_MESG(mlog[DEBUG]) <<"    intermediate state\n" <<(*ops->currentState() + fmt);
            } else {
                if (settings->showFinalStates)
                    SAWYER_MESG(mlog[DEBUG]) <<"    final state\n" <<(*ops->currentState() + fmt);
            }

            // In order for this to be a valid basic block, the instruction pointer must have a concrete value that points
            // to the next instruction.
            if (i + 1 < bblock_->nInstructions()) {
                BS::SValuePtr actualIp = semantics->instructionPointer(ops);
                rose_addr_t expectedIp = bblock_->instructions()[i+1]->get_address();
                if (actualIp->toUnsigned().orElse(expectedIp+1) != expectedIp) {
                    std::string mesg = "next IP should be " + StringUtility::addrToString(expectedIp) + " according to CFG";
                    tags.push_back(ErrorTag::instance(i, "unexpected IP", mesg, bblock_->instructions()[i], actualIp));
                    break;
                }
            }
        }
    }
    return tags;
}

} // namespace
} // namespace
} // namespace

#endif
