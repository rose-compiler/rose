/*
 * interleaveAcrossArraysCheck.h
 *
 *  Created on: Jul 12, 2011
 *      Author: sharma9
 */

#ifndef INTERLEAVEACROSSARRAYSCHECK_H_
#define INTERLEAVEACROSSARRAYSCHECK_H_

#include <set>
#include <string>
#include "rose.h"
#include "transformationWorklist.h"

using namespace std;

typedef bool SynthesizedAttribute;

class InterleaveAcrossArraysCheckSynthesizedAttributeType
{
 public:
		bool isFunctionRefExp;
		bool isArrayRef;
		InterleaveAcrossArraysCheckSynthesizedAttributeType()
		{
			isFunctionRefExp = false;
			isArrayRef = false;
		}
};

class ArrayRef
{
	public:
		ArrayRef(string _name, string _type, string _dim)
		{
			name = _name;
			type = _type;
			dimensions = _dim;
		}
		string name;
		string type;
		string dimensions;
};

class interleaveAcrossArraysCheck : public AstBottomUpProcessing<InterleaveAcrossArraysCheckSynthesizedAttributeType>
   {
     public:
		  //interleaveAcrossArraysCheck();
		  interleaveAcrossArraysCheck(Transformation* t);
		  virtual InterleaveAcrossArraysCheckSynthesizedAttributeType evaluateSynthesizedAttribute (
		  				   SgNode* n, SynthesizedAttributesList childAttributes );
          virtual void atTraversalStart();
          virtual void atTraversalEnd();
          void storeArrayReference(SgInitializedName* var, string variableName, string type);

     private:
          bool isContigousDecl;
          bool isArrayRefInFunctionCall;
          vector<ArrayRef> arrayRefList;
          Transformation* transformation;
          string outputName;
   };



#endif /* INTERLEAVEACROSSARRAYSCHECK_H_ */
