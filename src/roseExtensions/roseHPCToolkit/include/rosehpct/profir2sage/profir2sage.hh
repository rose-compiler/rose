// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \defgroup ROSEHPCT_PROFIR2SAGE  ROSE-HPCT ProfIR-to-Sage conversion
 *  \ingroup ROSEHPCT
 *
 *  This submodule implements utility routines that process a profile
 *  IR data structure, and attach metrics from this data structure to
 *  their corresponding nodes in the Sage III IR. The main driver is
 *  \ref RoseHPCT::attachMetrics.
 *
 *  $Id: profir2sage.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*!
 *  \file rosehpct/profir2sage/profir2sage.hh
 *  \brief Implements RoseHPCTIR-to-SageIII conversion routine.
 *  \ingroup ROSEHPCT_PROFIR2SAGE
 *
 *  $Id: profir2sage.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_PROFIR2SAGE_PROFIR2SAGE_HH)
//! rosehpct/profir2sage/profir2sage.hh included.
#define INC_PROFIR2SAGE_PROFIR2SAGE_HH

#include <iostream>
#include <string>
#include <vector>

#include <rosehpct/util/general.hh>
#include <rosehpct/profir/profir.hh>
#include <rosehpct/sage/sage.hh>

namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_PROFIR2SAGE
  //@{

  //! Attach metrics from a ProfIR tree to a Sage IR tree.
  void attachMetrics (const IRTree_t* hpc_root,
                      SgProject* sage_root,
                      bool verbose = false);
  //! A quick reference to all file nodes of Profile IRs 
  // obtained from multiple profile xml files
  extern std::set<const RoseHPCT::IRNode *> profFileNodes_;
  extern std::set<const RoseHPCT::IRNode *> profStmtNodes_;

  //@}
};

#endif

/* eof */
