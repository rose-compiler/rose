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
	if(SgClassType* memberClassType=isSgClassType(type)) {
	  CodeThorn::TypeSize typeSize=registerClassMembers(memberClassType,0); // start with 0 for each nested type
	  setTypeSize(type,typeSize);
	  offset+=typeSize;
	} else if(SgArrayType* arrayType=isSgArrayType(type)) {
	  CodeThorn::TypeSize typeSize=typeSizeMapping.determineTypeSize(type);
	  setTypeSize(type,typeSize);
	  offset+=typeSize;
	} else {
	  // only built-in scalar types
	  offset+=typeSizeMapping.determineTypeSize(type);
	}
      }
    }
    return offset;
  }

  SgType* VariableIdMappingExtended::strippedType(SgType* type) {
    return type->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount) {
    VariableIdMapping::computeVariableSymbolMapping(project, maxWarningsCount);
    computeTypeSizes();
    typeSizeMapping.computeOffsets(project,this);
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
    list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
    for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
      RoseAst ast(*k);
      ast.setWithTemplates(true);
      for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	CodeThorn::TypeSize totalSize=0;
	if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
	  //cout<<"DEBUG: var decl: "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<varDecl->unparseToString()<<endl;
	  SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
	  if(sym->get_symbol_basis()!=0) {
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
	      setTypeSize(classType,totalTypeSize);
	      setTotalSize(varId,totalTypeSize);
	    } else if(SgArrayType* arrayType=isSgArrayType(type)) {
	      getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
	      SgType* elementType=SageInterface::getArrayElementType(arrayType);
	      // the array base type must have been declared before or it is a pointer type
	      setElementSize(varId,getTypeSize(elementType));
	      setNumberOfElements(varId,typeSizeMapping.determineNumberOfElements(arrayType));
	      CodeThorn::TypeSize typeSize;
	      if(getElementSize(varId)!=unknownSizeValue() && getNumberOfElements(varId)!=unknownSizeValue())
		typeSize=getElementSize(varId)*getNumberOfElements(varId);
	      else
		typeSize=unknownSizeValue();
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
	  std::vector<SgInitializedName *> & funParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(*i);
	  for(auto initName : funParams) {
	    SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initName);
	    if(sym->get_symbol_basis()!=0) {
	      registerNewSymbol(sym);
	    }
	  }
	}
      }
      // creates variableid for each string literal in the entire program
      registerStringLiterals(project);
      //computeTypeSizes();
      //typeSizeMapping.computeOffsets(project,this);
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

void VariableIdMappingExtended::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    if(mappingVarIdToInfo[varId].variableScope!=VS_MEMBER) {
      os<<std::right<<std::setw(3)<<i
	<<", "<<std::setw(25)<<std::left<<"id:"+varId.toString()+":\""+varId.toString(this)+"\""
	//<<","<<SgNodeHelper::symbolToString(mappingVarIdToInfo[i].sym)  
	//<<","<<mappingVarIdToInfo[variableIdFromCode(i)]._sym
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

