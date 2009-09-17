#include <satire.h>
#include "satire/satiredfa.h"

namespace SATIrE {

const char *provided_analyzers[] = {
#include "satiredfa_analyzers"
    (const char *) 0
};

DataFlowAnalysis *makeProvidedAnalyzer(const char *name) {
    std::string analyzer = name;
#include "satiredfa_cases"
    return NULL;
}

}
