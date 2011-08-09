/*
 * interleaveAcrossArraysTransformation.h
 *
 *  Created on: Jul 12, 2011
 *      Author: sharma9
 */

#ifndef INTERLEAVEACROSSARRAYSTRANSFORMATION_H_
#define INTERLEAVEACROSSARRAYSTRANSFORMATION_H_

#include <set>
#include <string>
#include "rose.h"
#include "transformationWorklist.h"

typedef bool SynthesizedAttribute;

using namespace std;

class interleaveAcrossArraysTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
{
 public:
	interleaveAcrossArraysTraversal(Transformation* t);
   // virtual function must be defined
	  virtual SynthesizedAttribute evaluateSynthesizedAttribute (
				   SgNode* n, SynthesizedAttributesList childAttributes );
	  virtual void atTraversalStart();
	  virtual void atTraversalEnd();

 private:
	  void processArrayDeclarations();
	  //void processArrayRefExp();
	  void processPntrArrRefExp();
	  void storePntrArrRefExp(SgPntrArrRefExp* node);

	  Transformation* transformation;
	  string outputName;
	  vector<SgVariableDeclaration*> varDeclList;
	  vector<SgPntrArrRefExp*> pntrArrRefExpList;
	  SgVariableDeclaration* replaceVarDecl;
};


#endif /* INTERLEAVEACROSSARRAYSTRANSFORMATION_H_ */
