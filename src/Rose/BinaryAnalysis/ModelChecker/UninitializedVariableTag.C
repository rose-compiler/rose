#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/UninitializedVariableTag.h>

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

UninitializedVariableTag::UninitializedVariableTag(size_t nodeStep, TestMode tm, SgAsmInstruction *insn,
                                                   const BS::SValue::Ptr &addr, const FoundVariable &variable)
    : Tag(nodeStep), testMode_(tm), insn_(insn), addr_(addr), variable_(variable) {}

UninitializedVariableTag::~UninitializedVariableTag() {}

UninitializedVariableTag::Ptr
UninitializedVariableTag::instance(size_t nodeStep, TestMode tm, SgAsmInstruction *insn, const BS::SValue::Ptr &addr,
                                   const FoundVariable &variable) {
    ASSERT_forbid(TestMode::OFF == tm);
    ASSERT_not_null(addr);
    ASSERT_require(variable);
    return Ptr(new UninitializedVariableTag(nodeStep, tm, insn, addr, variable));
}

std::string
UninitializedVariableTag::name() const {
    return "uninitialized variable";
}

std::string
UninitializedVariableTag::printableName() const {
    // No lock necessary because testMode is a read-only property initialized in the constructor.
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
    retval += "-uninit-read";
    return retval;
}

void
UninitializedVariableTag::print(std::ostream &out, const std::string &prefix) const {
    // No locks necessary since all the data members are read-only.
    out <<prefix <<name() <<"\n";

    out <<prefix <<"  read ";
    switch (testMode_) {
        case TestMode::OFF:
            out <<" is uninitialized";
            break;
        case TestMode::MAY:
            out <<" may be uninitialized";
            break;
        case TestMode::MUST:
            out <<" must be uninitialized";
            break;
    }
    if (insn_)
        out <<" at instruction " <<insn_->toString();
    out <<"\n";

    out <<prefix <<"  attempted read from address " <<*addr_ <<"\n";
    out <<prefix <<"  variable " <<variable_;
    if (auto function = variable_.function())
        out <<" in " <<function->printableName();
    out <<"\n";
}

void
UninitializedVariableTag::toYaml(std::ostream &out, const std::string &prefix1) const {
    // No locks necessary since all the data members are read-only.
    out <<prefix1 <<"weakness: " <<name() <<"\n";
    std::string prefix(prefix1.size(), ' ');

    out <<prefix <<"direction: read\n";

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

    out <<prefix <<"variable: " <<StringUtility::yamlEscape(variable_.toString()) <<"\n";
    out <<prefix <<"location: " <<StringUtility::yamlEscape(StringUtility::addrToString(variable_.where())) <<"\n";
    if (auto function = variable_.function())
        out <<prefix <<"function: " <<StringUtility::yamlEscape(function->printableName()) <<"\n";
}

Sarif::Result::Ptr
UninitializedVariableTag::toSarif(const Sarif::Analysis::Ptr &analysis) const {
    auto result = Sarif::Result::instance(Sarif::Severity::ERROR, name());
    if (analysis) {
        if (auto rule = analysis->findRuleByName("UninitializedRead"))
            result->rule(rule);
    }
    return result;
}

} // namespace
} // namespace
} // namespace

#endif
