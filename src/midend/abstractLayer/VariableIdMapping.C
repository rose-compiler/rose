/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "CodeThornException.h"
#include "VariableIdMapping.h"
#include "RoseAst.h"
#include <set>
#include <vector>

using namespace std;
using namespace CodeThorn;

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
size_t VariableIdMapping::getArrayDimensions(SgArrayType* t, vector<size_t>* dimensions) {
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
  SgType* type=varSym->get_type();
  if(type) {
    if(SgTypedefType* typeDeftype=isSgTypedefType(type)) {
      while(typeDeftype) {
        //cout<<"DEBUG: found typedef type: "<<typeDeftype->unparseToString()<<endl;
        type=typeDeftype->get_base_type();
        typeDeftype=isSgTypedefType(type);
      }
    }
  }
  return type;
}

bool VariableIdMapping::hasBoolType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeBool(type);
}

bool VariableIdMapping::hasCharType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeChar(type)||isSgTypeSignedChar(type)||isSgTypeUnsignedChar(type)||isSgTypeChar16(type)||isSgTypeChar32(type);
}

bool VariableIdMapping::hasIntegerType(VariableId varId) {
  SgType* type=getType(varId);
  return SageInterface::isStrictIntegerType(type);
}

bool VariableIdMapping::hasEnumType(VariableId varId) {
  SgType* type=getType(varId);
  return SageInterface::IsEnum(type);
}

bool VariableIdMapping::hasFloatingPointType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeFloat(type)||isSgTypeDouble(type)||isSgTypeLongDouble(type)||isSgTypeFloat80(type)||isSgTypeFloat128(type);
}

bool VariableIdMapping::hasPointerType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgPointerType(type)!=0;
}

bool VariableIdMapping::hasReferenceType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgReferenceType(type);
}

bool VariableIdMapping::hasClassType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgClassType(type)!=0;
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
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    os<<i
      <<","<<varId.toString(this)
      //<<","<<SgNodeHelper::symbolToString(mappingVarIdToInfo[i].sym)  
      <<","<<mappingVarIdToInfo[variableIdFromCode(i)].sym
      <<","<<getNumberOfElements(varId)
      <<","<<getElementSize(varId)
      <<endl;
    ROSE_ASSERT(modeVariableIdForEachArrayElement?true:mappingSymToVarId[mappingVarIdToInfo[variableIdFromCode(i)].sym]==variableIdFromCode(i));
  }
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
VariableIdMapping::VariableIdSet VariableIdMapping::getVariableIdSet() {
  VariableIdSet set;
  for(map<SgSymbol*,VariableId>::iterator i=mappingSymToVarId.begin();i!=mappingSymToVarId.end();++i) {
    set.insert((*i).second);
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
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    myfile<<generateDotSgSymbol(mappingVarIdToInfo[variableIdFromCode(i)].sym)<<" [label=\""<<generateDotSgSymbol(mappingVarIdToInfo[variableIdFromCode(i)].sym)<<"\\n"
          <<"\\\""
          <<SgNodeHelper::symbolToString(mappingVarIdToInfo[variableIdFromCode(i)].sym)
          <<"\\\""
          <<"\""
          <<"]"
          <<endl;
  }
  myfile<<endl;
  // edges : sym->vid
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    myfile<<"_"<<mappingVarIdToInfo[variableIdFromCode(i)].sym
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
  // schroder3 (2016-08-23): Added if-else to make sure that this does not create a
  //  new mapping entry (with id 0) if the given symbol is not in the mapping yet
  //  (std::map's operator[] creates a new entry with value-initialization (zero-initialization
  //  in this case) if its argument is not in the map).
  if(mappingSymToVarId.count(sym)) {
    // intentionally a friend action to avoid that users can create VariableIds from int.
    newId=mappingSymToVarId[sym];
  }
  else {
    ROSE_ASSERT(!newId.isValid());
  }
  return newId;
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
SgSymbol* VariableIdMapping::getSymbol(VariableId varid) {
  ROSE_ASSERT(varid.isValid());
  ROSE_ASSERT(((size_t)varid._id)<mappingVarIdToInfo.size());
  return mappingVarIdToInfo[varid].sym;
}

void VariableIdMapping::setNumberOfElements(VariableId variableId, size_t size) {
  ROSE_ASSERT(variableId.isValid());
  //cout<<"DEBUG: VariableIdMapping::setNumberOfElements: "<<variableName(variableId)<<" size: "<<size<<endl;
  mappingVarIdToInfo[variableId].numberOfElements=size;
}

size_t VariableIdMapping::getNumberOfElements(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].numberOfElements;
}

