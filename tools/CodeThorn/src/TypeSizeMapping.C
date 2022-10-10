#include "sage3basic.h"
#include <sstream>
#include <iostream>
#include "TypeSizeMapping.h"
#include "CodeThornException.h"
#include "CodeThornLib.h"
#include "EStateTransferFunctions.h"
#include "AbstractValue.h"
#include "VariableIdMappingExtended.h"

using namespace std;
using namespace Sawyer::Message;

namespace CodeThorn {
  
  TypeSizeMapping::TypeSizeMapping() {
  }

  TypeSizeMapping::~TypeSizeMapping() {
  }

  void TypeSizeMapping::setBuiltInTypeSizes(std::vector<CodeThorn::TypeSize> mapping) {
    if(_mapping.size()==0 || mapping.size()==_mapping.size()) {
      _mapping=mapping;
    } else {
      throw CodeThorn::Exception("TypeSizeMapping: wrong number of type-size mapping values.");
    }
  }
  
  void TypeSizeMapping::setTypeSize(BuiltInType bitype, CodeThorn::TypeSize size) {
    ROSE_ASSERT(bitype<_mapping.size());
    _mapping[bitype]=size;
  }
  
  CodeThorn::TypeSize TypeSizeMapping::getBuiltInTypeSize(BuiltInType bitype) {
    ROSE_ASSERT(bitype<_mapping.size());
    return _mapping[bitype];
  }

  std::size_t TypeSizeMapping::sizeOfOp(BuiltInType bitype) {
    ROSE_ASSERT(bitype<_mapping.size());
    return getBuiltInTypeSize(bitype);
  }
  
  bool TypeSizeMapping::isCpp11StandardCompliant() {
    size_t byteSize=1;
    return byteSize==sizeOfOp(BITYPE_CHAR)
      && sizeOfOp(BITYPE_CHAR)<=sizeOfOp(BITYPE_SHORT)
      && sizeOfOp(BITYPE_SHORT)<=sizeOfOp(BITYPE_INT)
      && sizeOfOp(BITYPE_INT)<=sizeOfOp(BITYPE_LONG)
      && sizeOfOp(BITYPE_LONG)<=sizeOfOp(BITYPE_LONG_LONG);
  }

  bool TypeSizeMapping::isUndefinedTypeSize(CodeThorn::TypeSize size) {
    return size==-1;
  }

  BuiltInType TypeSizeMapping::determineBuiltInTypeId(SgType* sgType) {
    //cout<<"DEBUG: TypeSizeMapping::determineBuiltInTypeId: "<<sgType->class_name()<<endl;
    switch (sgType->variantT()) {
    case V_SgTypeVoid:
      return BITYPE_VOID;

    case V_SgPointerType:
      return BITYPE_POINTER;

    case V_SgTypeBool:
      return BITYPE_BOOL;

    case V_SgTypeChar:
    case V_SgTypeSignedChar:
    case V_SgTypeUnsignedChar: 
      return BITYPE_CHAR;

    case V_SgTypeShort:
    case V_SgTypeSignedShort:
    case V_SgTypeUnsignedShort:
      return BITYPE_SHORT;

    case V_SgTypeUnsignedInt:
    case V_SgTypeInt:
    case V_SgTypeSignedInt:
      return BITYPE_INT;

    case V_SgTypeUnsignedLong:
    case V_SgTypeLong:
    case V_SgTypeSignedLong:
      return BITYPE_LONG;

    case V_SgTypeUnsignedLongLong:
    case V_SgTypeLongLong:
    case V_SgTypeSignedLongLong:
      return BITYPE_LONG_LONG;

    case V_SgTypeFloat:
      return BITYPE_FLOAT;
      
    case V_SgTypeDouble:
      return BITYPE_DOUBLE;
      
    case V_SgTypeLongDouble:
      return BITYPE_LONG_DOUBLE;
      
    case V_SgReferenceType:
      return BITYPE_REFERENCE;
    case V_SgFunctionType:
      return BITYPE_POINTER;
    case V_SgEnumType:
      return BITYPE_INT; // enum size is of size int in C++
      //case V_SgTypeComplex:
    default:
      return BITYPE_UNKNOWN;
    }
    // unreachable
  }
  
