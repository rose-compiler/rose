
#include "sage3basic.h"
#include "CodeThornException.h"
#include "VariableIdMapping.h"
#include "RoseAst.h"
#include <set>
#include <vector>
#include "Diagnostics.h"
#include "SgNodeHelper.h"

using namespace std;
using namespace CodeThorn;
using namespace Rose::Diagnostics;

static int exprToInt(SgExpression* exp) {
  if(SgUnsignedLongVal* valExp = isSgUnsignedLongVal(exp))
    return valExp->get_value();
  else if(SgIntVal* valExpInt = isSgIntVal(exp))
    return valExpInt->get_value();
  else
    return -1;
}

bool VariableIdMapping::isUnknownSizeValue(TypeSize size) {
  return size==unknownSizeValue();
}

TypeSize VariableIdMapping::unknownSizeValue() {
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

VariableIdMapping::VariableIdMapping():linkAnalysis(false) {
}

VariableIdMapping::~VariableIdMapping() {
}

void VariableIdMapping::setLinkAnalysisFlag(bool flag) {
  linkAnalysis=flag;
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

bool VariableIdMapping::isOfBoolType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeBool(type);
}

bool VariableIdMapping::isOfCharType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeChar(type)||isSgTypeSignedChar(type)||isSgTypeUnsignedChar(type)||isSgTypeChar16(type)||isSgTypeChar32(type);
}

bool VariableIdMapping::isOfIntegerType(VariableId varId) {
  SgType* type=getType(varId);
  return SageInterface::isStrictIntegerType(type)||isSgTypeSigned128bitInteger(type);
}

bool VariableIdMapping::isOfEnumType(VariableId varId) {
  SgType* type=getType(varId);
  return SageInterface::IsEnum(type);
}

bool VariableIdMapping::isOfFloatingPointType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgTypeFloat(type)||isSgTypeDouble(type)||isSgTypeLongDouble(type)||isSgTypeFloat80(type)||isSgTypeFloat128(type);
}

bool VariableIdMapping::isOfPointerType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgPointerType(type)!=0;
}

bool VariableIdMapping::isOfReferenceType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgReferenceType(type);
}

bool VariableIdMapping::isOfClassType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgClassType(type)!=0;
}

bool VariableIdMapping::isOfArrayType(VariableId varId) {
  SgType* type=getType(varId);
  return isSgArrayType(type)!=0;
}

std::string VariableIdMapping::mangledName(VariableId varId) {
  if(SgSymbol* sym=getSymbol(varId))
    return sym->get_mangled_name();
  else
    return "*";
}

void VariableIdMapping::toStream(ostream& os) {
  for(size_t i=0;i<mappingVarIdToInfo.size();++i) {
    VariableId varId=variableIdFromCode(i);
    os<<i
      <<","<<varId.toString(this)
      //<<","<<SgNodeHelper::symbolToString(mappingVarIdToInfo[i].sym)  
      //<<","<<mappingVarIdToInfo[variableIdFromCode(i)]._sym
      <<","<<getVariableIdInfo(varId).variableScopeToString()
      <<","<<getVariableIdInfo(varId).aggregateTypeToString()
      <<",elems:"<<getNumberOfElements(varId)
      <<",elemsize:"<<getElementSize(varId)
      <<",total:"<<getTotalSize(varId)
      <<",offset:"<<getOffset(varId);
    if(isStringLiteralAddress(varId)) {
      os<<","<<"<non-symbol-string-literal-id>";
    } else if(isTemporaryVariableId(varId)) {
      os<<","<<"<non-symbol-memory-region-id>";
    } else if(SgSymbol* sym=getSymbol(varId)) {
      os<<","<<variableName(varId);
    } else {
      os<<","<<"<missing-symbol>";
    }
    os<<endl;
  }
}

VariableIdMapping::VariableIdSet VariableIdMapping::getVariableIdSet() {
  VariableIdSet set;
  for(map<SgSymbol*,VariableId>::iterator i=mappingSymToVarId.begin();i!=mappingSymToVarId.end();++i) {
    set.insert((*i).second);
  }
  return set;
}

VariableId VariableIdMapping::variableIdFromCode(int i) {
  VariableId id;
  id.setIdCode(i);
  return id;
}

void VariableIdMapping::generateStmtSymbolDotEdge(std::ofstream& file, SgNode* node,VariableId id) {
  file<<"_"<<node<<" "
      <<"->"
      <<id.toString()
      << endl;
}

string VariableIdMapping::generateDotSgSymbol(SgSymbol* sym) {
  stringstream ss;
  ss<<"_"<<sym;
  return ss.str();
}

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

