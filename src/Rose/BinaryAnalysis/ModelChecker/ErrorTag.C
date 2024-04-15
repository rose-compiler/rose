#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/ErrorTag.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Result.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

ErrorTag::ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn,
                   const Sawyer::Optional<uint64_t> &value)
    : NameTag(nodeStep, name), mesg_(mesg), insn_(insn), concrete_(value) {}

ErrorTag::ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn,
                   const SymbolicExpression::Ptr &value)
    : NameTag(nodeStep, name), mesg_(mesg), insn_(insn), symbolic_(value) {}

ErrorTag::ErrorTag(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn,
                   const BS::SValue::Ptr &value)
    : NameTag(nodeStep, name), mesg_(mesg), insn_(insn), svalue_(value) {}

ErrorTag::~ErrorTag() {}

ErrorTag::Ptr
ErrorTag::instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn) {
    return Ptr(new ErrorTag(nodeStep, name, mesg, insn, Sawyer::Nothing()));
}

ErrorTag::Ptr
ErrorTag::instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn,
                   uint64_t value) {
    return Ptr(new ErrorTag(nodeStep, name, mesg, insn, value));
}

ErrorTag::Ptr
ErrorTag::instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn,
                   const SymbolicExpression::Ptr &value) {
    return Ptr(new ErrorTag(nodeStep, name, mesg, insn, value));
}

ErrorTag::Ptr
ErrorTag::instance(size_t nodeStep, const std::string &name, const std::string &mesg, SgAsmInstruction *insn,
                   const BS::SValue::Ptr &value) {
    return Ptr(new ErrorTag(nodeStep, name, mesg, insn, value));
}

std::string
ErrorTag::message() const {
    // No lock necessary because mesg_ is immutable
    return mesg_;
}

Sawyer::Message::Importance
ErrorTag::importance() const {
    return importance_;
}

void
ErrorTag::importance(Sawyer::Message::Importance imp) {
    importance_ = imp;
}

void
ErrorTag::print(std::ostream &out, const std::string &prefix) const {
    // No lock necessary because name_ and mesg_ are read-only properties initialized in the constructor.
    out <<prefix <<name_ + ": " + mesg_ <<"\n";
    if (insn_)
        out <<prefix <<"  insn " <<insn_->toString() <<"\n";
    if (concrete_)
        out <<prefix <<"  value = " <<StringUtility::toHex(*concrete_) <<"\n";
    if (symbolic_) {
        SymbolicExpression::Formatter fmt;
        fmt.max_depth = 10;
        out <<prefix <<"  value = " <<(*symbolic_+fmt) <<"\n";
    }
    if (svalue_) {
        IS::SymbolicSemantics::Formatter fmt;
        fmt.expr_formatter.max_depth = 10;
        out <<prefix <<"  value = " <<(*svalue_+fmt) <<"\n";
    }
}

void
ErrorTag::toYaml(std::ostream &out, const std::string &prefix1) const {
    // No lock necessary because name_ and mesg_ are read-only properties initialized in the constructor.
    out <<prefix1 <<"name: " <<StringUtility::yamlEscape(name_) <<"\n";
    std::string prefix(prefix1.size(), ' ');
    out <<prefix <<"message: " <<StringUtility::yamlEscape(mesg_) <<"\n";
    if (insn_)
        out <<prefix <<"instruction: " <<StringUtility::yamlEscape(insn_->toString()) <<"\n";
    if (concrete_)
        out <<prefix <<"concrete-value: " <<StringUtility::toHex(*concrete_) <<"\n";
    if (symbolic_)
        out <<prefix <<"symbolic-value: " <<StringUtility::yamlEscape(boost::lexical_cast<std::string>(*svalue_)) <<"\n";
}

Sarif::Result::Ptr
ErrorTag::toSarif(const Sarif::Analysis::Ptr &analysis) const {
    auto result = Sarif::Result::instance(Sarif::Severity::ERROR, mesg_);
    if (analysis) {
        if (auto rule = analysis->findRuleByName("Error"))
            result->rule(rule);
    }
    return result;
}

} // namespace
} // namespace
} // namespace

#endif
