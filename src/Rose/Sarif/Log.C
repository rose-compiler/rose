#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Log.h>

#include <Rose/Sarif/Analysis.h>
#include <Rose/Sarif/Exception.h>

#include <Sawyer/FileSystem.h>
#include <boost/bind/bind.hpp>
#include <fstream>

#ifndef _MSC_VER
#include <array>
#include <cstdio>
#endif

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
    out <<p <<"$schema: \"https://docs.oasis-open.org/sarif/sarif/v2.1.0/errata01/os/schemas/sarif-schema-2.1.0.json\"\n";

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
Log::emitYaml(std::ostream &out) {
    emitYaml(out, "");
}

void
Log::emitJson(std::ostream &out) {
#ifdef _MSC_VER
    throw Sarif::Exception("JSON output is not supported when ROSE is configured to be built with a Microsoft compiler");
#else
    Sawyer::FileSystem::TemporaryFile yamlFile;
    emitYaml(yamlFile.stream());
    yamlFile.stream().close();

    const std::string cmd = "yq -ojson " + StringUtility::bourneEscape(yamlFile.name().string());
    std::array<char, 4096> buf;
    std::unique_ptr<FILE, decltype(&pclose)> input(popen(cmd.c_str(), "r"), pclose);
    if (!input)
        throw Sarif::Exception("cannot run command \"" + StringUtility::cEscape(cmd) + "\"");
    while (true) {
        const size_t nRead = fread(buf.data(), 1, buf.size(), input.get());
        if (0 == nRead) {
            if (ferror(input.get()))
                throw Sarif::Exception("problem reading command output from \"" + StringUtility::cEscape(cmd) + "\"");
            break;
        }

        out.write(buf.data(), nRead);
        if (!out)
            throw Sarif::Exception("problem writing to stream");
    }
#endif
}

std::string
Log::emissionPrefix() {
    return "";
}

} // namespace
} // namespace

#endif
