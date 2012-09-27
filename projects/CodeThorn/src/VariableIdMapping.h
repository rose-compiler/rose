#ifndef VARIABLEIDMAPPING_H
#define VARIABLEIDMAPPING_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "rose.h"
#include <string>
#include "SgNodeHelper.h"
#include "MyAst.h"

using namespace std;

class VariableId;
typedef string VariableName;

class VariableIdMapping {

 public:
  // the computation of the CodeThorn-defined ROSE-based variable-symbol mapping
  // creates a mapping of variableNames and its computed UniqueVariableSymbol
  void computeVariableSymbolMapping(SgProject* project);

  // checks whether the computed CodeThorn-defined ROSE-based variable-symbol mapping is bijective.
  bool isUniqueVariableSymbolMapping();

  void reportUniqueVariableSymbolMappingViolations();

  /* create a new unique variable symbol (should be used together with deleteUniqueVariableSymbol)
	 this is useful if additional (e.g. temporary) variables are introduced in an analysis
	 this function does NOT insert this new symbol in any symbol table
   */
  VariableId createUniqueTemporaryVariableId(string name);

  // delete a unique variable symbol (should be used together with createUniqueVariableSymbol)
  void deleteUniqueTemporaryVariableId(VariableId uniqueVarSym);

  class UniqueTemporaryVariableSymbol : public SgVariableSymbol {
  public:
	// Constructor: we only allow this single constructor
	UniqueTemporaryVariableSymbol(string name);
	// Destructor: default is sufficient
	
	// overrides inherited get_name (we do not use a declaration)
	SgName get_name() const;
  private:
	string _tmpName;
  };

  VariableId variableId(SgVariableDeclaration* decl);
  VariableId variableId(SgVarRefExp* varRefExp);
  VariableId variableId(SgInitializedName* initName);
  bool isTemporaryVariableId(VariableId varId);
  SgSymbol* getSymbol(VariableId varId);
  string variableName(VariableId varId);
  string uniqueLongVariableName(VariableId varId);

 private:
  typedef pair<string,SgSymbol*> MapPair;
  set<MapPair> checkSet;
  typedef pair<VariableId,VariableName> PairOfVarIdAndVarName;
  typedef set<PairOfVarIdAndVarName> TemporaryVariableIdMapping;
  TemporaryVariableIdMapping temporaryVariableIdMapping;
}; // end of class VariableIdMapping

class VariableId {
  friend class VariableIdMapping;
  friend bool operator<(VariableId id1, VariableId id2);
  friend bool operator==(VariableId id1, VariableId id2);
  friend class ConstraintSetHashFun; // TODO: investigate why getSymbol needs to be public
 public:
  VariableId();
  string toString() const;
  string variableName() const;
  string longVariableName() const;
  VariableId(SgSymbol* sym);
 public:
  SgSymbol* getSymbol() const; // only public because of ContraintSetHashFun
 private: 
  SgSymbol* sym;
};


bool operator<(VariableId id1, VariableId id2);
bool operator==(VariableId id1, VariableId id2);
bool operator!=(VariableId id1, VariableId id2);

#endif

