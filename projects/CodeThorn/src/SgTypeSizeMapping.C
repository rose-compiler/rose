#include "sage3basic.h"
#include "SgTypeSizeMapping.h"
#include "CodeThornLib.h"
#include <sstream>
#include <iostream>
#include "ExprAnalyzer.h"
#include "AbstractValue.h"

using namespace std;
using namespace Sawyer::Message;

namespace CodeThorn {
  
  CodeThorn::TypeSize SgTypeSizeMapping::determineTypeSize(SgType* sgType) {
    ROSE_ASSERT(_mapping.size()!=0);
    ROSE_ASSERT(sgType);
    CodeThorn::logger[TRACE]<<"DEBUG: determineTypeSize: "<<sgType->unparseToString()<<endl;
    // cache every type's computed size
    if(_typeToSizeMapping.find(sgType)!=_typeToSizeMapping.end()) {
      return _typeToSizeMapping[sgType];
    }
    switch (sgType->variantT()) {

    case V_SgPointerType:
      //case V_SgTypeNullPtr:
      return getTypeSize(BITYPE_POINTER);

    case V_SgTypeBool:
      return getTypeSize(BITYPE_BOOL);

    case V_SgTypeChar:
    case V_SgTypeSignedChar:
      return getTypeSize(BITYPE_SCHAR);
    case V_SgTypeUnsignedChar: 
      return getTypeSize(BITYPE_UCHAR);

    case V_SgTypeShort:
    case V_SgTypeSignedShort:
      return getTypeSize(BITYPE_SSHORT);
    case V_SgTypeUnsignedShort:
      return getTypeSize(BITYPE_USHORT);

    case V_SgTypeUnsignedInt:
      return getTypeSize(BITYPE_UINT);
    case V_SgTypeInt:
    case V_SgTypeSignedInt:
      return getTypeSize(BITYPE_SINT);

    case V_SgTypeUnsignedLong:
      return getTypeSize(BITYPE_ULONG);
    case V_SgTypeLong:
    case V_SgTypeSignedLong:
      return getTypeSize(BITYPE_SLONG);

    case V_SgTypeUnsignedLongLong:
      return getTypeSize(BITYPE_ULONG_LONG);
    case V_SgTypeLongLong:
    case V_SgTypeSignedLongLong:
      return getTypeSize(BITYPE_SLONG_LONG);

    case V_SgTypeFloat:
      return getTypeSize(BITYPE_FLOAT);
    case V_SgTypeDouble:
      return getTypeSize(BITYPE_DOUBLE);
    case V_SgTypeLongDouble:
      return getTypeSize(BITYPE_LONG_DOUBLE);
    case V_SgReferenceType:
      return getTypeSize(BITYPE_REFERENCE);
    case V_SgArrayType: {
      CodeThorn::logger[INFO]<<"DEBUG: ARRAYTYPE: "<<sgType->unparseToString()<<endl;
      SgArrayType* arrayType=isSgArrayType(sgType);
      CodeThorn::logger[TRACE]<<"DEBUG: ARRAYTYPE: p1"<<endl;
      CodeThorn::TypeSize elementTypeSize=determineElementTypeSize(arrayType);
      CodeThorn::logger[TRACE]<<"DEBUG: ARRAYTYPE: p2"<<endl;
      CodeThorn::TypeSize numberOfElements=determineNumberOfElements(arrayType);
      CodeThorn::logger[TRACE]<<"DEBUG: ARRAYTYPE: p3"<<endl;
      unsigned int totalSize=numberOfElements*elementTypeSize;
      if(elementTypeSize>0) {
        _typeToSizeMapping[sgType]=totalSize; // cache result
      }
      return totalSize;
    }
    case V_SgClassType: {
      CodeThorn::logger[INFO]<<"DEBUG: CLASSTYPE: "<<sgType->unparseToString()<<endl;
      typedef std::vector< std::pair< SgNode*, std::string > > DataMemberPointers;
      CodeThorn::TypeSize sum=0;
      DataMemberPointers dataMemPtrs=isSgClassType(sgType)->returnDataMemberPointers();
      // returnDataMemberPointers includes all declarations (methods need to be filtered)
      for(DataMemberPointers::iterator i=dataMemPtrs.begin();i!=dataMemPtrs.end();++i) {
        SgNode* node=(*i).first; 
        if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
          sum+=determineTypeSize(varDecl->get_type());
        }
      }
      if(sum>0) {
        _typeToSizeMapping[sgType]=sum;
      }
      return sum;
    }
    case V_SgFunctionType:
      return getTypeSize(BITYPE_POINTER);
      //case V_SgTypeComplex:

    default:
      //SAWYER_MESG(CodeThorn::logger[ERROR])<<"Unknown type:  "<<sgType->unparseToString()<<endl;
      return 0;
    }
    return 0;
  }

  CodeThorn::TypeSize SgTypeSizeMapping::determineElementTypeSize(SgArrayType* sgType) {
    //SgType* elementType=sgType->get_base_type();
    SgType* elementType=SageInterface::getArrayElementType(sgType);
    return determineTypeSize(elementType);
  }

  //! Calculate the number of elements of an array type
  unsigned int SgTypeSizeMapping::determineNumberOfElements(SgArrayType* t) {
    //return SageInterface::getArrayElementCount(sgType);
    ROSE_ASSERT(t);
    size_t result=0; 
    SgExpression * indexExp =  t->get_index();
    CodeThorn::logger[TRACE]<<"determineNumberOfElements:p1"<<endl;
    
    // assume dimension default to 1 if not specified ,such as a[] 
    if((indexExp == nullptr) || isSgNullExpression(indexExp)) {
      CodeThorn::logger[TRACE]<<"determineNumberOfElements:p2"<<endl;
      result = 0;
    } else { 
      if(AbstractValue::getVariableIdMapping()==nullptr) {
        CodeThorn::logger[TRACE]<<"AbstractValue::getVariableIdMapping()==nullptr"<<endl;
        //Take advantage of the fact that the value expression is always SgUnsignedLongVal in AST
        SgUnsignedLongVal * valExp = isSgUnsignedLongVal(indexExp);
        SgIntVal * valExpInt = isSgIntVal(indexExp);
        if(!(valExp || valExpInt)) {
          //SAWYER_MESG(CodeThorn::logger[ERROR])<<"Unexpected value: determineNumberOfElements: "<<indexExp->class_name()<<endl;
          //exit(1);
          return 0;
        } else {
          if (valExp)
            result = valExp->get_value(); 
          else 
            result = valExpInt->get_value();
        }
        CodeThorn::logger[TRACE]<<"determined result value."<<endl;
      } else {
        CodeThorn::logger[TRACE]<<"determineNumberOfElements:p3"<<endl;
        // variable id mapping is available in AbstractValue
        ExprAnalyzer tmpExprEvaluator;
        AbstractValue abstractSize=tmpExprEvaluator.evaluateExpressionWithEmptyState(indexExp);
        if(abstractSize.isConstInt()) {
          result=abstractSize.getIntValue();
        } else {
          // TODO: make the result of this entire function an abstract value
          result=0;
        }
      }
    }
    CodeThorn::logger[TRACE]<<"determineNumberOfElements:p4"<<endl;
    // consider multi dimensional case 
    SgArrayType* arraybase = isSgArrayType(t->get_base_type());
    if (arraybase)
      result = result * determineNumberOfElements(arraybase);
    CodeThorn::logger[TRACE]<<"determineNumberOfElements:p5"<<endl;
    return result;
  }
  
  CodeThorn::TypeSize SgTypeSizeMapping::determineTypeSizePointedTo(SgPointerType* sgType) {
    SgType* typePointedTo=sgType->get_base_type();
    return determineTypeSize(typePointedTo);
  }

  std::string SgTypeSizeMapping::toString() {
    std::ostringstream ss;
    for(auto entry : _typeToSizeMapping ) {
      ss<<entry.first->unparseToString()<<":"<<entry.second<<std::endl;
    }
    return ss.str();
  }

} // end of namespace CodeThorn
