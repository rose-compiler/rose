#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;
using namespace std;

namespace CodeThorn {

  static std::list<SgVariableDeclaration*> memberVariableDeclarationsList(SgClassType* classType) {
    std::list<SgVariableDeclaration*> declVarList;
    if(SgDeclarationStatement* declStmt1=classType->get_declaration()) {
      if(SgClassDeclaration* classDecl1=isSgClassDeclaration(declStmt1)) {
	if(SgDeclarationStatement* declStmt2=classDecl1->get_definingDeclaration()) {
	  if(SgClassDeclaration* classDecl2=isSgClassDeclaration(declStmt2)) {
	    if(SgClassDefinition* classDef=classDecl2->get_definition()) {
	      auto classMemberList=classDef->get_members();
	      for( auto classMember : classMemberList) {
		if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(classMember)) {
		  declVarList.push_back(varDecl);
		}
	      }
	    }
	  }
	}
      }
    }
    return declVarList;
  }
  
  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset) {
    //cout<<"DEBUG: register class members:"<<endl;
    std::list<SgVariableDeclaration*> memberList=memberVariableDeclarationsList(classType);
    return registerClassMembers(classType,memberList,offset);
  }

  CodeThorn::TypeSize VariableIdMappingExtended::numClassMembers(SgType* classType) {
    auto iter=classMembers.find(classType);
    if(iter!=classMembers.end()) {
      return static_cast<CodeThorn::TypeSize>(classMembers[classType].size());
    } else {
      return unknownSizeValue();
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset) {
    ROSE_ASSERT(classType!=nullptr);
    //cout<<"DEBUG: Class members of: "<<classType->unparseToString()<<":"<<memberList.size()<<endl;
    for(auto memberVarDecl : memberList) {
      if(TypeSizeMapping::isUnionDeclaration(memberVarDecl)) {
	cout<<"DEBUG: union detected! : "<<memberVarDecl->unparseToString()<<endl;
      }
      SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(memberVarDecl);
      if(sym->get_symbol_basis()!=0) {
	//cout<<"Register member decl:"<<memberVarDecl->unparseToString()<<endl;
	registerNewSymbol(sym);
	VariableId varId=variableId(sym);
	registerClassMemberVar(classType,varId);
	getVariableIdInfoPtr(varId)->variableScope=VS_MEMBER;
	setOffset(varId,offset);
	ROSE_ASSERT(varId.isValid());
	SgType* type=strippedType(sym->get_type());
	//cout<<"Type:"<<type->unparseToString()<<endl;
	CodeThorn::TypeSize typeSize=unknownSizeValue();
	if(SgClassType* memberClassType=isSgClassType(type)) {
	  typeSize=registerClassMembers(memberClassType,0); // start with 0 for each nested type
	  setTypeSize(type,typeSize);
	} else if(SgArrayType* arrayType=isSgArrayType(type)) {
	  typeSize=determineTypeSize(type);
	  setTypeSize(type,typeSize);
	} else {
	  // only built-in scalar types
	  typeSize=determineTypeSize(type);
	}
	if(typeSize!=unknownSizeValue()) {
	  offset+=typeSize;
	} else {
	  offset=unknownSizeValue();
	}
      }
    }
    return offset;
  }

  SgType* VariableIdMappingExtended::strippedType(SgType* type) {
    return type->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
  }

  void VariableIdMappingExtended::setTypeSize(VariableId varId, CodeThorn::TypeSize newTypeSize) {
    setTypeSize(getType(varId),newTypeSize);
  }

  void VariableIdMappingExtended::setTypeSize(SgType* type, CodeThorn::TypeSize newTypeSize) {
    auto typeIter=_typeSize.find(type);
    if(typeIter!=_typeSize.end()) {
      if((*typeIter).second!=newTypeSize) {
	stringstream ss;
	ss<<"type size mismatch: "<<type->unparseToString()<<": "<<_typeSize[type]<<" != "<<newTypeSize;
	recordWarning(ss.str());
      } else {
	_typeSize[type]=newTypeSize;
      }
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSize(SgType* type) {
    auto typeIter=_typeSize.find(type);
    if(typeIter!=_typeSize.end()) {
      return (*typeIter).second;
    } else {
      BuiltInType biTypeId=TypeSizeMapping::determineBuiltInTypeId(type);
      if(biTypeId!=BITYPE_UNKNOWN)
	return getBuiltInTypeSize(biTypeId);
      else
	return unknownSizeValue();
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getBuiltInTypeSize(enum CodeThorn::BuiltInType biType) {
    return typeSizeMapping.getBuiltInTypeSize(biType);
  }
  
  void VariableIdMappingExtended::recordWarning(std::string warningText) {
    _warnings.push_back(warningText);
  }
  
  void VariableIdMappingExtended::computeVariableSymbolMapping(SgProject* project, int maxWarningsCount) {
    //computeVariableSymbolMapping1(project,maxWarningsCount);
    computeVariableSymbolMapping2(project,maxWarningsCount);
    //computeVariableSymbolMapping3(project,maxWarningsCount);
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping1(SgProject* project, int maxWarningsCount) {
    VariableIdMapping::computeVariableSymbolMapping(project, maxWarningsCount);
    computeTypeSizes();
    //typeSizeMapping.computeOffsets(project,this);
  }

  bool VariableIdMappingExtended::symbolExists(SgSymbol* sym) {
    return mappingSymToVarId.find(sym)!=mappingSymToVarId.end();
  }

  SgVariableDeclaration* VariableIdMappingExtended::getVariableDeclarationFromSym(SgSymbol* sym) {
    return dynamic_cast<SgVariableDeclaration*>(SgNodeHelper::findVariableDeclarationWithVariableSymbol(sym));
  }

  CodeThorn::VariableIdSet VariableIdMappingExtended::getSetOfLocalVarIds() {
    CodeThorn::VariableIdSet globalVarIds;
    for(auto p : mappingSymToVarId) {
      if(getVariableIdInfoPtr(p.second)->variableScope==VS_LOCAL) {
	globalVarIds.insert(p.second);
      }
    }
    return globalVarIds;
  }

  std::list<SgVariableDeclaration*> VariableIdMappingExtended::getVariableDeclarationsOfVariableIdSet(VariableIdSet& vidSet) {
    std::list<SgVariableDeclaration*> decls;
    for(auto vid : vidSet) {
      decls.push_back(getVariableDeclaration(vid));
    }
    return decls;
  }

  CodeThorn::VariableIdSet VariableIdMappingExtended::getSetOfGlobalVarIds() {
    CodeThorn::VariableIdSet globalVarIds;
    for(auto p : mappingSymToVarId) {
      if(getVariableIdInfoPtr(p.second)->variableScope==VS_GLOBAL) {
	globalVarIds.insert(p.second);
      }
    }
    return globalVarIds;
  }

  std::list<SgVariableDeclaration*> VariableIdMappingExtended::getListOfGlobalVarDecls() {
    list<SgVariableDeclaration*> globalVarDecls;
    for(auto p : mappingSymToVarId) {
      if(getVariableIdInfoPtr(p.second)->variableScope==VS_GLOBAL) {
	SgVariableDeclaration* varDecl=getVariableDeclarationFromSym(p.first);
	globalVarDecls.push_back(varDecl);
      }
    }
    return globalVarDecls;
  }

  SgExprListExp* VariableIdMappingExtended::getAggregateInitExprListExp(SgVariableDeclaration* varDecl) {
    SgExpression* exp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
    // only a list if declaration has an aggregate initializer
    return dynamic_cast<SgExprListExp*>(exp);
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount) {
    list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
    int numVarDecls=0;
    int numSymbolExists=0;
    int numFunctionParams=0;
    for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
      RoseAst ast(*k);
      ast.setWithTemplates(true);
      for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	CodeThorn::TypeSize totalSize=0;
	if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
	  SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
	  if(sym->get_symbol_basis()!=0) {
	    if(symbolExists(sym)) {
	      //cout<<"VID: found symbol again: "<<sym->unparseToString()<<" (skipping)."<<endl;
	      numSymbolExists++;
	      continue;
	    }
	    //cout<<"DEBUG: var decl: "<<numVarDecls++<<":"<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<sym<<":"<<varDecl->unparseToString()<<endl;
	    registerNewSymbol(sym);
	    VariableId varId=variableId(sym);
	    if(SgNodeHelper::isGlobalVariableDeclaration(varDecl)) {
	      getVariableIdInfoPtr(varId)->variableScope=VS_GLOBAL;
	    } else {
	      getVariableIdInfoPtr(varId)->variableScope=VS_LOCAL;
	    }
	    SgType* type=strippedType(sym->get_type());// strip typedef and const
	    //cout<<"DEBUG:     type: "<<type->unparseToString()<<endl;
	    if(SgClassType* classType=isSgClassType(type)) {
	      getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
	      //cout<<"DEBUG: register class members:"<<endl;
	      std::list<SgVariableDeclaration*> memberVariableDeclarationList=memberVariableDeclarationsList(classType);
	      CodeThorn::TypeSize totalTypeSize=registerClassMembers(classType,memberVariableDeclarationList,0); // start with offset 0
	      setNumberOfElements(varId,numClassMembers(classType));
	      setTypeSize(classType,totalTypeSize); // size can also be unknown
	      setTotalSize(varId,totalTypeSize);
	    } else if(SgArrayType* arrayType=isSgArrayType(type)) {
	      getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
	      SgType* elementType=SageInterface::getArrayElementType(arrayType);
	      // the array base type must have been declared before or it is a pointer type
	      setElementSize(varId,determineTypeSize(elementType));
	      setNumberOfElements(varId,determineNumberOfArrayElements(arrayType));

	      // if number of elements hasn't been determined from type (e.g. int a[]={};) determine number of elements from initializer
	      if(getNumberOfElements(varId)==unknownSizeValue()) {
		if(SgExprListExp* initList=getAggregateInitExprListExp(varDecl)) {
		  //cout<<"DEBUG: found agg init: "<<initList->unparseToString()<<endl;
		  SgExpressionPtrList& exprPtrList=initList->get_expressions();
		  CodeThorn::TypeSize numInitializerElements=exprPtrList.size();
		  //cout<<"DEBUG: found agg init: "<<numInitializerElements<<" num elements!"<<endl;
		  setNumberOfElements(varId,numInitializerElements);
		}
	      }
		
	      CodeThorn::TypeSize typeSize;
	      if(getElementSize(varId)!=unknownSizeValue() && getNumberOfElements(varId)!=unknownSizeValue()) {
		typeSize=getElementSize(varId)*getNumberOfElements(varId);
	      } else {
		typeSize=unknownSizeValue();
	      }
	      setTypeSize(type,typeSize);
	      setTotalSize(varId,typeSize);
	    } else {
	      // built-in type
	      getVariableIdInfoPtr(varId)->aggregateType=AT_SINGLE;
	      BuiltInType biType=TypeSizeMapping::determineBuiltInTypeId(type);
	      setElementSize(varId,typeSizeMapping.getBuiltInTypeSize(biType));
	      setNumberOfElements(varId,1);
	      setTotalSize(varId,getElementSize(varId));
	    }
	  } else {
	    cout<<"DEBUG: no symbol basis."<<endl;
	  }
	}
	
	if(SgFunctionDefinition* funDef=isSgFunctionDefinition(*i)) {
	  //cout<<"DEBUG: fun def : "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<funDef->unparseToString()<<endl;
	  std::vector<SgInitializedName *> & funFormalParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(*i);
	  for(auto initName : funFormalParams) {
	    SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initName);
	    if(sym->get_symbol_basis()!=0) {
	      registerNewSymbol(sym);
	      VariableId varId=variableId(sym);
	      getVariableIdInfoPtr(varId)->variableScope=VS_LOCAL; // formal parameter declaration
	      numFunctionParams++;
	    }
	  }
	}
      }
      // creates variableid for each string literal in the entire program
      registerStringLiterals(project);
    }
    //cout<<"DEBUG VID size: "<<numVarDecls<<" symbol existed: "<<numSymbolExists<<" Num fun-params:"<<numFunctionParams<<endl;
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping3(SgProject* project, int maxWarningsCount) {
    registerAllVariableSymbols(project,maxWarningsCount); // registers vars + function params as global|local|
    registerStringLiterals(project);
    //registerMemberVariables(); // uses types of registered variable declarations+function params
    // computeTypeSizes(); // computes sizes of all types where possible without variables in size expressions
    // computeMemberVarOffsets(); // computes offsets based on type sizes
  }

  void VariableIdMappingExtended::registerAllVariableSymbols(SgProject* project, int maxWarningsCount) {
    std::list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(project);
    for(auto funDef : funDefs) {
      std::set<SgVariableDeclaration*> funLocalVarDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      for(auto localVarDecl : funLocalVarDecls) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(localVarDecl);
	if(sym->get_symbol_basis()!=0) {
	  registerNewSymbol(sym);
	  VariableId varId=variableId(sym);
	  if(SgNodeHelper::isGlobalVariableDeclaration(localVarDecl)) {
	    getVariableIdInfoPtr(varId)->variableScope=VS_GLOBAL;
	  } else {
	    getVariableIdInfoPtr(varId)->variableScope=VS_LOCAL;
	  }
	  SgType* type=strippedType(sym->get_type());// strip typedef and const
	  if(SgClassType* classType=isSgClassType(type)) {
	    getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
	    std::list<SgVariableDeclaration*> memberVariableDeclarationList=memberVariableDeclarationsList(classType);
	    //CodeThorn::TypeSize totalTypeSize=registerClassMembers(classType,memberVariableDeclarationList,0); // start with offset 0
	  } else if(SgArrayType* arrayType=isSgArrayType(type)) {
	    getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
	    //SgType* elementType=SageInterface::getArrayElementType(arrayType);
	  } else {
	    // built-in type
	    getVariableIdInfoPtr(varId)->aggregateType=AT_SINGLE;
	    //BuiltInType biType=TypeSizeMapping::determineBuiltInTypeId(type);
	  }
	}
      }
      std::vector<SgInitializedName *>& funFormalParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      for(auto initName : funFormalParams) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initName);
	if(sym->get_symbol_basis()!=0) {
	  registerNewSymbol(sym);
	  VariableId varId=variableId(sym);
	  getVariableIdInfoPtr(varId)->variableScope=VS_LOCAL; // formal parameter declaration
	}
      }
    }
  }

  // class members handling
  std::vector<VariableId> VariableIdMappingExtended::getRegisteredClassMemberVars(SgType* type) {
    return classMembers[type];
  }

  bool VariableIdMappingExtended::isRegisteredClassMemberVar(SgType* type,VariableId varId) {
    std::vector<VariableId>& v=classMembers[type];
    return std::find(v.begin(), v.end(), varId)!=v.end();
  }

  void VariableIdMappingExtended::registerClassMemberVar(SgType* type,VariableId varId) {
    if(!isRegisteredClassMemberVar(type,varId)) {
      classMembers[type].push_back(varId);
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSize(VariableId varId) {
    return getTypeSize(getType(varId));
  }

  size_t  VariableIdMappingExtended::getNumVarIds() {
    return mappingVarIdToInfo.size();
  }
}

void VariableIdMappingExtended::classMemberOffsetsToStream(ostream& os, SgType* type, int32_t nestingLevel) {
  ROSE_ASSERT(nestingLevel>0);
  ROSE_ASSERT(type);
  string nestingLevelIndent=std::string(nestingLevel, '@');
  for(auto mvarId : classMembers[type]) {
    os<<"     "<<nestingLevelIndent;
    os<<setw(2)<<mappingVarIdToInfo[mvarId].offset<<": ";
    os<<"id:"+mvarId.toString()+":\""+mvarId.toString(this)+"\"";
    os<<","<<getVariableIdInfo(mvarId).aggregateTypeToString();
    os<<endl;
    if(mappingVarIdToInfo[mvarId].aggregateType==AT_STRUCT) {
      // recurse into nested type and increase nesting level by 1
      classMemberOffsetsToStream(os,getType(mvarId),nestingLevel+1);
    }
  }
}

CodeThorn::TypeSize VariableIdMappingExtended::determineElementTypeSize(SgArrayType* sgType) {
  //SgType* elementType=sgType->get_base_type();
  SgType* elementType=SageInterface::getArrayElementType(sgType);
  return determineTypeSize(elementType);
}

CodeThorn::TypeSize VariableIdMappingExtended::determineNumberOfArrayElements(SgArrayType* arrayType) {
  //cout<<"DEBUG: VariableIdMappingExtended::determineNumberOfArrayElements:" <<arrayType->unparseToString()<<endl;
  SgExpression * indexExp =  arrayType->get_index();
  //cout<<"DEBUG:VariableIdMappingExtended::determineNumberOfArrayElements: indexExp: "<< indexExp->unparseToString()<<endl;
#if 0
  {
    AbstractValue::setVariableIdMapping(this);
    ExprAnalyzer tmpExprEvaluator;
    AbstractValue abstractSize=tmpExprEvaluator.evaluateExpressionWithEmptyState(indexExp);
    if(abstractSize.isConstInt()) {
      cout<<"TypeSizeMapping: indexExp (eval): "<<abstractSize.getIntValue()<<endl;
      return abstractSize.getIntValue();
    } else {
      cout<<"TypeSizeMapping: indexExp (eval): non-const."<<endl;
    }
  }
#endif
  if((indexExp == nullptr) || isSgNullExpression(indexExp)) {
    return unknownSizeValue();
  } else { 
    if(arrayType->get_is_variable_length_array()) {
      return unknownSizeValue();
    } else {
      return arrayType->get_number_of_elements();
    }
  }
}

// also uses registerClassMembers 
CodeThorn::TypeSize VariableIdMappingExtended::determineTypeSize(SgType* type0) {
  if(type0) {
    SgType* type=strippedType(type0);// strip typedef and const
    if(SgClassType* classType=isSgClassType(type)) {
      std::list<SgVariableDeclaration*> memberVariableDeclarationList=memberVariableDeclarationsList(classType);
      CodeThorn::TypeSize totalClassTypeSize=registerClassMembers(classType,memberVariableDeclarationList,0); // start with offset 0
      return totalClassTypeSize;
    } else if(SgArrayType* arrayType=isSgArrayType(type)) {
      SgType* elementType=SageInterface::getArrayElementType(arrayType);
      CodeThorn::TypeSize elementTypeSize=determineTypeSize(elementType);
      CodeThorn::TypeSize numberOfElements=determineNumberOfArrayElements(arrayType);
      if(numberOfElements!=unknownSizeValue()&&elementTypeSize!=unknownSizeValue()) {
	CodeThorn::TypeSize totalArraySize=elementTypeSize*numberOfElements;
	return totalArraySize;
      } else {
	//cout<<"DEBUG: array size unknown: "<<type->unparseToString()<<":"<<numberOfElements<<" * "<<elementTypeSize<<endl;
	return unknownSizeValue();
      }
    } else {
      // built-in type
      BuiltInType biTypeId=TypeSizeMapping::determineBuiltInTypeId(type);
      if(biTypeId!=BITYPE_UNKNOWN)
	return getBuiltInTypeSize(biTypeId);
    }
  }
  return unknownSizeValue();
}


void VariableIdMappingExtended::typeSizeOverviewtoStream(ostream& os) {
  int32_t structNum=0;
  int32_t arrayNum=0;
  int32_t singleNum=0;
  int32_t unknownNum=0;
  CodeThorn::TypeSize maxStructElements=0;
  CodeThorn::TypeSize maxStructTotalSize=0;
  CodeThorn::TypeSize maxArrayElements=0;
  CodeThorn::TypeSize maxArrayElementSize=0;
  CodeThorn::TypeSize maxArrayTotalSize=0;
  cout<<"Varible-id mapping size: "<<mappingVarIdToInfo.size()<<endl;
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    if(getVariableIdInfo(varId).variableScope==VS_MEMBER)
      continue;
    switch(getVariableIdInfo(varId).aggregateType) {
      case AT_STRUCT:
	structNum++;
	maxStructElements=max(maxStructElements,getNumberOfElements(varId));
	if(getTotalSize(varId)!=unknownSizeValue())
	  maxStructTotalSize=max(maxStructTotalSize,getTotalSize(varId));
	break;
      case AT_ARRAY:
	arrayNum++;
	if(getNumberOfElements(varId)!=unknownSizeValue())
	  maxArrayElements=max(maxArrayElements,getNumberOfElements(varId));
	if(getElementSize(varId)!=unknownSizeValue())
	  maxArrayElementSize=max(maxArrayElementSize,getElementSize(varId));
	if(getTotalSize(varId)!=unknownSizeValue())
	  maxArrayTotalSize=max(maxArrayTotalSize,getElementSize(varId));
	break;
      case AT_SINGLE:
	singleNum++;
	break;
      case AT_UNKNOWN:
	unknownNum++;
	break;
    }
  }
  cout<<"================================================="<<endl;
  cout<<"Type Size Overview"<<endl;
  cout<<"================================================="<<endl;
  cout<<"Number of struct variables          : "<<structNum<<endl;
  cout<<"Number of array variables           : "<<arrayNum<<endl;
  cout<<"Number of built-in type variables   : "<<singleNum<<endl;
  cout<<"Number of unknown size variables    : "<<unknownNum<<endl;
  cout<<"Maximum struct size                 : "<<maxStructTotalSize<<" bytes"<<endl;
  cout<<"Maximum array size                  : "<<maxArrayTotalSize<<" bytes"<<endl;
  cout<<"Maximum array element size          : "<<maxArrayElementSize<<endl;
  cout<<"Maximum number of elements in struct: "<<maxStructElements<<endl;
  cout<<"Maximum number of elements in array : "<<maxArrayElements<<endl;
  cout<<"================================================="<<endl;
}

