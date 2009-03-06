// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
\defgroup ROSEHPCT_HL  ROSE-HPCT End-user Interface
\ingroup ROSEHPCT

The \ref ROSEHPCT_HL defines an interface to the \ref ROSEHPCT for use
by "end-users."

For a simple example, see \ref examples/test_highlevel.cc .

$Id: rosehpct.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
*/

/*!
 *  \file sage/rosehpct.hh
 *  \brief Implements a end-user interface to \ref ROSEHPCT.
 *  \ingroup ROSEHPCT_HL
 *
 *  $Id: rosehpct.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_ROSEHPCT_HH)
//! rosehpct.hh included.
#define INC_ROSEHPCT_HH

#include <iostream>
#include <string>
#include <vector>

#include "xml2profir/xml2profir.hh"
#include "profir2sage/profir2sage.hh"

class SgProject;

//! Namespace for \ref ROSEHPCT.
namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_HL
  //@{

  extern bool enable_debug; // output debug information

  //! Loads HPCToolkit XML profiling data given on the command-line.
  ProgramTreeList_t loadHPCTProfiles (std::vector<std::string>& argvList);

  //! Attach HPCToolkit metrics to the tree.
  void attachMetrics (const ProgramTreeList_t& profiles,
                      SgProject* proj,
                      bool verbose = false);

  //! Same as attachMetrics but _without_ parent scope propagation.
  void attachMetricsRaw (const ProgramTreeList_t& profiles,
                         SgProject* proj,
                         bool verbose = false);

  //! Get HPCToolkit profile data options from the command-line.
  FilenameList_t getProfileOptions (std::vector<std::string>& argvList);

  //! Extract equivalent paths from a set of command-line options.
  EquivPathMap_t getEquivPaths (std::vector<std::string>& argvList);
  //@}
}

#endif

// eof