void VariableIdMapping::setElementSize(VariableId variableId, size_t size) {
  ROSE_ASSERT(variableId.isValid());
  mappingVarIdToInfo[variableId].elementSize=size;
}

size_t VariableIdMapping::getElementSize(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].elementSize;
}

bool VariableIdMapping::isAnonymousBitfield(SgInitializedName* initName) {
  if(SgDeclarationStatement* declStmt=initName->get_declaration ()) { 
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(declStmt)) { 
      // check if the expression for the size of the bitfield exists
      if(varDecl->get_bitfield()) {
        // the variable declaration is a bitfield. Check whether it has a name
        string bitfieldName=string(initName->get_name());
        if(bitfieldName.size()==0) {
          return true;
        }
      }
    }
  }
  return false;
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
      // Try to find a symbol, type, and initializer of the current node:
      SgSymbol* sym = 0;
      SgType* type = 0;
      SgInitializer* initializer = 0;

      // schroder3 (2016-08-18): Added variables that are "declared" in a lambda capture. There is currently no SgInitializedName for
      //  these closure variables.
      if(SgLambdaCapture* lambdaCapture = isSgLambdaCapture(*i)) {
        // Add the closure variable (which is a member of enclosing lambda's anonymous class) to the mapping. (Even in case of a
        //  capture by reference there is a new reference member that references to the same variable as the captured reference.)
        SgVarRefExp* closureVar = isSgVarRefExp(lambdaCapture->get_closure_variable());
        ROSE_ASSERT(closureVar);
        sym = closureVar->get_symbol();
        ROSE_ASSERT(sym);
        type = closureVar->get_type();
      } else if(SgInitializedName* initName = isSgInitializedName(*i)) {
        //cout<<"DEBUG VIM: @ initName: "<<initName->unparseToString()<<endl;
        // Variable/ parameter found: Try to get its symbol:
        sym = initName->search_for_symbol_from_symbol_table();
        if(sym) {
          //cout<<"DEBUG VIM: symbol: "<<sym<<endl;
          // determine the declaration to check for bitfields
          if(isAnonymousBitfield(initName)) {
            // MS (2018-12-4/2019-03-20): workaround: ROSE BUG: if a struct contains more than one anonymous bitfield
            // they are assigned the same symbol.
            // ROSE AST BUG WORKAROUND (ROSE-1867)
            continue;
          }
          type = initName->get_type();
          initializer = initName->get_initializer();
        } else {  
          //cout << "computeVariableSymbolMapping: SgInitializedName \"" << initName->unparseToString() << "\" without associated symbol found." << endl;
          // Registration is not possible without symbol.
          // This is presumably a parameter in a declaration, a built-in variable (e.g. __builtin__x), an enum value, or a child of a SgCtorInitializerList.
          //  TODO: Is it possible to assert this?
          //  ==> It is okay to ignore these.
          //cout<<"DEBUG VIM: NO symbol! "<<endl;
        }
      }
      if(sym) {
        // Symbol found. There should be a type:
        ROSE_ASSERT(type);
        // Check if the symbol is already registered:
        if(symbolSet.find(sym) == symbolSet.end()) {
          // New symbol: Check for array symbol:
          if(SgArrayType* arrayType=isSgArrayType(type)) {
            // Try to find the array dimensions:
            // returns 0 if type does not contain size
            int arraySize = getArrayElementCount(arrayType);
            if(arraySize==0) {
              // check the initializer
              arraySize=getArrayDimensionsFromInitializer(isSgAggregateInitializer(initializer));
            }
            if(arraySize > 0) {
              //cout<<"INFO: found array decl: size: "<<arraySize<<" :: "<<(*i)->unparseToString()<<endl;
              // Array dimensions found: Registration as array symbol:
              registerNewArraySymbol(sym, arraySize);
              // Remember that this symbol is already registered:
              symbolSet.insert(sym);
              // Do not register as normal symbol and continue with next iteration:
              continue;
            }
            else {
              // Array dimensions are not available at compile time: Register as normal symbol (no continue;)
            }
          }
          // Register new symbol as normal variable symbol:
          registerNewSymbol(sym);
          // Remember that this symbol was already registered:
          symbolSet.insert(sym);
        }
      }
    }
  }
  // creates variableid for each string literal in the entire program
  registerStringLiterals(project);
  return;
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
string VariableIdMapping::variableName(VariableId varId) {
  ROSE_ASSERT(varId.isValid());
  SgSymbol* sym=getSymbol(varId);
  return SgNodeHelper::symbolToString(sym);
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
string VariableIdMapping::uniqueVariableName(VariableId varId) {
  if(!isTemporaryVariableId(varId)) {
    if(!varId.isValid())
      return "$invalidId";
    else
      return variableName(varId)+"#"+varId.toString().substr(1);
  } else {
    return string("tmp")+"#"+varId.toString().substr(1);
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
  ROSE_ASSERT(initName);
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
 * \date 2017.
 */
bool VariableIdMapping::isHeapMemoryRegionId(VariableId varId) {
  return isTemporaryVariableId(varId);
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
bool VariableIdMapping::isTemporaryVariableId(VariableId varId) {
  return dynamic_cast<UniqueTemporaryVariableSymbol*>(getSymbol(varId))!=0;
}

bool VariableIdMapping::isVariableIdValid(VariableId varId) {
  return varId.isValid() && ((size_t)varId._id) < mappingVarIdToInfo.size() && varId._id >= 0;
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
// deprecated (use createAndRegisterVariableId instead)
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
  SgSymbol* sym=createAndRegisterNewSymbol(name);
  VariableId newVarId=variableId(sym);
  temporaryVariableIdMapping.insert(make_pair(newVarId,name));
  return newVarId;
}

SgSymbol* VariableIdMapping::createAndRegisterNewSymbol(std::string name) {
  SgSymbol* sym=new UniqueTemporaryVariableSymbol(name);
  registerNewSymbol(sym);
  return sym;
}

CodeThorn::VariableId VariableIdMapping::createAndRegisterNewVariableId(std::string name) {
  SgSymbol* sym=createAndRegisterNewSymbol(name);
  VariableId varId=variableId(sym);
  setNumberOfElements(varId,0); // MS 3/3/2019: changed default from 1 to 0.
  return varId;
}

CodeThorn::VariableId VariableIdMapping::createAndRegisterNewMemoryRegion(std::string name, int regionSize) {
  SgSymbol* sym=createAndRegisterNewSymbol(name);
  VariableId varId=variableId(sym);
  setNumberOfElements(varId,regionSize);
  return varId;
}

void VariableIdMapping::registerNewArraySymbol(SgSymbol* sym, int arraySize) {
  ROSE_ASSERT(arraySize>0);
  if(mappingSymToVarId.find(sym)==mappingSymToVarId.end()) {
    // map symbol to var-id of array variable symbol
    size_t newVariableIdCode=mappingVarIdToInfo.size();
    mappingSymToVarId[sym]=variableIdFromCode(newVariableIdCode);
    VariableId tmpVarId=variableIdFromCode(newVariableIdCode);

    if(getModeVariableIdForEachArrayElement()) {
      // assign one var-id for each array element (in addition to the variable id of the array itself!)
      for(int i=0;i<arraySize;i++) {
        size_t newArrayElemVariableIdCode=mappingVarIdToInfo.size();
        VariableId newArrayElemVarId=variableIdFromCode(newArrayElemVariableIdCode);
        mappingVarIdToInfo[newArrayElemVarId].sym=sym;
      }
    } else {
      // assign one vari-id for entire array
      mappingVarIdToInfo[tmpVarId].sym=sym;
    }
    // size needs to be set *after* mappingVarIdToInfo[].sym has been updated
    setNumberOfElements(tmpVarId,arraySize);
  } else {
    stringstream ss;
    ss<< "VariableIdMapping: registerNewArraySymbol: attempt to register existing array symbol "<<sym<<":"<<SgNodeHelper::symbolToString(sym);
    throw CodeThorn::Exception(ss.str());
  }
}

void VariableIdMapping::registerNewSymbol(SgSymbol* sym) {
  ROSE_ASSERT(sym);
  if(mappingSymToVarId.find(sym)==mappingSymToVarId.end()) {
    // Due to arrays there can be multiple ids for one symbol (one id
    //  for each array element but only one symbol for the whole array)
    //  but there can not be multiple symbols for one id. The symbol count
    //  therefore must be less than or equal to the id count:
    ROSE_ASSERT(mappingSymToVarId.size() <= mappingVarIdToInfo.size());
    // If one of the sizes is zero, the other size have to be zero too:
    ROSE_ASSERT(mappingSymToVarId.size() == 0 ? mappingVarIdToInfo.size() == 0 : true);
    ROSE_ASSERT(mappingVarIdToInfo.size() == 0 ? mappingSymToVarId.size() == 0 : true);

    // Create new mapping entry:
    size_t newIdCode = mappingVarIdToInfo.size();
    mappingSymToVarId[sym] = variableIdFromCode(newIdCode);
    mappingVarIdToInfo[variableIdFromCode(newIdCode)].sym=sym;
    // set size to 1 (to compute bytes, multiply by size of type)
    VariableId newVarId;
    newVarId.setIdCode(newIdCode);
    setNumberOfElements(newVarId,0); // MS 3/11/2019: changed default from 1 to 0.
    // Mapping in both directions must be possible:
    ROSE_ASSERT(mappingSymToVarId.at(mappingVarIdToInfo[variableIdFromCode(newIdCode)].sym) == variableIdFromCode(newIdCode));
    ROSE_ASSERT(mappingVarIdToInfo[mappingSymToVarId.at(sym)].sym == sym);
  } else {
    stringstream ss;
    ss<< "Error: attempt to register existing symbol "<<sym<<":"<<SgNodeHelper::symbolToString(sym);
    throw CodeThorn::Exception(ss.str());
  }
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
// we use a function as a destructor may delete it multiple times
void VariableIdMapping::deleteUniqueTemporaryVariableId(VariableId varId) {
  if(isTemporaryVariableId(varId)) {
    //cerr<<"DEBUG WARNING: not deleting temporary variable id symbol."<<endl;
    //delete getSymbol(varId);
  } else {
    throw CodeThorn::Exception("VariableIdMapping::deleteUniqueTemporaryVariableSymbol: improper id operation.");
  }
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

VariableIdMapping::VariableIdInfo::VariableIdInfo():
  numberOfElements(0),
  elementSize(0),
  offset(0),
  sym(0) {
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
VariableId::VariableId():_id(-1){
}

const char * const VariableId::idKindIndicator = "V";

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
string
VariableId::toString() const {
  stringstream ss;
  ss<<idKindIndicator<<_id;
  return ss.str();
}

string
VariableId::toUniqueString(VariableIdMapping& vim) const {
  return vim.uniqueVariableName(*this);
}

string
VariableId::toUniqueString(VariableIdMapping* vim) const {
  if(vim)
    return vim->uniqueVariableName(*this);
  else
    return toString();
}

string
VariableId::toString(VariableIdMapping& vim) const {
  return vim.variableName(*this);
}

string
VariableId::toString(VariableIdMapping* vim) const {
  if(vim)
    return vim->variableName(*this);
  else
    return toString();
}

bool CodeThorn::operator<(VariableId id1, VariableId id2) {
  return id1._id<id2._id;
}
bool CodeThorn::operator==(VariableId id1, VariableId id2) {
  return id1._id==id2._id;
}
bool CodeThorn::operator!=(VariableId id1, VariableId id2) {
  return !(id1==id2);
}

VariableIdSet& CodeThorn::operator+=(VariableIdSet& s1, VariableIdSet& s2) {
  for(VariableIdSet::iterator i=s2.begin();i!=s2.end();++i) {
    s1.insert(*i);
  }
  return s1;
}

/*! 
 * \author Markus Schordan
 * \date 2012.
 */
size_t CodeThorn::hash_value(const VariableId& vid) {
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
    VariableId vid; // creates default invalid id (isValid(vid)==false).
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

std::map<SgStringVal*,VariableId>* VariableIdMapping::getStringLiteralsToVariableIdMapping() {
  return &sgStringValueToVariableIdMapping;
}

VariableId VariableIdMapping::getStringLiteralVariableId(SgStringVal* sval) {
  return sgStringValueToVariableIdMapping[sval];
}

int VariableIdMapping::numberOfRegisteredStringLiterals() {
  return (int)sgStringValueToVariableIdMapping.size();
}

bool VariableIdMapping::isStringLiteralAddress(VariableId stringVarId) {
  return variableIdToSgStringValueMapping.find(stringVarId)!=variableIdToSgStringValueMapping.end();
}

bool VariableIdMapping::isFunctionParameter(VariableId varId) {
  if(SgVariableSymbol* varSym=isSgVariableSymbol(getSymbol(varId))) {
    if(SgInitializedName* initName=varSym->get_declaration()) {
      if(isSgFunctionParameterList(initName->get_parent())) {
        return true;
      }
    }
  }
  return false;
}


void VariableIdMapping::registerStringLiterals(SgNode* root) {
  string prefix="$string";
  int num=1;
  RoseAst ast(root);
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgNode* node=*i;
    if(SgStringVal* stringVal=isSgStringVal(node)) {
      //cout<<"registerStringLiterals:: found string literal"<<stringVal->unparseToString()<<endl;
      if(sgStringValueToVariableIdMapping.find(stringVal)==sgStringValueToVariableIdMapping.end()) {
        // found new string literal
        // obtain new variableid
        stringstream ss;
        ss<<prefix<<num++;
        CodeThorn::VariableId newVariableId=createAndRegisterNewVariableId(ss.str());
        sgStringValueToVariableIdMapping[stringVal]=newVariableId;
        variableIdToSgStringValueMapping[newVariableId]=stringVal;
        // the size of the memory region of a string is its length + 1 (for terminating 0).
        setNumberOfElements(newVariableId,stringVal->get_value().size()+1);
        // ensure that maps being built for mapping in both directions are of same size
        ROSE_ASSERT(sgStringValueToVariableIdMapping.size()==variableIdToSgStringValueMapping.size());
        //cout<<"registered."<<endl;
      } else {
        //cout<<"NOT registered."<<endl;
      }
    }
  }
}

void VariableIdMapping::setModeVariableIdForEachArrayElement(bool active) {
  ROSE_ASSERT(mappingVarIdToInfo.size()==0); 
  modeVariableIdForEachArrayElement=active;
}

bool VariableIdMapping::getModeVariableIdForEachArrayElement() {
  return modeVariableIdForEachArrayElement;
}