  CodeThorn::TypeSize TypeSizeMapping::determineTypeSize(SgType* sgType) {
    ROSE_ASSERT(_mapping.size()!=0);
    ROSE_ASSERT(sgType);
    //SAWYER_MESG(CodeThorn::logger[TRACE])<<"DEBUG: determineTypeSize: "<<sgType->unparseToString()<<endl;
    // cache every type's computed size
    if(_typeToSizeMapping.find(sgType)!=_typeToSizeMapping.end()) {
      return _typeToSizeMapping[sgType];
    }
    // remove typedef indirection
    sgType=sgType->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
    switch (sgType->variantT()) {

    case V_SgArrayType: {
      //SAWYER_MESG(CodeThorn::logger[TRACE])<<"DEBUG: ARRAYTYPE: "<<sgType->unparseToString()<<endl;
      SgArrayType* arrayType=isSgArrayType(sgType);
      CodeThorn::TypeSize elementTypeSize=determineElementTypeSize(arrayType);
      //if(elementTypeSize>10000) {
      //	cout<<"DEBUG: ARRAY: ELEMENT TYPE SIZE:"<<elementTypeSize<<endl;
      //	return -1;
      //}
      CodeThorn::TypeSize numberOfElements=determineNumberOfElements(arrayType);
      //if(numberOfElements>10000) {
      //	cout<<"DEBUG: ARRAY: NUM ELEMENTS:"<<elementTypeSize<<endl;
      //	return -1;
      //}
      unsigned int totalSize=-1;
      if(elementTypeSize!=-1 && numberOfElements!=-1) {
	totalSize=numberOfElements*elementTypeSize;
	if(elementTypeSize>0) {
	  _typeToSizeMapping[sgType]=totalSize; // cache result
	}
      }
      return totalSize;
    }
      /*
    case V_SgClassType: {
      SAWYER_MESG(CodeThorn::logger[TRACE])<<"CLASSTYPE: "<<sgType->unparseToString()<<endl;
      CodeThorn::TypeSize sum=0;
      std::list<SgVariableDeclaration*> varDeclList=SgNodeHelper::memberVariableDeclarationsList(isSgClassType(sgType));
      for(auto varDecl : varDeclList) {
        sum+=determineTypeSize(varDecl->get_type());
      }
      if(sum>0) {
        _typeToSizeMapping[sgType]=sum;
      }
      return sum;
    }
      */
    default: {
      //unsigned int totalSize=-1;
      //totalSize=getBuiltInTypeSize(determineBuiltInTypeId(sgType));
      //if(totalSize>10000) {
      //	cout<<"DEBUG: TYPESIZE: DEFAULT (BUILT-IN): typesize="<<totalSize<<endl;
      //	return -1;
      //}

      //SAWYER_MESG(CodeThorn::logger[WARN])<<"VID:TSM:Unknown type:  "<<sgType->unparseToString()<<":"<<AstTerm::astTermWithNullValuesToString(sgType)<<endl;
      return 0;
    }
    }
    return 0;
  }

  CodeThorn::TypeSize TypeSizeMapping::determineElementTypeSize(SgArrayType* sgType) {
    //SgType* elementType=sgType->get_base_type();
    SgType* elementType=SageInterface::getArrayElementType(sgType);
    return determineTypeSize(elementType);
  }

  CodeThorn::TypeSize unknownSizeValue() { return -1; }
  
