#ifndef ROSE_Sarif_Log_H
#define ROSE_Sarif_Log_H
#include <Rose/Sarif/Node.h>

namespace Rose {
namespace Sarif {

/** SARIF results log.
 *
 *  This class represents an entire SARIF log, which will accumulate results from analyses and eventually emit them as a document
 *  that can be stored in a file.  The log is a tree data structure, an internal representation of the SARIF data, which does not
 *  impart any particular file format to the data. The top level of the tree (children of the log) is a list of @ref Analysis
 *  objects stored in the @ref analyses data member, each of which in turn stores the results for that analysis.
 *
 *  The SARIF internal representation can be unparsed in two ways: on demand by calling an "emit" function, or incrementally as
 *  nodes are added. The incremental output is intended for long-running analysis that wants to stream results to a consumer (such
 *  as a file or pipe) as they are produced. A log can be switched from on-demand to incremental mode at any time. */
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