VariableId VariableIdMapping::variableId(SgSymbol* sym) {
  ROSE_ASSERT(sym);
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

TypeSize VariableIdMapping::getNumberOfElements(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].numberOfElements;
}

void VariableIdMapping::setElementSize(VariableId variableId, TypeSize size) {
  ROSE_ASSERT(variableId.isValid());
  mappingVarIdToInfo[variableId].elementSize=size;
}

TypeSize VariableIdMapping::getElementSize(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].elementSize;
}

void VariableIdMapping::setNumDimensionElements(VariableId variableId, TypeSize dimNr, TypeSize numElements) {
  ROSE_ASSERT(variableId.isValid());
  mappingVarIdToInfo[variableId].numDimensionElements[dimNr]=numElements;
}

TypeSize VariableIdMapping::getNumDimensionElements(VariableId variableId, TypeSize dimNr) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].numDimensionElements[dimNr];
}


void VariableIdMapping::setTotalSize(VariableId variableId, TypeSize size) {
  ROSE_ASSERT(variableId.isValid());
  mappingVarIdToInfo[variableId].totalSize=size;
}

TypeSize VariableIdMapping::getTotalSize(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].totalSize;
}

void VariableIdMapping::setOffset(VariableId variableId, TypeSize size) {
  ROSE_ASSERT(variableId.isValid());
  mappingVarIdToInfo[variableId].offset=size;
}

TypeSize VariableIdMapping::getOffset(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].offset;
}

void VariableIdMapping::setIsMemberVariable(VariableId variableId, bool flag) {
  ROSE_ASSERT(variableId.isValid());
  if(flag)
    mappingVarIdToInfo[variableId].variableScope=VS_MEMBER;
  else
    mappingVarIdToInfo[variableId].variableScope=VS_UNKNOWN;
}

bool VariableIdMapping::isMemberVariable(VariableId variableId) {
  ROSE_ASSERT(variableId.isValid());
  return mappingVarIdToInfo[variableId].variableScope==VS_MEMBER;
}

bool VariableIdMapping::isVolatile(VariableId variableId) {
  return mappingVarIdToInfo[variableId].isVolatileFlag;
}

void VariableIdMapping::setVolatileFlag(VariableId variableId, bool flag) {
  mappingVarIdToInfo[variableId].isVolatileFlag=flag;
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

void VariableIdMapping::computeVariableSymbolMapping(SgProject* project, int maxWarningsCount) {
  int numWarningsCount=0;
  set<SgSymbol*> symbolSet;
  list<SgGlobal*> globList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator k=globList.begin();k!=globList.end();++k) {
    RoseAst ast(*k);
    ast.setWithTemplates(true);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgSymbol* sym = 0;
      SgInitializedName* initName = NULL;
      // schroder3 (2016-08-18): Added variables that are "declared" in a lambda capture. There is currently no SgInitializedName for
      //  these closure variables.
      if(SgLambdaCapture* lambdaCapture = isSgLambdaCapture(*i)) {
        // Add the closure variable (which is a member of enclosing lambda's anonymous class) to the mapping. (Even in case of a
        //  capture by reference there is a new reference member that references to the same variable as the captured reference.)
        SgVarRefExp* closureVar = isSgVarRefExp(lambdaCapture->get_closure_variable());
        ROSE_ASSERT(closureVar);
        sym = closureVar->get_symbol();
        ROSE_ASSERT(sym);
        //type = closureVar->get_type();
      } else if((initName = isSgInitializedName(*i))) {
        // Variable/ parameter found: Try to get its symbol:
        sym = initName->search_for_symbol_from_symbol_table();
        if(sym) {
          // determine the declaration to check for bitfields
          if(isAnonymousBitfield(initName)) {
            // MS (2018-12-4/2019-03-20): workaround: ROSE BUG: if a struct contains more than one anonymous bitfield
            // they are assigned the same symbol.
            // ROSE AST BUG WORKAROUND (ROSE-1867)
            continue;
          }
        }
      }
      if(sym) {
        // Check if the symbol is already registered:
        if(symbolSet.find(sym) == symbolSet.end()) {
          // Register new symbol as normal variable symbol:
          // ensure it's a "valid" symbol 
          if(sym->get_symbol_basis()!=0) {
            registerNewSymbol(sym);
            // Remember that this symbol was already registered:
            symbolSet.insert(sym);
          }
        }
      } else {
        numWarningsCount++;
        std::string name("UNKNOWN");
        if(initName) {
          name = initName->get_qualified_name().getString();
        }
        if(numWarningsCount<maxWarningsCount || -1 == maxWarningsCount) {
          if(initName) {
            Rose::Diagnostics::mlog[WARN]<<"VariableIdMapping: No symbol available for SgInitializedName " << name << " at "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<endl;
          }
        } else if(numWarningsCount==maxWarningsCount) {
          Rose::Diagnostics::mlog[WARN]<<"VariableIdMapping: No symbol available for SgInitializedName " << name << " at "<<SgNodeHelper::sourceFilenameLineColumnToString(*i)<<endl;
          Rose::Diagnostics::mlog[WARN]<<"VariableIdMapping: No symbol available for SgInitializedName: maximum warning count of "<<maxWarningsCount<<" reached."<<endl;
        }
      }
    }
  }
  // creates variableid for each string literal in the entire program
  registerStringLiterals(project);

  if(linkAnalysis) {
    performLinkAnalysisRemapping();
  }

  return;
}

