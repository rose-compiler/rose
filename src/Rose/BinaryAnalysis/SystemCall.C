#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/SystemCall.h>

#include <Rose/BinaryAnalysis/AbstractLocation.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

void
SystemCall::declare(const Declaration &declaration) {
    declarations_.insert(declaration.id, declaration);
}

void
SystemCall::declare(const std::vector<Declaration> &declarations) {
    for (const Declaration &declaration: declarations)
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
    for (const Declaration &declaration: declarations_.values()) {
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
    for (SgAsmInstruction *insn: bblock->instructions()) {
        if (isSystemCall(insn))
            return insn;
    }
    return NULL;
}

Sawyer::Optional<SystemCall::Declaration>
SystemCall::analyze(const Partitioner2::Partitioner::ConstPtr &partitioner, const Partitioner2::BasicBlock::Ptr &bblock,
                    SgAsmInstruction *syscallInsn, const SmtSolver::Ptr& /*=null*/) const {
    using namespace Rose::BinaryAnalysis::InstructionSemantics;
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bblock);

    if (!ident_.isValid())
        return Sawyer::Nothing();
    if (!syscallInsn)
        syscallInsn = hasSystemCall(bblock);
    if (!syscallInsn)
        return Sawyer::Nothing();
    
    // Instantiate the instruction semantics layers
    BaseSemantics::RiscOperators::Ptr ops = partitioner->newOperators();
    BaseSemantics::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
    if (!cpu)
        throw Exception("no instruction semantics for architecture");

    // Analyze the basic block.
    bool foundSyscallInsn = false;
    for (SgAsmInstruction *insn: bblock->instructions()) {
        if (insn == syscallInsn) {
            foundSyscallInsn = true;
            break;
        }
        cpu->processInstruction(insn);
    }
    if (!foundSyscallInsn)
        return Sawyer::Nothing();
    
    // Read the value from the abstract location
    BaseSemantics::SValue::Ptr baseVal;
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
    if (auto bvu = baseVal->toUnsigned())
        return lookup(*bvu);

    return Sawyer::Nothing();
}

} // namespace
} // namespace

#endif