  //! Calculate the number of elements of an array type
  CodeThorn::TypeSize TypeSizeMapping::determineNumberOfElements(SgArrayType* t) {
    cout<<"TypeSizeMapping::determineNumberOfElements:" <<t->unparseToString()<<endl;

    //return SageInterface::getArrayElementCount(sgType);
    ROSE_ASSERT(t);
    size_t result=0; 
    SgExpression * indexExp =  t->get_index();
    cout<<"TypeSizeMapping: indexExp: "<< indexExp->unparseToString()<<endl;
    //cout<<"SageInterface::getArrayElementCount(sgType): "<<SageInterface::getArrayElementCount(t)<<endl; (doesn't work with cast)
  
    if((indexExp == nullptr) || isSgNullExpression(indexExp)) {
      result = unknownSizeValue();
    } else {
      if(t->get_is_variable_length_array ()) {
	result=-1;
      } else {
	result=t->get_number_of_elements ();
      }
    }

    if((indexExp == nullptr) || isSgNullExpression(indexExp)) {
      result = unknownSizeValue();
    } else { 
      goto done;
      if(AbstractValue::getVariableIdMapping()==nullptr) {
	while(SgCastExp* castExp=isSgCastExp(indexExp)) {
	  indexExp=castExp->get_operand_i();
	}
        //Take advantage of the fact that the value expression is always SgUnsignedLongVal in AST
        SgUnsignedLongVal * valExp = isSgUnsignedLongVal(indexExp);
        SgIntVal * valExpInt = isSgIntVal(indexExp);
        if(!(valExp || valExpInt)) {
          //SAWYER_MESG(CodeThorn::logger[ERROR])<<"Unexpected value: determineNumberOfElements: "<<indexExp->class_name()<<endl;
          //exit(1);
          return unknownSizeValue();
        } else {
          if (valExp)
            result = valExp->get_value(); 
          else 
            result = valExpInt->get_value();
        }
      } else {
        // variable id mapping is available in AbstractValue
        EStateTransferFunctions* tmpExprEvaluator=new EStateTransferFunctions();
        AbstractValue abstractSize=tmpExprEvaluator->evaluateExpressionWithEmptyState(indexExp);
	if(abstractSize.isConstInt()) {
          result=abstractSize.getIntValue();
        } else {
          // TODO: make the result of this entire function an abstract value
          result=unknownSizeValue();
        }
	delete tmpExprEvaluator;
      }
    }
    // consider multi dimensional case 
#if 0
    SgArrayType* arraybase = isSgArrayType(t->get_base_type());
    if (arraybase)
      result = result * determineNumberOfElements(arraybase);
#endif
  done:
    cout<<"TypeSizeMapping: indexExp: (result): "<< result<<endl;
    return result;
  }
  
  CodeThorn::TypeSize TypeSizeMapping::determineTypeSizePointedTo(SgPointerType* sgType) {
    SgType* typePointedTo=sgType->get_base_type();
    return determineTypeSize(typePointedTo);
  }

  std::string TypeSizeMapping::toString() {
    std::ostringstream ss;
    for(auto entry : _typeToSizeMapping ) {
      ss<<entry.first->unparseToString()<<":"<<entry.second<<std::endl;
    }
    return ss.str();
  }

  std::list<SgVariableDeclaration*> TypeSizeMapping::getDataMembers(SgClassDefinition* classDef) {
    std::list<SgVariableDeclaration*> varDeclList;
    typedef std::vector< std::pair< SgNode*, std::string > > DataMemberPointers;
    DataMemberPointers dataMemPtrs=classDef->returnDataMemberPointers();
    // returnDataMemberPointers includes all declarations (methods need to be filtered)
    for(DataMemberPointers::iterator i=dataMemPtrs.begin();i!=dataMemPtrs.end();++i) {
      SgNode* node=(*i).first;
      if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
        varDeclList.push_back(varDecl);
      }
    }
    return varDeclList;
  }
  
  bool TypeSizeMapping::isClassOrStructDeclaration(SgNode* node) {
    if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
      node=classDef->get_declaration();
    }
    if(SgClassDeclaration* classDecl=isSgClassDeclaration(node)) {
      SgClassDeclaration::class_types classType=classDecl->get_class_type();
      return classType==SgClassDeclaration::e_class||classType==SgClassDeclaration::e_struct;
    }
    return false;
  }

  bool TypeSizeMapping::isUnionDeclaration(SgNode* node) {
    if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
      node=classDef->get_declaration();
    }
    if(SgClassDeclaration* classDecl=isSgClassDeclaration(node)) {
      SgClassDeclaration::class_types classType=classDecl->get_class_type();
      return classType==SgClassDeclaration::e_union;
    }
    return false;
  }


} // end of namespace CodeThorn
