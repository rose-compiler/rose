/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "VariableIdMapping.h"

void VariableIdMapping::computeVariableSymbolMapping(SgProject* project) {

  list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
	MyAst ast(*k);
	for(MyAst::iterator i=ast.begin();i!=ast.end();++i) {
	  SgSymbol* sym=0;
	  bool found=false;
	  if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
		sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
		if(sym)
		  found=true;
		else
		  ;//cerr<<"WARNING: ROSE-AST ERROR: VariableDeclaration without associated symbol found. Ignoring.";
	  }
	  if(SgVarRefExp* varRef=isSgVarRefExp(*i)) {
		sym=SgNodeHelper::getSymbolOfVariable(varRef);
		if(sym)
		  found=true;
		else
		  ;//cerr<<"WARNING: ROSE-AST ERROR: VarRefExp without associated symbol found. Ignoring.";
	  }
	  if(found) {
		cout << "Found:"<<SgNodeHelper::symbolToString(sym)<<endl;
		string longName=SgNodeHelper::uniqueLongVariableName(*i);
		MapPair pair=make_pair(longName,sym);
		checkSet.insert(pair);
	  }
	}
  }
  cout << "STATUS: computeVariableSymbolMapping: done."<<endl;
  return;
}

// Check if computed variable symbol mapping is bijective
bool VariableIdMapping::isUniqueVariableSymbolMapping() {
  int numOfPairs=checkSet.size();
  set<string> nameSet;
  set<SgSymbol*> symbolSet;
  bool mappingOK=true;
  for(set<MapPair>::iterator i=checkSet.begin();i!=checkSet.end();++i) {
	nameSet.insert((*i).first);
	symbolSet.insert((*i).second);
  }
  return ((nameSet.size()==checkSet.size()) && (symbolSet.size()==checkSet.size())&& mappingOK);
}

void VariableIdMapping::reportUniqueVariableSymbolMappingViolations() {
  // in case of an error we perform some expensive operations to provide a proper error explanation
  if(!isUniqueVariableSymbolMapping()) {
	for(set<MapPair>::iterator i=checkSet.begin();i!=checkSet.end();++i) {
	  for(set<MapPair>::iterator j=checkSet.begin();j!=checkSet.end();++j) {
		// check if we find a pair with same name but different symbol
		if((*i).first==(*j).first && i!=j) {
		  cout << "    Problematic mapping: same name  : "<<(*i).first <<" <-> "<<(*i).second
			   << " <==> "
			   <<(*j).first <<" <-> "<<(*j).second
			   <<endl;
		}
		if((*i).second==(*j).second && i!=j) {
		  cout << "    Problematic mapping: same symbol: "<<(*i).first <<" <-> "<<(*i).second
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
	variableName(varId);
	//return SgNodeHelper::uniqueLongVariableName(getSymbol(varId));
  } else {
	return "$$$tmp"+variableName(varId);
  }
}

VariableId VariableIdMapping::variableId(SgVariableDeclaration* decl) {
  return VariableId(SgNodeHelper::getSymbolOfVariableDeclaration(decl));
}

VariableId VariableIdMapping::variableId(SgVarRefExp* varRefExp) {
  return VariableId(SgNodeHelper::getSymbolOfVariable(varRefExp));
}

VariableId VariableIdMapping::variableId(SgInitializedName* initName) {
  return VariableId(SgNodeHelper::getSymbolOfInitializedName(initName));
}

bool VariableIdMapping::isTemporaryVariableId(VariableId varId) {
  return dynamic_cast<UniqueTemporaryVariableSymbol*>(varId.getSymbol());
}

SgSymbol* VariableIdMapping::getSymbol(VariableId varId) {
  return varId.getSymbol();
}

VariableId
VariableIdMapping::createUniqueTemporaryVariableId(string name) {
  for(TemporaryVariableIdMapping::iterator i=temporaryVariableIdMapping.begin();
	  i!=temporaryVariableIdMapping.end();
	  ++i) {
	PairOfVarIdAndVarName id_name=*i;
	if(id_name.second==name) {
	  // name for temporary variable exists, return existing id
	  return id_name.first;
	}
  }
  // temporary variable with name 'name' does not exist yet, create, register, and return
  VariableId newVarId=VariableId(new UniqueTemporaryVariableSymbol(name));
  temporaryVariableIdMapping.insert(make_pair(newVarId,name));
  return newVarId;
}

// we use a function as a destructor may delete it multiple times
void VariableIdMapping::deleteUniqueTemporaryVariableId(VariableId varId) {
  if(isTemporaryVariableId(varId.getSymbol()))
	delete varId.getSymbol();
  else
	throw "VariableIdMapping::deleteUniqueTemporaryVariableSymbol: improper id operation.";
}

VariableIdMapping::UniqueTemporaryVariableSymbol::UniqueTemporaryVariableSymbol(string name) : SgVariableSymbol() {
  _tmpName=name;
}

SgName VariableIdMapping::UniqueTemporaryVariableSymbol::get_name() const {
  return SgName(_tmpName);
}

VariableId::VariableId():sym(0){
}
VariableId::VariableId(SgSymbol* sym):sym(sym){
}
SgSymbol* VariableId::getSymbol() const {
  return sym;
}

string
VariableId::variableName() const {
  SgSymbol* sym=getSymbol();
  return SgNodeHelper::symbolToString(sym);
}

string
VariableId::longVariableName() const {
  SgSymbol* sym=getSymbol();
  // TODO: MS: long names do not work with SgNodehelper from SgSymbol. We can only support this with precomputed VariableIdMappings (we do not want to use mangled names)
  return SgNodeHelper::symbolToString(sym);
}

bool operator<(VariableId id1, VariableId id2) {
  return id1.getSymbol()<id2.getSymbol();
}
bool operator==(VariableId id1, VariableId id2) {
  return id1.getSymbol()==id2.getSymbol();
}
