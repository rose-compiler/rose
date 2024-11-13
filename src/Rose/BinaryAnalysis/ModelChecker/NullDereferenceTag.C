#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/NullDereferenceTag.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Result.h>
#include <Rose/StringUtility/Escape.h>

#include <SgAsmInstruction.h>

#include <boost/lexical_cast.hpp>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

NullDereferenceTag::NullDereferenceTag(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValue::Ptr &addr)
    : Tag(nodeStep), testMode_(tm), ioMode_(io), insn_(insn), addr_(addr) {}

NullDereferenceTag::~NullDereferenceTag() {}

NullDereferenceTag::Ptr
NullDereferenceTag::instance(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValue::Ptr &addr) {
    ASSERT_forbid(TestMode::OFF == tm);
    ASSERT_not_null(addr);
    return Ptr(new NullDereferenceTag(nodeStep, tm, io, insn, addr));
}

std::string
NullDereferenceTag::name() const {
    return "null pointer dereference";
}

std::string
NullDereferenceTag::printableName() const {
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
    retval += "-null";
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
NullDereferenceTag::print(std::ostream &out, const std::string &prefix) const {
    // No locks necessary since all the data members are read-only.
    out <<prefix <<name() <<"\n";

    std::string io = IoMode::READ == ioMode_ ? "read" : "write";
    std::string toFrom = IoMode::READ == ioMode_ ? "read from" : "write to";

    out <<prefix <<"  " <<io;
    switch (testMode_) {
        case TestMode::OFF:                             // shouldn't normally occur
            out <<" is null";
            break;
        case TestMode::MAY:
            out <<" may be null";
            break;
        case TestMode::MUST:
            out <<" must be null";
            break;
    }
    if (insn_)
        out <<" at instruction " <<insn_->toString();
    out <<"\n";

    out <<prefix <<"  attempted " <<toFrom <<" address " <<*addr_ <<"\n";
}

void
NullDereferenceTag::toYaml(std::ostream &out, const std::string &prefix1) const {
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

    if (insn_) {
        Unparser::Base::Ptr unparser = insn_->architecture()->newInstructionUnparser();
        unparser->settings().colorization.enabled = Color::Enabled::OFF;
        out <<prefix <<"instruction: " <<StringUtility::yamlEscape(unparser->unparse(insn_)) <<"\n";
    }

    out <<prefix <<"memory-address: " <<StringUtility::yamlEscape(boost::lexical_cast<std::string>(*addr_)) <<"\n";
}

Sarif::Result::Ptr
NullDereferenceTag::toSarif(const Sarif::Analysis::Ptr &analysis) const {
    auto result = Sarif::Result::instance(Sarif::Severity::ERROR, name());
    if (analysis) {
        if (auto rule = analysis->findRuleByName("NullPointerDereference"))
            result->rule(rule);
    }
    return result;
}

} // namespace
} // namespace
} // namespace

#endif
