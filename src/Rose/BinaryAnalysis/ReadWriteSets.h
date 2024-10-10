#ifndef ROSE_BinaryAnalysis_ReadWriteSets_H
#define ROSE_BinaryAnalysis_ReadWriteSets_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/BinaryAnalysis/Variables.h>

namespace Rose {
namespace BinaryAnalysis {

/** Sets of variables based on whether they're read or written.
 *
 *  This class is mostly just a reorganization of the results from a variable detection analysis, and thus has the same caveats as
 *  @ref Variables "that analysis". */
class ReadWriteSets {
public:
    /** Shared ownership pointer for @ref ReadWriteSets. */
    using Ptr = ReadWriteSetsPtr;

    /** Settings that control this analysis. */
    struct Settings {
        /** Whether to find local (stack) variables.
         *
         *   Local variables can be found by searching locally within the desired functions. */
        bool findingLocalVariables;

        /** Whether to find global variables.
         *
         *  Finding global variables is a global analysis that must look at all functions and symbol tables in the binary specimen
         *  even when the results of this analysis are expected to show only those global variables referenced by a particular set
         *  of functions. */
        bool findingGlobalVariables;

        /** Settings for the analysis that finds the variables. */
        Variables::VariableFinder::Settings variableFinder;

        Settings()
            : findingLocalVariables(true), findingGlobalVariables(true) {}
    };

private:
    Settings settings_;
    Partitioner2::PartitionerConstPtr partitioner_;
    Variables::StackVariables stackVariables_;
    Variables::GlobalVariables globalVariables_;

public:
    ~ReadWriteSets();
protected:
    ReadWriteSets() = delete;
    ReadWriteSets(const Partitioner2::PartitionerConstPtr&, const Settings&);
public:
    /** Allocating constructor. */
    static Ptr instance(const Partitioner2::PartitionerConstPtr&, const Settings& = Settings());

    /** Property: Settings for this analysis.
     *
     * @{ */
    const Settings& settings() const;
    Settings& settings();
    /** @} */

    /** Command-line switches for this analysis.
     *
     *  The returned command line switch group will capture a reference to the `Settings` argument so that when the command-line
     *  is parsed later those settings will be adjusted. The returned switch group does not affect the `variableFinder` switches
     *  in the settings--you should call @ref Variables::VariableFinder::commandLineSwitches if you want to do that. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

    /** Analyze a function.
     *
     *  The function is analyzed (or re-analyzed if it was previously analyzed) and the results are saved in this object. */
    void analyze(const Partitioner2::FunctionPtr&);

    /** Print results.
     *
     *  The multi-line output is sent to the specified stream and each line of output is prefixed by the specified string. */
    void print(std::ostream&, const std::string &prefix = "") const;


#if 0 // [Robb Matzke 2024-09-24]
    /** Test whether a function was analyzed already. */
    bool exists(const Partitioner2::FunctionPtr&) const;

    /** Forget the results for the specified function. */
    void erase(const Partitioner2::FunctionPtr&);
#endif
};

/** Print a read-write set. */
std::ostream& operator<<(std::ostream&, const ReadWriteSets&);

} // namespace
} // namespace

#endif
#endif
