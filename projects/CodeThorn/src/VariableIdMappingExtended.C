#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;
using namespace std;

namespace CodeThorn {

  static std::pair<bool,std::list<SgVariableDeclaration*> > memberVariableDeclarationsList(SgClassType* classType) {
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
	    return make_pair(true,declVarList);
	  }
	}
      }
    }
    return make_pair(false,declVarList);
  }
  
  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset) {
    //cout<<"DEBUG: register class members:"<<endl;
    auto result=memberVariableDeclarationsList(classType);
    // if member variables of class cannot be determined, return
    // unknown size. The number of class members can also be 0,
    // therefore the additional parameter is used to cover all other
    // cases.
    if(result.first==false)
      return unknownSizeValue();
    else
      return registerClassMembers(classType,result.second,offset);
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
	//cout<<"DEBUG: union detected! : "<<memberVarDecl->unparseToString()<<endl;
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
	  getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
	} else if(SgArrayType* arrayType=isSgArrayType(type)) {
	  typeSize=determineTypeSize(type);
	  setTypeSize(type,typeSize);
	  getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
	} else {
	  // only built-in scalar types
	  typeSize=determineTypeSize(type);
	  getVariableIdInfoPtr(varId)->aggregateType=AT_SINGLE;
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

  CodeThorn::VariableIdSet VariableIdMappingExtended::getSetOfVarIds(VariableIdMapping::VariableScope vs) {
    CodeThorn::VariableIdSet varIds;
    for(auto p : mappingSymToVarId) {
      if(getVariableIdInfoPtr(p.second)->variableScope==vs) {
	varIds.insert(p.second);
      }
    }
    return varIds;
  }

  CodeThorn::VariableIdSet VariableIdMappingExtended::getSetOfLocalVarIds() {
    return getSetOfVarIds(VariableIdMapping::VariableScope::VS_LOCAL);
  }

  CodeThorn::VariableIdSet VariableIdMappingExtended::getSetOfFunParamVarIds() {
    return getSetOfVarIds(VariableIdMapping::VariableScope::VS_FUNPARAM);
  }

  CodeThorn::VariableIdSet VariableIdMappingExtended::getSetOfGlobalVarIds() {
    return getSetOfVarIds(VariableIdMapping::VariableScope::VS_GLOBAL);
  }

  std::list<SgVariableDeclaration*> VariableIdMappingExtended::getVariableDeclarationsOfVariableIdSet(VariableIdSet& vidSet) {
    std::list<SgVariableDeclaration*> decls;
    for(auto vid : vidSet) {
      decls.push_back(getVariableDeclaration(vid));
    }
    return decls;
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

  bool VariableIdMappingExtended::isMemberVariableDeclaration(SgVariableDeclaration* varDecl) {
    return isSgClassDefinition(varDecl->get_parent());
  }

  void VariableIdMappingExtended::setVarIdInfoFromType(VariableId varId, SgType* type, SgVariableDeclaration* linkedDecl) {
    type=strippedType(type);// strip typedef and const
    //cout<<"DEBUG:     type: "<<type->unparseToString()<<endl;
    if(SgClassType* classType=isSgClassType(type)) {
      getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
      //cout<<"DEBUG: register class members:"<<endl;
      CodeThorn::TypeSize totalTypeSize;
      auto result=memberVariableDeclarationsList(classType);
      if(result.first)
	totalTypeSize=registerClassMembers(classType,result.second,0); // start with offset 0
      else
	totalTypeSize=unknownSizeValue();
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
      // initializer can only exist if decl is available, which is not the case for formal function parameters (function is called with 3rd arg=0 in this case)
      if(getNumberOfElements(varId)==unknownSizeValue() && linkedDecl) {
	if(SgExprListExp* initList=getAggregateInitExprListExp(linkedDecl)) {
	  ROSE_ASSERT(initList);
	  SgExpressionPtrList& exprPtrList=initList->get_expressions();
	  CodeThorn::TypeSize numInitializerElements=exprPtrList.size();
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
	    if(isMemberVariableDeclaration(varDecl))
	      continue;
	    //cout<<"DEBUG: var decl: "<<numVarDecls++<<":"<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<sym<<":"<<varDecl->unparseToString()<<endl;
	    registerNewSymbol(sym);
	    VariableId varId=variableId(sym);
	    // the getVariableDeclaration function obtains the declaration through the symbol
	    SgVariableDeclaration* linkedDecl=getVariableDeclaration(varId);
#if 0
	    if(linkedDecl!=varDecl) {
	      cout<<"INFO: using linked decl instead of matched decl:"<<endl;
	      cout<<"      matched: "<<varDecl->unparseToString();
	      cout<<"      linked : "<<linkedDecl->unparseToString();
	    }
#endif
	    if(SgNodeHelper::isGlobalVariableDeclaration(linkedDecl)) {
	      getVariableIdInfoPtr(varId)->variableScope=VS_GLOBAL;
	    } else {
	      getVariableIdInfoPtr(varId)->variableScope=VS_LOCAL;
	    }
	    setVarIdInfoFromType(varId,sym->get_type(),linkedDecl);
	  } else {
	    cout<<"Warning: no symbol basis for sym:"<<sym<<endl;
	  }
	}
	if(SgFunctionDefinition* funDef=isSgFunctionDefinition(*i)) {
	  //cout<<"DEBUG: fun def : "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<funDef->unparseToString()<<endl;
	  std::vector<SgInitializedName *> & funFormalParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(*i);
	  for(auto initName : funFormalParams) {
	    SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initName);
	    if(symbolExists(sym)) {
	      cout<<"WARNING: found formal parameter with same symbol again: "<<sym->unparseToString()<<" (skipping)."<<endl;
	      numSymbolExists++;
	      continue;
	    }
	    if(sym->get_symbol_basis()!=0) {
	      registerNewSymbol(sym);
	      VariableId varId=variableId(sym);
	      getVariableIdInfoPtr(varId)->variableScope=VS_FUNPARAM; // formal parameter declaration
	      setVarIdInfoFromType(varId,sym->get_type(),0); // last arg indicates variable declaration (does not exist for formal parameters in ROSE AST)
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
      auto result=memberVariableDeclarationsList(classType);
      if(result.first)
	return registerClassMembers(classType,result.second,0); // start with offset 0
      else
	return unknownSizeValue();
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
  int32_t stringLiteralNum=0;
  int32_t unknownSizeNum=0;
  int32_t unspecifiedSizeNum=0;
  int32_t globalVarNum=0;
  int32_t localVarNum=0;
  int32_t paramVarNum=0;
  int32_t memberVarNum=0;
  int32_t unknownVarNum=0;
  CodeThorn::TypeSize maxStructElements=0;
  CodeThorn::TypeSize maxStructTotalSize=0;
  CodeThorn::TypeSize maxArrayElements=0;
  CodeThorn::TypeSize maxArrayElementSize=0;
  CodeThorn::TypeSize maxArrayTotalSize=0;
  cout<<"Varible-id mapping size: "<<mappingVarIdToInfo.size()<<endl;
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    // do not include function params and member vars in max and var counts (reported later separately)
    if(!(getVariableIdInfo(varId).variableScope==VS_MEMBER||getVariableIdInfo(varId).variableScope==VS_FUNPARAM)) {
      switch(getVariableIdInfo(varId).aggregateType) {
      case AT_STRUCT:
	structNum++;
	if(getNumberOfElements(varId)!=unknownSizeValue())
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
	  maxArrayTotalSize=max(maxArrayTotalSize,getTotalSize(varId));
	break;
      case AT_SINGLE:
	singleNum++;
	break;
      case AT_STRING_LITERAL:
	stringLiteralNum++;
	break;
      case AT_UNKNOWN:
	unknownSizeNum++;
	break;
	// intentionally no default case to get compiler warning if not properly supported
      }
      if(getVariableIdInfo(varId).unspecifiedSize) {
	unspecifiedSizeNum++;
      }
    }
    switch(getVariableIdInfo(varId).variableScope) {
    case VS_GLOBAL: globalVarNum++; break;
    case VS_LOCAL: localVarNum++;break;
    case VS_FUNPARAM: paramVarNum++; break;
    case VS_MEMBER: memberVarNum++; break;
    case VS_UNKNOWN: if(getVariableIdInfo(varId).aggregateType!=AT_STRING_LITERAL) unknownVarNum++; break;
      // intentionally no default case to get compiler warning if not properly supported
    }

  }
  cout<<"================================================="<<endl;
  cout<<"Variable Type Size and Scope Overview"<<endl;
  cout<<"================================================="<<endl;
  cout<<"Number of struct vars               : "<<structNum<<endl;
  cout<<"Number of array vars                : "<<arrayNum<<endl;
  cout<<"Number of built-in type vars        : "<<singleNum<<endl;
  cout<<"Number of string literals           : "<<stringLiteralNum<<endl;
  cout<<"Number of unknown (size) vars       : "<<unknownSizeNum<<endl;
  cout<<"Number of unspecified size vars     : "<<unspecifiedSizeNum<<endl;
  cout<<"-------------------------------------------------"<<endl;
  cout<<"Number of global vars               : "<<globalVarNum<<endl;
  cout<<"Number of local vars                : "<<localVarNum<<endl;
  cout<<"Number of function param vars       : "<<paramVarNum<<endl;
  cout<<"Number of struct/class member vars  : "<<memberVarNum<<endl;
  cout<<"Number of unknown (scope) vars      : "<<unknownVarNum<<endl;
  cout<<"-------------------------------------------------"<<endl;
  cout<<"Maximum struct size                 : "<<maxStructTotalSize<<" bytes"<<endl;
  cout<<"Maximum array size                  : "<<maxArrayTotalSize<<" bytes"<<endl;
  cout<<"Maximum array element size          : "<<maxArrayElementSize<<endl;
  cout<<"Maximum number of elements in struct: "<<maxStructElements<<endl;
  cout<<"Maximum number of elements in array : "<<maxArrayElements<<endl;
  cout<<"================================================="<<endl;
}

std::string VariableIdMappingExtended::varIdInfoToString(VariableId varId) {
  std::stringstream ss;
  ss<<std::setw(25)<<std::left<<"id:"+varId.toString()+":\""+varId.toString(this)+"\""
    <<","<<std::setw(8)<<getVariableIdInfo(varId).variableScopeToString()
    <<","<<std::setw(8)<<getVariableIdInfo(varId).aggregateTypeToString();
  switch(getVariableIdInfo(varId).aggregateType) {
  case AT_STRUCT:
    ss<<",elems:"<<getNumberOfElements(varId)
      <<",total:"<<getTotalSize(varId);
    break;
  case AT_ARRAY:
  case AT_STRING_LITERAL:
    ss<<",elems:"<<getNumberOfElements(varId)
      <<",elemsize:"<<getElementSize(varId)
      <<",total:"<<getTotalSize(varId);
    break;
  case AT_SINGLE:
    ss<<",elems:"<<getNumberOfElements(varId)
      <<",elemsize:"<<getElementSize(varId)
      <<",total:"<<getTotalSize(varId);
    break;
  case AT_UNKNOWN:
    ss<<"???";
    break;
  default:
    cerr<<"Error: VariableIdMappingExtended::toStream: Undefined aggregate type in variable id mapping."<<endl;
    exit(1);
  }
  if(isStringLiteralAddress(varId)) {
    ss<<","<<"<non-symbol-string-literal-id>";
  } else if(isTemporaryVariableId(varId)) {
    ss<<","<<"<non-symbol-memory-region-id>";
  } else if(SgSymbol* sym=getSymbol(varId)) {
    ss<<","<<variableName(varId);
  } else {
    ss<<","<<"<missing-symbol>";
  }
  return ss.str();
}

void VariableIdMappingExtended::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    if(mappingVarIdToInfo[varId].variableScope!=VS_MEMBER) {
      os<<std::right<<std::setw(3)<<i<<",";
      os<<varIdInfoToString(varId);
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
