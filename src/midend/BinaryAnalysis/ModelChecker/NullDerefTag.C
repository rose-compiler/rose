#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <BinaryAnalysis/ModelChecker/NullDerefTag.h>

#include <BaseSemanticsSValue.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

NullDerefTag::NullDerefTag(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValuePtr &addr)
    : Tag(nodeStep), testMode_(tm), ioMode_(io), insn_(insn), addr_(addr) {}

NullDerefTag::~NullDerefTag() {}

NullDerefTag::Ptr
NullDerefTag::instance(size_t nodeStep, TestMode tm, IoMode io, SgAsmInstruction *insn, const BS::SValuePtr &addr) {
    ASSERT_forbid(TestMode::OFF == tm);
    ASSERT_not_null(addr);
    return Ptr(new NullDerefTag(nodeStep, tm, io, insn, addr));
}

std::string
NullDerefTag::printableName() const {
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
NullDerefTag::print(std::ostream &out, const std::string &prefix) const {
    // No locks necessary since all the data members are read-only.
    out <<prefix <<"null pointer dereference\n";

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
        out <<prefix <<" at instruction " <<insn_->toString();
    out <<"\n";

    out <<prefix <<"  attempted " <<toFrom <<" address " <<*addr_ <<"\n";
}

} // namespace
} // namespace
} // namespace

#endif
