#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Diagnostics.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CodeThornException.h"

using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

Sawyer::Message::Facility CodeThorn::FunctionIdMapping::logger;

FunctionIdMapping::FunctionIdMapping() {

}
FunctionId FunctionIdMapping::getFunctionIdFromFunctionDef(SgFunctionDefinition* funDef) {
  return mappingFunctionDefToFunctionId.at(funDef);
}
SgFunctionDefinition* FunctionIdMapping::getFunctionDefFromFunctionId(CodeThorn::FunctionId funId) {
  auto iter=mappingFunctionIdToFunctionDef.find(funId);
  if(iter==mappingFunctionIdToFunctionDef.end()) {
    return 0;
  } else {
    return (*iter).second;
  }
}

SgFunctionType* FunctionIdMapping::getTypeFromFunctionId(FunctionId id) const {
  SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(getSymbolFromFunctionId(id));
  ROSE_ASSERT(functionSymbol);
  SgFunctionType* functionType = isSgFunctionType(functionSymbol->get_type());
  ROSE_ASSERT(functionType);
  return functionType;
}

void FunctionIdMapping::toStream(ostream& os) const {
  for(size_t i=0;i<mappingFunctionIdToSymbol.size();++i) {
    os<<""<<i
      <<","<<mappingFunctionIdToSymbol[i]
      <<","<<SgNodeHelper::symbolToString(mappingFunctionIdToSymbol[i])
      <<endl;
    // Const cast because operator[] of map is not const. This member function
    // is still const because we do not change this FunctionIdMapping.
    ROSE_ASSERT(const_cast<FunctionIdMapping*>(this)->mappingSymbolToFunctionId[mappingFunctionIdToSymbol[i]]==i);
  }
}

std::string FunctionIdMapping::getFunctionIdSetAsString(const FunctionIdSet& set) {
  std::ostringstream ostr;
  ostr << "[";
  for(FunctionIdSet::const_iterator it = set.begin(); it != set.end(); ) {
    ostr << "<" << (*it).toString() << ", " << getFunctionNameFromFunctionId(*it)  << ">";
    it++;
    if(it != set.end()) {
      ostr << ", ";
    }
  }
  ostr << "]";
  return ostr.str();
}

std::string FunctionIdMapping::toString() const {
  ostringstream oss;
  toStream(oss);
  return oss.str();
}

FunctionIdMapping::FunctionIdSet FunctionIdMapping::getFunctionIdSet() const {
  FunctionIdSet set;
  for(map<SgSymbol*,size_t>::const_iterator i=mappingSymbolToFunctionId.begin();i!=mappingSymbolToFunctionId.end();++i) {
    size_t t=(*i).second;
    FunctionId id;
    id.setIdCode(t);
    set.insert(id);
  }
  return set;
}

void FunctionIdMapping::generateStmtSymbolDotEdge(std::ofstream& file, SgNode* node,FunctionId id) {
  file<<"_"<<node<<" "
      <<"->"
      <<id.toString()
      << endl;
}

string FunctionIdMapping::generateDotSgSymbol(SgSymbol* sym) {
  stringstream ss;
  ss<<"_"<<sym;
  return ss.str();
}

FunctionId FunctionIdMapping::getFunctionIdFromSymbol(SgSymbol* symbol) const {
  assert(symbol);
  FunctionId newFunctionId;
  if(mappingSymbolToFunctionId.count(symbol)) {
    // intentionally a friend action to avoid that users can create FunctionIds from int.
    newFunctionId._id = mappingSymbolToFunctionId.at(symbol);
  }
  else {
    ROSE_ASSERT(!newFunctionId.isValid());
  }
  return newFunctionId;
}

FunctionId FunctionIdMapping::getFunctionIdFromDeclaration(const SgDeclarationStatement* decl) const {
  return getFunctionIdFromSymbol(decl->search_for_symbol_from_symbol_table());
}

FunctionId FunctionIdMapping::getFunctionIdFromCode(int code) {
  FunctionId id;
  id.setIdCode(code);
  return id;
}

SgSymbol* FunctionIdMapping::getSymbolFromFunctionId(FunctionId id) const {
  ROSE_ASSERT(id.isValid());
  //ROSE_ASSERT(((size_t)id._id)<mappingFunctionIdToSymbol.size());
  if(id._id >= 0 && static_cast<size_t>(id._id) < mappingFunctionIdToSymbol.size()) {
    return mappingFunctionIdToSymbol[id._id];
  }
  else {
    return 0;
  }
}


