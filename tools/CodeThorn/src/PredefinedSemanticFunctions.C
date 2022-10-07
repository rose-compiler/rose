#include "sage3basic.h"
#include "EStateTransferFunctions.h"
#include "CodeThornException.h"
#include "CTAnalysis.h" // dependency on process-functions
#include "CppStdUtilities.h"
#include "CodeThornCommandLineOptions.h"
#include "CodeThornLib.h"
#include "PredefinedSemanticFunctions.h"

using namespace CodeThorn;
using namespace CodeThorn;
using namespace Sawyer::Message;

namespace PredefinedSemanticFunctions {
  /* TODO: in case an error is detected the target region remains unmodified. Change to invalidating all elements of target region */
  SingleEvalResult evalFunctionCallMemCpy(EStateTransferFunctions* exprAnalyzer, SgFunctionCallExp* funCall, EStatePtr estate) {
    //cout<<"DETECTED: memcpy: "<<funCall->unparseToString()<<endl;
    SingleEvalResult res;
    // memcpy is a void function, no return value
    res.init(estate,AbstractValue(CodeThorn::Top()));
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==3) {
      AbstractValue memcpyArgs[3];
      int i=0;
      for(SgExpressionPtrList::iterator argIter=argsList.begin();argIter!=argsList.end();++argIter) {
        SgExpression* arg=*argIter;
        SingleEvalResult sres=exprAnalyzer->evaluateExpression(arg,estate);
        AbstractValue argVal=sres.result;
        memcpyArgs[i++]=argVal;
      }
      if(memcpyArgs[0].isTop()||memcpyArgs[1].isTop()||memcpyArgs[2].isTop()) {
        exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate->label());
        return res; // returns top
      }
      int memRegionSizeTarget=exprAnalyzer->getMemoryRegionNumElements(memcpyArgs[0]);
      int copyRegionElementSizeTarget=exprAnalyzer->getMemoryRegionElementSize(memcpyArgs[0]);
      int memRegionSizeSource=exprAnalyzer->getMemoryRegionNumElements(memcpyArgs[1]);
      int copyRegionElementSizeSource=exprAnalyzer->getMemoryRegionElementSize(memcpyArgs[1]);

