#include <rose.h>

#include <Partitioner2/Partitioner.h>
#include <PathFinder/semantics.h>
#include <SymbolicMemory2.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
namespace BaseSemantics = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace SymbolicSemantics = Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;

namespace PathFinder {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantic Utilities
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SymbolicSemantics::Formatter
symbolicFormat(const std::string &prefix) {
    SymbolicSemantics::Formatter retval;
    retval.set_line_prefix(prefix);
    retval.expr_formatter.max_depth = settings.maxExprDepth;
    retval.expr_formatter.show_type = settings.showExprType;
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperatorsPtr
RiscOperators::instance(const Partitioner2::Partitioner *partitioner,
                        const RegisterDictionary *regdict,
                        const SmtSolver::Ptr &solver) {
    BaseSemantics::SValuePtr protoval = SValue::instance();
    BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryStatePtr memory;
    switch (settings.searchMode) {
        case SEARCH_MULTI:
            // If we're sending multiple paths at a time to the SMT solver then we need to provide the SMT solver with
            // detailed information about how memory is affected on those different paths.
            memory = BaseSemantics::SymbolicMemory::instance(protoval, protoval);
            break;
        case SEARCH_SINGLE_DFS:
        case SEARCH_SINGLE_BFS:
            // We can perform memory-related operations and simplifications inside ROSE, which results in more but smaller
            // expressions being sent to the SMT solver.
            memory = SymbolicSemantics::MemoryState::instance(protoval, protoval);
            break;
    }
    ASSERT_not_null(memory);
    BaseSemantics::StatePtr state = State::instance(registers, memory);
    return RiscOperatorsPtr(new RiscOperators(partitioner, state, solver));
}

std::string
RiscOperators::varComment(const std::string &varName) const {
    return State::promote(currentState())->varComment(varName);
}

void
RiscOperators::varComment(const std::string &varName, const std::string &comment) {
    State::promote(currentState())->varComment(varName, comment);
}

std::string
RiscOperators::commentForVariable(RegisterDescriptor reg, const std::string &accessMode) const {
    const RegisterDictionary *regs = currentState()->registerState()->get_register_dictionary();
    std::string varComment = RegisterNames(regs)(reg) + " first " + accessMode;
    if (pathInsnIndex_ == (size_t)(-1) && currentInstruction() == NULL) {
        varComment += " by initialization";
    } else {
        if (pathInsnIndex_ != (size_t)(-1))
            varComment += " at path position #" + StringUtility::numberToString(pathInsnIndex_);
        if (SgAsmInstruction *insn = currentInstruction())
            varComment += " by " + unparseInstructionWithAddress(insn);
    }
    return varComment;
}

std::string
RiscOperators::commentForVariable(const BaseSemantics::SValuePtr &addr, const std::string &accessMode, size_t byteNumber,
                                  size_t nBytes) const {
    std::string varComment = "first " + accessMode + " at ";
    if (pathInsnIndex_ != (size_t)(-1))
        varComment += "path position #" + StringUtility::numberToString(pathInsnIndex_) + ", ";
    varComment += "instruction " + unparseInstructionWithAddress(currentInstruction());

    // Sometimes we can save useful information about the address.
    if (nBytes != 1) {
        SymbolicExpr::Ptr addrExpr = SValue::promote(addr)->get_expression();
        if (SymbolicExpr::LeafPtr addrLeaf = addrExpr->isLeafNode()) {
            if (addrLeaf->isIntegerConstant()) {
                varComment += "\n";
                if (nBytes > 1) {
                    varComment += StringUtility::numberToString(byteNumber) + " of " +
                                  StringUtility::numberToString(nBytes) + " bytes starting ";
                }
                varComment += "at address " + addrLeaf->toString();
            }
        } else if (SymbolicExpr::InteriorPtr addrINode = addrExpr->isInteriorNode()) {
            if (addrINode->getOperator() == SymbolicExpr::OP_ADD && addrINode->nChildren() == 2 &&
                addrINode->child(0)->isIntegerVariable() &&
                addrINode->child(1)->isIntegerConstant()) {
                SymbolicExpr::LeafPtr base = addrINode->child(0)->isLeafNode();
                SymbolicExpr::LeafPtr offset = addrINode->child(1)->isLeafNode();
                varComment += "\n";
                if (nBytes > 1) {
                    varComment += StringUtility::numberToString(byteNumber) + " of " +
                                  StringUtility::numberToString(nBytes) + " bytes starting ";
                }
                varComment += "at address ";
                if (base->comment().empty()) {
                    varComment = base->toString();
                } else {
                    varComment += base->comment();
                }
                Sawyer::Container::BitVector tmp = offset->bits();
                if (tmp.get(tmp.size()-1)) {
                    varComment += " - 0x" + tmp.negate().toHex();
                } else {
                    varComment += " + 0x" + tmp.toHex();
                }
            }
        }
    }
    return varComment;
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(partitioner_);
    Super::startInstruction(insn);
    if (mlog[DEBUG]) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        mlog[DEBUG] <<"  +-------------------------------------------------\n"
                    <<"  | " <<unparseInstructionWithAddress(insn) <<"\n"
                    <<"  +-------------------------------------------------\n"
                    <<"    state before instruction:\n"
                    <<(*currentState() + fmt);
    }
}

void
RiscOperators::finishInstruction(SgAsmInstruction *insn) {
    if (mlog[DEBUG]) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        mlog[DEBUG] <<"    state after instruction:\n" <<(*currentState()+fmt);
    }
    Super::finishInstruction(insn);
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    SValuePtr retval = SValue::promote(Super::readRegister(reg, dflt));
    SymbolicExpr::Ptr expr = retval->get_expression();
    if (expr->isLeafNode()) {
        std::string comment = commentForVariable(reg, "read");
        State::promote(currentState())->varComment(expr->isLeafNode()->toString(), comment);
    }
    return retval;
}
    
