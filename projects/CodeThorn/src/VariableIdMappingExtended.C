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
    cout<<"DEBUG: register class members:"<<endl;
    std::list<SgVariableDeclaration*> memberList=memberVariableDeclarationsList(classType);
    return registerClassMembers(classType,memberList,offset);
  }
  
  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset) {
    cout<<"DEBUG: Class members of: "<<classType->unparseToString()<<":"<<memberList.size()<<endl;
    int numClassMembers=0;
    for(auto memberVarDecl : memberList) {
      SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(memberVarDecl);
      if(sym->get_symbol_basis()!=0) {
	cout<<"Register member decl:"<<memberVarDecl->unparseToString()<<endl;
	registerNewSymbol(sym);
	numClassMembers++;
	VariableId varId=variableId(sym);
	setOffset(varId,offset);
	ROSE_ASSERT(varId.isValid());
	SgType* type=strippedType(sym->get_type());
	cout<<"Type:"<<type->unparseToString()<<endl;
	if(SgClassType* memberClassType=isSgClassType(type)) {
	  CodeThorn::TypeSize typeSize=registerClassMembers(memberClassType,0); // start with 0 for each nested type
	  offset+=typeSize;
	} else if(SgArrayType* arrayType=isSgArrayType(type)) {
	  offset+=typeSizeMapping.determineTypeSize(type);
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
    /*
    if(type) {
      if(SgTypedefType* typeDeftype=isSgTypedefType(type)) {
	while(typeDeftype) {
	  //cout<<"DEBUG: found typedef type: "<<typeDeftype->unparseToString()<<endl;
	  type=typeDeftype->get_base_type();
	  typeDeftype=isSgTypedefType(type);
	}
      }
    }
    return type;
    */
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount) {
    VariableIdMapping::computeVariableSymbolMapping(project, maxWarningsCount);
    computeTypeSizes();
    typeSizeMapping.computeOffsets(project,this);
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
  void VariableIdMappingExtended::recordWarning(std::string warningText) {
    _warnings.push_back(warningText);
  }
  
  void VariableIdMappingExtended::setTypeSize(VariableId varId, CodeThorn::TypeSize newTypeSize) {
    setTypeSize(getType(varId),newTypeSize);
  }
  
  void VariableIdMappingExtended::computeVariableSymbolMapping(SgProject* project, int maxWarningsCount) {
    list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
    for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
      RoseAst ast(*k);
      ast.setWithTemplates(true);
      for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	CodeThorn::TypeSize totalSize=0;
	if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
	  cout<<"DEBUG: var decl: "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<varDecl->unparseToString()<<endl;
	  SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
	  if(sym->get_symbol_basis()!=0) {
	    registerNewSymbol(sym);
	    VariableId varId=variableId(sym);
	    SgType* type=strippedType(sym->get_type());// strip typedef and const
	    cout<<"DEBUG:     type: "<<type->unparseToString()<<endl;
	    if(SgClassType* classType=isSgClassType(type)) {
	      cout<<"DEBUG: register class members:"<<endl;
	      std::list<SgVariableDeclaration*> memberList=memberVariableDeclarationsList(classType);
	      CodeThorn::TypeSize totalTypeSize=registerClassMembers(classType,memberList,0); // start with offset 0
	      setNumberOfElements(varId,memberList.size());
	      setTotalSize(varId,totalTypeSize);
	    } else if(SgArrayType* arrayType=isSgArrayType(type)) {
	      setElementSize(varId,typeSizeMapping.determineElementTypeSize(arrayType));
	      setNumberOfElements(varId,typeSizeMapping.determineNumberOfElements(arrayType));
	      CodeThorn::TypeSize typeSize=getElementSize(varId)*getNumberOfElements(varId);
	      setTotalSize(varId,typeSize);
	    } else {
	      // built-in type
	      setElementSize(varId,typeSizeMapping.determineTypeSize(type));
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

  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSize(CodeThorn::BuiltInType biType) {
    return typeSizeMapping.getTypeSize(biType);
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSize(SgType* type) {
    return typeSizeMapping.determineTypeSize(type);
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

void VariableIdMappingExtended::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    os<<i
      <<","<<varId.toString(this)
      //<<","<<SgNodeHelper::symbolToString(mappingVarIdToInfo[i].sym)  
      //<<","<<mappingVarIdToInfo[variableIdFromCode(i)]._sym
      <<","<<getNumberOfElements(varId)
      <<","<<getElementSize(varId)
      <<","<<getOffset(varId);
    if(isStringLiteralAddress(varId)) {
      os<<","<<"<non-symbol-string-literal-id>";
    } else if(isTemporaryVariableId(varId)) {
      os<<","<<"<non-symbol-memory-region-id>";
    } else if(SgSymbol* sym=getSymbol(varId)) {
      os<<","<<sym->get_mangled_name();
    } else {
      os<<","<<"<missing-symbol>";
    }
    if(isMemberVariable(varId)) {
      os<<",data-member";
    } else {
      os<<",variable";
    }
    os<<endl;
  }
}
