#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Log.h>
#include <Rose/Sarif/Rule.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

Rose::Sarif::Log::Ptr
makeSarifLog() {
    return Rose::Sarif::Log::instance();
}

void
insertSarifRules(const Rose::Sarif::Analysis::Ptr &analysis) {
    ASSERT_not_null(analysis);

    if (!analysis->findRuleById("nullptr")) {
        auto rule = Rose::Sarif::Rule::instance("RBMC20241", "memory at low addresses should not be read or written");
        rule->name("NullPointerDereference");
        rule->helpUri("file://src/Rose/BinaryAnalysis/ModelChecker/NullDereferenceTag.h");
        analysis->rules().push_back(rule);
    }

    if (!analysis->findRuleById("uninit")) {
        auto rule = Rose::Sarif::Rule::instance("RBMC20242", "memory should be initialized before being read");
        rule->name("UninitializedRead");
        rule->helpUri("file://src/Rose/BinaryAnalysis/ModelChecker/UninitializedVariableTag.h");
        analysis->rules().push_back(rule);
    }

    if (!analysis->findRuleById("oob")) {
        auto rule = Rose::Sarif::Rule::instance("RBMC20243", "the intended variable access was out-of-bounds");
        rule->name("OutOfBoundsAccess");
        rule->helpUri("file://src/Rose/BinaryAnalysis/ModelChecker/OutOfBoundsTag.h");
        analysis->rules().push_back(rule);
    }

    if (!analysis->findRuleById("error")) {
        auto rule = Rose::Sarif::Rule::instance("RBMC20244", "the analysis encountered an internal problem");
        rule->name("Error");
        rule->helpUri("file://src/Rose/BinaryAnalysis/ModelChecker/ErrorTag.h");
        analysis->rules().push_back(rule);
    }
}

} // namespace
} // namespace
} // namespace

#endif
