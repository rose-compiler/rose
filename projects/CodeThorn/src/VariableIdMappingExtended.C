#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;
using namespace std;

namespace CodeThorn {

  void VariableIdMappingExtended::createTypeLists() {
    _memPoolTraversal.traverseMemoryPool();
  }

  void VariableIdMappingExtended::initTypeSizes() {
    for(auto type:_memPoolTraversal.classTypes)
      _typeSize[type]=unknownSizeValue();
    for(auto type:_memPoolTraversal.arrayTypes)
      _typeSize[type]=unknownSizeValue();
    for(auto type:_memPoolTraversal.builtInTypes)
      _typeSize[type]=unknownSizeValue();
  }

  void VariableIdMappingExtended::computeTypeSizes() {
    for(auto tPair:_typeSize) {
      if(tPair.second==unknownSizeValue()) {
	SgType* t=tPair.first;
	_typeSize[t]=computeTypeSize(t);
      }
    }
  }

  void VariableIdMappingExtended::dumpTypeLists() {
    _memPoolTraversal.dumpClassTypes();
    _memPoolTraversal.dumpArrayTypes();
    cout<<"Number of class types: "<<_memPoolTraversal.classTypes.size()<<endl;
    cout<<"Number of array types: "<<_memPoolTraversal.arrayTypes.size()<<endl;
  }

  void VariableIdMappingExtended::dumpTypeSizes() {
    cout<<"Type sizes:"<<endl;
    for(auto tPair:_typeSize) {
      cout<<tPair.first->unparseToString()<<" : "<<tPair.second<<endl;
    }
  }

  bool VariableIdMappingExtended::isDataMemberAccess(SgVarRefExp* varRefExp) {
    return (varRefExp!=nullptr) && (isSgDotExp(varRefExp->get_parent())||isSgArrowExp(varRefExp->get_parent())) && (isSgBinaryOp(varRefExp->get_parent())->get_rhs_operand()==varRefExp);
  }

  bool VariableIdMappingExtended::isGlobalOrLocalVariableAccess(SgVarRefExp* varRefExp) {
    return (varRefExp!=nullptr) && !isDataMemberAccess(varRefExp);
  }
  