void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &value) {
    SymbolicExpr::Ptr expr = SValue::promote(value)->get_expression();
    if (expr->isLeafNode()) {
        std::string comment = commentForVariable(reg, "write");
        State::promote(currentState())->varComment(expr->isLeafNode()->toString(), comment);
    }
    Super::writeRegister(reg, value);
}

/** Read memory.
 *
 *  If multi-path is enabled, then return a new memory expression that describes the process of reading a value from the
 *  specified address; otherwise, actually read the value and return it.  In any case, record some information about the
 *  address that's being read if we've never seen it before. */
BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt_, const BaseSemantics::SValuePtr &cond) {

    BaseSemantics::SValuePtr dflt = dflt_;
    const size_t nBytes = dflt->get_width() / 8;
    if (cond->is_number() && !cond->get_number())
        return dflt_;

    // If we know the address and that memory exists, then read the memory to obtain the default value.
    uint8_t buf[8];
    if (addr->is_number() && nBytes < sizeof(buf) &&
        nBytes == partitioner_->memoryMap()->at(addr->get_number()).limit(nBytes).read(buf).size()) {
        // FIXME[Robb P. Matzke 2015-05-25]: assuming little endian
        uint64_t value = 0;
        for (size_t i=0; i<nBytes; ++i)
            value |= (uint64_t)buf[i] << (8*i);
        dflt = number_(dflt->get_width(), value);
    }

    // Read from the symbolic state, and update the state with the default from real memory if known.
    BaseSemantics::SValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);

    if (!currentInstruction())
        return retval;                              // not called from dispatcher on behalf of an instruction

    // Save a description of the variable
    SymbolicExpr::Ptr valExpr = SValue::promote(retval)->get_expression();
    if (valExpr->isIntegerVariable()) {
        std::string comment = commentForVariable(addr, "read");
        State::promote(currentState())->varComment(valExpr->isLeafNode()->toString(), comment);
    }

    // Save a description for its addresses
    for (size_t i=0; i<nBytes; ++i) {
        SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
        if (va->get_expression()->isLeafNode()) {
            std::string comment = commentForVariable(addr, "read", i, nBytes);
            State::promote(currentState())->varComment(va->get_expression()->isLeafNode()->toString(), comment);
        }
    }
    return retval;
}

/** Write value to memory.
 *
 *  If multi-path is enabled, then return a new memory expression that updates memory with a new address/value pair;
 *  otherwise update the memory directly.  In any case, record some information about the address that was written if we've
 *  never seen it before. */
void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                           const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond) {
    if (cond->is_number() && !cond->get_number())
        return;
    Super::writeMemory(segreg, addr, value, cond);

    // Save a description of the variable
    SymbolicExpr::Ptr valExpr = SValue::promote(value)->get_expression();
    if (valExpr->isIntegerVariable()) {
        std::string comment = commentForVariable(addr, "write");
        State::promote(currentState())->varComment(valExpr->isLeafNode()->toString(), comment);
    }

    // Save a description for its addresses
    size_t nBytes = value->get_width() / 8;
    for (size_t i=0; i<nBytes; ++i) {
        SValuePtr va = SValue::promote(add(addr, number_(addr->get_width(), i)));
        if (va->get_expression()->isLeafNode()) {
            std::string comment = commentForVariable(addr, "read", i, nBytes);
            State::promote(currentState())->varComment(va->get_expression()->isLeafNode()->toString(), comment);
        }
    }
}

} // namespace
