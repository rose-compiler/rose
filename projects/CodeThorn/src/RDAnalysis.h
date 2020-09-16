#ifndef RDANALYSIS_H
#define RDANALYSIS_H

// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "RDLattice.h"
#include "DFAnalysisBase.h"
#include "AstUtility.h"
#include "DFAstAttribute.h"
#include "RDTransferFunctions.h"

namespace CodeThorn {

class RDPropertyStateFactory : public PropertyStateFactory {
 public:
  RDPropertyStateFactory();
  virtual RDLattice* create();
  virtual ~RDPropertyStateFactory();
};

/*!
  * \author Markus Schordan
  * \date 2013.
 */
class RDAnalysis : public DFAnalysisBase {
 public:
  RDAnalysis();
  ~RDAnalysis();
  CodeThorn::LabelSet getPreRDs(Label lab, VariableId);
  CodeThorn::LabelSet getPostRDs(Label lab, VariableId);
  virtual void initializeExtremalValue(Lattice* element);

  void initialize(SgProject* root);

 protected:
  virtual DFAstAttribute* createDFAstAttribute(Lattice* elem);
};

} // end of namespace CodeThorn

#endif
