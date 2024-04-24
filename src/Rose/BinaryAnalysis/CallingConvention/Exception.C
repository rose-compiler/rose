#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/Exception.h>

#include <Rose/StringUtility/Escape.h>

#include <SgAsmType.h>

#include <boost/lexical_cast.hpp>

#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

AllocationError
AllocationError::cannotAllocateArgument(int argIndex, const std::string &argName, SgAsmType *type, const std::string &decl) {
    return AllocationError("cannot allocate function argument #" + boost::lexical_cast<std::string>(argIndex) +
                           (argName.empty() ? "" : " \"" + StringUtility::cEscape(argName) + "\"") +
                           (type ? " having type " + type->toString() : "") +
                           (decl.empty() ? "" : " in \"" + StringUtility::cEscape(decl) + "\""));
}

AllocationError
AllocationError::cannotAllocateReturn(SgAsmType *type, const std::string &decl) {
    return AllocationError("cannot allocate return value" +
                           (type ? " having type " + type->toString() : "") +
                           (decl.empty() ? "" : " in \"" + StringUtility::cEscape(decl) + "\""));
}

} // namespace
} // namespace
} // namespace

#endif
