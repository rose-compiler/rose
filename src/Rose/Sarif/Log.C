#include <sage3basic.h>
#include <Rose/Sarif/Log.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Exception.h>

#include <boost/bind/bind.hpp>
#include <fstream>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

Log::~Log() {}

Log::Log()
    : analyses(*this) {
    analyses.beforeResize(boost::bind(&Log::checkAnalysesResize, this, _1, _2));
    analyses.afterResize(boost::bind(&Log::handleAnalysesResize, this, _1, _2));
}

Log::Ptr
Log::instance() {
    return Ptr(new Log);
}

void
Log::incremental(std::ostream &out) {
    if (out_)
        throw IncrementalError("log is already incremental");
    out_ = &out;
    emitYaml(out, "");
}

std::ostream&
Log::incrementalStream() {
    if (out_) {
        return *out_;
    } else {
        static std::ofstream empty;
        return empty;
    }
}

bool
Log::isIncremental() {
    return out_ != nullptr;
}

void
Log::checkAnalysesResize(int delta, const Analysis::Ptr &analysis) {
    if (!analysis)
        throw Sarif::Exception("cannot add null analysis to the log");
    if (isIncremental() && delta < 0)
        throw IncrementalError("analyses cannot be removed from a log");
}

void
Log::handleAnalysesResize(int delta, const Analysis::Ptr &analysis) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(analyses.empty());

        // Make sure we can't change this pointer in the future
        lock(analyses.back(), "analyses");

        // Prior analysis can no longer be modified
        if (analyses.size() >= 2)
            analyses[analyses.size() - 2]->freeze();

        // Emit this new analysis
        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == analyses.size())
            out <<p <<"runs:\n";
        analysis->emitYaml(out, makeListPrefix(p));
    }
}

void
Log::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    out <<firstPrefix <<"version: 2.1.0\n";
    const std::string p = makeNextPrefix(firstPrefix);
    out <<p <<"$schema: \"https://schemastore.azurewebsites.net/schemas/json/sarif-2.1.0-rtm.4.json\"\n";

    if (!analyses.empty()) {
        out <<p <<"runs:\n";
        for (auto &analysis: analyses) {
            analysis->emitYaml(out, makeListPrefix(p));
            if (isIncremental()) {
                lock(analysis, "analyses");
                if (analysis != analyses.back())
                    analysis->freeze();
            }
        }
    }
}

void
Log::emit(std::ostream &out) {
    emitYaml(out, "");
}

std::string
Log::emissionPrefix() {
    return "";
}

} // namespace
} // namespace
