// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
using namespace std;
using namespace CodeThorn;

#include "CollectionOperators.h"

#include "RDTransferFunctions.hpp"

RDAnalyzer::RDAnalyzer() {
  _transferFunctions=new RDTransferFunctions();
}

RDAnalyzer::~RDAnalyzer() {
  delete _transferFunctions;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void RDAnalyzer::attachInInfoToAst(string attributeName) {
  if(!_preInfoIsValid)
    computeAllPreInfo();
  attachInfoToAst(attributeName,true);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void RDAnalyzer::attachOutInfoToAst(string attributeName) {
  attachInfoToAst(attributeName,false);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void RDAnalyzer::attachInfoToAst(string attributeName,bool inInfo) {
  if(inInfo && !_preInfoIsValid)
    computeAllPreInfo();
  LabelSet labelSet=_flow.nodeLabels();
  for(LabelSet::iterator i=labelSet.begin();
      i!=labelSet.end();
      ++i) {
    ROSE_ASSERT(*i<_analyzerData.size());
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. function call)
    if(!_labeler->isFunctionExitLabel(*i) /* && !_labeler->isCallReturnLabel(lab)*/)
      if(*i >=0 ) {
        if(inInfo)
          _labeler->getNode(*i)->setAttribute(attributeName,new RDAstAttribute(&_analyzerDataPreInfo[*i]));
        else
          _labeler->getNode(*i)->setAttribute(attributeName,new RDAstAttribute(&_analyzerData[*i]));
      }
  }
}

RDAnalyzer::iterator RDAnalyzer::begin() {
  return _analyzerData.begin();
}
  
RDAnalyzer::iterator RDAnalyzer::end() {
  return _analyzerData.end();
}
size_t RDAnalyzer::size() {
  return _analyzerData.size();
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */

RDLattice RDAnalyzer::transfer(Label lab, RDLattice element) {
  ROSE_ASSERT(_transferFunctions);
  return _transferFunctions->transfer(lab,element);
}

