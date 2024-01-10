#ifndef ROSE_Sarif_Log_H
#define ROSE_Sarif_Log_H
#include <Rose/Sarif/Node.h>

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
 *  For an incremental log the information is emitted in the following order. Once one of the information collections is emitted
 *  it is an error to go back and modify an earlier collection. The error is indicated by throwing a @ref Sarif::IncrementalError.
 *
 *  @li Properties of this object.
 *  @li The list of @ref analyses "analysis" objects.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C log_example */
class Log: public Node {
public:
    /** Shared-ownership pointer to a @ref Log object.
     *
     * @{ */
    using Ptr = LogPtr;
    using ConstPtr = LogConstPtr;
    /** @} */

private:
    std::ostream *out_ = nullptr;

public:
    /** List of analyses.
     *
     *  A log has a list of zero or more analyses, each of which has zero or more results. */
    EdgeVector<Analysis> analyses;

public:
    ~Log();
protected:
    Log();                                             // use `instance` instead

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Emit the analysis results to a stream. */
    void emit(std::ostream&);

    /** Start synchronous emission.
     *
     *  When a log is in the incremental emission state, adding a new node to the log (or any of its descendents) immediately
     *  outputs the node to a stream, and modifying properties of a node that's attached to the log (or any of its descendants) is
     *  an error since the property would have already been emitted. When a log is in incremental mode, nodes must be attached to a
     *  tree in a well defined order. If the order requirements are violated then an @ref IncrementalError exception is thrown. */
    void incremental(std::ostream&);

public:
    bool isIncremental() override;
    std::ostream& incrementalStream() override;
    void emitYaml(std::ostream&, const std::string&) override;
    std::string emissionPrefix() override;

private:
    // Callback for prior to resizing the `analyses` member
    void checkAnalysesResize(int delta, const AnalysisPtr&);

    // Callback after resizing the `analyses` member
    void handleAnalysesResize(int delta, const AnalysisPtr&);
};

} // namespace
} // namespace
#endif
