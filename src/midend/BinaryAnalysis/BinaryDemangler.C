#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryDemangler.h>

#include <rose_getline.h>

#include <cctype>
#include <boost/algorithm/string/trim.hpp>
#include <Sawyer/FileSystem.h>

namespace Rose {
namespace BinaryAnalysis {

void
Demangler::fillCache(const std::vector<std::string> &mangledNames) {
    // Save mangled names to a file.  If the mangled name contains certain special characters then don't attempt to demangle it.
    Sawyer::FileSystem::TemporaryFile mangledFile;
    BOOST_FOREACH (const std::string &s, mangledNames) {
        bool isGood = true;
        for (size_t i=0; isGood && i<s.size(); ++i)
            isGood = isgraph(s[i]);
        if (isGood) {
            mangledFile.stream() <<s <<"\n";
        } else {
            mangledFile.stream() <<"\n";
        }
    }
    mangledFile.stream().close();

    // Run c++filt and read its output.
    std::string cmd = (cxxFiltExe_.empty() ? "c++filt" : cxxFiltExe_.string()) +
                      " -s " + (compiler_.empty() ? "auto" : compiler_) +
                      " < " + mangledFile.name().string();

    const char *failure = NULL;
    if (FILE *f = popen(cmd.c_str(), "r")) {
        char *line = NULL;
        size_t linesz = 0;
        for (size_t i = 0; i < mangledNames.size(); ++i) {
            ssize_t nread = rose_getline(&line, &linesz, f);
            if (nread <= 0) {
                failure = "short read";
                break;
            }
            std::string s(line, line+nread);
            boost::trim(s);
            if (s.empty())
                s = mangledNames[i];
            nameMap_.insert(mangledNames[i], s);
        }
        if (pclose(f) != 0)
            failure = "command failed";
        if (line!=NULL)
            free(line);
    }

    if (failure != NULL) {
        throw std::runtime_error(std::string(failure) + " in Rose::BinaryAnalysis::Demangler for command \"" +
                                 StringUtility::cEscape(cmd) + "\"");
    }
}

std::string
Demangler::demangle(const std::string &mangledName) {
    std::string retval;
    if (!nameMap_.getOptional(mangledName).assignTo(retval)) {
        std::vector<std::string> mangledNames(1, mangledName);
        fillCache(mangledNames);
        retval = nameMap_[mangledName];                 // an exception here means fillCache failed
    }
    return retval;
}

void
Demangler::insert(const std::string &mangledName, const std::string &demangledName) {
    ASSERT_forbid(mangledName.empty());
    ASSERT_forbid(demangledName.empty());
    nameMap_.insert(mangledName, demangledName);
}


} // namespace
} // namespace

#endif
