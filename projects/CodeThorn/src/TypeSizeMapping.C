#include "sage3basic.h"
#include <sstream>
#include <iostream>
#include "TypeSizeMapping.h"
#include "CodeThornException.h"
#include "CodeThornLib.h"
#include "ExprAnalyzer.h"
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
  
  CodeThorn::TypeSize TypeSizeMapping::getTypeSize(BuiltInType bitype) {
    ROSE_ASSERT(bitype<_mapping.size());
    return _mapping[bitype];
  }

  std::size_t TypeSizeMapping::sizeOfOp(BuiltInType bitype) {
    ROSE_ASSERT(bitype<_mapping.size());
    return getTypeSize(bitype);
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

  CodeThorn::TypeSize TypeSizeMapping::determineTypeSize(SgType* sgType) {
    ROSE_ASSERT(_mapping.size()!=0);
    ROSE_ASSERT(sgType);
    CodeThorn::logger[TRACE]<<"DEBUG: determineTypeSize: "<<sgType->unparseToString()<<endl;
    // cache every type's computed size
    if(_typeToSizeMapping.find(sgType)!=_typeToSizeMapping.end()) {
      return _typeToSizeMapping[sgType];
    }
    switch (sgType->variantT()) {

    case V_SgPointerType:
      return getTypeSize(BITYPE_POINTER);

    case V_SgTypeBool:
      return getTypeSize(BITYPE_BOOL);

    case V_SgTypeChar:
    case V_SgTypeSignedChar:
    case V_SgTypeUnsignedChar: 
      return getTypeSize(BITYPE_CHAR);

    case V_SgTypeShort:
    case V_SgTypeSignedShort:
    case V_SgTypeUnsignedShort:
      return getTypeSize(BITYPE_SHORT);

    case V_SgTypeUnsignedInt:
    case V_SgTypeInt:
    case V_SgTypeSignedInt:
      return getTypeSize(BITYPE_INT);

    case V_SgTypeUnsignedLong:
    case V_SgTypeLong:
    case V_SgTypeSignedLong:
      return getTypeSize(BITYPE_LONG);

    case V_SgTypeUnsignedLongLong:
    case V_SgTypeLongLong:
    case V_SgTypeSignedLongLong:
      return getTypeSize(BITYPE_LONG_LONG);

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

  CodeThorn::TypeSize TypeSizeMapping::determineElementTypeSize(SgArrayType* sgType) {
    //SgType* elementType=sgType->get_base_type();
    SgType* elementType=SageInterface::getArrayElementType(sgType);
    return determineTypeSize(elementType);
  }

  //! Calculate the number of elements of an array type
  CodeThorn::TypeSize TypeSizeMapping::determineNumberOfElements(SgArrayType* t) {
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

  void TypeSizeMapping::initializeOffsets(SgProject* root) {
#if 0
    ROSE_ASSERT(root);
    RoseAst ast(root);
    int numUnknownVarType=0;
    int numNonValidVarId=0;
    int numZeroTypeSize=0;
    for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgNode* node=*i;
      ROSE_ASSERT(node);
      if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
        //cout<<"DEBUG: class def: "<<classDef->unparseToString()<<endl;
        //cout<<"DEBUG: Class Definition: "<<classDef->unparseToString()<<endl;
        std::list<SgVariableDeclaration*> dataMembers=getDataMembers(classDef);
        int offset=0;
        for(auto dataMember : dataMembers) {
          //cout<<"DEBUG: at data member: "<<dataMember->unparseToString()<<endl;
          if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(dataMember)) {
            SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
            if(VariableIdMapping::isAnonymousBitfield(initName)) {
              // ROSE AST BUG WORKAROUND (ROSE-1867): anonymous bitfields are assigned the same SgSymbol
              continue;
            }
            //cout<<"DEBUG: struct data member decl: "<<dataMember->unparseToString()<<" : ";
            VariableId varId=AbstractValue::getVariableIdMapping()->variableId(dataMember);
            if(varId.isValid()) {
              SgType* varType=AbstractValue::getVariableIdMapping()->getType(varId);
              if(varType) {

                // TODO: recursive type size initialization for nested struct/class/union members
                // currently nested types are ignored!
                //if(isStruct(type) ...) initialize(variableIdMapping, dataMember);

                int typeSize=AbstractValue::getVariableIdMapping()->getTypeSize(varType);
                if(typeSize==0) {
                  numZeroTypeSize++;
                  //cout<<"DEBUG: Type of size 0: "<<varType->unparseToString()<<endl;
                }
              
                // different varids can be mapped to the same offset
                // every varid is inserted exactly once.
                if(varIdTypeSizeMap.find(varId)!=varIdTypeSizeMap.end()) {
                  // if the same varId is found, ensure that the offset
                  // is the same again (e.g. the same headerfile is
                  // included in 2 different files, both provided on the
                  // command line
                  if(varIdTypeSizeMap[varId]!=offset) {
                    continue; // ROSE AST WORKAROUND (for BUG ROSE-1879): ignore double entries in structs which are the result of a bug
                    // do nothing for now
                    //cerr<<"WARNING: Data structure offset mismatch at "<<SgNodeHelper::sourceFilenameLineColumnToString(dataMember)<<":"<<dataMember->unparseToString()<<":"<<varIdTypeSizeMap[varId]<<" vs "<<offset<<endl;

                    //variableIdMapping->toStream(cerr);
                    //cerr<<"Internal error: TypeSizeMapping::initializeOffsets: varid already exists."<<endl;
                    //cerr<<"existing var id: "<<varId.toUniqueString(variableIdMapping)<<endl;
                    //cerr<<"Symbol: "<<variableIdMapping->getSymbol(varId)<<endl;
                    //cerr<<"Type: "<<variableIdMapping->getType(varId)->unparseToString()<<endl;
                    //cerr<<"Declaration: "<<node->unparseToString()<<endl;
                    //exit(1);
                  }                  
                } else {
                  //cout<<" DEBUG Offset: "<<offset<<endl;
                  varIdTypeSizeMap.emplace(varId,offset);
                }
                // for unions the offset is not increased (it is the same for all members)
                if(!isUnionDeclaration(node)) {
                  offset+=typeSize;
                }
              } else {
                // could not determine var type
                // ...
                //cout<<"DEBUG: unknown var type."<<endl;
                numUnknownVarType++;
              }
            } else {
              // non valid var id
              // throw ...
              cerr<<"Internal Error: StructureAccessLookup: invalid varid."<<endl;
              numNonValidVarId++;
              exit(1);
            }
          }
        }
        // skip subtree of class definition (would revisit nodes).
        i.skipChildrenOnForward();
      }
    }
#if 0
    cerr<<"DEBUG: Number of unknown var types: "<<numUnknownVarType<<endl;
    cerr<<"DEBUG: Number of non-valid varids: "<<numNonValidVarId<<endl;
    cerr<<"DEBUG: Number of types with 0 size: "<<numZeroTypeSize<<endl;
    cerr<<"DEBUG: typesize map size: "<<varIdTypeSizeMap.size()<<endl;
#endif
#endif
  }

  int TypeSizeMapping::getOffset(CodeThorn::VariableId varId) {
#if 0
    ROSE_ASSERT(varId.isValid());
    auto varIdOffsetPairIter=varIdTypeSizeMap.find(varId);
    if(varIdOffsetPairIter!=varIdTypeSizeMap.end()) {
      return (*varIdOffsetPairIter).second;
    } else {
      return 0;
    }
#endif
    return 0;
  }

  bool TypeSizeMapping::isStructMember(CodeThorn::VariableId varId) {
    // TODO: add info to VariableIdMapping::VariableIdInfo
    // return getVariableIdInfo(varId).isStructMember()
    return false;
  }


} // end of namespace CodeThorn
