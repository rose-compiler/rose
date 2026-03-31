#ifndef ROSE_BinaryAnalysis_CilStringFinder_H
#define ROSE_BinaryAnalysis_CilStringFinder_H 1

#include <featureTests.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <rosedll.h>
#include <functional>
#include <vector>
#include <string>

struct SgProject;


namespace Rose {
namespace BinaryAnalysis {


/// A StringFinder class for CIL binaries
/// \todo integrate with the binary StringFinder
struct ROSE_DLL_API CilStringFinder
{
  using StringCollectorFn = std::function<void(std::string s)>; ///< callback for storing strings

  /// Settings for specifying which kind of CIL strings shall be found.
  enum CilStringKinds
  {
    none     = 0,
    literals = 1 << 0, ///< literals on the US heap referenced in the code section of methods
    metadata = 1 << 1, ///< strings referenced in the CIL metadata
    all      = literals | metadata,
  };


  /// depending on the settings of \p kind finds strings in \p proj and calls \p collector for each one.
  /// \param proj     a ROSE project containing CIL metadata
  /// \param collecor a string collection functor
  /// \param kinds    settings identifying which strings shall be found
  void
  collect(const SgProject& proj, StringCollectorFn collector, CilStringKinds kinds = CilStringKinds::all) const;

  /// depending on the settings of \p kind finds strings in \p proj and calls \p collector for each one.
  /// \param proj     a ROSE project containing CIL metadata
  /// \param collecor a string collection functor
  /// \param kinds    settings identifying which strings shall be found
  /// \return a vector of strings found in a CIL project \p proj.
  std::vector<std::string>
  find(const SgProject& proj, CilStringKinds kinds = CilStringKinds::all) const;
};

}
}

#endif /* ROSE_ENABLE_BINARY_ANALYSIS */

#endif /* ROSE_BinaryAnalysis_CilStringFinder_H */
