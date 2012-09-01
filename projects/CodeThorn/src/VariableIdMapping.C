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
		found=true;
	  }
	  if(SgVarRefExp* varRef=isSgVarRefExp(*i)) {
		sym=SgNodeHelper::getSymbolOfVariable(varRef);
		found=true;
	  }
	  if(found) {
		string longName=SgNodeHelper::uniqueLongVariableName(*i);
		MapPair pair=make_pair(longName,sym);
		checkSet.insert(pair);
	  }
	}
  }
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
  if(!isTemporaryVariableId(varId)) 
	return SgNodeHelper::uniqueLongVariableName(getSymbol(varId));
  else
	return "$$$tmp"+variableName(varId);
}

VariableId VariableIdMapping::variableId(SgVariableDeclaration* decl) {
  return VariableId(SgNodeHelper::getSymbolOfVariableDeclaration(decl));
}

VariableId VariableIdMapping::variableId(SgVarRefExp* varRefExp) {
  return VariableId(SgNodeHelper::getSymbolOfVariable(varRefExp));
}

bool VariableIdMapping::isTemporaryVariableId(VariableId varId) {
  return dynamic_cast<UniqueTemporaryVariableSymbol*>(varId.getSymbol());
}

SgSymbol* VariableIdMapping::getSymbol(VariableId varId) {
  return varId.getSymbol();
}

SgSymbol* 
VariableIdMapping::createUniqueTemporaryVariableId(string name) {
  return new UniqueTemporaryVariableSymbol(name);
}

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
SgSymbol* VariableId::getSymbol() {
  return sym;
}