string FunctionIdMapping::getFunctionNameFromFunctionId(FunctionId id) const {
  SgSymbol* sym=getSymbolFromFunctionId(id);
  return SgNodeHelper::symbolToString(sym);
}


string FunctionIdMapping::getUniqueShortNameFromFunctionId(FunctionId id) const {
  if(!isTemporaryFunctionId(id)) {
    if(!id.isValid())
      return "$invalidFunctionId";
    else
      return getFunctionNameFromFunctionId(id)+"_"+id.toString("");
  } else {
    return string("tmp")+"_"+id.toString("");
  }
}

void FunctionIdMapping::registerNewSymbol(SgSymbol* symbol) {
  ROSE_ASSERT(symbol);
  if(mappingSymbolToFunctionId.find(symbol)==mappingSymbolToFunctionId.end()) {
    // Sizes should match:
    ROSE_ASSERT(mappingSymbolToFunctionId.size() == mappingFunctionIdToSymbol.size());

    // Create new mapping entry:
    size_t newIdCode = mappingFunctionIdToSymbol.size();
    mappingSymbolToFunctionId[symbol] = newIdCode;
    mappingFunctionIdToSymbol.push_back(symbol);

    // Mapping in both directions must be possible:
    ROSE_ASSERT(mappingSymbolToFunctionId.at(mappingFunctionIdToSymbol[newIdCode]) == newIdCode);
    ROSE_ASSERT(mappingFunctionIdToSymbol[mappingSymbolToFunctionId.at(symbol)] == symbol);
  } else {
    cerr<< "Error: attempt to register existing symbol "<<symbol<<":"<<SgNodeHelper::symbolToString(symbol)<<endl;
    exit(1);
  }
}


bool FunctionIdMapping::isTemporaryFunctionId(FunctionId id) const {
  return dynamic_cast<UniqueTemporarySymbol*>(getSymbolFromFunctionId(id))!=0;
}


FunctionId FunctionIdMapping::createUniqueTemporaryFunctionId(string name) {
  for(TemporaryFunctionIdMapping::iterator i=temporaryFunctionIdMapping.begin();
      i!=temporaryFunctionIdMapping.end();
      ++i) {
    PairOfFunctionIdAndFunctionIdName id_name_pair=*i;
    if(id_name_pair.second==name) {
      // name for temporary id exists, return existing id
      return id_name_pair.first;
    }
  }
  // temporary with name 'name' does not exist yet, create, register, and return
  SgSymbol* sym=new UniqueTemporarySymbol(name);
  registerNewSymbol(sym);
  FunctionId newVarFunctionId=getFunctionIdFromSymbol(sym);
  temporaryFunctionIdMapping.insert(make_pair(newVarFunctionId,name));
  return newVarFunctionId;
}




// we use a function as a destructor may delete it multiple times
void FunctionIdMapping::deleteUniqueTemporaryFunctionId(FunctionId varFunctionId) {
  if(isTemporaryFunctionId(varFunctionId)) {
    // FIXME: dangling pointer!?
    delete getSymbolFromFunctionId(varFunctionId);
  }
  else
    throw CodeThorn::Exception("FunctionIdMapping::deleteUniqueTemporarySymbol: improper id operation.");
}


FunctionIdMapping::UniqueTemporarySymbol::UniqueTemporarySymbol(string name) : SgSymbol() {
  _tmpName=name;
}


SgName FunctionIdMapping::UniqueTemporarySymbol::get_name() const {
  return SgName(_tmpName);
}

SgType* FunctionIdMapping::UniqueTemporarySymbol::get_type() const {
  ROSE_ASSERT(false);
  return 0;
}

FunctionIdMapping::FunctionIdSet FunctionIdMapping::determineFunctionIdsOfDeclarations(set<const SgDeclarationStatement*> decls) const {
  FunctionIdMapping::FunctionIdSet resultSet;
  for(set<const SgDeclarationStatement*>::const_iterator i=decls.begin();i!=decls.end();++i) {
    resultSet.insert(getFunctionIdFromDeclaration(*i));
  }
  return resultSet;
}


std::string FunctionIdMapping::getStringFromFunctionId(FunctionId id) const {
  return id.toString();
}

SgFunctionDeclaration* FunctionIdMapping::getFunctionDeclaration(FunctionId funcFunctionId) const {
  SgSymbol* funcSym=getSymbolFromFunctionId(funcFunctionId);
  return SgNodeHelper::findFunctionDeclarationWithFunctionSymbol(funcSym);
}


