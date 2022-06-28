#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/OobTag.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

OobTag::OobTag(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValue::Ptr &addr,
               const Variables::StackVariable &intendedVariable, const AddressInterval &intendedVariableLocation,
               const Variables::StackVariable &accessedVariable, const AddressInterval &accessedVariableLocation)
    : Tag(nodeStep), testMode_(tm), ioMode_(io), insn_(insn), addr_(addr), intendedVariable_(intendedVariable),
      intendedVariableLocation_(intendedVariableLocation), accessedVariable_(accessedVariable),
      accessedVariableLocation_(accessedVariableLocation) {}

OobTag::~OobTag() {}

OobTag::Ptr
OobTag::instance(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValue::Ptr &addr,
                 const Variables::StackVariable &intendedVariable, const AddressInterval &intendedVariableLocation,
                 const Variables::StackVariable &accessedVariable, const AddressInterval &accessedVariableLocation) {
    ASSERT_forbid(TestMode::OFF == tm);
    ASSERT_not_null(addr);
    return Ptr(new OobTag(nodeStep, tm, io, insn, addr,
                          intendedVariable, intendedVariableLocation,
                          accessedVariable, accessedVariableLocation));
}

std::string
OobTag::name() const {
    return "out-of-bounds access";
}

std::string
OobTag::printableName() const {
    // No lock necessary because ioMode and testMode are read-only properties initialized in the constructor.
    std::string retval;
    switch (testMode_) {
        case TestMode::MUST:
            retval = "must";
            break;
        case TestMode::MAY:
            retval = "may";
            break;
        case TestMode::OFF:
            break;
    }
    retval += "-oob";
    switch (ioMode_) {
        case IoMode::READ:
            retval += "-read";
            break;
        case IoMode::WRITE:
            retval += "-write";
            break;
    }
    return retval;
}

void
OobTag::print(std::ostream &out, const std::string &prefix) const {
    // No locks necessary since all the data members are read-only.
    out <<prefix <<name() <<"\n";

    std::string io = IoMode::READ == ioMode_ ? "read" : "write";
    std::string toFrom = IoMode::READ == ioMode_ ? "read from" : "write to";

    out <<prefix <<"  " <<io;
    switch (testMode_) {
        case TestMode::OFF:
            out <<" is out of bounds";
            break;
        case TestMode::MAY:
            out <<" may be out of bounds";
            break;
        case TestMode::MUST:
            out <<" must be out of bounds";
            break;
    }
    if (insn_)
        out <<" at instruction " <<insn_->toString();
    out <<"\n";

    out <<prefix <<"  attempted " <<toFrom <<" address " <<*addr_ <<"\n";

    if (intendedVariableLocation_) {
        out <<prefix <<"  intended to access " <<intendedVariable_
            <<" at " <<StringUtility::addrToString(intendedVariableLocation_);
        if (auto function = intendedVariable_.function())
            out <<" in " <<function->printableName();
        out <<"\n";
    }

    if (accessedVariableLocation_) {
        out <<prefix <<"  actually accessed " <<accessedVariable_
            <<" at " <<StringUtility::addrToString(accessedVariableLocation_);
        if (auto function = accessedVariable_.function())
            out <<" in " <<function->printableName();
        out <<"\n";
    }
}

void
OobTag::toYaml(std::ostream &out, const std::string &prefix1) const {
    // No locks necessary since all the data members are read-only.
    out <<prefix1 <<"weakness: " <<name() <<"\n";
    std::string prefix(prefix1.size(), ' ');

    switch (ioMode_) {
        case IoMode::READ:
            out <<prefix <<"direction: read\n";
            break;
        case IoMode::WRITE:
            out <<prefix <<"direction: write\n";
            break;
    }

    switch (testMode_) {
        case TestMode::OFF:
            break;
        case TestMode::MAY:
            out <<prefix <<"test-mode: may\n";
            break;
        case TestMode::MUST:
            out <<prefix <<"test-mode: must\n";
            break;
    }

    if (insn_)
        out <<prefix <<"instruction: " <<StringUtility::yamlEscape(insn_->toString()) <<"\n";

    out <<prefix <<"memory-address: " <<StringUtility::yamlEscape(boost::lexical_cast<std::string>(*addr_)) <<"\n";

    if (intendedVariableLocation_) {
        out <<prefix <<"intended:\n";
        out <<prefix <<"  variable: " <<StringUtility::yamlEscape(intendedVariable_.toString()) <<"\n";
        out <<prefix <<"  location: " <<StringUtility::yamlEscape(StringUtility::addrToString(intendedVariableLocation_)) <<"\n";
        if (auto function = intendedVariable_.function())
            out <<prefix <<"  function: " <<StringUtility::yamlEscape(function->printableName()) <<"\n";
    }

    if (accessedVariableLocation_) {
        out <<prefix <<"actual:\n";
        out <<prefix <<"  variable: " <<StringUtility::yamlEscape(accessedVariable_.toString()) <<"\n";
        out <<prefix <<"  location: " <<StringUtility::yamlEscape(StringUtility::addrToString(accessedVariableLocation_)) <<"\n";
        if (auto function = accessedVariable_.function())
            out <<prefix <<"  function: " <<StringUtility::yamlEscape(function->printableName()) <<"\n";
    }
}

} // namespace
} // namespace
} // namespace

#endif
