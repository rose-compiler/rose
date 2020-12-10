#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinarySystemCall.h>

#include <AbstractLocation.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/Partitioner.h>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

void
SystemCall::declare(const Declaration &declaration) {
    declarations_.insert(declaration.id, declaration);
}

void
SystemCall::declare(const std::vector<Declaration> &declarations) {
    BOOST_FOREACH (const Declaration &declaration, declarations)
        declare(declaration);
}

void
SystemCall::declare(const boost::filesystem::path &headerFileName) {
    declare(parseHeaderFile(headerFileName));
}

Sawyer::Optional<SystemCall::Declaration>
SystemCall::lookup(uint64_t id) const {
    return declarations_.getOptional(id);
}

std::vector<SystemCall::Declaration>
SystemCall::lookup(const std::string &name) const {
    std::vector<Declaration> retval;
    BOOST_FOREACH (const Declaration &declaration, declarations_.values()) {
        if (declaration.name == name)
            retval.push_back(declaration);
    }
    return retval;
}

// class method
std::vector<SystemCall::Declaration>
SystemCall::parseHeaderFile(const boost::filesystem::path &headerFileName) {
    std::vector<Declaration> retval;
    std::ifstream headerFile(headerFileName.c_str());
    std::string line;
    boost::regex re("^#\\s*define\\s+__NR_(\\w+)\\s+(\\d+)");
    while (std::getline(headerFile, line)) {
        boost::smatch matched;
        if (boost::regex_search(line, matched, re)) {
            std::string name = matched.str(1);
            uint64_t id = boost::lexical_cast<uint64_t>(matched.str(2));
            retval.push_back(Declaration(id, name));
        }
    }
    return retval;
}

bool
SystemCall::isSystemCall(SgAsmInstruction *insn_) const {
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_)) {
        if (insn->get_kind() == x86_syscall) {
            return true;
        } else if (insn->get_kind() == x86_int && insn->nOperands() == 1) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(0)))
                return ival->get_absoluteValue() == 0x80;
        }
    }

    return false;
}

SgAsmInstruction*
SystemCall::hasSystemCall(const Partitioner2::BasicBlock::Ptr &bblock) const {
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        if (isSystemCall(insn))
            return insn;
    }
    return NULL;
}

Sawyer::Optional<SystemCall::Declaration>
SystemCall::analyze(const Partitioner2::Partitioner &partitioner, const Partitioner2::BasicBlock::Ptr &bblock,
                    SgAsmInstruction *syscallInsn, const SmtSolver::Ptr &solver /*=null*/) const {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;
    ASSERT_not_null(bblock);

    if (!ident_.isValid())
        return Sawyer::Nothing();
    if (!syscallInsn)
        syscallInsn = hasSystemCall(bblock);
    if (!syscallInsn)
        return Sawyer::Nothing();
    
    // Instantiate the instruction semantics layers
    BaseSemantics::RiscOperatorsPtr ops = partitioner.newOperators();
    BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops); 
    if (!cpu)
        throw Exception("no instruction semantics for architecture");

    // Analyze the basic block.
    bool foundSyscallInsn = false;
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        if (insn == syscallInsn) {
            foundSyscallInsn = true;
            break;
        }
        cpu->processInstruction(insn);
    }
    if (!foundSyscallInsn)
        return Sawyer::Nothing();
    
    // Read the value from the abstract location
    BaseSemantics::SValuePtr baseVal;
    if (ident_.isRegister()) {
        baseVal = ops->undefined_(ident_.getRegister().nBits());
        baseVal = ops->peekRegister(ident_.getRegister(), baseVal);
    } else {
        ASSERT_require(ident_.isAddress());
        baseVal = ops->undefined_(8 * ident_.nBytes());
        baseVal = ops->peekMemory(RegisterDescriptor(), ident_.getAddress(), baseVal);
    }

    // Convert the value to a syscall identification number
    ASSERT_not_null(baseVal);
    if (baseVal->is_number() && baseVal->get_width() <= 64)
        return lookup(baseVal->get_number());

    return Sawyer::Nothing();
}

} // namespace
} // namespace

#endif
