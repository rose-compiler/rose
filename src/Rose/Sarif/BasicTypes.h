#ifndef ROSE_Sarif_BasicTypes_H
#define ROSE_Sarif_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <RoseFirst.h>

#include <memory>

#include <Rose/Sarif/NodeDeclarations.h>

namespace Rose {
namespace Sarif {

/** Analysis result kind. */
enum class Kind {
    PASS,                                               /**< Rule was evaluated and no problem found. */
    OPEN,                                               /**< Rule was evaluated with insufficient information. */
    INFORMATIONAL,                                      /**< Rule was evaluated and produced a purely informational result. */
    NOT_APPLICABLE,                                     /**< Rule was not evaluated because it doesn't apply. */
    REVIEW,                                             /**< The result requires review by a human. */
    FAIL                                                /**< The result represents a problem whose severity is specified. */
};

/** Analysis result severity. */
enum class Severity {
    NONE,                                               /**< No specified severity. */
    NOTE,                                               /**< A minor problem. */
    WARNING,                                            /**< A problem. */
    ERROR                                               /**< A serious problem. */
};

} // namespace
} // namespace

#endif
#endif
