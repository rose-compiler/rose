#ifndef ROSE_BinaryAnalysis_BasicTypes_H
#define ROSE_BinaryAnalysis_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RegisterDictionary;
using RegisterDictionaryPtr = Sawyer::SharedPointer<RegisterDictionary>; /**< Reference counting pointer. */

} // namespace
} // namespace

#endif
#endif
