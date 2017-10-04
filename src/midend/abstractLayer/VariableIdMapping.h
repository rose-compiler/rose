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

#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "TypeSizeMapping.h"

namespace SPRAY {

class VariableId;
typedef std::string VariableName;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class VariableIdMapping {
  /* NOTE: cases where the symbol is in the ROSE AST:
     1) SgInitializedName in forward declaration (symbol=0)
     2) CtorInitializerList (symbol=0)
     the symbol is missing in both cases, a VariableId can be assign to the passed SgInitializedName pointer.
  */

 public:
  VariableIdMapping();
  typedef std::set<VariableId> VariableIdSet;

  /*
    create the mapping between symbols in the AST and associated
    variable-ids. Each variable in the project is assigned one
    variable-id (including global variables, local variables,
    class/struct/union data members)
  */    
  void computeVariableSymbolMapping(SgProject* project);

  /* create a new unique variable symbol (should be used together with
     deleteUniqueVariableSymbol) this is useful if additional
     (e.g. temporary) variables are introduced in an analysis this
     function does NOT insert this new symbol in any symbol table
   */
  VariableId createUniqueTemporaryVariableId(std::string name);
  bool isTemporaryVariableId(VariableId varId);
  bool isHeapMemoryRegionId(VariableId varId);

  // delete a unique variable symbol (should be used together with createUniqueVariableSymbol)
  void deleteUniqueTemporaryVariableId(VariableId uniqueVarSym);

  class UniqueTemporaryVariableSymbol : public SgVariableSymbol {
  public:
    UniqueTemporaryVariableSymbol(std::string name);
    // Destructor: default is sufficient
    
    // overrides inherited get_name (we do not use a declaration)
    SgName get_name() const;
  private:
    std::string _tmpName;
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
  // schroder3 (2016-07-05): Returns whether the given variable is a reference variable
  bool hasReferenceType(VariableId varId);
  // returns true if this variable has any signed or unsigned integer type (short,int,long,longlong)
  bool hasIntegerType(VariableId varId);
  // returns true if this variable has any floating-point type (float,double,longdouble)
  bool hasFloatingPointType(VariableId varId);
  bool hasPointerType(VariableId varId);
  bool hasArrayType(VariableId varId);
  bool hasClassType(VariableId varId);
  SgVariableDeclaration* getVariableDeclaration(VariableId varId);
  // schroder3 (2016-07-05): Returns whether the given variable is valid in this mapping
  bool isVariableIdValid(VariableId varId);
  std::string variableName(VariableId varId);
  std::string uniqueLongVariableName(VariableId varId);
  std::string uniqueShortVariableName(VariableId varId);

  // set number of elements of the memory region determined by this variableid
  void setNumberOfElements(VariableId variableId, size_t size);
  // get number of elements of the memory region determined by this variableid
  size_t getNumberOfElements(VariableId variableId);

  // set the size of an element of the memory region determined by this variableid
  void setElementSize(VariableId variableId, size_t size);
  // get the size of an element of the memory region determined by this variableid
  size_t getElementSize(VariableId variableId);

  SgSymbol* createAndRegisterNewSymbol(std::string name);
  SPRAY::VariableId createAndRegisterNewVariableId(std::string name);
  SPRAY::VariableId createAndRegisterNewMemoryRegion(std::string name, int regionSize);
  void registerNewSymbol(SgSymbol* sym);
  void registerNewArraySymbol(SgSymbol* sym, int arraySize);
  void toStream(std::ostream& os);
  void generateDot(std::string filename,SgNode* astRoot);

  VariableIdSet getVariableIdSet();

  VariableIdSet determineVariableIdsOfVariableDeclarations(std::set<SgVariableDeclaration*> varDecls);
  VariableIdSet determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList);
  VariableIdSet variableIdsOfAstSubTree(SgNode* node);

  /* if this mode is activated variable ids are created for each element of arrays with fixed size
     e.g. a[3] gets assigned 3 variable-ids (where the first one denotes a[0])
     this mode must be set before the mapping is computed with computeVariableSymbolMapping
  */
  void setModeVariableIdForEachArrayElement(bool active) { ROSE_ASSERT(mappingVarIdToSym.size()==0); modeVariableIdForEachArrayElement=active; }
  bool getModeVariableIdForEachArrayElement() { return modeVariableIdForEachArrayElement; }
  SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar);
  size_t getArrayDimensions(SgArrayType* t, std::vector<size_t> *dimensions = NULL);
  size_t getArrayElementCount(SgArrayType* t);
  size_t getArrayDimensionsFromInitializer(SgAggregateInitializer* init, std::vector<size_t> *dimensions = NULL);
  VariableId idForArrayRef(SgPntrArrRefExp* ref);
 private:
  void generateStmtSymbolDotEdge(std::ofstream&, SgNode* node,VariableId id);
  std::string generateDotSgSymbol(SgSymbol* sym);
  typedef std::pair<std::string,SgSymbol*> MapPair;
  typedef std::pair<VariableId,VariableName> PairOfVarIdAndVarName;
  typedef std::set<PairOfVarIdAndVarName> TemporaryVariableIdMapping;
  TemporaryVariableIdMapping temporaryVariableIdMapping;
  VariableId addNewSymbol(SgSymbol* sym);

  // used for mapping in both directions
  std::vector<SgSymbol*> mappingVarIdToSym;
  std::map<size_t,size_t> mappingVarIdToNumberOfElements;
  std::map<size_t,size_t> mappingVarIdToElementSize;
  std::map<SgSymbol*,size_t> mappingSymToVarId;
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
  std::string toString() const;
  std::string toString(VariableIdMapping& vid) const;
  //std::string toUniqueString() const;
  std::string toUniqueString(VariableIdMapping& vid) const;

  /* if VariableIdMapping is a valid pointer a variable name is returned
     otherwise toString() is called and a generic name (V..) is returned.
  */
  std::string toString(VariableIdMapping* vid) const;
  std::string toUniqueString(VariableIdMapping* vid) const;

  int getIdCode() const { return _id; }
  void setIdCode(int id) {_id=id;}
  bool isValid() const { return _id!=-1; }
  static const char * const idKindIndicator;
 public:

 private: 
  int _id;
};

size_t hash_value(const VariableId& vid);

bool operator<(VariableId id1, VariableId id2);
bool operator==(VariableId id1, VariableId id2);
bool operator!=(VariableId id1, VariableId id2);
VariableIdSet& operator+=(VariableIdSet& s1, VariableIdSet& s2);

}

#endif
