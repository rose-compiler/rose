#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <Sawyer/Optional.h>
#include <string>
#include <vector>

#ifdef ROSE_IMPL
#include <Rose/Sarif/Artifact.h>
#include <Rose/Sarif/Rule.h>
#include <Rose/Sarif/Result.h>
#endif

namespace Rose {
namespace Sarif {

/** One run of one analysis.
 *
 *  This class represents one run of one analysis and all the results from that run. An analysis can be one of many analyses run by
 *  a single tool, or can be the only analysis run by a tool.  Each analysis has a name and an optional command-line invocation. All
 *  the results for one analysis are stored in that object's @ref results vector. The results may point to specific @ref rules and
 *  @ref artifacts associated with this analysis.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_analysis */
class Analysis: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Analysis name.
     *
     *  The analysis name is any string, usually one line, breifly describing the analysis. This property is set by the
     *  constructor. */
    [[Rosebud::ctor_arg, Rosebud::mutators()]]
    std::string name;

    /** Property: Version.
     *
     *  Version string for the tool. */
    [[Rosebud::property]]
    std::string version;

    /** Property: Information URI.
     *
     *  URI pointing to more information about this analysis. */
    [[Rosebud::property]]
    std::string informationUri;

    /** Rules associated with this analysis. */
    [[Rosebud::not_null]]
    EdgeVector<Rule> rules;

    /** Property: Command-line.
     *
     *  The array of strings forming the command-line for this analysis.  The first member of the array is the name of the tool, and
     *  subsequent members are the arguments passed to that tool. */
    [[Rosebud::large]]
    std::vector<std::string> commandLine;

    /** Property: Command exit status.
     *
     *  This is the exit status of the command if the command did not exit because of a signal. */
    [[Rosebud::property]]
    Sawyer::Optional<int> exitStatus;

    /** Artifacts for this analysis. */
    [[Rosebud::not_null]]
    EdgeVector<Artifact> artifacts;

    /** Results for this analysis. */
    [[Rosebud::not_null]]
    EdgeVector<Result> results;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Find a rule by its ID.
     *
     *  Scans through the @ref rules and returns the first one whose ID is the specified string, or null if no such rule exists. */
    RulePtr findRuleById(const std::string&);

    /** Find a rule by its name.
     *
     *  Scans through the @ref rules and returns the first one whose name property matches the specified string, or null if no such
     *  rule exists. */
    RulePtr findRuleByName(const std::string&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Private functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Construct a shell command-line from the command-line arguments
    static std::string makeShellCommand(const std::vector<std::string>&);
};

} // namespace
} // namespace
#endif
