// Analysis to determine whether functions return values. This looks at the call site for each function to see if the caller
// uses a return value.  It works only for 32-bit x86 and assumes that the return value is via EAX.

#include "rose.h"
#include "BinaryReturnValueUsed.h"

using namespace rose::BinaryAnalysis;

int main(int argc, char *argv[]) {
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    assert(!interps.empty());                            // specimem must have at least one binary interpretation;
    SgAsmInterpretation *interp = interps.back();

    ReturnValueUsed::Results results = ReturnValueUsed::analyze(interp);
    for (ReturnValueUsed::Results::const_iterator ri=results.begin(); ri!=results.end(); ++ri) {
        printf("%-48s %6zu/%-6zu %3d%%\n", ri->first->get_name().c_str(), ri->second.nUsed,
               (ri->second.nUsed + ri->second.nUnused),
               (int)round(100.0 * ri->second.nUsed / (ri->second.nUsed + ri->second.nUnused)));
    }

    return 0;
}
