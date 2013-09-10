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