      int copyRegionElementSize=0; // TODO: use AbstractValue for all sizes
      // check if size to copy is either top
      if(memcpyArgs[2].isTop()) {
        exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate->label());
        return res;
      } else if(memRegionSizeTarget!=memRegionSizeSource) {
        // check if the element size of the two regions is different (=> conservative analysis result; will be modelled in future)
        exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate->label());
        return res;
      } else {
        if(copyRegionElementSizeTarget!=copyRegionElementSizeSource) {
          if(copyRegionElementSizeTarget!=0)
            copyRegionElementSize=copyRegionElementSizeTarget;
          else if(copyRegionElementSizeSource!=0)
            copyRegionElementSize=copyRegionElementSizeSource;
          else
            copyRegionElementSize=std::max(copyRegionElementSizeSource,copyRegionElementSizeTarget);
          ROSE_ASSERT(copyRegionElementSize!=0);
        } else {
          copyRegionElementSize=copyRegionElementSizeTarget;
        }
      }

      bool errorDetected=false;
      int copyRegionLengthValue=memcpyArgs[2].getIntValue();

      // the copy function length argument is converted here into number of elements. This needs to be adapted if the repsentation of size is changed.
      if(copyRegionElementSize==0) {
        cout<<"WARNING: memcpy: copy region element size is 0. Recording potential out of bounds access."<<endl;
        exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate->label());
        return res;
      }
      int copyRegionNumElements=copyRegionLengthValue/copyRegionElementSize;

      // clamp values since 0 is considered already an error
      if(memRegionSizeSource==-1) memRegionSizeSource=0;
      if(memRegionSizeTarget==-1) memRegionSizeTarget=0;

      if(memRegionSizeSource<copyRegionNumElements) {
        if(memRegionSizeSource==0) {
          errorDetected=true;
          exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate->label());
        } else {
          errorDetected=true;
          exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate->label());
        }
      }
      if(memRegionSizeTarget<copyRegionNumElements) {
        if(memRegionSizeTarget==0) {
          errorDetected=true;
          exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate->label());
        } else {
          errorDetected=true;
          exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate->label());
        }
      }
      if(!errorDetected) {
        // no error occured. Copy region.
        PStatePtr newPState=estate->pstate();
        AbstractValue targetPtr=memcpyArgs[0];
        AbstractValue sourcePtr=memcpyArgs[1];
        AbstractValue one=CodeThorn::AbstractValue(1);
        for(int i=0;i<copyRegionNumElements;i++) {
          exprAnalyzer->writeToMemoryLocation(estate->label(),newPState,targetPtr,exprAnalyzer->readFromMemoryLocation(estate->label(),newPState,sourcePtr));
          targetPtr=AbstractValue::operatorAdd(targetPtr,one); // targetPtr++;
          sourcePtr=AbstractValue::operatorAdd(sourcePtr,one); // sourcePtr++;
        }
      }
      return res;
    } else {
      cerr<<"Error: unknown memcpy function (number of arguments != 3)"<<funCall->unparseToString()<<endl;
      exit(1);
    }
    return res;
  }

  SingleEvalResult evalFunctionCallStrLen(EStateTransferFunctions* exprAnalyzer, SgFunctionCallExp* funCall, EStatePtr estate) {
    SingleEvalResult res;
    res.init(estate,AbstractValue(CodeThorn::Top()));
    //cout<<"DEBUG:evalFunctionCallStrLen:"<<funCall->unparseToString()<<endl;
    res.init(estate,AbstractValue(CodeThorn::Top()));
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==1) {
      AbstractValue functionArgs[1];
      int i=0;
      for(SgExpressionPtrList::iterator argIter=argsList.begin();argIter!=argsList.end();++argIter) {
        SgExpression* arg=*argIter;
	//cout<<"DEBUG: evalFunctionCallStrLen: arg: "<<arg->unparseToString()<<endl;
        SingleEvalResult sres=exprAnalyzer->evaluateExpression(arg,estate);
        AbstractValue argVal=sres.result;
        functionArgs[i++]=argVal;
      }
      //cout<<"DEBUG: evalFunctionCallStrLen: processing args done."<<endl;
      // the argument (string pointer) is now in functionArgs[0];
      // compute length now
      // read value and proceed on pointer until 0 is found. Also check size of memory region.
      AbstractValue stringPtr=functionArgs[0];
      int pos=0;
      while(1) {
        AbstractValue abstractPos=AbstractValue(pos);
        AbstractValue currentPos=AbstractValue::operatorAdd(stringPtr,abstractPos);
        //cout<<"DEBUG: evalFunctionCallStrLen: currentPos:"<<currentPos.toString()<<endl;
        if(currentPos.isTop()) {
          exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate->label());
          break;
        }

        // check bounds of string's memory region
        if(stringPtr.isPtr()) {
          CodeThorn::TypeSize numElements=exprAnalyzer->getVariableIdMapping()->getNumberOfElements(stringPtr.getVariableId());
          if(VariableIdMapping::isUnknownSizeValue(numElements)) {
            // unknown size of string memmory, return top
            return res;
          }
          AbstractValue numElementsAV(numElements);
          AbstractValue cmpResult=AbstractValue::operatorMoreOrEq(abstractPos,numElementsAV);
          if(!cmpResult.isTop()&&cmpResult.isTrue()) {
            exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate->label());
            break;
          }
        } else {
          // abstraction is too coarse to reason on pointer values, return top
          return res;
        }

        AbstractValue currentPosValue=exprAnalyzer->readFromMemoryLocation(estate->label(),estate->pstate(),currentPos);
        //cout<<"DEBUG: currentPosValue:"<<currentPosValue.toString()<<endl;
        // if the memory location that is read, does not exist, it is an out-of-bounds access
        if(currentPosValue.isBot()) {
          //cout<<estate.pstate()->toString()<<endl;
          exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate->label());
          break;
        }
        AbstractValue cmpResult=(currentPosValue==AbstractValue(0));
        if(cmpResult.isTrue()) {
          // found 0
          AbstractValue finalResult=AbstractValue(pos);
          res.init(estate,finalResult);
          return res;
        } else if(cmpResult.isFalse()) {
          pos++;
        } else {
          // top or bot
          break;
        }
      }
    }
    // fallthrough for top/bot
    // return top for unknown (or out-of-bounds access)
    res.init(estate,AbstractValue(CodeThorn::Top()));
    //cout<<"DEBUG:evalFunctionCallStrLen:"<<funCall->unparseToString()<<" finished."<<endl;
    return res;
  }

} // end of namespace PredefinedSemanticFunctions

