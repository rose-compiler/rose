#include "rose.h"
#include <string>
#include "SgNodeHelper.h"
#include "MyAst.h"

using namespace std;

#ifndef VARIABLEIDMAPPING_H
#define VARIABLEIDMAPPING_H


typedef SgSymbol* VariableId;

class VariableIdMapping {
 public:
  // the computation of the CodeThorn-defined ROSE-based variable-symbol mapping
  // creates a mapping of variableNames and its computed UniqueVariableSymbol
  // the mapping must be bijective
  void computeUniqueVariableSymbolMapping(SgProject* project);

  // checks whether the computed CodeThorn-defined ROSE-based variable-symbol mapping is bijective.
  bool isUniqueVariableSymbolMapping();

  void reportUniqueVariableSymbolMappingViolations();

  /* create a new unique variable symbol (should be used together with deleteUniqueVariableSymbol)
	 this is useful if additional (e.g. temporary) variables are introduced in an analysis
	 this function does NOT insert this new symbol in any symbol table
   */
  SgSymbol* createUniqueTemporaryVariableId(string name);

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
  VariableId variableId(SgVarRefExp* decl);
  bool isTemporaryVariableId(VariableId varId);
  SgSymbol* getSymbol(VariableId varId);
  string variableName(VariableId varId);
  string uniqueLongVariableName(VariableId varId);

 private:
  typedef pair<string,SgSymbol*> MapPair;
  set<MapPair> checkSet;

}; // end of class VariableIdMapping

#endif