void VariableIdMappingExtended::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    if(mappingVarIdToInfo[varId].variableScope!=VS_MEMBER) {
      os<<std::right<<std::setw(3)<<i
	<<", "<<std::setw(25)<<std::left<<"id:"+varId.toString()+":\""+varId.toString(this)+"\""
	<<","<<std::setw(8)<<getVariableIdInfo(varId).variableScopeToString()
	<<","<<std::setw(8)<<getVariableIdInfo(varId).aggregateTypeToString();
      switch(getVariableIdInfo(varId).aggregateType) {
      case AT_STRUCT:
	os<<",elems:"<<getNumberOfElements(varId)
	  <<",total:"<<getTotalSize(varId);
	break;
      case AT_ARRAY:
	os<<",elems:"<<getNumberOfElements(varId)
	  <<",elemsize:"<<getElementSize(varId)
	  <<",total:"<<getTotalSize(varId);
	break;
      case AT_SINGLE:
	os<<",elems:"<<getNumberOfElements(varId)
	  <<",elemsize:"<<getElementSize(varId)
	  <<",total:"<<getTotalSize(varId);
	break;
      case AT_UNKNOWN:
	os<<"???";
	break;
      default:
	cerr<<"Error: VariableIdMappingExtended::toStream: Undefined aggregate type in variable id mapping."<<endl;
	exit(1);
      }
      if(isStringLiteralAddress(varId)) {
	os<<","<<"<non-symbol-string-literal-id>";
      } else if(isTemporaryVariableId(varId)) {
	os<<","<<"<non-symbol-memory-region-id>";
      } else if(SgSymbol* sym=getSymbol(varId)) {
	os<<","<<variableName(varId);
      } else {
	os<<","<<"<missing-symbol>";
      }
      os<<endl;

      if(mappingVarIdToInfo[varId].aggregateType==AT_STRUCT) {
	int32_t nestingLevel=1;
	classMemberOffsetsToStream(os,getType(varId),nestingLevel);
      }
    }
  }
}

// OLD METHODS (VIM2)

  std::string VariableIdMappingExtended::typeSizeMappingToString() {
    return typeSizeMapping.toString();
  }

  void VariableIdMappingExtended::computeTypeSizes() {
    // compute size for all variables
    VariableIdSet varIdSet=getVariableIdSet();
    for(auto vid : varIdSet) {
      SgType* varType=getType(vid);
      if(varType) {
        if(SgArrayType* arrayType=isSgArrayType(varType)) {
          setElementSize(vid,typeSizeMapping.determineElementTypeSize(arrayType));
          setNumberOfElements(vid,typeSizeMapping.determineNumberOfElements(arrayType));
        } else {
          setElementSize(vid,typeSizeMapping.determineTypeSize(varType));
          setNumberOfElements(vid,1);
        }
      }
    }
  }

