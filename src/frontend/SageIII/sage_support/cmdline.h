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
namespace /*SageSupport::*/Cmdline {
static void
makeSysIncludeList(const Rose_STL_Container<string> &dirs,
                   Rose_STL_Container<string> &result);
} // namespace SageSupport::Cmdline
} // namespace SageSupport
#endif // ROSE_SAGESUPPORT_CMDLINE_H

