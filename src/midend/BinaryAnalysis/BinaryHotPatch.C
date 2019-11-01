#include <sage3basic.h>

#include <BaseSemanticsRiscOperators.h>
#include <BaseSemanticsSValue.h>
#include <BinaryHotPatch.h>
#include <boost/foreach.hpp>

using namespace Sawyer::Message::Common;

typedef Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValuePtr SValuePtr;
typedef Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperatorsPtr RiscOperatorsPtr;

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
HotPatch::apply(const RiscOperatorsPtr &ops) const {
    ASSERT_not_null(ops);
    size_t nMatches = 0;

    BOOST_FOREACH (const Record &record, records_) {
        bool matched = false;
        switch (record.type()) {
            case Record::PATCH_NONE:
                break;

            case Record::PATCH_REGISTER:
                if (!record.reg().isEmpty() && record.oldValue() != NULL) {
                    SValuePtr currentValue = ops->peekRegister(record.reg());
                    if (record.oldValue()->must_equal(currentValue)) {
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
