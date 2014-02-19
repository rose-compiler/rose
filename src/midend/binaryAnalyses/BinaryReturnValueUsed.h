#ifndef ROSE_BinaryReturnValueUsed_H
#define ROSE_BinaryReturnValueUsed_H

namespace rose {
namespace BinaryAnalysis {

/** Contains functions that analyze whether a function returns a value which is used by the caller.
 *
 *  Currently only able to handle 32-bit x86 code that returns values via one of the AX registers. [Robb P. Matzke 2014-02-18] */
namespace ReturnValueUsed {

/** Counts how many times a return value is used versus unused. */
struct UsageCounts {
    size_t nUsed;                                       /**< Number of call sites where a return value is used. */
    size_t nUnused;                                     /**< Number of call sites where no return value is used. */
    UsageCounts(): nUsed(0), nUnused(0) {}
    void update(bool used);                             /**< Increment one of the counts. */
    UsageCounts& operator+=(const UsageCounts &other);
};

/** Accumulated results over multiple function calls. The keys are the functions that have been called, and the values are the
 *  counts for how many times a return value was used or unused by the caller. */
typedef Map<SgAsmFunction*, UsageCounts> Results;

/** Analyzes all the function calls in the specified interpretation. */
Results analyze(SgAsmInterpretation*);

} // namespace
} // namespace
} // namespace

#endif
