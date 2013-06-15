/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "VariableIdMapping.h"

using namespace CodeThorn;

void VariableIdMapping::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToSym.size();++i) {
	os<<"("<<i
	  <<","<<mappingVarIdToSym[i]
	  <<","<<SgNodeHelper::symbolToString(mappingVarIdToSym[i])
	  <<" ::: "<<SgNodeHelper::uniqueLongVariableName(mappingVarIdToSym[i])
	  <<")"<<endl;
	assert(mappingSymToVarId[mappingVarIdToSym[i]]==i);
  }
}

VariableId VariableIdMapping::variableId(SgSymbol* sym) {
  assert(sym);
  VariableId newId;
  // intentionally a friend action to avoid that users can create VariableIds from int.
  newId._id=mappingSymToVarId[sym]; 
  return newId;
}

SgSymbol* VariableIdMapping::getSymbol(VariableId varid) {
  return mappingVarIdToSym[varid._id];
}
//SgSymbol* VariableIdMapping::getSymbol(VariableId varId) {
//  return varId.getSymbol();
//}

void VariableIdMapping::computeVariableSymbolMapping(SgProject* project) {
  set<SgSymbol*> symbolSet;
  list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
	RoseAst ast(*k);
	for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	  SgSymbol* sym=0;
	  bool found=false;
	  if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
		sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
		if(sym)
		  found=true;
		else
		  cerr<<"WARNING: computeVariableSymbolMapping: VariableDeclaration without associated symbol found. Ignoring.";
		assert(!isSgVariableDefinition(sym));
	  }
	  if(SgVarRefExp* varRef=isSgVarRefExp(*i)) {
		sym=SgNodeHelper::getSymbolOfVariable(varRef);
		if(sym)
		  found=true;
		else
		  cerr<<"WARNING: computeVariableSymbolMapping: VarRefExp without associated symbol found. Ignoring.";
		assert(!isSgVariableDefinition(sym));
	  }
	  if(found) {
		//cout << "INFO: var: "<<SgNodeHelper::symbolToString(sym)<<endl;
		// currently not possible because variables in forward declarations are not represented by a SgSymbol (only as SgName).
#if 0
		if(SgNodeHelper::isVariableSymbolInFunctionForwardDeclaration(sym)) {
		  // special case: we exclude variables in forward declarations to be used in variable id mappings.
		  cout << "INFO: ignoring variable in forward declaration: "<<SgNodeHelper::symbolToString(sym)<<endl;
		  continue;
		}
#endif		
		string longName=SgNodeHelper::uniqueLongVariableName(sym);
		
		// ensure all symbols are SgVariableSymbol
		SgVariableSymbol* finalvarsym=isSgVariableSymbol(sym);
		assert(finalvarsym);
		MapPair pair=make_pair(longName,finalvarsym);
		checkSet.insert(pair);
		if(symbolSet.find(finalvarsym)==symbolSet.end()) {
		  assert(finalvarsym);
		  registerNewSymbol(finalvarsym);
		  symbolSet.insert(finalvarsym);
		}
	  }
	}
  }
  cout << "STATUS: computeVariableSymbolMapping: done."<<endl;
  return;
}

// Check if computed variable symbol mapping is bijective
bool VariableIdMapping::isUniqueVariableSymbolMapping() {
  size_t numOfPairs=checkSet.size();
  set<string> nameSet;
  set<SgSymbol*> symbolSet;
  bool mappingOK=true;
  for(set<MapPair>::iterator i=checkSet.begin();i!=checkSet.end();++i) {
	nameSet.insert((*i).first);
	symbolSet.insert((*i).second);
  }
  return ((nameSet.size()==numOfPairs) && (symbolSet.size()==numOfPairs)&& mappingOK);
}

void VariableIdMapping::reportUniqueVariableSymbolMappingViolations() {
  // in case of an error we perform some expensive operations to provide a proper error explanation
  if(!isUniqueVariableSymbolMapping()) {
	for(set<MapPair>::iterator i=checkSet.begin();i!=checkSet.end();++i) {
	  for(set<MapPair>::iterator j=checkSet.begin();j!=checkSet.end();++j) {
		// check if we find a pair with same name but different symbol
		if((*i).first==(*j).first && i!=j) {
		  cout << "Problematic mapping: same name  : "<<(*i).first <<" <-> "<<(*i).second
			   << " <==> "
			   <<(*j).first <<" <-> "<<(*j).second
			   <<endl;
		  // look up declaration and print

		  SgVariableSymbol* varsym1=isSgVariableSymbol((*i).second);
		  SgVariableSymbol* varsym2=isSgVariableSymbol((*j).second);
		  SgDeclarationStatement* vardecl1=SgNodeHelper::findVariableDeclarationWithVariableSymbol(varsym1);
		  SgDeclarationStatement* vardecl2=SgNodeHelper::findVariableDeclarationWithVariableSymbol(varsym2);
		  assert(vardecl1);
		  assert(vardecl2);
		  string lc1=SgNodeHelper::sourceFilenameLineColumnToString(vardecl1);
		  string lc2=SgNodeHelper::sourceFilenameLineColumnToString(vardecl2);
		  cout << "  VarSym1:"<<varsym1  <<" Decl1:"<<lc1<<" @"<<vardecl1<<": "<<vardecl1->unparseToString()<< endl;
		  cout << "  VarSym2:"<<varsym2  <<" Decl2:"<<lc2<< " @"<<vardecl2<<": "<<vardecl2->unparseToString()<< endl;
		  cout << "------------------------------------------------------------------"<<endl;
		}
		if((*i).second==(*j).second && i!=j) {
		  cout << "Problematic mapping: same symbol: "<<(*i).first <<" <-> "<<(*i).second
			   << " <==> "
			   <<(*j).first <<" <-> "<<(*j).second
			   <<endl;
		}
	  }
	}
  } else {
	// no violations to report
  }
}

