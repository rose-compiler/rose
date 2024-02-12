#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Log.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Exception.h>
#include <ROSE_UNUSED.h>

#include <Sawyer/FileSystem.h>

namespace Rose {
namespace Sarif {

bool
Log::emit(std::ostream &out) {
    out <<"{"
        <<"\"version\":\"2.1.0\""
        <<",\"$schema\":\"https://docs.oasis-open.org/sarif/sarif/v2.1.0/errata01/os/schemas/sarif-schema-2.1.0.json\"";

    if (!analyses().empty()) {
        out <<",\"runs\":[";
        std::string sep;
        for (auto &analysis: analyses()) {
            out <<sep;
            analysis->emit(out);
            sep = ",";
        }
        out <<"]";
    }

    out <<"}";
    return true;
}

} // namespace
} // namespace

#endif