SgSymbol* VariableIdMapping::selectLinkSymbol(std::set<SgSymbol*>& symSet) {
  ROSE_ASSERT(symSet.size()>0);
  return *symSet.begin();
}

void VariableIdMapping::performLinkAnalysisRemapping() {
  for(VarNameToSymMappingType::iterator i=mappingGlobalVarNameToSymSet.begin();i!=mappingGlobalVarNameToSymSet.end();++i) {
    std::set<SgSymbol*>& symSet=(*i).second;
    VariableId v;
    if(symSet.size()>0) {
      v=variableId(selectLinkSymbol(symSet)); // choose some symbol
      ROSE_ASSERT(v.isValid());
      for (std::set<SgSymbol*>::iterator i=symSet.begin();i!=symSet.end();++i) {
        SgSymbol* sym=*i;
        if(sym!=mappingVarIdToInfo[v].sym) {
          mappingSymToVarId[sym]=v;
          mappingVarIdToInfo[v].sym=sym;
          mappingVarIdToInfo[v].relinked=true;
        }
      }
    }
  }
}

string VariableIdMapping::variableName(VariableId varId) {
  ROSE_ASSERT(varId.isValid());
  SgSymbol* sym=getSymbol(varId);
  return SgNodeHelper::symbolToString(sym);
}

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

VariableId VariableIdMapping::variableId(SgVariableDeclaration* decl) {
  return variableId(SgNodeHelper::getSymbolOfVariableDeclaration(decl));
}

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
  result = varId; //MS 05/20/020: modified to always return array id. previously: variableIdOfArrayElement(varId, index);
  return result;
}


bool VariableIdMapping::isHeapMemoryRegionId(VariableId varId) {
  return isTemporaryVariableId(varId);
}

bool VariableIdMapping::isTemporaryVariableId(VariableId varId) {
  return isTemporaryVariableIdSymbol(getSymbol(varId));
}

bool VariableIdMapping::isTemporaryVariableIdSymbol(SgSymbol* sym) {
  return dynamic_cast<UniqueTemporaryVariableSymbol*>(sym)!=0;
}

bool VariableIdMapping::isVariableIdValid(VariableId varId) {
  return varId.isValid() && ((size_t)varId._id) < mappingVarIdToInfo.size() && varId._id >= 0;
}

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
  setNumberOfElements(varId,unknownSizeValue());
  return varId;
}

CodeThorn::VariableId VariableIdMapping::createAndRegisterNewMemoryRegion(std::string name, int regionSize) {
  SgSymbol* sym=createAndRegisterNewSymbol(name);
  VariableId varId=variableId(sym);
  setNumberOfElements(varId,regionSize);
  return varId;
}

void VariableIdMapping::registerNewArraySymbol(SgSymbol* sym, TypeSize arraySize) {
  if(mappingSymToVarId.find(sym)==mappingSymToVarId.end()) {
    // map symbol to var-id of array variable symbol
    size_t newVariableIdCode=mappingVarIdToInfo.size();
    mappingSymToVarId[sym]=variableIdFromCode(newVariableIdCode);
    VariableId tmpVarId=variableIdFromCode(newVariableIdCode);

    // assign one vari-id for entire array
    mappingVarIdToInfo[tmpVarId].sym=sym;
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
  // ensure every symbol is only registered once (symbols of linked global variables can be processed multiple times)
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

    ROSE_ASSERT(sym);
    // exclude temporary variables from link analysis
    if(!isTemporaryVariableIdSymbol(sym)) {
      // determine if sym is declared in global scope and store information required for link analysis
      // this check only succeeds for global variables (it filters member variables)
      SgScopeStatement* scope=sym->get_scope();
      if(isSgGlobal(scope)) {
        SgName name=sym->get_mangled_name();
        mappingGlobalVarNameToSymSet[name].insert(sym); // set of symbols that map to a variable with the same name
      }
    }
    
    VariableId newVarId=variableId(sym);
    // set size to 1 (to compute bytes, multiply by size of type)
    setNumberOfElements(newVarId,unknownSizeValue()); // unknown number of elements
    // Mapping in both directions must be possible:
    ROSE_ASSERT(mappingSymToVarId.at(mappingVarIdToInfo[variableIdFromCode(newIdCode)].sym) == variableIdFromCode(newIdCode));
    ROSE_ASSERT(mappingVarIdToInfo[mappingSymToVarId.at(sym)].sym == sym);
  }
}

