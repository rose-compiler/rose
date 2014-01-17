#ifndef ROSE_SAGESUPPORT_CMDLINE_H
#define ROSE_SAGESUPPORT_CMDLINE_H

/**
 * \file    cmdline.h
 * \author  Justin Too <too1@llnl.gov>
 * \date    April 4, 2012
 */

/*-----------------------------------------------------------------------------
 *  Dependencies
 *---------------------------------------------------------------------------*/
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage_support.h"

namespace SageSupport {
namespace Cmdline {
  static void
  makeSysIncludeList(const Rose_STL_Container<string> &dirs,
                     Rose_STL_Container<string> &result);

  //! Convert <tt>-I &lt;path&gt;</tt> to <tt>-I&lt;path&gt;</tt>
  //
  // TOO1 (11/21/2013): Current CLI handling assumes that there is no space
  // between the -I option and its <path> option. That is,
  //
  //      +----------+------------+
  //      | Valid    |  -I<path>  |
  //      +----------+------------+
  //      | Invalid  |  -I <path> |
  //      +----------+------------+
  //
  // Note: Path argument is validated for existence.
  //
  std::vector<std::string>
  NormalizeIncludePathOptions (std::vector<std::string>& argv);

  void
  ProcessKeepGoing (SgProject* project, std::vector<std::string>& argv);

  namespace X10 {
    static std::string option_prefix = "-rose:x10:";

    void
    Process (SgProject* project, std::vector<std::string>& argv);

    // -rose:x10
    void
    ProcessX10Only (SgProject* project, std::vector<std::string>& argv);
  } // namespace SageSupport::Cmdline::X10

} // namespace SageSupport::Cmdline
} // namespace SageSupport
#endif // ROSE_SAGESUPPORT_CMDLINE_H

