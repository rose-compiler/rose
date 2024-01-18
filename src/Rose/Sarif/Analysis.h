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
 *  the results for one analysis are stored in that object's @ref results vector. The results may point to specific @ref rules and
 *  @ref artifacts associated with this analysis.
 *
 *  For an incremental log the information is emitted in the following order. Once one of the information collections is emitted
 *  it is an error to go back and modify an earlier collection. The error is indicated by throwing a @ref Sarif::IncrementalError.
 *
 *  @li Properties such as @ref Analysis::name "name", @ref commandLine "command-line", @ref exitStatus "exit status", etc.
 *  @li The list of @ref rules.
 *  @li The list of @ref artifacts.
 *  @li The list of @ref results.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C analysis_example */
class Analysis: public Node {
public:
    /** Shared-ownership pointer to an @ref Analysis object.
     *
     * @{ */
    using Ptr = AnalysisPtr;
    using ConstPtr = AnalysisConstPtr;
    /** @} */

    // Data is emitted in this order. Each group of data members must be fully emitted before the next group.
    // Group 1: These are all part of the runs[*].tool.driver object
private:
    std::string name_;                                  // analysis name
    std::string version_;
    std::string informationUri_;                        // optional URI pointing to more information

    // Group 2: Stored in runs[*].tool.driver.rules
public:
    /** Rules for this analysis. */
    EdgeVector<Rule> rules;                             // part of runs[*].tool object

    // Group 3: These are all part of the "runs[*].invocations[0]" object
private:
    std::vector<std::string> commandLine_;              // command and arguments, must be emitted before exit status
    Sawyer::Optional<int> exitStatus_;                  // exit status of the analysis

    // Group 4: each vector must be emitted in full before the next vector
public:
    /** Artifacts for this analysis. */
    EdgeVector<Artifact> artifacts;
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
    const std::string& name() const;

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

    /** Property: Version.
     *
     *  Version string for the tool.
     *
     * @{ */
    const std::string& version() const;
    void version(const std::string&);
    /** @} */

    /** Property: Information URI.
     *
     *  URI pointing to more information about this analysis.
     *
     * @{ */
    const std::string& informationUri() const;
    void informationUri(const std::string&);
    /** @} */

    /** Find a rule by its ID.
     *
     *  Scans through the @ref rules and returns the first one whose ID is the specified string, or null if no such rule exists. */
    RulePtr findRuleById(const std::string&);

    /** Find a rule by its name.
     *
     *  Scans through the @ref rules and returns the first one whose name property matches the specified string, or null if no such
     *  rule exists. */
    RulePtr findRuleByName(const std::string&);

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    // Construct a shell command-line from the command-line arguments
    static std::string makeShellCommand(const std::vector<std::string>&);

    void emitCommandLine(std::ostream&, const std::string &prefix);
    void emitExitStatus(std::ostream&, const std::string &prefix);
    void emitVersion(std::ostream&, const std::string &prefix);
    void emitInformationUri(std::ostream&, const std::string &prefix);

    // These signal slots are called before ("check") and after ("handle") a vector of edges is resized.
    void checkRulesResize(int delta, const RulePtr&);
    void handleRulesResize(int delta, const RulePtr&);
    void checkArtifactsResize(int delta, const ArtifactPtr&);
    void handleArtifactsResize(int delta, const ArtifactPtr&);
    void checkResultsResize(int delta, const ResultPtr&);
    void handleResultsResize(int delta, const ResultPtr&);
};

} // namespace
} // namespace
#endif
