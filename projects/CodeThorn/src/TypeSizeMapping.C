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
      CodeThorn::logger[TRACE]<<"DEBUG: ARRAYTYPE: "<<sgType->unparseToString()<<endl;
      SgArrayType* arrayType=isSgArrayType(sgType);
      CodeThorn::TypeSize elementTypeSize=determineElementTypeSize(arrayType);
      CodeThorn::TypeSize numberOfElements=determineNumberOfElements(arrayType);
      unsigned int totalSize=numberOfElements*elementTypeSize;
      if(elementTypeSize>0) {
        _typeToSizeMapping[sgType]=totalSize; // cache result
      }
      return totalSize;
    }
    case V_SgClassType: {
      CodeThorn::logger[TRACE]<<"CLASSTYPE: "<<sgType->unparseToString()<<endl;
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
  
    // assume dimension default to 1 if not specified ,such as a[] 
    if((indexExp == nullptr) || isSgNullExpression(indexExp)) {
      result = 0;
    } else { 
      if(AbstractValue::getVariableIdMapping()==nullptr) {
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
      } else {
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
    // consider multi dimensional case 
    SgArrayType* arraybase = isSgArrayType(t->get_base_type());
    if (arraybase)
      result = result * determineNumberOfElements(arraybase);
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

  void TypeSizeMapping::computeOffsets(SgNode* root,CodeThorn::VariableIdMappingExtended* vim) {
#if 1
    ROSE_ASSERT(root);
    ROSE_ASSERT(vim);
    RoseAst ast(root);
    int numUnknownVarType=0;
    int numNonValidVarId=0;
    int numZeroTypeSize=0;
    int numUndefinedTypeSize=0;
    for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
#if 1
      SgNode* node=*i;
      ROSE_ASSERT(node);
      if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
        CodeThorn::logger[TRACE]<<"class def: "<<classDef->unparseToString()<<endl;
        std::list<SgVariableDeclaration*> dataMembers=getDataMembers(classDef);
        int offset=0;
        for(auto dataMember : dataMembers) {
          CodeThorn::logger[TRACE]<<"DEBUG: at data member: "<<dataMember->unparseToString()<<endl;
#if 1
          if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(dataMember)) {
            SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
            if(VariableIdMapping::isAnonymousBitfield(initName)) {
              // ROSE AST BUG WORKAROUND (ROSE-1867): anonymous bitfields are assigned the same SgSymbol
              continue;
            }
            CodeThorn::logger[TRACE]<<"struct data member decl: "<<dataMember->unparseToString()<<" : ";
            VariableId varId=vim->variableId(dataMember);
            vim->setIsMemberVariable(varId,true);
            if(varId.isValid()) {
              SgType* varType=vim->getType(varId);
              if(varType) {
                // determine if size of type is already known
                int typeSize=vim->getTypeSize(varType);
                if(isUndefinedTypeSize(typeSize)) {
                  if(isClassOrStructDeclaration(varDecl)) {
                    computeOffsets(varDecl,vim);
                    typeSize=vim->getTypeSize(varType);
                  }
                  // different varids can be mapped to the same offset
                  // every varid is inserted exactly once.
                  if(_typeToSizeMapping.find(varType)!=_typeToSizeMapping.end()) {
                    // if the same varId is found, ensure that the offset
                    // is the same again (e.g. the same headerfile is
                    // included in 2 different files, both provided on the
                    // command line
                    if(_typeToSizeMapping[varType]!=(size_t)offset) {
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
                    // typesize is unknown, invalidate offset computation
                    offset=-1;
                    break;
                  }
                  // for unions the offset is not increased (it is the same for all members)
                  if(!isUnionDeclaration(node)) {
                    offset+=typeSize;
                  }
                } else {
                  // type size is already known
                  vim->setOffset(varId,offset);
                  if(!isUnionDeclaration(node)) {
                    offset+=typeSize;
                  }
                }
              } else {
                // could not determine var type
                // ...
                //cout<<"DEBUG: unknown var type."<<endl;
                numUnknownVarType++;
              }
            } else {
              // non valid var id
              CodeThorn::logger[WARN]<<"TypeSizeMapping (offset computation): invalid varid at "
                                     <<SgNodeHelper::sourceFilenameLineColumnToString(varDecl)<<endl;
              numNonValidVarId++;
            }
          } else {
            // var decl is 0
          }
#endif
        } // end of loop on all data members of a type
        // skip subtree of class definition (would revisit nodes).
        i.skipChildrenOnForward();
      }
#endif
    }  // iterate on all declarations
#if 1
    CodeThorn::logger[INFO]<<"Number of unknown var types: "<<numUnknownVarType<<endl;
    CodeThorn::logger[INFO]<<"Number of non-valid varids: "<<numNonValidVarId<<endl;
    CodeThorn::logger[INFO]<<"Number of types with 0 size: "<<numZeroTypeSize<<endl;
    //CodeThorn::logger[INFO]<<"Typesize map size: "<<_typeToSizeMapping.size()<<endl;
#endif
#endif
  }

#if 0
  int TypeSizeMapping::getOffset(CodeThorn::VariableId varId) {
    ROSE_ASSERT(varId.isValid());
    auto varIdOffsetPairIter=varIdTypeSizeMap.find(varId);
    if(varIdOffsetPairIter!=varIdTypeSizeMap.end()) {
      return (*varIdOffsetPairIter).second;
    } else {
      return 0;
    }
    return 0;
  }
#endif

} // end of namespace CodeThorn