  list<SgVarRefExp*> VariableIdMappingExtended::structAccessesInsideFunctions(SgProject* project) {
    list<SgVarRefExp*> varRefExpList;
    list<SgFunctionDefinition*> funDefList=SgNodeHelper::listOfFunctionDefinitions(project);
    for(list<SgFunctionDefinition*>::iterator i=funDefList.begin();i!=funDefList.end();++i) {
      RoseAst ast(*i);
      for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
	if(SgVarRefExp* varRefExp=isSgVarRefExp(*j)) {
	  if(isDataMemberAccess(varRefExp)) {
	    varRefExpList.push_back(varRefExp);
	  }
	}
      }
    }
    return varRefExpList;
  }

  list<SgVarRefExp*> VariableIdMappingExtended::variableAccessesInsideFunctions(SgProject* project) {
    list<SgVarRefExp*> varRefExpList;
    list<SgFunctionDefinition*> funDefList=SgNodeHelper::listOfFunctionDefinitions(project);
    for(list<SgFunctionDefinition*>::iterator i=funDefList.begin();i!=funDefList.end();++i) {
      RoseAst ast(*i);
      for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
	if(SgVarRefExp* varRefExp=isSgVarRefExp(*j)) {
	  if(!(isDataMemberAccess(varRefExp))) {
	    varRefExpList.push_back(varRefExp);
	  }
	}
      }
    }
    return varRefExpList;
  }

  int32_t VariableIdMappingExtended::repairVarRefExpAccessList(list<SgVarRefExp*>& accesses, string accessName) {
    int32_t numErrors=0;
    // check if all symbols of VarRefExp in structure accesses are represented in gathered class data members' symbols
    for(auto v:accesses) {
      VariableId varId=variableId(v);
      if(!varId.isValid()) {
	// report error, and record error
	numErrors++;
	SgVariableSymbol* varSym=v->get_symbol();
	SgInitializedName* initName=varSym->get_declaration();
	SgVariableDeclaration* decl=isSgVariableDeclaration(initName->get_declaration());
	if(decl && isMemberVariableDeclaration(decl)) {
	  if(SgClassDefinition* cdef=isSgClassDefinition(decl->get_parent())) {
	    //cout<<"Repairing AST symbol inconsistency #"<<numErrors<<": found class of unregistered symbol:"<<cdef->get_qualified_name ()<<" defined in:"<<SgNodeHelper::sourceFilenameLineColumnToString(cdef)<<endl;
	    SgClassDeclaration* classDecl=cdef->get_declaration();
	    ROSE_ASSERT(classDecl);
	    SgClassType* classType=classDecl->get_type();
	    ROSE_ASSERT(classType);
	    //cout<<": registering class members. of unregistered symbol:"<<cdef->get_qualified_name ()<<" defined in:"<<SgNodeHelper::sourceFilenameLineColumnToString(cdef)<<endl;
	    // register class members based on definition (not SgType)
	    std::list<SgVariableDeclaration*> memberList;
	    auto classMemberList=cdef->get_members();
	    for( auto classMember : classMemberList) {
	      if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(classMember)) {
		memberList.push_back(varDecl);
	      }
	    }
	    bool repairMode=true;
	    registerClassMembers(classType, memberList , 0, repairMode); // this is *not* the classType through which this definition can be found (BUG), but an equivalent one
	  } else {
	    //cout<<": could not find class of unregistered symbol!";
	  }
	}
      }
    }
    return numErrors;
  }

  int32_t VariableIdMappingExtended::checkVarRefExpAccessList(list<SgVarRefExp*>& accesses, string accessName) {
    int32_t numErrors=0;
    // check if all symbols of VarRefExp in structure accesses are represented in gathered class data members' symbols
    for(auto v:accesses) {
      VariableId varId=variableId(v);
      if(!varId.isValid()) {
	// report error, and record error
	numErrors++;
	SgVariableSymbol* varSym=v->get_symbol();
	cout<<"Error #"<<numErrors<<": "<<accessName<<" with unregistered symbol:"<<varSym<<": "<<varSym->get_name();
	// print expression
	SgExpression* eroot=v;
	while(SgExpression* p=isSgExpression(eroot->get_parent()))
	  eroot=p;
	cout<<":"<<SgNodeHelper::sourceFilenameLineColumnToString(eroot)<<":"<<eroot->unparseToString();
	SgInitializedName* initName=varSym->get_declaration();
	SgVariableDeclaration* decl=isSgVariableDeclaration(initName->get_declaration());
	if(decl && isMemberVariableDeclaration(decl)) {
	  if(SgClassDefinition* cdef=isSgClassDefinition(decl->get_parent())) {
	    cout<<": found class of unregistered symbol:"<<cdef->get_qualified_name ()<<" defined in:"<<SgNodeHelper::sourceFilenameLineColumnToString(cdef);
	    //SgClassDeclaration* classDecl=cdef->get_declaration();
	    //ROSE_ASSERT(classDecl);
	    //SgClassType* classType=classDecl->get_type();
	    //ROSE_ASSERT(classType);
	  } else {
	    cout<<": could not find class of unregistered symbol!";
	  }
	}
	cout<<endl;
      }
    }
    return numErrors;
  }
  

  bool VariableIdMappingExtended::consistencyCheck(SgProject* project) {
    list<SgVarRefExp*> varAccesses=variableAccessesInsideFunctions(project);
    int32_t numVarErrors=checkVarRefExpAccessList(varAccesses,"var access");
    list<SgVarRefExp*> structAccesses=structAccessesInsideFunctions(project);
    int32_t numStructErrors=checkVarRefExpAccessList(structAccesses,"struct access");
    if(numVarErrors>0||numStructErrors) {
      cout<<"\nINFO: ROSE AST Symbol Consistency check FAILED (var access errors:"<<numVarErrors<<", struct access errors:"<<numStructErrors<<")"<<endl;
    } else {
      //cout<<"INFO: ROSE AST Symbol consistency check PASSED."<<endl;
    }
    return numVarErrors==0 && numStructErrors==0;
  }

  bool VariableIdMappingExtended::isUnion(SgClassType* type) {
    if(SgClassDeclaration* classDecl=getClassDeclarationOfClassType(type)) {
      SgClassDeclaration::class_types classType=classDecl->get_class_type();
      return classType==SgClassDeclaration::e_union;
    }
    return false;
  }

  SgClassDeclaration* VariableIdMappingExtended::getClassDeclarationOfClassType(SgClassType* classType) {
    if(SgDeclarationStatement* declStmt1=classType->get_declaration()) {
      if(SgClassDeclaration* classDecl1=isSgClassDeclaration(declStmt1)) {
	if(SgDeclarationStatement* declStmt2=classDecl1->get_definingDeclaration()) {
	  if(SgClassDeclaration* classDecl2=isSgClassDeclaration(declStmt2)) {
	    return classDecl2;
	  }
	}
      }
    }
    return nullptr;
  }
  
  std::pair<bool,std::list<SgVariableDeclaration*> > VariableIdMappingExtended::memberVariableDeclarationsList(SgClassType* classType) {
    std::list<SgVariableDeclaration*> declVarList;
    if(SgClassDeclaration* classDecl2=getClassDeclarationOfClassType(classType)) {
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
    return make_pair(false,declVarList);
  }
  
  CodeThorn::TypeSize VariableIdMappingExtended::numClassMembers(SgType* classType) {
    auto iter=classMembers.find(classType);
    if(iter!=classMembers.end()) {
      return static_cast<CodeThorn::TypeSize>(classMembers[classType].size());
    } else {
      return unknownSizeValue();
    }
  }

  void VariableIdMappingExtended::removeDataMembersOfClass(SgClassType* classType) {
    auto iter=classMembers.find(classType);
    //cout<<"Removing data members of class:"<<classType->unparseToString()<<" : "<<(*iter).second.size()<<endl;
    if(iter!=classMembers.end()) {
      // TODO: mark varids as invalidated
      (*iter).second.clear();
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset, bool repairMode) {
    ROSE_ASSERT(offset==0); // this parameter can be removed

    //cout<<"DEBUG: register class members/2:"<<endl;
    auto result=memberVariableDeclarationsList(classType);
    // if member variables of class cannot be determined, return
    // unknown size. The number of class members can also be 0,
    // therefore the additional parameter is used to cover all other
    // cases.
    //cout<<"DEBUG: register class members/2 (2):"<<endl;
    if(result.first==false)
      return unknownSizeValue();
    else
      return registerClassMembers(classType,result.second,offset,repairMode);
  }

  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset, bool repairMode) {
    ROSE_ASSERT(offset==0); // this parameter can be removed and turned into a local variable
    ROSE_ASSERT(classType!=nullptr);
    CodeThorn::TypeSize totalSize=0;
    //cout<<"DEBUG: Class members of: "<<classType->unparseToString()<<":"<<memberList.size()<<endl;
    if(repairMode) {
      // remove current entries of classType (will be replaced with new ones)
      removeDataMembersOfClass(classType);
    }

    for(auto memberVarDecl : memberList) {
      SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(memberVarDecl);
      if(sym->get_symbol_basis()!=0) {
	//cout<<"DEBUG: Register member decl:"<<memberVarDecl->unparseToString()<<endl;
	registerNewSymbol(sym);
	SgType* type=sym->get_type();
	VariableId varId=variableId(sym);
	registerClassMemberVar(classType,varId);
	getVariableIdInfoPtr(varId)->variableScope=VS_MEMBER;
	setOffset(varId,offset);
	ROSE_ASSERT(varId.isValid());
	//cout<<"DEBUG: Type:"<<type->unparseToString()<<endl;
	CodeThorn::TypeSize typeSize=unknownSizeValue();
	if(SgClassType* memberClassType=isSgClassType(type)) {
	  typeSize=registerClassMembers(memberClassType,0,repairMode); // start with 0 for each nested type
	  setTypeSize(type,typeSize);
	  setNumberOfElements(varId,classMembers[type].size());
	  setTotalSize(varId,typeSize);
	  getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
	} else if(SgArrayType* arrayType=isSgArrayType(type)) {
	  typeSize=determineTypeSize(type);
	  setTypeSize(type,typeSize);
	  setNumberOfElements(varId,determineNumberOfArrayElements(arrayType));
	  setTotalSize(varId,typeSize);
	  getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
	} else {
	  // only built-in scalar types
	  typeSize=determineTypeSize(type);
	  setTotalSize(varId,typeSize);
	  setNumberOfElements(varId,1);
	  setElementSize(varId,typeSize);
	  getVariableIdInfoPtr(varId)->aggregateType=AT_SINGLE;
	}
	if(typeSize!=unknownSizeValue()) {
	  if(isUnion(classType)) {
	    // offset remains unchanged (=0)
	    totalSize=std::max(totalSize,typeSize);
	  } else {
	    offset+=typeSize;
	    totalSize+=typeSize;
	  }
	} else {
	  offset=unknownSizeValue();
	  totalSize=unknownSizeValue();
	}
      }
    }
    return totalSize;
  }

  SgType* VariableIdMappingExtended::strippedType(SgType* type) {
    //return type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_POINTER_MEMBER_TYPE);
    type=type->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
    //while(isSgPointerType(type))
    //  type=isSgPointerType(type)->get_base_type();
    //return type->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_POINTER_TYPE);
    return type;
  }

  SgType* VariableIdMappingExtended::strippedType2(SgType* type) {
    return type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_POINTER_MEMBER_TYPE);
    //type=type->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
    //return type->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_POINTER_TYPE);
    return type;
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

  CodeThorn::TypeSize VariableIdMappingExtended::computeTypeSize(SgType* type) {
    return unknownSizeValue();
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSizeNew(SgType* type) {
    auto typeIter=_typeSize.find(type);
    if(typeIter!=_typeSize.end()) {
      return (*typeIter).second;
    } else {
      return unknownSizeValue();
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

    if(getAstConsistencySymbolCheckFlag()) {
      //cout<<"INFO: running AST symbol check."<<endl;
      bool checkOk=consistencyCheck(project);
      if(!checkOk)
	exit(1);
    } else {
      // use temporary workaround (would make check pass, but creates more than one entry per data member of same struct)
      // repair struct access symbols if necessary
      list<SgVarRefExp*> structAccesses=structAccessesInsideFunctions(project);
      int32_t numStructErrorsDetected=repairVarRefExpAccessList(structAccesses,"struct access");
    }
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping1(SgProject* project, int maxWarningsCount) {
    VariableIdMapping::computeVariableSymbolMapping(project, maxWarningsCount);
    cerr<<"Computevariablesymbolmapping1 has been removed."<<endl;
    exit(1);
  }

  bool VariableIdMappingExtended::symbolExists(SgSymbol* sym) {
    return mappingSymToVarId.find(sym)!=mappingSymToVarId.end();
  }

  // returns nullptr for string-literal varIds
  SgVariableDeclaration* VariableIdMappingExtended::getVariableDeclaration(VariableId varId) {
    ROSE_ASSERT(varId.isValid());
    return getVariableIdInfoPtr(varId)->getVarDecl();
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
      auto d=getVariableDeclaration(vid);
      if(d)
	decls.push_back(d);
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

  /*
    classType: totalTypeSize=registerClassMembers
    arrayType: elementSize=determineTypeSize(elementType) -> registerClassMembers
               totalTypeSize=elementSize*getNumberOfElements(varId)
    builtinType: totalTypeSize=typeSizeMapping.getBuiltInTypeSize(biType)
   */
  void VariableIdMappingExtended::setVarIdInfoFromType(VariableId varId, SgType* type_ignored, SgVariableDeclaration* linkedDecl_ignored) {
    VariableIdInfo* varIdInfo=getVariableIdInfoPtr(varId);
      
    if(varIdInfo->getVarDecl()) {
      if(SgNodeHelper::isGlobalVariableDeclaration(varIdInfo->getVarDecl())) {
	varIdInfo->variableScope=VS_GLOBAL;
      } else {
	varIdInfo->variableScope=VS_LOCAL;
      }
    }

    SgType* type=varIdInfo->getType();
    if(type==nullptr)
      return;
    type=strippedType(type);// strip typedef and const
    //cout<<"DEBUG:  setVarIdInfoFrom type: "<<type->unparseToString()<<endl;
    if(SgClassType* classType=isSgClassType(type)) {
      getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
      //cout<<"DEBUG: register class members:"<<endl;
      CodeThorn::TypeSize totalTypeSize;
      auto result=memberVariableDeclarationsList(classType);
      if(result.first) {
	totalTypeSize=registerClassMembers(classType,result.second,0); // start with offset 0
      } else {
	totalTypeSize=unknownSizeValue();
      }
      setNumberOfElements(varId,numClassMembers(classType));
      setTypeSize(classType,totalTypeSize); // size can also be unknown
      setTotalSize(varId,totalTypeSize);
    } else if(SgArrayType* arrayType=isSgArrayType(type)) {
      getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
      SgType* elementType=SageInterface::getArrayElementType(arrayType);
      // the array base type must have been declared before or it is a pointer type
      //cout<<"DEBUG: register array type with element type:"<<elementType->unparseToString()<<endl;
      setElementSize(varId,determineTypeSize(elementType));
      setNumberOfElements(varId,determineNumberOfArrayElements(arrayType));

      // if number of elements hasn't been determined from type (e.g. int a[]={};) determine number of elements from initializer
      // initializer can only exist if decl is available, which is not the case for formal function parameters (function is called with 3rd arg=0 in this case)
      if(getNumberOfElements(varId)==unknownSizeValue() && varIdInfo->getVarDecl()) {
	if(SgExprListExp* initList=getAggregateInitExprListExp(varIdInfo->getVarDecl())) {
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
      //cout<<"DEBUG: register built-in type : "<<type->unparseToString()<<endl;
      // built-in type
      getVariableIdInfoPtr(varId)->aggregateType=AT_SINGLE;
      BuiltInType biType=TypeSizeMapping::determineBuiltInTypeId(type);
      setElementSize(varId,typeSizeMapping.getBuiltInTypeSize(biType));
      setNumberOfElements(varId,1);
      setTotalSize(varId,getElementSize(varId));
      /*
      // register (but do not include in total size computation) pointed-to types
      if(SgPointerType* ptrType=isSgPointerType(type)) {
	cout<<"DEBUG: pointer type: "<<ptrType->unparseToString()<<endl;
	type=strippedType2(type);
	cout<<"DEBUG: pointer typ after striptype2: "<<type->unparseToString()<<endl;
	if(SgClassType* classType=isSgClassType(type)) {
	  cout<<"DEBUG: class type: "<<classType->unparseToString()<<endl;
	  if(true || !isRegisteredType(classType)) {
	    cout<<"DEBUG: register new type: "<<classType->unparseToString()<<endl;
	    auto result=memberVariableDeclarationsList(classType);
	    CodeThorn::TypeSize totalTypeSize=unknownSizeValue();
	    if(true || result.first) {
	      totalTypeSize=registerClassMembers(classType,result.second,0); // start with offset 0
	    } else {
	      totalTypeSize=unknownSizeValue();
	    }
	    setTypeSize(classType,totalTypeSize); // size can also be unknown
	    registerType(classType);
	  }
	}
      }
      */
    }
  }

  bool VariableIdMappingExtended::isRegisteredType(SgType* type) {
    return _registeredTypes.find(type)!=_registeredTypes.end();
  }
  void VariableIdMappingExtended::registerType(SgType* type) {
    _registeredTypes.insert(type);
  }
  
  /*
    computeVariableSymbolMapping2: 
      for each var decl  : setVarIdInfoFromType(varId->sym->type)
      for each param decl: setVarIdInfoFromType(param=varId->sym->type)
      registerStringLiterals(project);
   */
  void VariableIdMappingExtended::registerClassMembersNew() {
    for(auto classType:_memPoolTraversal.classTypes) {
      registerClassMembers(classType, 0);
      //cout<<"REGISTER CLASS MEMBERS OF:"<<classType->get_name()<<endl;
    }
  }

  // SgInitializedName: SgName get_qualified_name(); SgSymbol* search_for_symbol_from_symbol_table();
  // SgSymbol: SgName get_mangled_name(); SgDeclarationStatement* get_declaration();
  // SgDeclarationStatement->SgVariableDeclaration: SgInitializedName * get_decl_item();
  // VarRefExp: SgVariableSymbol* get_symbol(); SgInitializedName * get_declaration()
  
  void VariableIdMappingExtended::computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount) {

    createTypeLists();
    initTypeSizes();
    computeTypeSizes(); // currently does not compute any typesizes
    registerClassMembersNew();

#if 0
    RoseAst ast(project);
    int ct=0;
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      /*
      if(SgClassDeclaration* classDecl=isSgClassDeclaration(*i)) {
	SgClassType* classType=classDecl->get_type();
	registerClassMembers(classType, 0);
	ct++;
      }
      */
      if(SgClassDefinition* classDef=isSgClassDefinition(*i)) {
	SgClassDeclaration* classDecl=classDef->get_declaration();
	SgClassType* classType=classDecl->get_type();
	registerClassMembers(classType, 0);
	ct++;
      }
    }
#else
    int ct=0;
    for(auto classDef:_memPoolTraversal.classDefinitions) {
      SgClassDeclaration* classDecl=classDef->get_declaration();
      SgClassType* classType=classDecl->get_type();
      registerClassMembers(classType, 0);
      ct++;
    }
#endif

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
	  if(isMemberVariableDeclaration(varDecl))
	    continue;
	  addVariableDeclaration(varDecl);
	  //cout<<"DEBUG: registering var decl: "<<numVarDecls++<<":"<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<varDecl->unparseToString()<<endl;
	}

	if(SgFunctionDefinition* funDef=isSgFunctionDefinition(*i)) {
	  //cout<<"DEBUG: fun def : "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<funDef->unparseToString()<<endl;
	  std::vector<SgInitializedName *> & funFormalParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(*i);
	  for(auto initName : funFormalParams) {
	    //cout<<"DEBUG: registering param: "<<initName->unparseToString()<<endl;
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
	      //cout<<"DEBUG: setting varidfromtype for param: "<<initName->unparseToString()<<endl;
	      setVarIdInfoFromType(varId,sym->get_type(),0); // last arg indicates variable declaration (does not exist for formal parameters in ROSE AST)
	      numFunctionParams++;
	    } else {
	      cout<<"Warning: no symbol basis found for function parameter: "<<initName->unparseToString()<<endl;
	    }
	  }
	}
      }

      // 2nd pass over all variable declarations, to ensure all settings are based on the variable declaration with an initializer (if available)
      for (auto pair: mappingVarIdToInfo) {
	setVarIdInfoFromType(pair.first,0,0);
      }

      // creates variableid for each string literal in the entire program
      registerStringLiterals(project);
    }
    //cout<<"INFO: Number of registered class types    : "<<ct<<endl;
    //cout<<"INFO: Number of registered var decls      : "<<numVarDecls<<endl;
    //cout<<"INFO: Number of registered function params: "<<numFunctionParams<<endl;
  }

  void VariableIdMappingExtended::addVariableDeclaration(SgVariableDeclaration* varDecl) {
    SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
    //cout<<"DEBUG: computeVariableSymbolMapping2: addVariableDeclaration: "<<varDecl->unparseToString()<<": sym="<<sym<<endl;
    if(sym->get_symbol_basis()!=0) {
      VariableIdInfo* vidInfo=0;
      if(symbolExists(sym)) {
	vidInfo=getVariableIdInfoPtr(variableId(sym));
	//cout<<"DEBUG: VID: found symbol again: "<<sym->unparseToString();
      } else {
	registerNewSymbol(sym);
	vidInfo=getVariableIdInfoPtr(variableId(sym));
      }
      ROSE_ASSERT(vidInfo);
      vidInfo->addVariableDeclaration(varDecl); // record all variable declarations that map to the same symbol
    } else {
      stringstream ss;
      ss<<sym;
      recordWarning("no symbol basis for sym:"+ss.str());
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
  //cout<<"Varible-id mapping size: "<<mappingVarIdToInfo.size()<<endl;
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

void VariableIdMappingExtended::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    if(mappingVarIdToInfo[varId].variableScope!=VS_MEMBER) {
      os<<std::right<<std::setw(3)<<i<<",";
      os<<varIdInfoToString(varId);
      os<<endl;
      if(mappingVarIdToInfo[varId].aggregateType==AT_STRUCT) {
	os<<"Data members "<<"("<<classMembers[getType(varId)].size()<<"):"<<endl;
	int32_t nestingLevel=1;
	classMemberOffsetsToStream(os,getType(varId),nestingLevel);
      }
    }
  }
}

void VariableIdMappingExtended::classMemberOffsetsToStream(ostream& os, SgType* type, int32_t nestingLevel) {
  ROSE_ASSERT(nestingLevel>0);
  ROSE_ASSERT(type);
  string nestingLevelIndent1="     ";
  string nestingLevelIndent2=std::string(nestingLevel, '@');
  string nestingLevelIndent=nestingLevelIndent1+nestingLevelIndent2;
  for(auto mvarId : classMembers[type]) {
    os<<nestingLevelIndent<<setw(3)<<getOffset(mvarId)<<":"<<varIdInfoToString(mvarId);
    os<<endl;
    if(mappingVarIdToInfo[mvarId].aggregateType==AT_STRUCT) {
      os<<nestingLevelIndent<<"Data members "<<"("<<classMembers[getType(mvarId)].size()<<"):"<<endl;
      // recurse into nested type and increase nesting level by 1
      classMemberOffsetsToStream(os,getType(mvarId),nestingLevel+1);
    }
  }
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
    ss<<",type:"<<getType(varId)->unparseToString();
  } else {
    ss<<","<<"<missing-symbol>";
  }
  return ss.str();
}

void VariableIdMappingExtended::setAstSymbolCheckFlag(bool flag) {
  _astConsistencySymbolCheckFlag=flag;
}

bool VariableIdMappingExtended::getAstConsistencySymbolCheckFlag() {
  return _astConsistencySymbolCheckFlag;
}

std::vector<VariableId> VariableIdMappingExtended::getClassMembers(SgType* type) {
  return classMembers[type];
}

// OLD METHODS (VIM2)

std::string VariableIdMappingExtended::typeSizeMappingToString() {
  return typeSizeMapping.toString();
}

