#ifndef ROSE_Sarif_Analysis_H
#define ROSE_Sarif_Analysis_H
#include <Rose/Sarif/Node.h>

#include <Sawyer/Optional.h>
#include <string>
#include <vector>

namespace Rose {
namespace Sarif {

/** One run of one analysis.
 *
 *  This class represents one run of one analysis and all the results from that run. An analysis can be one of many analyses run by
 *  a single tool, or can be the only analysis run by a tool.  Each analysis has a name and an optional command-line invocation. All
 *  the results for one analysis are stored in that object's @ref results vector. */
class Analysis: public Node {
public:
    /** Shared-ownership pointer to an @ref Analysis object.
     *
     * @{ */
    using Ptr = AnalysisPtr;
    using ConstPtr = AnalysisConstPtr;
    /** @} */

private:
    std::string name_;                                  // analysis name
    std::vector<std::string> commandLine_;              // command and arguments
    Sawyer::Optional<int> exitStatus_;                  // exit status of the analysis

public:
    /** Results for this analysis. */
    EdgeVector<Result> results;

public:
    ~Analysis();
protected:
    explicit Analysis(const std::string &name);         // use `instance` instaed
public:
    /** Allocating constructor.
     *
     *  The @p name is the name of the analysis. */
    static Ptr instance(const std::string &name);

    /** Property: Analysis name.
     *
     *  The analysis name is any string, usually one line, breifly describing the analysis. This property is set by the
     *  constructor. */
    const std::string &name() const;

    /** Property: Command-line.
     *
     *  The array of strings forming the command-line for this analysis.  The first member of the array is the name of the tool, and
     *  subsequent members are the arguments passed to that tool.
     *
     * @{ */
    const std::vector<std::string>& commandLine() const;
    void commandLine(const std::vector<std::string>&);
    /** @} */

    /** Property: Command exit status.
     *
     *  This is the exit status of the command if the command did not exit because of a signal.
     *
     * @{ */
    const Sawyer::Optional<int>& exitStatus() const;
    void exitStatus(const Sawyer::Optional<int>&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    // Construct a shell command-line from the command-line arguments
    static std::string makeShellCommand(const std::vector<std::string>&);

    // Emit YAML for a command line if it isn't empty.
    void emitCommandLine(std::ostream&, const std::string &prefix);

    // Emit YAML for an exit status if it isn't empty. If the command-line isn't empty, then this must be called immediately after
    // emitCommandLine.
    void emitExitStatus(std::ostream&, const std::string &prefix);

    // Called before a new result is added to the `results` vector.
    void checkResultsResize(int delta, const ResultPtr&);

    // Called after adding a new result to the `results` vector.
    void handleResultsResize(int delta, const ResultPtr&);
};

} // namespace
} // namespace
#endif
