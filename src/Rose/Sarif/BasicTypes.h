#ifndef ROSE_Sarif_BasicTypes_H
#define ROSE_Sarif_BasicTypes_H
#include <memory>

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

class Analysis;
class Artifact;
class Exception;
class IncrementalError;
class Location;
class Log;
class Node;
class Result;
class Rule;

/** Shared-ownership pointer to an @ref Analysis object.
 *
 * @{ */
using AnalysisPtr = std::shared_ptr<Analysis>;
using AnalysisConstPtr = std::shared_ptr<const Analysis>;
/** @} */

/** Shared-ownership pointer to an @ref Artifact object.
 *
 * @{ */
using ArtifactPtr = std::shared_ptr<Artifact>;
using ArtifactConstPtr = std::shared_ptr<const Artifact>;
/** @} */

/** Shared-ownership pointer to a @ref Location object.
 *
 * @{ */
using LocationPtr = std::shared_ptr<Location>;
using LocationConstPtr = std::shared_ptr<const Location>;
/** @} */

/** Shared-ownership pointer to a @ref Log object.
 *
 * @{ */
using LogPtr = std::shared_ptr<Log>;
using LogConstPtr = std::shared_ptr<const Log>;
/** @} */

/** Shared-ownership pointer to a @ref Node object.
 *
 *  @{ */
using NodePtr = std::shared_ptr<Node>;
using NodeConstPtr = std::shared_ptr<const Node>;
/** @} */

/** Shared-ownership pointer to a @ref Result object.
 *
 *  @{ */
using ResultPtr = std::shared_ptr<Result>;
using ResultConstPtr = std::shared_ptr<const Result>;
/** @} */

/** Shared-ownership pointer to a @ref Rule object.
 *
 * @{ */
using RulePtr = std::shared_ptr<Rule>;
using RuleConstPtr = std::shared_ptr<const Rule>;
/** @} */

} // namespace
} // namespace

#endif
