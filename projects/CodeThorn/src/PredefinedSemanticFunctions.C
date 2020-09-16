#include "sage3basic.h"
#include "ExprAnalyzer.h"
#include "CodeThornException.h"
#include "Analyzer.h" // dependency on process-functions
#include "CppStdUtilities.h"
#include "CodeThornCommandLineOptions.h"
#include "CodeThornLib.h"
#include "PredefinedSemanticFunctions.h"

using namespace CodeThorn;
using namespace CodeThorn;
using namespace Sawyer::Message;

namespace PredefinedSemanticFunctions {
/* TODO: in case an error is detected the target region remains unmodified. Change to invalidating all elements of target region */
  list<SingleEvalResultConstInt> evalFunctionCallMemCpy(ExprAnalyzer* exprAnalyzer, SgFunctionCallExp* funCall, EState estate) {
  //cout<<"DETECTED: memcpy: "<<funCall->unparseToString()<<endl;
  SingleEvalResultConstInt res;
  // memcpy is a void function, no return value
  res.init(estate,AbstractValue(CodeThorn::Top()));
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  if(argsList.size()==3) {
    AbstractValue memcpyArgs[3];
    int i=0;
    for(SgExpressionPtrList::iterator argIter=argsList.begin();argIter!=argsList.end();++argIter) {
      SgExpression* arg=*argIter;
      list<SingleEvalResultConstInt> resList=exprAnalyzer->evaluateExpression(arg,estate);
      if(resList.size()!=1) {
        cerr<<"Error: conditional control-flow in function argument expression. Expression normalization required."<<endl;
        exit(1);
      }
      SingleEvalResultConstInt sres=*resList.begin();
      AbstractValue argVal=sres.result;
      memcpyArgs[i++]=argVal;
    }
    if(memcpyArgs[0].isTop()||memcpyArgs[1].isTop()||memcpyArgs[2].isTop()) {
      exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate.label());
      return ExprAnalyzer::listify(res); // returns top
    }
    int memRegionSizeTarget=exprAnalyzer->getMemoryRegionNumElements(memcpyArgs[0]);
    int copyRegionElementSizeTarget=exprAnalyzer->getMemoryRegionElementSize(memcpyArgs[0]);
    int memRegionSizeSource=exprAnalyzer->getMemoryRegionNumElements(memcpyArgs[1]);
    int copyRegionElementSizeSource=exprAnalyzer->getMemoryRegionElementSize(memcpyArgs[1]);

    int copyRegionElementSize=0; // TODO: use AbstractValue for all sizes
    // check if size to copy is either top
    if(memcpyArgs[2].isTop()) {
      exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate.label());
      return ExprAnalyzer::listify(res);
    } else if(memRegionSizeTarget!=memRegionSizeSource) {
      // check if the element size of the two regions is different (=> conservative analysis result; will be modelled in future)
      exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate.label());
      return ExprAnalyzer::listify(res);
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
      exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate.label());
      return ExprAnalyzer::listify(res);
    }
    int copyRegionNumElements=copyRegionLengthValue/copyRegionElementSize;

    // clamp values since 0 is considered already an error
    if(memRegionSizeSource==-1) memRegionSizeSource=0;
    if(memRegionSizeTarget==-1) memRegionSizeTarget=0;

    if(memRegionSizeSource<copyRegionNumElements) {
      if(memRegionSizeSource==0) {
        errorDetected=true;
        exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate.label());
      } else {
        errorDetected=true;
        exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate.label());
      }
    }
    if(memRegionSizeTarget<copyRegionNumElements) {
      if(memRegionSizeTarget==0) {
        errorDetected=true;
        exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate.label());
      } else {
        errorDetected=true;
        exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate.label());
      }
    }
    if(!errorDetected) {
      // no error occured. Copy region.
      PState newPState=*estate.pstate();
      AbstractValue targetPtr=memcpyArgs[0];
      AbstractValue sourcePtr=memcpyArgs[1];
      AbstractValue one=CodeThorn::AbstractValue(1);
      for(int i=0;i<copyRegionNumElements;i++) {
        exprAnalyzer->writeToMemoryLocation(estate.label(),&newPState,targetPtr,exprAnalyzer->readFromMemoryLocation(estate.label(),&newPState,sourcePtr));
        targetPtr=AbstractValue::operatorAdd(targetPtr,one); // targetPtr++;
        sourcePtr=AbstractValue::operatorAdd(sourcePtr,one); // sourcePtr++;
      }
    }
    return ExprAnalyzer::listify(res);
  } else {
    cerr<<"Error: unknown memcpy function (number of arguments != 3)"<<funCall->unparseToString()<<endl;
    exit(1);
  }
  return ExprAnalyzer::listify(res);
  }

  list<SingleEvalResultConstInt> evalFunctionCallStrLen(ExprAnalyzer* exprAnalyzer, SgFunctionCallExp* funCall, EState estate) {
    SingleEvalResultConstInt res;
    //cout<<"DEBUG:evalFunctionCallStrLen:"<<funCall->unparseToString()<<endl;
    res.init(estate,AbstractValue(CodeThorn::Top()));
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==1) {
      AbstractValue functionArgs[1];
      int i=0;
      for(SgExpressionPtrList::iterator argIter=argsList.begin();argIter!=argsList.end();++argIter) {
        SgExpression* arg=*argIter;
        list<SingleEvalResultConstInt> resList=exprAnalyzer->evaluateExpression(arg,estate);
        if(resList.size()!=1) {
          cerr<<"Error: conditional control-flow in function argument expression. Expression normalization required."<<endl;
          exit(1);
        }
        SingleEvalResultConstInt sres=*resList.begin();
        AbstractValue argVal=sres.result;
        functionArgs[i++]=argVal;
      }
      // the argument (string pointer) is now in functionArgs[0];
      // compute length now
      // read value and proceed on pointer until 0 is found. Also check size of memory region.
      AbstractValue stringPtr=functionArgs[0];
      int pos=0;
      while(1) {
        AbstractValue AbstractPos=AbstractValue(pos);
        AbstractValue currentPos=(stringPtr+AbstractPos);
        //cout<<"DEBUG: currentPos:"<<currentPos.toString()<<endl;
        if(currentPos.isTop()) {
          exprAnalyzer->recordPotentialOutOfBoundsAccessLocation(estate.label());
          break;
        }
#if 0
        // TODO: not working yet because the memory region of strings are not properly registered with size yet
        // check bounds of string's memory region
        if(!accessIsWithinArrayBounds(stringPtr.getVariableId(),pos)) {
          exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate.label());
          break;
        }
#endif
        AbstractValue currentPosValue=exprAnalyzer->readFromMemoryLocation(estate.label(),estate.pstate(),currentPos);
        //cout<<"DEBUG: currentPosValue:"<<currentPosValue.toString()<<endl;
        // if the memory location that is read, does not exist, it is an out-of-bounds access
        if(currentPosValue.isBot()) {
          //cout<<estate.pstate()->toString()<<endl;
          exprAnalyzer->recordDefinitiveOutOfBoundsAccessLocation(estate.label());
          break;
        }
        AbstractValue cmpResult=(currentPosValue==AbstractValue(0));
        if(cmpResult.isTrue()) {
          // found 0
          AbstractValue finalResult=AbstractValue(pos);
          res.init(estate,finalResult);
          return ExprAnalyzer::listify(res);
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
    return ExprAnalyzer::listify(res);
  }

} // end of namespace PredefinedSemanticFunctions