string VariableIdMapping::variableName(VariableId varId) {
  SgSymbol* sym=getSymbol(varId);
  return SgNodeHelper::symbolToString(sym);
}

string VariableIdMapping::uniqueLongVariableName(VariableId varId) {
  if(!isTemporaryVariableId(varId)) {
	return variableName(varId);
	//return SgNodeHelper::uniqueLongVariableName(getSymbol(varId));
  } else {
	return "$$$tmp"+variableName(varId);
  }
}

VariableId VariableIdMapping::variableId(SgVariableDeclaration* decl) {
  return variableId(SgNodeHelper::getSymbolOfVariableDeclaration(decl));
}

VariableId VariableIdMapping::variableId(SgVarRefExp* varRefExp) {
  return variableId(SgNodeHelper::getSymbolOfVariable(varRefExp));
}

VariableId VariableIdMapping::variableId(SgInitializedName* initName) {
  assert(initName);
  return variableId(SgNodeHelper::getSymbolOfInitializedName(initName));
}

bool VariableIdMapping::isTemporaryVariableId(VariableId varId) {
  return dynamic_cast<UniqueTemporaryVariableSymbol*>(getSymbol(varId));
}

VariableId
VariableIdMapping::createUniqueTemporaryVariableId(string name) {
  for(TemporaryVariableIdMapping::iterator i=temporaryVariableIdMapping.begin();
	  i!=temporaryVariableIdMapping.end();
	  ++i) {
	PairOfVarIdAndVarName id_name_pair=*i;
	if(id_name_pair.second==name) {
	  // name for temporary variable exists, return existing id
	  return id_name_pair.first;
	}
  }
  // temporary variable with name 'name' does not exist yet, create, register, and return
  SgSymbol* sym=new UniqueTemporaryVariableSymbol(name);
  registerNewSymbol(sym);
  VariableId newVarId=variableId(sym);
  temporaryVariableIdMapping.insert(make_pair(newVarId,name));
  return newVarId;
}

void VariableIdMapping::registerNewSymbol(SgSymbol* sym) {
  if(mappingSymToVarId.find(sym)==mappingSymToVarId.end()) {
	mappingSymToVarId[sym]=mappingVarIdToSym.size();
	mappingVarIdToSym.push_back(sym);
  } else {
	cerr<< "Error: attempt to register existing symbol "<<sym<<":"<<SgNodeHelper::symbolToString(sym)<<endl;
	exit(1);
  }
}

// we use a function as a destructor may delete it multiple times
void VariableIdMapping::deleteUniqueTemporaryVariableId(VariableId varId) {
  if(isTemporaryVariableId(varId))
	delete getSymbol(varId);
  else
	throw "VariableIdMapping::deleteUniqueTemporaryVariableSymbol: improper id operation.";
}

VariableIdMapping::UniqueTemporaryVariableSymbol::UniqueTemporaryVariableSymbol(string name) : SgVariableSymbol() {
  _tmpName=name;
}

SgName VariableIdMapping::UniqueTemporaryVariableSymbol::get_name() const {
  return SgName(_tmpName);
}

VariableId::VariableId():_id(-1){
}
//VariableId::VariableId(int id):_id(id){
//}

string
VariableId::toString() const {
  stringstream ss;
  ss<<"V"<<_id;
  return ss.str();
}

#if 0
VariableId::VariableId(SgSymbol* sym):sym(sym){
}
SgSymbol* VariableId::getSymbol() const {
  return sym;
}

string
VariableId::variableName() const {
  SgSymbol* sym=getSymbol();
  if(sym==0) return "id-no-var";
  //stringstream ss;
  //ss<<sym<<":"<<SgNodeHelper::symbolToString(sym);
  //return ss.str();
  return SgNodeHelper::symbolToString(sym);
}

string
VariableId::longVariableName() const {
  SgSymbol* sym=getSymbol();
  if(sym==0) return "id-no-var";
  // TODO: MS: long names do not work with SgNodehelper from SgSymbol. We can only support this with precomputed VariableIdMappings (we do not want to use mangled names)
  return variableName();
}
#endif

bool CodeThorn::operator<(VariableId id1, VariableId id2) {
  return id1._id<id2._id;
}
bool CodeThorn::operator==(VariableId id1, VariableId id2) {
  return id1._id==id2._id;
}
bool CodeThorn::operator!=(VariableId id1, VariableId id2) {
  return !(id1==id2);
}