FunctionId FunctionIdMapping::getFunctionIdFromFunctionRef(SgFunctionRefExp* functionRef) const {
  return getFunctionIdFromSymbol(functionRef->get_symbol());
}

FunctionId FunctionIdMapping::getFunctionIdFromFunctionRef(SgTemplateFunctionRefExp* functionRef) const {
  return getFunctionIdFromSymbol(functionRef->get_symbol());
}

FunctionId FunctionIdMapping::getFunctionIdFromFunctionRef(SgMemberFunctionRefExp* functionRef) const {
  return getFunctionIdFromSymbol(functionRef->get_symbol());
}

FunctionId FunctionIdMapping::getFunctionIdFromFunctionRef(SgTemplateMemberFunctionRefExp* functionRef) const {
  return getFunctionIdFromSymbol(functionRef->get_symbol());
}


void FunctionIdMapping::generateDot(string filename, SgNode* astRoot) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);

  myfile<<"digraph FunctionIdMapping {"<<endl;
  myfile<<"rankdir=LR"<<endl;

  // nodes
  for(size_t i=0;i<mappingFunctionIdToSymbol.size();++i) {
    myfile<<generateDotSgSymbol(mappingFunctionIdToSymbol[i])<<" [label=\""<<generateDotSgSymbol(mappingFunctionIdToSymbol[i])<<"\\n"
          <<"\\\""
          <<SgNodeHelper::symbolToString(mappingFunctionIdToSymbol[i])
          <<"\\\""
          <<"\""
          <<"]"
          <<endl;
  }
  myfile<<endl;
  // edges : sym->id
  for(size_t i=0;i<mappingFunctionIdToSymbol.size();++i) {
    myfile<<"_"<<mappingFunctionIdToSymbol[i]
          <<"->"
          <<getFunctionIdFromCode(i).toString()
          <<endl;
  }
  // edges: stmt/expr->sym
  
  RoseAst ast(astRoot);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgFunctionDeclaration* funcdecl=isSgFunctionDeclaration(*i)) {
      generateStmtSymbolDotEdge(myfile,funcdecl,getFunctionIdFromDeclaration(funcdecl));
    }
    if(SgFunctionRefExp* funcref=isSgFunctionRefExp(*i)) {
      generateStmtSymbolDotEdge(myfile,funcref,getFunctionIdFromFunctionRef(funcref));
    }
  }
  myfile<<"}"<<endl;
  myfile.close();
}

void FunctionIdMapping::computeFunctionSymbolMapping(SgProject* project) {

  // compute symbols of all function declarations
  set<SgSymbol*> symbolSet;
  list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
    RoseAst ast(*k);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(*i)) {
        SgFunctionSymbol* funcSymbol = SgNodeHelper::getSymbolOfFunctionDeclaration(funcDecl);
        ROSE_ASSERT(funcSymbol);
        if(symbolSet.find(funcSymbol) == symbolSet.end()) {
          registerNewSymbol(funcSymbol);
          symbolSet.insert(funcSymbol);
        }
      }
    }
  }

  // compute biderictional mapping FunctionDefinition<->FunctionSymbol
  std::list<SgFunctionDefinition*> funDefList=SgNodeHelper::listOfFunctionDefinitions(project);
  int nr=1;
  for(auto funDef : funDefList) {
    SgFunctionDeclaration* funDecl=funDef->get_declaration();
    if(!funDecl) {
      cerr<<"Error: FunctionIdMapping: function definition without declaration."<<endl;
      cerr<<"funDef: "<<funDef->unparseToString()<<endl;
      exit(1);
    }
    SAWYER_MESG(logger[TRACE])<<"Processing function def "<<nr<<": "<<SgNodeHelper::getFunctionName(funDef);
    // build bi-directional mapping
    FunctionId funDefFunId=getFunctionIdFromDeclaration(funDecl);
    ROSE_ASSERT(funDefFunId.isValid());
    SAWYER_MESG(logger[TRACE])<<" FunctionId:" << funDefFunId.toString();
    auto funDefIter=mappingFunctionDefToFunctionId.find(funDef);
    if(funDefIter!=mappingFunctionDefToFunctionId.end()) {
      // ensure that existing mapping is the same
      ROSE_ASSERT((*funDefIter).second==funDefFunId);
    } else {
      mappingFunctionDefToFunctionId[funDef]=funDefFunId;
    }

    auto funIdIter=mappingFunctionIdToFunctionDef.find(funDefFunId);
    if(funIdIter!=mappingFunctionIdToFunctionDef.end()) {
      // ensure that existing mapping is the same
      if((*funIdIter).second==0) {
        mappingFunctionIdToFunctionDef[funDefFunId]=funDef;
        SAWYER_MESG(logger[TRACE])<<" setting FunctionId to def: "<<funDef<<endl;
      } else {
        SAWYER_MESG(logger[TRACE])<<" Problem: FunctionId maps to more than one def: ";
        SAWYER_MESG(logger[TRACE])<<"FunctionId1: "<<(*funIdIter).first.toString()<<" ==>" <<(*funIdIter).second << "|" << funDef<<endl;
#if 0
        cout<<"- 1 ------------------"<<endl;
        cout<<(*funIdIter).second->unparseToString()<<endl;
        cout<<"- 2 ------------------"<<endl;
        cout<<funDef->unparseToString()<<endl;
        cout<<"----------------------"<<endl;
#endif
        //ROSE_ASSERT((*funIdIter).second==funDef);
      }
    } else {
      mappingFunctionIdToFunctionDef[funDefFunId]=funDef;
      SAWYER_MESG(logger[TRACE])<<" setting FunctionId to def: "<<funDef<<endl;
    }
    nr++;
  }

  // compute mapping for each functionref
  computeFunctionCallMapping(project);

  //cout<<"STATUS: computeFunctionSymbolMapping: done."<<endl;
  return;
}

