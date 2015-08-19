/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"                                 // every librose .C file must start with this

#include "VariableIdMapping.h"
#include "RoseAst.h"
#include <set>
#include <vector>

using namespace std;
using namespace SPRAY;

int exprToInt(SgExpression* exp) {
  if(SgUnsignedLongVal* valExp = isSgUnsignedLongVal(exp))
    return valExp->get_value();
  else if(SgIntVal* valExpInt = isSgIntVal(exp))
    return valExpInt->get_value();
  else
    return -1;
}

// Puts sizes of the array dimensions into the vector,
// returns total element count.
size_t VariableIdMapping::getArrayDimensions(SgArrayType* t, vector<size_t> *dimensions/* = NULL*/) {
  ROSE_ASSERT(t);
  size_t result = 0;
  int curIndex = exprToInt(t->get_index());
  if(curIndex != -1)
    result = curIndex;
  SgArrayType* arrayBase = isSgArrayType(t->get_base_type());
  if(dimensions)
    dimensions->push_back(curIndex);
  if(arrayBase)
    result *= getArrayDimensions(arrayBase, dimensions);
  return result;
}

size_t VariableIdMapping::getArrayElementCount(SgArrayType* t) {
  return getArrayDimensions(t);
}

// Calculates sizes of array dimensions based on its initializer size,
// puts them in *dimensions*, returns total element count or 0 if there's no initializer.
size_t VariableIdMapping::getArrayDimensionsFromInitializer(SgAggregateInitializer* init,
                                                            vector<size_t> *dimensions/* = NULL*/) {
  if(!init)
    return 0;
  SgExpressionPtrList& initializers = init->get_initializers()->get_expressions();
  if(initializers.empty())
    return 0;
  size_t result = initializers.size();
  if(dimensions)
    dimensions->push_back(result);
  if(SgAggregateInitializer* nested_init = isSgAggregateInitializer(initializers[0]))
    result *= getArrayDimensionsFromInitializer(nested_init, dimensions);
  return result;
}

VariableIdMapping::VariableIdMapping():modeVariableIdForEachArrayElement(false) {
}

SgVariableDeclaration* VariableIdMapping::getVariableDeclaration(VariableId varId) {
  SgSymbol* varSym=getSymbol(varId);
  return isSgVariableDeclaration(SgNodeHelper::findVariableDeclarationWithVariableSymbol(varSym));
}

SgType* VariableIdMapping::getType(VariableId varId) {
  SgSymbol* varSym=getSymbol(varId);
  return varSym->get_type();
}

bool VariableIdMapping::hasIntegerType(VariableId varId) {
  SgType* type=getType(varId);
  return SageInterface::isStrictIntegerType(type);
}

