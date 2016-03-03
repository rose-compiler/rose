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

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#if !defined(INC_ROSEHPCT_HH)
//! rosehpct.hh included.
#define INC_ROSEHPCT_HH

#include <iostream>
#include <string>
#include <vector>
#include "util/general.hh"
#include "sage/sage.hh"
#include "xml2profir/xml2profir.hh"
#include "profir/profir.hh"
#include "profir2sage/profir2sage.hh"
#include "gprof/gprof_info.hh"

class SgProject;

//! Namespace for \ref ROSEHPCT.
namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_HL
  //@{
  //! A set of predefined metric names, 
  //  please use them instead of hard coded values in the code
  //  All other PAPI event names are standardized. 
  const std::string m_wallclock="WALLCLOCK (us).[0,0] (E)";  // DXN: Laksono changed from [0] to [0,0]!
  const std::string m_percentage="WALLCLOCK_EXCLUSIVE_%";  // DXN
  const std::string m_totalcycle="PAPI_TOT_CYC";

  //! A quick reference to all file and non-stmt nodes of the original Profile IR trees
  // This gives a global view of where to find hot portions
  ROSE_DLL_API extern std::set<const RoseHPCT::IRNode *> profFileNodes_;
  ROSE_DLL_API extern std::set<const RoseHPCT::IRNode *> profStmtNodes_;
  // a map of all profile tree's nodes to matched ROSE AST tree of the current SgProject
  // Ideally, the profile trees of different metric set should be merged into one single tree
  ROSE_DLL_API extern std::map<const RoseHPCT::IRNode *, std::set<SgLocatedNode *> > profSageMap_;

  //! flags to accept command line option
  ROSE_DLL_API extern bool enable_debug; // output debug information
  ROSE_DLL_API extern bool gprof_only; // if users are using gprof result as an alternative
  ROSE_DLL_API extern std::string gprof_file_name; // We expect only on gprof input file

  //! Loads HPCToolkit XML or GNU gprof text profiling data given on the command-line.
  ROSE_DLL_API ProgramTreeList_t loadProfilingFiles(std::vector<std::string>& argvList);

  //! Attach HPCToolkit metrics to the tree.
  ROSE_DLL_API void attachMetrics (const ProgramTreeList_t& profiles,
                      SgProject* proj,
                      bool verbose = false);

  //! Same as attachMetrics but _without_ parent scope propagation.
  ROSE_DLL_API void attachMetricsRaw (const ProgramTreeList_t& profiles,
                         SgProject* proj,
                         bool verbose = false);

  //! Get HPCToolkit profile data options from the command-line.
  ROSE_DLL_API FilenameList_t getProfileOptions (std::vector<std::string>& argvList);

  //! Extract equivalent paths from a set of command-line options.
  ROSE_DLL_API EquivPathMap_t getEquivPaths (std::vector<std::string>& argvList);
  //@}
}

#endif

// eof
