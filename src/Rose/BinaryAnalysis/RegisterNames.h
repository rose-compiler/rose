#ifndef ROSE_BinaryAnalysis_RegisterNames_H
#define ROSE_BinaryAnalysis_RegisterNames_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BasicTypes.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>

namespace Rose {
namespace BinaryAnalysis {

/** Convert a register descriptor to a name.
 *
 *  Prints a register name even when no dictionary is available or when the dictionary doesn't contain an entry for the
 *  specified descriptor. */
class RegisterNames {
public:
    /** Constructor.
     *
     *  A RegisterDictionary can be supplied to the constructor, or to each operator() call.
     *
     * @{ */
    RegisterNames();
    explicit RegisterNames(const RegisterDictionaryPtr &dict);
    /** @} */

    /** Obtain a name for a register descriptor.
     *
     *  If a dictionary is supplied, then it will be used instead of the dictionary that was supplied to the constructor.
     *
     *  @{ */
    std::string operator()(RegisterDescriptor) const;
    std::string operator()(RegisterDescriptor, const RegisterDictionaryPtr &dict) const;
    /** @} */

    RegisterDictionaryPtr dictionary;    /**< Dictionary supplied to the constructor. */
    std::string prefix = "REG";          /**< The leading part of a register name. */
    std::string suffix;                  /**< String to print at the very end of the generated name. */
    int showOffset = -1;                 /**< 0=>never show offset; positive=>always show; negative=>show only when non-zero */
    std::string offsetPrefix = "@";      /**< String printed before the offset when the offset is shown. */
    std::string offsetSuffix;            /**< String printed after the offset when the offset is shown. */
    int showSize = -1;                   /**< 0=>never; positive=>always; negative=>when offset is non-zero */
    std::string sizePrefix = "+";        /**< String printed prior to the size when the size is printed. */
    std::string sizeSuffix;              /**< String printed after the size when the size is printed. */
};

} // namespace
} // namespace
#endif
#endif