bool VariableIdMapping::hasFloatingPointType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeFloat(type)||isSgTypeDouble(type)||isSgTypeLongDouble(type);
}
bool VariableIdMapping::hasPointerType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgPointerType(type)!=0;
}
bool VariableIdMapping::hasArrayType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgArrayType(type)!=0;
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void VariableIdMapping::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToSym.size();++i) {
    os<<""<<i
      <<","<<mappingVarIdToSym[i]
      <<","<<SgNodeHelper::symbolToString(mappingVarIdToSym[i])
      <<","<<SgNodeHelper::uniqueLongVariableName(mappingVarIdToSym[i])
      <<endl;
    ROSE_ASSERT(modeVariableIdForEachArrayElement?true:mappingSymToVarId[mappingVarIdToSym[i]]==i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableIdMapping::VariableIdSet VariableIdMapping::getVariableIdSet() {
  VariableIdSet set;
  for(map<SgSymbol*,size_t>::iterator i=mappingSymToVarId.begin();i!=mappingSymToVarId.end();++i) {
    size_t t=(*i).second;
    VariableId id;
    id.setIdCode(t);
    set.insert(id);
  }
  return set;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableId VariableIdMapping::variableIdFromCode(int i) {
  VariableId id;
  id.setIdCode(i);
  return id;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void VariableIdMapping::generateStmtSymbolDotEdge(std::ofstream& file, SgNode* node,VariableId id) {
  file<<"_"<<node<<" "
      <<"->"
      <<id.toString()
      << endl;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string VariableIdMapping::generateDotSgSymbol(SgSymbol* sym) {
  stringstream ss;
  ss<<"_"<<sym;
  return ss.str();
}

#if 0
SgSymbol*
my_search_for_symbol_from_symbol_table(SgInitializedName* initname) const
   {
     SgSymbol *symbol;
     SgInitializedName* aPrevDeclItem = p_prev_decl_item;
     while(aPrevDeclItem != NULL && aPrevDeclItem->p_prev_decl_item != NULL)
        {
       // DQ (11/19/2011): This loop will not terminate if (aPrevDeclItem->p_prev_decl_item == aPrevDeclItem).
       // This should not happen but this mistake has happened and this assertion helps it be caught instead 
       // of be an infinite loop.
          ROSE_ASSERT(aPrevDeclItem->p_prev_decl_item != aPrevDeclItem);

          aPrevDeclItem = aPrevDeclItem->p_prev_decl_item;
        }

     if (aPrevDeclItem != NULL)
          symbol = aPrevDeclItem->get_symbol_from_symbol_table();
       else
          symbol = get_symbol_from_symbol_table();

     assert(symbol != NULL);
     return symbol;
   }
#endif


/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void VariableIdMapping::generateDot(string filename, SgNode* astRoot) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);

  myfile<<"digraph VarIdMapping {"<<endl;
  myfile<<"rankdir=LR"<<endl;

  // nodes
  for(size_t i=0;i<mappingVarIdToSym.size();++i) {
    myfile<<generateDotSgSymbol(mappingVarIdToSym[i])<<" [label=\""<<generateDotSgSymbol(mappingVarIdToSym[i])<<"\\n"
          <<"\\\""
          <<SgNodeHelper::symbolToString(mappingVarIdToSym[i])
          <<"\\\""
          <<"\""
          <<"]"
          <<endl;
  }
  myfile<<endl;
  // edges : sym->vid
  for(size_t i=0;i<mappingVarIdToSym.size();++i) {
    myfile<<"_"<<mappingVarIdToSym[i]
          <<"->"
          <<variableIdFromCode(i).toString()
          <<endl;
  }
  // edges: stmt/expr->sym
  
  RoseAst ast(astRoot);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(*i)) {
      generateStmtSymbolDotEdge(myfile,vardecl,variableId(vardecl));
    }
    if(SgVarRefExp* varref=isSgVarRefExp(*i)) {
      generateStmtSymbolDotEdge(myfile,varref,variableId(varref));
    }
    if(SgInitializedName* initname=isSgInitializedName(*i)) {
      // ROSE-BUG-1
      // this guard protects from calling search_for_symbol .. which fails for this kind of SgInitializedName nodes.
#if 1
      SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initname);
      if(sym)
        generateStmtSymbolDotEdge(myfile,initname,variableId(initname));
#else          
      cout << "AT:"<<initname->get_name()<<endl;
      if(initname->get_name()=="") {
        cerr<<"WARNING: SgInitializedName::get_name()==\"\" .. skipping."<<endl;
      } else {
        // check for __built_in - prefix
        string s=initname->get_name();
        string prefix="__builtin";
        if(s.substr(0,prefix.size())==prefix)
          cerr<<"WARNING: variable with prefix \""<<prefix<<"\" in SgInitializedName::get_name()=="<<initname->get_name()<<" -- skipping"<<endl;
        else {
          if(initname->get_prev_decl_item()==0 && initname->get_symbol_from_symbol_table()==0)
            cerr<<"WARNING: SgInitializedName: symbol-look-up would fail: get_name()=="<<initname->get_name()<< " .. skipping."<<endl;
          else
            generateStmtSymbolDotEdge(myfile,initname,variableId(initname));
        }
      }
#endif
    }
  }
  myfile<<"}"<<endl;
  myfile.close();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableId VariableIdMapping::variableId(SgSymbol* sym) {
  assert(sym);
  VariableId newId;
  // intentionally a friend action to avoid that users can create VariableIds from int.
  newId._id=mappingSymToVarId[sym]; 
  return newId;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgSymbol* VariableIdMapping::getSymbol(VariableId varid) {
  ROSE_ASSERT(varid.isValid());
  ROSE_ASSERT(varid._id<mappingVarIdToSym.size());
  return mappingVarIdToSym[varid._id];
}
//SgSymbol* VariableIdMapping::getSymbol(VariableId varId) {
//  return varId.getSymbol();
//}

void VariableIdMapping::setSize(VariableId variableId, size_t size) {
  ROSE_ASSERT(hasArrayType(variableId));
  mappingVarIdToSize[variableId._id]=size;
}

size_t VariableIdMapping::getSize(VariableId variableId) {
  ROSE_ASSERT(hasArrayType(variableId));
  return mappingVarIdToSize[variableId._id];
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void VariableIdMapping::computeVariableSymbolMapping(SgProject* project) {
  set<SgSymbol*> symbolSet;
  list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
    RoseAst ast(*k);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgSymbol* sym=0;
      bool found=false;
      int arraySize=-1; // -1 denotes: not an array
      if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
        sym=SgNodeHelper::getSymbolOfVariableDeclaration(varDecl);
        if(sym) {
          found=true;
          if(/*modeVariableIdForEachArrayElement &&*/ SgNodeHelper::isArrayDeclaration(varDecl)) {
            SgNode* initName0=varDecl->get_traversalSuccessorByIndex(1); // get-InitializedName
            ROSE_ASSERT(initName0);
            SgInitializedName* initName=isSgInitializedName(initName0);
            ROSE_ASSERT(initName);
            SgType* type=initName->get_type();
            if(SgArrayType* arrayType=isSgArrayType(type)) {
              //cout<<"DEBUG: found array type."<<endl;
              // returns 0 if type does not contain size
              arraySize=getArrayElementCount(arrayType);
            }
            if(arraySize==0) {
              // check the initializer
              arraySize=getArrayDimensionsFromInitializer(isSgAggregateInitializer(initName->get_initializer()));
            }
            if(arraySize > 0) {
              //cout<<"INFO: found array decl: size: "<<arraySize<<" :: "<<varDecl->unparseToString()<<endl;
              registerNewArraySymbol(sym, arraySize);
              symbolSet.insert(sym);
              found = false;
            }
          }
        } else {
          cerr<<"WARNING: computeVariableSymbolMapping: VariableDeclaration without associated symbol found. Ignoring.";
        }
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
        //string longName=SgNodeHelper::uniqueLongVariableName(sym);
        
        // ensure all symbols are SgVariableSymbol
        SgVariableSymbol* finalvarsym=isSgVariableSymbol(sym);
        assert(finalvarsym);
        //MapPair pair=make_pair(longName,finalvarsym);
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

/*! 
  * \author Markus Schordan
  * \date 2012.
 */string VariableIdMapping::variableName(VariableId varId) {
  SgSymbol* sym=getSymbol(varId);
  return SgNodeHelper::symbolToString(sym);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string VariableIdMapping::uniqueLongVariableName(VariableId varId) {
  if(!isTemporaryVariableId(varId)) {
    return variableName(varId);
    //return SgNodeHelper::uniqueLongVariableName(getSymbol(varId));
  } else {
    return "$$$tmp"+variableName(varId);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string VariableIdMapping::uniqueShortVariableName(VariableId varId) {
  if(!isTemporaryVariableId(varId)) {
    if(!varId.isValid())
      return "$invalidId";
    else
      return variableName(varId)+"_"+varId.toString().substr(1);
    //return SgNodeHelper::uniqueLongVariableName(getSymbol(varId));
  } else {
    return string("tmp")+"_"+varId.toString().substr(1);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableId VariableIdMapping::variableId(SgVariableDeclaration* decl) {
  return variableId(SgNodeHelper::getSymbolOfVariableDeclaration(decl));
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableId VariableIdMapping::variableId(SgVarRefExp* varRefExp) {
  return variableId(SgNodeHelper::getSymbolOfVariable(varRefExp));
}

VariableId VariableIdMapping::variableId(SgInitializedName* initName) {
  assert(initName);
  SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(initName);
  if(sym)
    return variableId(sym);
  else
    return VariableId(); // always defaults to a value different to all mapped values
}

VariableId VariableIdMapping::variableIdOfArrayElement(VariableId arrayVar, int elemIndex) {
  int idCode = arrayVar.getIdCode();
  int elemIdCode = idCode + elemIndex;
  VariableId elemVarId;
  elemVarId.setIdCode(elemIdCode);
  return elemVarId;
}

// Returns a valid VariableId corresponding to *ref*
// if indices in *ref* are all integers and sizes of
// all array dimensions are known.
// Returns an invalid VariableId otherwise.
VariableId VariableIdMapping::idForArrayRef(SgPntrArrRefExp* ref)
{
  assert(ref);
  VariableId result;
  result.setIdCode(-1);

  // Check failure conditions
  if(isSgPntrArrRefExp(ref->get_parent()))
    return result;
  SgExpression* varRef;
  vector<SgExpression*> subscripts;
  vector<SgExpression*>* pSubscripts = &subscripts;
  SageInterface::isArrayReference(ref, &varRef, &pSubscripts);
  SgVarRefExp* arrVar = isSgVarRefExp(varRef);
  if(!arrVar)
    return result;
  SgArrayType* arrType = isSgArrayType(SageInterface::convertRefToInitializedName(arrVar)->get_type());
  if(!arrType)
    return result;
  vector<size_t> arrayDimensions;
  size_t arrSize = getArrayDimensions(arrType, &arrayDimensions);
  if(!arrSize) {
    arrayDimensions.clear();
    arrSize = getArrayDimensionsFromInitializer(
                isSgAggregateInitializer(SageInterface::convertRefToInitializedName(arrVar)->get_initializer()), 
                &arrayDimensions);
  }
  if(!arrSize)
    return result; // Array size is unknown
  assert(arrayDimensions.size() == subscripts.size());

  // Calculate the index as below.
  // int a[M][N][K];
  // a[x][y][z] => index = x*N*K + y*K + z.
  int index = 0;
  for(unsigned i = 0; i < subscripts.size(); i++) {
    int curIndex = exprToInt(subscripts[i]);
    if(curIndex == -1)
      return result;
    int dimension_size = (i == arrayDimensions.size() - 1 ? 1 : arrayDimensions[i + 1]);
    for(unsigned d = i + 2; d < arrayDimensions.size(); d++)
      dimension_size*= arrayDimensions[d];
    index += curIndex*dimension_size;
  }

  VariableId varId = variableId(arrVar);
  result = variableIdOfArrayElement(varId, index);
  return result;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool VariableIdMapping::isTemporaryVariableId(VariableId varId) {
  return dynamic_cast<UniqueTemporaryVariableSymbol*>(getSymbol(varId))!=0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
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

void VariableIdMapping::registerNewArraySymbol(SgSymbol* sym, int arraySize) {
  ROSE_ASSERT(arraySize>0);
  if(mappingSymToVarId.find(sym)==mappingSymToVarId.end()) {
    // map symbol to var-id of array variable symbol
    size_t newVariableIdCode=mappingVarIdToSym.size();
    mappingSymToVarId[sym]=newVariableIdCode;
    VariableId tmpVarId=variableIdFromCode(newVariableIdCode);
    if(getModeVariableIdForEachArrayElement()) {
      // assign one var-id for each array element
      for(int i=0;i<arraySize;i++) {
        mappingVarIdToSym.push_back(sym);
      }
    } else {
      // assign one vari-id for entire array
      mappingVarIdToSym.push_back(sym);
    }
    // size needs to be set *after* mappingVarIdToSym has been updated
    setSize(tmpVarId,arraySize);
  } else {
    cerr<< "Error: attempt to register existing array symbol "<<sym<<":"<<SgNodeHelper::symbolToString(sym)<<endl;
    exit(1);
  }
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

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// we use a function as a destructor may delete it multiple times
void VariableIdMapping::deleteUniqueTemporaryVariableId(VariableId varId) {
  if(isTemporaryVariableId(varId))
    delete getSymbol(varId);
  else
    throw "VariableIdMapping::deleteUniqueTemporaryVariableSymbol: improper id operation.";
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableIdMapping::UniqueTemporaryVariableSymbol::UniqueTemporaryVariableSymbol(string name) : SgVariableSymbol() {
  _tmpName=name;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgName VariableIdMapping::UniqueTemporaryVariableSymbol::get_name() const {
  return SgName(_tmpName);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableId::VariableId():_id(-1){
}
//VariableId::VariableId(int id):_id(id){
//}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
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

bool SPRAY::operator<(VariableId id1, VariableId id2) {
  return id1._id<id2._id;
}
bool SPRAY::operator==(VariableId id1, VariableId id2) {
  return id1._id==id2._id;
}
bool SPRAY::operator!=(VariableId id1, VariableId id2) {
  return !(id1==id2);
}

VariableIdSet& SPRAY::operator+=(VariableIdSet& s1, VariableIdSet& s2) {
  for(VariableIdSet::iterator i=s2.begin();i!=s2.end();++i) {
    s1.insert(*i);
  }
  return s1;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
size_t SPRAY::hash_value(const VariableId& vid) {
  return vid.getIdCode();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableIdMapping::VariableIdSet VariableIdMapping::determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> varDecls) {
  VariableIdMapping::VariableIdSet resultSet;
  for(set<SgVariableDeclaration*>::iterator i=varDecls.begin();i!=varDecls.end();++i) {
    SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(*i);
    if(sym) {
      resultSet.insert(variableId(sym));
    }
  }
  return resultSet;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableIdMapping::VariableIdSet VariableIdMapping::determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList) {
  VariableIdMapping::VariableIdSet resultSet;
  for(SgInitializedNamePtrList::iterator i=namePtrList.begin();i!=namePtrList.end();++i) {
    assert(*i);
    SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(*i);
    if(sym) {
      resultSet.insert(variableId(sym));
    }
  }
  return resultSet;
}

VariableIdMapping::VariableIdSet VariableIdMapping::variableIdsOfAstSubTree(SgNode* node) {
  VariableIdSet vset;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    VariableId vid; // default creates intentionally an invalid id.
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      vid=variableId(varDecl);
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(*i)) {
      vid=variableId(varRefExp);
    } else if(SgInitializedName* initName=isSgInitializedName(*i)) {
      vid=variableId(initName);
    }
    if(vid.isValid())
      vset.insert(vid);
  }
  return vset;
}

SgExpressionPtrList& VariableIdMapping::getInitializerListOfArrayVariable(VariableId arrayVar) {
  SgVariableDeclaration* decl=this->getVariableDeclaration(arrayVar);
  return SgNodeHelper::getInitializerListOfAggregateDeclaration(decl);
}
