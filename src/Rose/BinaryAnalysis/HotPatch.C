#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/HotPatch.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/Diagnostics.h>

using namespace Sawyer::Message::Common;

using SValue = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::SValue;
using RiscOperators = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RiscOperators;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility HotPatch::mlog;

// class method
void
HotPatch::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::HotPatch");
        mlog.comment("semantic hot patching");
    }
}

size_t
HotPatch::apply(const RiscOperators::Ptr &ops) const {
    ASSERT_not_null(ops);
    size_t nMatches = 0;

    for (const Record &record: records_) {
        bool matched = false;
        switch (record.type()) {
            case Record::PATCH_NONE:
                break;

            case Record::PATCH_REGISTER:
                if (!record.reg().isEmpty() && record.oldValue() != NULL) {
                    SValue::Ptr currentValue = ops->peekRegister(record.reg());
                    if (record.oldValue()->mustEqual(currentValue)) {
                        ASSERT_not_null(record.newValue());
                        SAWYER_MESG(mlog[DEBUG]) <<"changing " <<record.reg()
                                                 <<" from " <<*currentValue
                                                 <<" to " <<*record.newValue() <<"\n";
                        ops->writeRegister(record.reg(), record.newValue());
                        matched = true;
                    }
                }
                break;
        }

        if (matched) {
            ++nMatches;
            switch (record.behavior()) {
                case Record::MATCH_CONTINUE:
                    break;
                case Record::MATCH_BREAK:
                    return nMatches;
            }
        }
    }
    return nMatches;
}

} // namespace
} // namespace

#endif