FunctionIdMapping::FunctionIdSet FunctionIdMapping::getFunctionIdsOfFunctionDeclarations(set<SgFunctionDeclaration*> funcDecls) {
  FunctionIdMapping::FunctionIdSet resultSet;
  for(set<SgFunctionDeclaration*>::iterator i=funcDecls.begin();i!=funcDecls.end();++i) {
    SgSymbol* sym=SgNodeHelper::getSymbolOfFunctionDeclaration(*i);
    if(sym) {
      resultSet.insert(getFunctionIdFromSymbol(sym));
    }
  }
  return resultSet;
}

FunctionIdMapping::FunctionIdSet FunctionIdMapping::getFunctionIdsOfAstSubTree(SgNode* node) {
  FunctionIdSet idSet;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    FunctionId id; // default creates intentionally an invalid id.
    if(SgFunctionDeclaration* funcDecl=isSgFunctionDeclaration(*i)) {
      id=getFunctionIdFromDeclaration(funcDecl);
    } else if(SgFunctionRefExp* funcRefExp=isSgFunctionRefExp(*i)) {
      id=getFunctionIdFromFunctionRef(funcRefExp);
    }
    if(id.isValid()) {
      idSet.insert(id);
    }
  }
  return idSet;
}

void FunctionIdMapping::computeFunctionCallMapping(SgProject* project) {
  std::list<SgFunctionDefinition*> funDefList=SgNodeHelper::listOfFunctionDefinitions(project);
  RoseAst ast(project);
  for(auto node:ast) {
    if(SgFunctionRefExp* functionRefExp=isSgFunctionRefExp(node)) {
      FunctionCallTarget funCallTarget;

      // case 1: function id mapping can resolve it
      FunctionId funId=getFunctionIdFromFunctionRef(functionRefExp);
      if(funId.isValid()) {
        if(SgFunctionDefinition* funDef=getFunctionDefFromFunctionId(funId)) {
          funCallTarget.setDefinition(funDef);
          funCallTarget.setDeclaration(funDef->get_declaration());
        }
      }
      functionCallMapping[functionRefExp].push_back(funCallTarget);
    } else if(isSgTemplateFunctionRefExp(node)
              ||isSgMemberFunctionRefExp(node)
              ||isSgTemplateMemberFunctionRefExp(node)) {
      cerr<<"WARNING: "<<SgNodeHelper::sourceFilenameLineColumnToString(node)<<": unsupported kind of function ref exp: "<<node->unparseToString()<<endl;
    }
  }
}

SgFunctionDefinition* FunctionIdMapping::resolveFunctionRef(SgFunctionRefExp* funRefExp) {

  // this is guaranteed since all funRefExps are supposed to be in the map
  ROSE_ASSERT(functionCallMapping.find(funRefExp)!=functionCallMapping.end()); 

  // function definition can be 0
  if(functionCallMapping[funRefExp].size()==1) {
    return functionCallMapping[funRefExp][0].getDefinition();
  } else if(functionCallMapping[funRefExp].size()>=2) {
    cout<<"WARNING: multiple targets in function resolution."<<endl;
    return functionCallMapping[funRefExp][0].getDefinition();
  }
  return 0;
}

void FunctionIdMapping::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::FunctionIdMapping", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}
