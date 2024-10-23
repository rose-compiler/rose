// [Robb Matzke 2024-10-22]: Deprecated. Use Rose::BinaryAnalysis::Unparser
#ifndef ROSE_AsmUnparser_H
#define ROSE_AsmUnparser_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

#include <ROSE_DEPRECATED.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <ostream>
#include <string>

class SgNode;

namespace Rose {
namespace BinaryAnalysis {

// [Robb Matzke 2024-10-23]: Deprecated
class AsmUnparser {
public:
    using LabelMap = std::map<uint64_t, std::string>;

    virtual size_t unparse(std::ostream&, SgNode *ast) ROSE_DEPRECATED("use Rose::BinaryAnalysis::Unparser");
};

} // namespace
} // namespace

#endif
#endif
