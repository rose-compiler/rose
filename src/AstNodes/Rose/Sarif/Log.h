#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#ifdef ROSE_IMPL
#include <Rose/Sarif/Analysis.h>
#endif

namespace Rose {
namespace Sarif {

/** SARIF results log.
 *
 *  This class represents an entire SARIF log, which accumulates results from analyses and eventually emits them as a document that
 *  can be stored in a file.  The log is the root of a tree data structure, an internal representation of the SARIF data, which does
 *  not impart any particular file format to the data. See @ref Rose::Sarif for an overview.
 *
 *  A @ref Sarif::Log is a list of @ref Sarif::Analysis objects which correspond to analyses run individually or as part of a larger
 *  tool.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_example */
class Log: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** List of analyses.
     *
     *  A log has a list of zero or more analyses, each of which has zero or more results. */
    [[Rosebud::not_null]]
    EdgeVector<Analysis> analyses;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
