#ifndef VARIABLEIDMAPPING_H
#define VARIABLEIDMAPPING_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <string>
#include <map>
#include <vector>
#include <boost/unordered_set.hpp>

// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "sage3basic.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"

using namespace std;

  class VariableId;
  typedef string VariableName;
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class VariableIdMapping {
  /* TODO: possible workaround: because the AST implementation is not completed for the following cases:
     1) SgInitializedName in forward declaration (symbol=0)
     2) CtorInitializerList (symbol=0)
     the symbol is missing in both cases, a VariableId can be assign to the passed SgInitializedName pointer.
  */

 public:
  VariableIdMapping();
  //typedef boost::unordered_set<VariableId> VariableIdSet;
  typedef set<VariableId> VariableIdSet;

  // the computation of the ROSE-based variable-symbol mapping
  // creates a mapping of variableNames and its computed UniqueVariableSymbol
  void computeVariableSymbolMapping(SgProject* project);

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

  typedef size_t VarId;
  VariableId variableId(SgVariableDeclaration* decl);
  VariableId variableId(SgVarRefExp* varRefExp);
  VariableId variableId(SgInitializedName* initName);
  VariableId variableId(SgSymbol* sym);
  VariableId variableIdFromCode(int);
  VariableId variableIdOfArrayElement(VariableId arrayVar, int elemIndex);
  SgSymbol* getSymbol(VariableId varId);
  SgType* getType(VariableId varId);
  // returns true if this variable has any signed or unsigned integer type (short,int,long,longlong)
  bool hasIntegerType(VariableId varId);
  // returns true if this variable has any floating-point type (float,double,longdouble)
  bool hasFloatingPointType(VariableId varId);
  bool hasPointerType(VariableId varId);
  bool hasArrayType(VariableId varId);
  SgVariableDeclaration* getVariableDeclaration(VariableId varId);
  bool isTemporaryVariableId(VariableId varId);
  string variableName(VariableId varId);
  string uniqueLongVariableName(VariableId varId);
  string uniqueShortVariableName(VariableId varId);

  void registerNewSymbol(SgSymbol* sym);
  void registerNewArraySymbol(SgSymbol* sym, int arraySize);
  void toStream(ostream& os);
  void generateDot(string filename,SgNode* astRoot);

  VariableIdSet getVariableIdSet();

  VariableIdSet determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> varDecls);
  VariableIdSet determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList);
  VariableIdSet variableIdsOfAstSubTree(SgNode* node);

  /* if this mode is activated variable ids are created for each element of arrays with fixed size
     e.g. a[3] gets assigned 3 variable-ids (where the first one denotes a[0])
     this mode must be set before the mapping is computed with computeVariableSymbolMapping
  */
  void setModeVariableIdForEachArrayElement(bool active) { ROSE_ASSERT(mappingVarIdToSym.size()==0); modeVariableIdForEachArrayElement=active; }
  SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar);

 private:
  void generateStmtSymbolDotEdge(std::ofstream&, SgNode* node,VariableId id);
  string generateDotSgSymbol(SgSymbol* sym);
  typedef pair<string,SgSymbol*> MapPair;
  typedef pair<VariableId,VariableName> PairOfVarIdAndVarName;
  typedef set<PairOfVarIdAndVarName> TemporaryVariableIdMapping;
  TemporaryVariableIdMapping temporaryVariableIdMapping;
  VariableId addNewSymbol(SgSymbol* sym);
  // used for mapping in both directions
  vector<SgSymbol*> mappingVarIdToSym;
  map<SgSymbol*,size_t> mappingSymToVarId;
  bool modeVariableIdForEachArrayElement;
}; // end of class VariableIdMapping

 typedef VariableIdMapping::VariableIdSet VariableIdSet;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class VariableId {
  friend class VariableIdMapping;
  friend bool operator<(VariableId id1, VariableId id2);
  friend bool operator==(VariableId id1, VariableId id2);
  friend class ConstraintSetHashFun;
  //friend size_t hash_value(const VariableId&);
 public:
  VariableId();
  string toString() const;
  int getIdCode() const { return _id; }
  // we intentionally do not provide a constructor for int because this would clash 
  // with overloaded functions that are using ConstIntLattice (which has an implicit 
  // type conversion for int)
  void setIdCode(int id) {_id=id;}
  //string variableName() const;
  //string longVariableName() const;
  //VariableId(SgSymbol* sym);
  bool isValid() { return _id!=-1; }
 public:
  //SgSymbol* getSymbol() const; // only public because of ContraintSetHashFun

 private: 
  //SgSymbol* sym;
  int _id;
};

 size_t hash_value(const VariableId& vid);

bool operator<(VariableId id1, VariableId id2);
bool operator==(VariableId id1, VariableId id2);
bool operator!=(VariableId id1, VariableId id2);
VariableIdSet& operator+=(VariableIdSet& s1, VariableIdSet& s2);

#endif
