#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"
#include "AstTerm.h"
#include "CppStdUtilities.h"
#include "AstUtility.h"

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
    //for(auto type:_memPoolTraversal.builtInTypes) {
      //_typeSize[type]=unknownSizeValue();
    //}
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

  // sets used/unused flag in VIM
  void VariableIdMappingExtended::determineVarUsage(SgProject* project) {
    CodeThorn::VariableIdSet usedVarsInGlobalInitializers=AstUtility::usedVariablesInGlobalVariableInitializers(project, this);
    CodeThorn::VariableIdSet allUsedVars=usedVarsInGlobalInitializers;
    std::list<SgVarRefExp*> usedVarsInsideFunctionsVarRefs=VariableIdMappingExtended::variableAccessesInsideFunctions(project);
    CodeThorn::VariableIdSet usedVarsInsideFunctions;
    for(auto varRef : usedVarsInsideFunctionsVarRefs) {
      VariableId varId=variableId(varRef);
      if(varId.isValid()) {
	allUsedVars.insert(varId);
      }
    }
    for(auto p : mappingSymToVarId) {
      VariableId varId=p.second;
      if(allUsedVars.find(varId)==allUsedVars.end())
	getVariableIdInfoPtr(varId)->isUsed=false;
    }
  }

  bool VariableIdMappingExtended::isDataMemberAccess(SgVarRefExp* varRefExp) {
    if(varRefExp==nullptr) {
      this->appendErrorReportLine(string("isDataMemberAccess::varRefExp == 0"));
      return false;
    }
    if(SgNode* p=varRefExp->get_parent()) {
      return (isSgDotExp(p)||isSgArrowExp(p)) && (isSgBinaryOp(p)->get_rhs_operand()==varRefExp);
    } else {
      appendErrorReportLine(SgNodeHelper::locationToString(varRefExp)+" varRefExp->get_parent() == 0 (isDataMemberAccess)");
      return false;
    }
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

  std::list<SgVarRefExp*> VariableIdMappingExtended::variableAccessesInsideFunctions(SgProject* project) {
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

  SgStatement* VariableIdMappingExtended::correspondingStmtOfExpression(SgExpression* exp) {
    // traverse upwards until a stmt node is found
    SgNode* node=exp;
    while(!isSgStatement(node)) {
      node=node->get_parent();
      if(node==nullptr)
	return 0;
    }
    return isSgStatement(node);
  }

  std::list<std::pair<SgStatement*,SgVarRefExp*>> VariableIdMappingExtended::computeCorrespondingStmtsOfBrokenExpressions(list<SgVarRefExp*>& accesses) {
    std::list<std::pair<SgStatement*,SgVarRefExp*>> list;
    for(auto v:accesses) {
      VariableId varId=variableId(v);
      if(!varId.isValid()) {
	SgStatement* stmt=correspondingStmtOfExpression(v);
	// node can be 0, this indicates that the AST has broken parent pointers
	list.push_back(make_pair(stmt,v));
      }
    }
    return list;
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
	if(varSym==0) {
	  appendErrorReportLine("AST symbol check error#"+std::to_string(numErrors)+": VarRefExp:get_symbol() == 0."+SgNodeHelper::locationToString(varSym));
	  continue;
	}
	stringstream ss;
        ss<<"AST symbol check error #"<<numErrors<<": "<<accessName<<" with unregistered symbol:"<<varSym<<": "<<varSym->get_name();
        // print expression
        SgExpression* eroot=v;
	ROSE_ASSERT(eroot);
        while(SgExpression* p=isSgExpression(eroot->get_parent())) {
          eroot=p;
	}
	ROSE_ASSERT(eroot);
        ss<<":"<<SgNodeHelper::locationToString(eroot)<<":"<<eroot->unparseToString();
        SgInitializedName* initName=varSym->get_declaration();
	if(initName==0) {
	  appendErrorReportLine(ss.str()+" initName=varSym->get_declaration()==0");
	  continue;
	}
        SgVariableDeclaration* decl=isSgVariableDeclaration(initName->get_declaration());
        if(decl && isMemberVariableDeclaration(decl)) {
          if(SgClassDefinition* cdef=isSgClassDefinition(decl->get_parent())) {
            cout<<": found class of unregistered symbol:"<<cdef->get_qualified_name ()<<" defined in:"<<SgNodeHelper::sourceFilenameLineColumnToString(cdef);
            //SgClassDeclaration* classDecl=cdef->get_declaration();
            //ROSE_ASSERT(classDecl);
            //SgClassType* classType=classDecl->get_type();
            //ROSE_ASSERT(classType);
          } else {
            ss<<": could not find class of unregistered symbol!";
          }
        }
        appendErrorReportLine(ss.str());
      }
    }
    return numErrors;
  }


  bool VariableIdMappingExtended::astSymbolCheck(SgProject* project) {
    list<SgVarRefExp*> varAccesses=variableAccessesInsideFunctions(project);
    int32_t numVarErrors=checkVarRefExpAccessList(varAccesses,"var access");
    list<SgVarRefExp*> structAccesses=structAccessesInsideFunctions(project);
    int32_t numStructErrors=checkVarRefExpAccessList(structAccesses,"struct access");
    if(numVarErrors>0||numStructErrors) {
      string line="AST symbol check: FAIL (var access errors:"+std::to_string(numVarErrors)+", struct access errors:"+std::to_string(numStructErrors)+")";
      cout<<"\nINFO: "<<line<<endl;
      appendErrorReportLine(line);
    } else {
      //cout<<"INFO: ROSE AST Symbol check PASSED."<<endl;
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

  void VariableIdMappingExtended::registerReturnVariable() {
    _returnVarId=createUniqueTemporaryVariableId(string("$return"));
  }

  VariableId VariableIdMappingExtended::getReturnVariableId() {
    return _returnVarId;
  }

  bool VariableIdMappingExtended::isReturnVariableId(VariableId varId) {
    return varId==getReturnVariableId();
  }

  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset, bool replaceClassDataMembersMode) {
    ROSE_ASSERT(offset==0); // this parameter can be removed

    auto result=memberVariableDeclarationsList(classType);
    // if member variables of class cannot be determined, return
    // unknown size. The number of class members can also be 0,
    // therefore the additional parameter is used to cover all other
    // cases.
    if(result.first==false)
      return unknownSizeValue();
    else
      return registerClassMembers(classType,result.second,offset,replaceClassDataMembersMode);
  }

  CodeThorn::TypeSize VariableIdMappingExtended::registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset, bool replaceClassDataMembersMode) {
    ROSE_ASSERT(offset==0); // this parameter can be removed and turned into a local variable
    ROSE_ASSERT(classType!=nullptr);
    CodeThorn::TypeSize totalSize=0;
    if(replaceClassDataMembersMode) {
      // remove current entries of classType (will be replaced with new ones)
      removeDataMembersOfClass(classType);
    }

    for(auto memberVarDecl : memberList) {
      SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(memberVarDecl);
      if(sym->get_symbol_basis()!=0) {
        registerNewSymbol(sym);
        SgType* type=sym->get_type();
        VariableId varId=variableId(sym);
        registerClassMemberVar(classType,varId);
        getVariableIdInfoPtr(varId)->variableScope=VS_MEMBER;
        setOffset(varId,offset);
        ROSE_ASSERT(varId.isValid());
        CodeThorn::TypeSize typeSize=unknownSizeValue();
        if(SgClassType* memberClassType=isSgClassType(type)) {
          getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
          typeSize=registerClassMembers(memberClassType,0,replaceClassDataMembersMode); // start with 0 for each nested type
          setTypeSize(type,typeSize);
          setNumberOfElements(varId,classMembers[type].size());
          setTotalSize(varId,typeSize);
        } else if(SgArrayType* arrayType=isSgArrayType(type)) {
          getVariableIdInfoPtr(varId)->aggregateType=AT_ARRAY;
          typeSize=determineTypeSize(type);
          setTypeSize(type,typeSize);
          auto numElements=determineNumberOfArrayElements(arrayType);
          setNumberOfElements(varId,numElements);
          SgType* elementType=SageInterface::getArrayElementType(arrayType);
          setElementSize(varId,determineTypeSize(elementType));
          setTotalSize(varId,typeSize);
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
    if(typeIter==_typeSize.end()) {
      // new entry
      _typeSize[type]=newTypeSize;
    } else {
      if((*typeIter).second==unknownSizeValue()) {
        // entry with exists with UnknownTypeSize, set new type size
        _typeSize[type]=newTypeSize;
      } else {
        if((*typeIter).second==newTypeSize) {
          // nothing to do, same value different to UnknownSize already exists
        } else {
          stringstream ss;
          ss<<"type size mismatch: "<<type->unparseToString()<<": known size: "<<_typeSize[type]<<", new size: "<<newTypeSize<<" (ignored)";
          appendErrorReportLine(ss.str());
        }
      }
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::computeTypeSize(SgType* /*type*/) {
    return unknownSizeValue();
  }

  // not used
  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSizeNew(SgType* type) {
    auto typeIter=_typeSize.find(type);
    if(typeIter!=_typeSize.end()) {
      return (*typeIter).second;
    } else {
      return unknownSizeValue();
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getTypeSize(SgType* type) {
    type=strippedType(type);
    auto typeIter=_typeSize.find(type);
    if(typeIter!=_typeSize.end()) {
      return (*typeIter).second;
    } else {
      BuiltInType biTypeId=TypeSizeMapping::determineBuiltInTypeId(type);
      if(biTypeId!=BITYPE_UNKNOWN) {
        return getBuiltInTypeSize(biTypeId);
      } else {
        return unknownSizeValue();
      }
    }
  }

  CodeThorn::TypeSize VariableIdMappingExtended::getBuiltInTypeSize(enum CodeThorn::BuiltInType biType) {
    return typeSizeMapping.getBuiltInTypeSize(biType);
  }

  void VariableIdMappingExtended::computeVariableSymbolMapping(SgProject* project, int maxWarningsCount) {
    computeVariableSymbolMapping2(project,maxWarningsCount);
    determineVarUsage(project);

    bool symbolCheckOk=astSymbolCheck(project);
    if(!symbolCheckOk) {
#if 0
      list<SgVarRefExp*> structAccesses=structAccessesInsideFunctions(project);
      BrokenExprStmtList list=computeCorrespondingStmtsOfBrokenExpressions(structAccesses);
      if(list.size()>0) {
	appendErrorReportLine("Structure accesses with invalid varid: "+std::to_string(list.size()));
	uint32_t saErrNr=1;
	for (auto p : list) {
	  if(p.first) {
	    appendErrorReportLine("struct access error #"+std::to_string(saErrNr)+": "+SgNodeHelper::locationToString(p.first));
	  } else {
	    appendErrorReportLine("struct access error #"+std::to_string(saErrNr)+": "+"unknown stmt location");
	  }
	}
      }
#endif
    }
    generateErrorReport(symbolCheckOk);

    if(getAstSymbolCheckFlag()) {
      if(symbolCheckOk) {
	if(_status) cout<<"STATUS: AST symbol check passed."<<endl;
      } else {
	cerr<<"Error: AST symbol check failed (see file "<<errorReportFileName<<")"<<endl;
        exit(1);
      }
    }

    if(getArrayAbstractionIndex()>=0) {
      computeMemOffsetRemap();
    }
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
  void VariableIdMappingExtended::setVarIdInfoFromType(VariableId varId) {
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
    if(SgClassType* classType=isSgClassType(type)) {
      getVariableIdInfoPtr(varId)->aggregateType=AT_STRUCT;
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
      // built-in type
      getVariableIdInfoPtr(varId)->aggregateType=AT_SINGLE;
      BuiltInType biType=TypeSizeMapping::determineBuiltInTypeId(type);
      setElementSize(varId,typeSizeMapping.getBuiltInTypeSize(biType));
      setNumberOfElements(varId,1);
      setTotalSize(varId,getElementSize(varId));
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

  void VariableIdMappingExtended::setErrorReportFileName(std::string name) {
    errorReportFileName=name;
  }

  void VariableIdMappingExtended::generateErrorReport(bool astSymbolCheckResult) {
    stringstream ss;
    if(astSymbolCheckResult) {
      ss<<"AST symbol check: PASS"<<endl;
    } else {
      ss<<"AST symbol check: FAIL"<<endl;
    }
    if(errorReport.size()==0) {
      ss<<"VIM check: PASS"<<endl;
    } else {
      ss<<"VIM check: WARN"<<endl;
      uint32_t errorNr=1;
      for(auto s : errorReport) {
	ss<<"Error "<<std::to_string(errorNr)<<": "<<s<<endl;
	errorNr++;
      }
      ss<<"Total errors: "<<errorReport.size()<<endl;
    }
    if(errorReportFileName.size()>0) {
      if(!CppStdUtilities::writeFile(errorReportFileName, ss.str())) {
	cerr<<"Error: could not generate variable-id-mapping report file: "<<errorReportFileName<<endl;
      } else {
	if(_status) cout<<"STATUS: generated variable-id-mapping report file: "<<errorReportFileName<<endl;
      }
    }
  }

  void VariableIdMappingExtended::appendErrorReportLine(string s) {
    errorReport.push_back(s);
  }
  // SgInitializedName: SgName get_qualified_name(); SgSymbol* search_for_symbol_from_symbol_table();
  // SgSymbol: SgName get_mangled_name(); SgDeclarationStatement* get_declaration();
  // SgDeclarationStatement->SgVariableDeclaration: SgInitializedName * get_decl_item();
  // VarRefExp: SgVariableSymbol* get_symbol(); SgInitializedName * get_declaration()

  void VariableIdMappingExtended::computeVariableSymbolMapping2(SgProject* project, int /*maxWarningsCount*/) {

    createTypeLists();
    initTypeSizes();
    computeTypeSizes(); // currently does not compute any typesizes
    registerClassMembersNew();
    registerReturnVariable();

    int ct=0;
    for(auto classDef:_memPoolTraversal.classDefinitions) {
      SgClassDeclaration* classDecl=classDef->get_declaration();
      if(classDecl==nullptr) {
	appendErrorReportLine(SgNodeHelper::locationToString(classDef)+": class definition in memory pool: get_declaration() == 0.");
	continue;
      }
      SgClassType* classType=classDecl->get_type();
      if(classType==nullptr) {
	appendErrorReportLine(SgNodeHelper::locationToString(classDecl)+": class declaration (from definition) in memory pool: get_type() == 0.");
	continue;
      }
      registerClassMembers(classType, 0);
      ct++;
    }

    list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
    int numVarDecls=0;
    int numSymbolExists=0;
    int numFunctionParams=0;
    for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
      RoseAst ast(*k);
      ast.setWithTemplates(true);
      for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
        if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
          if(isMemberVariableDeclaration(varDecl))
            continue;
	  Sg_File_Info* fi=varDecl->get_file_info();
	  logger[TRACE]<<"varDecl: "<<varDecl<<" parent:"<<varDecl->get_parent()<<" file_id:"<<fi->get_file_id()<<" AST:"<<AstTerm::astTermWithNullValuesToString(varDecl)<<endl;
          addVariableDeclaration(varDecl);
          logger[TRACE]<<"registering var decl: "<<++numVarDecls<<":"<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<":"<<varDecl->unparseToString()<<endl;
        }
        if(SgFunctionDefinition* funDef=isSgFunctionDefinition(*i)) {
          std::vector<SgInitializedName *> & funFormalParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(*i);
	  int paramNr=0;
          for(auto initName : funFormalParams) {
	    paramNr++;
            SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initName);
            if(symbolExists(sym)) {
	      stringstream ss;
	      ss<<sym;
              appendErrorReportLine(SgNodeHelper::locationToString(funDef)+": found formal parameter with same symbol (in SgInitializedName) again: "+ss.str()+" (skipping).");
              numSymbolExists++;
              continue;
            }
            if(sym->get_symbol_basis()!=0) {
              registerNewSymbol(sym);

              VariableId varId=variableId(sym);
              getVariableIdInfoPtr(varId)->variableScope=VS_FUNPARAM; // formal parameter declaration
              getVariableIdInfoPtr(varId)->setTypeFromInitializedName(initName);
              SgType* type=getVariableIdInfoPtr(varId)->getType();
              if(type) {
                setVarIdInfoFromType(varId);
              }
              numFunctionParams++;
            } else {
              appendErrorReportLine(SgNodeHelper::locationToString(funDef)+": no symbol basis found for function parameter nr "+std::to_string(paramNr)+" (starting at 1)");
            }
          }
        }
      }
      // 2nd pass over all variable declarations, to ensure all settings are based on the variable declaration with an initializer (if available)
      for (auto pair: mappingVarIdToInfo) {
        setVarIdInfoFromType(pair.first);
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
    if(sym) {
      if(sym->get_symbol_basis()!=0) {
	VariableIdInfo* vidInfo=0;
	if(symbolExists(sym)) {
	  vidInfo=getVariableIdInfoPtr(variableId(sym));
	} else {
	  registerNewSymbol(sym);
	  vidInfo=getVariableIdInfoPtr(variableId(sym));
	}
	ROSE_ASSERT(vidInfo);
	vidInfo->addVariableDeclaration(varDecl); // record all variable declarations that map to the same symbol
      } else {
	stringstream ss;
	ss<<sym;
	appendErrorReportLine(SgNodeHelper::locationToString(varDecl)+": no symbol basis for symbol:"+ss.str());
      }
    } else {
      appendErrorReportLine(SgNodeHelper::locationToString(varDecl)+": variable declaration with no symbol");
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

  CodeThorn::TypeSize VariableIdMappingExtended::determineElementTypeSize(SgArrayType* sgType) {
    //SgType* elementType=sgType->get_base_type();
    SgType* elementType=SageInterface::getArrayElementType(sgType);
    return determineTypeSize(elementType);
  }

  CodeThorn::TypeSize VariableIdMappingExtended::determineNumberOfArrayElements(SgArrayType* arrayType) {
    SgExpression * indexExp =  arrayType->get_index();
    CodeThorn::TypeSize numElems=unknownSizeValue();
    if((indexExp == nullptr) || isSgNullExpression(indexExp)) {
      numElems=unknownSizeValue();
    } else {
      if(arrayType->get_is_variable_length_array()) {
        numElems=unknownSizeValue();
      } else {
        numElems=arrayType->get_number_of_elements();
      }
    }
    return numElems;
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
          // intentionally no default case to get compiler warning if any enum is missing
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
        // intentionally no default case to get compiler warning if any enum is missing
      }

    }
    os<<"================================================="<<endl;
    os<<"Variable Type Size and Scope Overview"<<endl;
    os<<"================================================="<<endl;
    os<<"Number of struct vars               : "<<structNum<<endl;
    os<<"Number of array vars                : "<<arrayNum<<endl;
    os<<"Number of built-in type vars        : "<<singleNum<<endl;
    os<<"Number of string literals           : "<<stringLiteralNum<<endl;
    os<<"Number of unknown (size) vars       : "<<unknownSizeNum<<endl;
    os<<"Number of unspecified size vars     : "<<unspecifiedSizeNum<<endl;
    os<<"-------------------------------------------------"<<endl;
    os<<"Number of global vars               : "<<globalVarNum<<endl;
    os<<"Number of local vars                : "<<localVarNum<<endl;
    os<<"Number of function param vars       : "<<paramVarNum<<endl;
    os<<"Number of struct/class member vars  : "<<memberVarNum<<endl;
    os<<"Number of unknown (scope) vars      : "<<unknownVarNum<<endl;
    os<<"-------------------------------------------------"<<endl;
    os<<"Maximum struct size                 : "<<maxStructTotalSize<<" bytes"<<endl;
    os<<"Maximum array size                  : "<<maxArrayTotalSize<<" bytes"<<endl;
    os<<"Maximum array element size          : "<<maxArrayElementSize<<endl;
    os<<"Maximum number of elements in struct: "<<maxStructElements<<endl;
    os<<"Maximum number of elements in array : "<<maxArrayElements<<endl;
    os<<"================================================="<<endl;
  }

  void VariableIdMappingExtended::toStream(ostream& os) {
    for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
      VariableId varId=variableIdFromCode(i);
      if(true||mappingVarIdToInfo[varId].variableScope!=VS_MEMBER) {
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
  std::vector<VariableId> VariableIdMappingExtended::getClassMembers(VariableId varId) {
    ROSE_ASSERT(getVariableIdInfo(varId).aggregateType==AT_STRUCT);
    //SgType* type=getVariableIdInfo(varId).getType();
    SgType* type=getType(varId);
    if(type) {
      return getClassMembers(type);
    }
    std::vector<VariableId> emptyVec;
    return emptyVec;
  }

  void VariableIdMappingExtended::setArrayAbstractionIndex(int32_t remapIndex) {
    _arrayAbstractionIndex=remapIndex;
  }
  int32_t VariableIdMappingExtended::getArrayAbstractionIndex() {
    return _arrayAbstractionIndex;
  }

  void VariableIdMappingExtended::computeMemOffsetRemap() {
    ROSE_ASSERT(_arrayAbstractionIndex>=0);
    for(auto varIdPair : mappingVarIdToInfo) {
      if(varIdPair.second.variableScope!=VS_MEMBER) {
        int32_t remapIndex=_arrayAbstractionIndex;
        memOffsetRemap(varIdPair.first,varIdPair.first,remapIndex,0,0, IDX_ORIGINAL);
      }
    }
  }

  void VariableIdMappingExtended::memOffsetRemap(VariableId memRegId, VariableId varId, int32_t remapIndex, CodeThorn::TypeSize regionOffset, CodeThorn::TypeSize remappedOffset, IndexRemappingEnum mappingType) {
    switch(getVariableIdInfo(varId).aggregateType) {
    case AT_STRUCT: {
      registerMapping(memRegId, regionOffset, remappedOffset, mappingType);
      std::vector<VariableId> members=getClassMembers(varId);
      for(auto memberVarId : members) {
        CodeThorn::TypeSize mVarOffset=getOffset(memberVarId);
        if(mVarOffset==unknownSizeValue()) {
          return; // skip remapping for rest of type if offset is unknown
        }
        regionOffset+=mVarOffset;
        remappedOffset+=mVarOffset;
        memOffsetRemap(memRegId,memberVarId,remapIndex,regionOffset,remappedOffset,mappingType);
      }
      break;
    }
    case AT_ARRAY:
    case AT_STRING_LITERAL: {
      auto elemSize=getElementSize(varId);
      if(elemSize==unknownSizeValue()) {
        return; // skip remapping for rest of type if elemSize is unknown
      }
      CodeThorn::TypeSize regionStartOffset=regionOffset;
      for(CodeThorn::TypeSize i=0;i<getNumberOfElements(varId);i++) {
        if(i>=remapIndex) {
          registerMapping(memRegId, regionOffset,remappedOffset, IDX_REMAPPED);
          remappedOffset=regionStartOffset+remapIndex*elemSize;
        } else {
          registerMapping(memRegId, regionOffset,remappedOffset, mappingType);
          remappedOffset+=elemSize;
        }
        regionOffset+=elemSize; // concrete offset
        //if(getVariableIdInfo(varId).aggregateType==AT_ARRAY)
        //memOffsetRemap(xxxx,remapIndex,regionOffset,remappedOffset); multidim: remap fixed-sized multi-dim arrays (only difference to STRING_LITERAL)
      }
      break;
    }
    case AT_SINGLE: {
      registerMapping(memRegId, regionOffset,remappedOffset, mappingType);
      regionOffset+=getTotalSize(varId);
      remappedOffset+=getTotalSize(varId);
      break;
    }
    case AT_UNKNOWN:
      // do not include in mapping
      break;
    default:
      cerr<<"Error: VariableIdMappingExtended::memOffsetRemap: Undefined aggregate type in variable id mapping."<<endl;
      exit(1);
    }
  }

  VariableIdMappingExtended::OffsetAbstractionMappingEntry::OffsetAbstractionMappingEntry() {
  }
  VariableIdMappingExtended::OffsetAbstractionMappingEntry::OffsetAbstractionMappingEntry(TypeSize remappedOffset, IndexRemappingEnum mappingType) {
    this->remappedOffset=remappedOffset;
    this->mappingType=mappingType;
  }
  CodeThorn::TypeSize VariableIdMappingExtended::OffsetAbstractionMappingEntry::getRemappedOffset() {
    return remappedOffset;
  }
  VariableIdMappingExtended::IndexRemappingEnum VariableIdMappingExtended::OffsetAbstractionMappingEntry::getIndexRemappingType() {
    return mappingType;
  }

  void VariableIdMappingExtended::registerMapping(VariableId varId, CodeThorn::TypeSize regionOffset,CodeThorn::TypeSize remappedOffset, IndexRemappingEnum mappingType) {
    ROSE_ASSERT(varId.isValid());
    _offsetAbstractionMapping[varId][regionOffset]=OffsetAbstractionMappingEntry(remappedOffset,mappingType);
  }
  VariableIdMappingExtended::OffsetAbstractionMappingEntry VariableIdMappingExtended::getOffsetAbstractionMappingEntry(VariableId varId, CodeThorn::TypeSize regionOffset) {
    ROSE_ASSERT(varId.isValid());
    return _offsetAbstractionMapping[varId][regionOffset];
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
    } else if(getSymbol(varId)) {
      ss<<","<<variableName(varId);
      ss<<",type:"<<getType(varId)->unparseToString();
    } else {
      ss<<","<<"<missing-symbol>";
    }
    return ss.str();
  }

  void VariableIdMappingExtended::setAstSymbolCheckFlag(bool flag) {
    _astSymbolCheckFlag=flag;
  }

  bool VariableIdMappingExtended::getAstSymbolCheckFlag() {
    return _astSymbolCheckFlag;
  }

  std::vector<VariableId> VariableIdMappingExtended::getClassMembers(SgType* type) {
    return classMembers[type];
  }

  void VariableIdMappingExtended::MemPoolTraversal::visit(SgNode* node) {
    if(SgClassType* ctype=isSgClassType(node)) {
      classTypes.insert(ctype);
    } else if(SgArrayType* atype=isSgArrayType(node)) {
      arrayTypes.insert(atype);
    } else if(SgType* btype=isSgType(node)) {
      builtInTypes.insert(btype);
    } else if(SgClassDefinition* cdef=isSgClassDefinition(node)) {
      classDefinitions.insert(cdef);
    }
  }

  void VariableIdMappingExtended::MemPoolTraversal::dumpClassTypes() {
    int i=0;
    for(auto t:classTypes) {
      auto mList=memberVariableDeclarationsList(t);
      std::cout<<i++<<": class Type (";
      if(mList.first)
        std::cout<<mList.second.size();
      else
        std::cout<<"unknown";
      std::cout<<") :"<<t->unparseToString()<<std::endl;
    }
  }

  void VariableIdMappingExtended::MemPoolTraversal::dumpArrayTypes() {
    int i=0;
    for(auto t:arrayTypes) {
      std::cout<<"Array Type "<<i++<<":"<<t->unparseToString()<<std::endl;
    }
  }

  std::string VariableIdMappingExtended::typeSizeMappingToString() {
    stringstream ss;
    for(auto p: _typeSize) {
      ss<<p.first->unparseToString()<<": "<<AstTerm::astTermWithNullValuesToString(p.first)<<": "<<p.second<<endl;
    }
    return ss.str();
  }
  void VariableIdMappingExtended::setStatusFlag(bool flag) {
    _status=flag;
  }

  string VariableIdMappingExtended::unusedVariablesCsvReport() {
    stringstream ss;
    for(auto p : mappingSymToVarId) {
      VariableId varId=p.second;
      auto varInfo=getVariableIdInfoPtr(varId);
      if(!varInfo->isUsed) {
	// found unused variable, write it to the report with some info about the location and scope of variable
	SgVariableDeclaration* varDecl=getVariableDeclaration(varId);
	if(varDecl) {
	  // if varId is a function parameter, there is no declaration (there is only a SgInitializedName)
	  ss<<varInfo->variableScopeToString()<<","
	    <<varInfo->aggregateTypeToString()<<","
	    <<SgNodeHelper::locationToString(varDecl)<<","
	    <<variableName(varId)
	    <<endl;
	}
      }
    }
    return ss.str();
  }

}
