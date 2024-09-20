#ifndef ROSE_BinaryAnalysis_ReadWriteSets_H
#define ROSE_BinaryAnalysis_ReadWriteSets_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Rose/BinaryAnalysis/Variables.h>

namespace Rose {
namespace BinaryAnalysis {

class ReadWriteSets {
public:
    /** Shared ownership pointer for @ref ReadWriteSets. */
    using Ptr = ReadWriteSetsPtr;

    /** Settings that control this analysis. */
    struct Settings {
        Variables::VariableFinder::Settings variableFinder;
    };

private:
    Settings settings_;
    Partitioner2::PartitionerConstPtr partitioner_;

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

    /** Analyze a function.
     *
     *  The function is analyzed (or re-analyzed if it was previously analyzed) and the results are saved in this object. */
    void analyze(const Partitioner2::FunctionPtr&);

#if 0 // [Robb Matzke 2024-09-24]
    /** Test whether a function was analyzed already. */
    bool exists(const Partitioner2::FunctionPtr&) const;

    /** Forget the results for the specified function. */
    void erase(const Partitioner2::FunctionPtr&);
#endif
};

} // namespace
} // namespace

#endif
#endif