// we use a function as a destructor may delete it multiple times
void VariableIdMapping::deleteUniqueTemporaryVariableId(VariableId varId) {
  if(isTemporaryVariableId(varId)) {
    //cerr<<"DEBUG WARNING: not deleting temporary variable id symbol."<<endl;
    //delete getSymbol(varId);
  } else {
    throw CodeThorn::Exception("VariableIdMapping::deleteUniqueTemporaryVariableSymbol: improper id operation.");
  }
}

VariableIdMapping::UniqueTemporaryVariableSymbol::UniqueTemporaryVariableSymbol(string name) : SgVariableSymbol() {
  _tmpName=name;
}

SgName VariableIdMapping::UniqueTemporaryVariableSymbol::get_name() const {
  return SgName(_tmpName);
}

VariableIdMapping::VariableIdInfo::VariableIdInfo():
  sym(0),
  numberOfElements(VariableIdMapping::unknownSizeValue()),
  elementSize(VariableIdMapping::unknownSizeValue()),
  totalSize(VariableIdMapping::unknownSizeValue()),
  offset(VariableIdMapping::unknownSizeValue()),
  aggregateType(AT_UNKNOWN),
  variableScope(VS_UNKNOWN),
  isVolatileFlag(false),
  relinked(false)
{
}

VariableId::VariableId():_id(-1) {
}

const char * const VariableId::idKindIndicator = "V";

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

size_t CodeThorn::hash_value(const VariableId& vid) {
  return vid.getIdCode();
}

VariableIdMapping::VariableIdInfo VariableIdMapping::getVariableIdInfo(VariableId vid) {
  return *getVariableIdInfoPtr(vid);
}

VariableIdMapping::VariableIdInfo* VariableIdMapping::getVariableIdInfoPtr(VariableId vid) {
  return &mappingVarIdToInfo[vid];
}

void VariableIdMapping::setVariableIdInfo(VariableId vid, VariableIdInfo vif) {
  mappingVarIdToInfo[vid]=vif;
}
  
std::string VariableIdMapping::VariableIdInfo::aggregateTypeToString() {
  switch(aggregateType) {
  case AT_UNKNOWN:
    return "unknown";
  case AT_SINGLE:
    return "single";
  case AT_ARRAY:
    return "array";
  case AT_STRUCT:
    return "struct";
  }
  return "undefined-aggregate-type-enum";
}

std::string VariableIdMapping::VariableIdInfo::variableScopeToString() {
  switch(variableScope) {
  case VS_UNKNOWN:
    return "unknown";
  case VS_LOCAL:
    return "local";
  case VS_GLOBAL:
    return "global";
  case VS_MEMBER:
    return "member";
  }
  return "undefined-variable-scope-enum";
}

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
  ast.setWithTemplates(true);
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

bool VariableIdMapping::hasAssignInitializer(VariableId arrayVar) {
  SgVariableDeclaration* decl=this->getVariableDeclaration(arrayVar);
  return SgNodeHelper::hasAssignInitializer(decl);
}

bool VariableIdMapping::isAggregateWithInitializerList(VariableId arrayVar) {
 SgVariableDeclaration* decl=this->getVariableDeclaration(arrayVar);
 return SgNodeHelper::isAggregateDeclarationWithInitializerList(decl);
}

SgExpressionPtrList& VariableIdMapping::getInitializerListOfArrayVariable(VariableId arrayVar) {
  ROSE_ASSERT(isAggregateWithInitializerList(arrayVar));
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
  ast.setWithTemplates(true);
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
        int elementSize=1; // char
        setElementSize(newVariableId,elementSize);
        setTotalSize(newVariableId,getNumberOfElements(newVariableId)*elementSize);
        setOffset(newVariableId,unknownSizeValue());
        getVariableIdInfoPtr(newVariableId)->aggregateType=AT_ARRAY; // string literals are maintained as arrays with known length (includes terminating 0)

        // ensure that maps being built for mapping in both directions are of same size
        ROSE_ASSERT(sgStringValueToVariableIdMapping.size()==variableIdToSgStringValueMapping.size());
      } else {
        // already registered
      }
    }
  }
}
