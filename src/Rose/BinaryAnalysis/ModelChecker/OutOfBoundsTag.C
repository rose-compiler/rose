#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/OutOfBoundsTag.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Result.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

OutOfBoundsTag::OutOfBoundsTag(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValue::Ptr &addr,
                               const FoundVariable &intendedVariable, const FoundVariable &accessedVariable)
    : Tag(nodeStep), testMode_(tm), ioMode_(io), insn_(insn), addr_(addr), intendedVariable_(intendedVariable),
      accessedVariable_(accessedVariable) {}

OutOfBoundsTag::~OutOfBoundsTag() {}

OutOfBoundsTag::Ptr
OutOfBoundsTag::instance(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValue::Ptr &addr,
                         const FoundVariable &intendedVariable, const FoundVariable &accessedVariable) {
    ASSERT_forbid(TestMode::OFF == tm);
    ASSERT_not_null(addr);
    return Ptr(new OutOfBoundsTag(nodeStep, tm, io, insn, addr, intendedVariable, accessedVariable));
}

std::string
OutOfBoundsTag::name() const {
    return "out-of-bounds access";
}

std::string
OutOfBoundsTag::printableName() const {
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
OutOfBoundsTag::print(std::ostream &out, const std::string &prefix) const {
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

    if (intendedVariable_) {
        out <<prefix <<"  intended to access " <<intendedVariable_;
        if (auto function = intendedVariable_.function())
            out <<" in " <<function->printableName();
        out <<"\n";
    }

    if (accessedVariable_) {
        out <<prefix <<"  actually accessed " <<accessedVariable_;
        if (auto function = accessedVariable_.function())
            out <<" in " <<function->printableName();
        out <<"\n";
    }
}

void
OutOfBoundsTag::toYaml(std::ostream &out, const std::string &prefix1) const {
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

    if (intendedVariable_) {
        out <<prefix <<"intended:\n";
        out <<prefix <<"  variable: " <<StringUtility::yamlEscape(intendedVariable_.toString()) <<"\n";
        out <<prefix <<"  location: " <<StringUtility::yamlEscape(StringUtility::addrToString(intendedVariable_.where())) <<"\n";
        if (auto function = intendedVariable_.function())
            out <<prefix <<"  function: " <<StringUtility::yamlEscape(function->printableName()) <<"\n";
    }

    if (accessedVariable_) {
        out <<prefix <<"actual:\n";
        out <<prefix <<"  variable: " <<StringUtility::yamlEscape(accessedVariable_.toString()) <<"\n";
        out <<prefix <<"  location: " <<StringUtility::yamlEscape(StringUtility::addrToString(accessedVariable_.where())) <<"\n";
        if (auto function = accessedVariable_.function())
            out <<prefix <<"  function: " <<StringUtility::yamlEscape(function->printableName()) <<"\n";
    }
}

Sarif::Result::Ptr
OutOfBoundsTag::toSarif(const Sarif::Analysis::Ptr &analysis) const {
    auto result = Sarif::Result::instance(Sarif::Severity::ERROR, name());
    if (analysis) {
        if (auto rule = analysis->findRuleByName("OutOfBoundsAccess"))
            result->rule(rule);
    }
    return result;
}

} // namespace
} // namespace
} // namespace

#endif
