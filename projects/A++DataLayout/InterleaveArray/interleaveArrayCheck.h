/*
 * interleaveAcrossArraysCheck.h
 *
 *  Created on: Jul 12, 2011
 *      Author: sharma9
 */

#ifndef INTERLEAVEARRAYCHECK_H_
#define INTERLEAVEARRAYCHECK_H_

#include <set>
#include <string>
#include "rose.h"
#include "../transformationWorklist.h"
#include "../utility.h"

using namespace std;


class InterleaveArrayCheckSynthesizedAttributeType
{
 public:
		bool isFunctionRefExp;
		bool isArrayRef;
		InterleaveArrayCheckSynthesizedAttributeType()
		{
			isFunctionRefExp = false;
			isArrayRef = false;
		}
};


class interleaveArrayCheck : public AstBottomUpProcessing<InterleaveArrayCheckSynthesizedAttributeType>
   {
     public:
		  interleaveArrayCheck(Transformation* t);
		  virtual InterleaveArrayCheckSynthesizedAttributeType evaluateSynthesizedAttribute (
		  				   SgNode* n, SynthesizedAttributesList childAttributes );
          virtual void atTraversalStart();
          virtual void atTraversalEnd();
          void storeArrayReference(SgInitializedName* var, string variableName, string type);

     private:
          bool isArrayRefInFunctionCall;
          vector<ArrayRef> arrayRefList;
          Transformation* transformation;
          string outputName;
   };



#endif /* INTERLEAVEARRAYCHECK_H_ */
