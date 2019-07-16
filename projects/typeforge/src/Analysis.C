#include "sage3basic.h"
#include "Analysis.h"
#include "TFHandles.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include <boost/graph/graphviz.hpp>
#include "ToolConfig.hpp"

#define DEBUG__Analysis 0

namespace Typeforge {

using namespace std;

//Returns the base type of the given type or nullptr if it is the base type
SgType* getBaseType(SgType* type){
  if(SgArrayType* arrayType = isSgArrayType(type)) return arrayType->get_base_type();
  if(SgPointerType* ptrType = isSgPointerType(type)) return ptrType->get_base_type();
  if(SgTypedefType* typeDef = isSgTypedefType(type)) return typeDef->get_base_type();
  if(SgReferenceType* refType = isSgReferenceType(type)) return refType->get_base_type();
  if(SgModifierType* modType = isSgModifierType(type)) return modType->get_base_type();
  return nullptr;
}

#define DEBUG__isArrayPointerType 0

//returns true if the type contains a pointer or array
bool isArrayPointerType(SgType* type) {
  if(type == nullptr) return false;

#if DEBUG__isArrayPointerType
  std::cout << "isArrayPointerType(" << type->class_name() << " * type = " << type << ")" << std::endl;
#endif

  if (SgClassType * xtype = isSgClassType(type)) {
    SgDeclarationStatement * decl_stmt = xtype->get_declaration();
    assert(decl_stmt != nullptr);

#if DEBUG__isArrayPointerType
  std::cout << "  decl_stmt = " << decl_stmt << " (" << decl_stmt->class_name() << ")" << std::endl;
#endif

    SgTemplateInstantiationDecl * ti_decl = isSgTemplateInstantiationDecl(decl_stmt);
    if (ti_decl == nullptr) return false;

#if DEBUG__isArrayPointerType
  std::cout << "  ti_decl   = " << ti_decl << " (" << ti_decl->class_name() << ")" << std::endl;
#endif
 
    SgTemplateClassDeclaration * td_decl = ti_decl->get_templateDeclaration();
    assert(td_decl != nullptr);

#if DEBUG__isArrayPointerType
  std::cout << "  td_decl   = " << td_decl << " (" << td_decl->class_name() << ")" << std::endl;
  std::cout << "      ->get_qualified_name() = " << td_decl->get_qualified_name() << std::endl;
#endif

    return td_decl->get_qualified_name() == "::std::vector";
  }

  if(isSgArrayType(type)) return true;
  if(isSgPointerType(type)) return true;

  return isArrayPointerType(getBaseType(type));
}

//Method to compare if two types could be interconnected currently just looks at base type
static bool sameType(SgType* typeOne, SgType* typeTwo){
  return typeOne == nullptr || typeTwo == nullptr || typeOne == typeTwo || typeOne->findBaseType() == typeTwo->findBaseType();
}

//given a node finds the enclosing function's name
string getFunctionNameOfNode(SgNode* node){
  SgFunctionDefinition* funDef = SgNodeHelper::getClosestParentFunctionDefinitionOfLocatedNode(isSgLocatedNode(node));
  if(!funDef){
    SgNode* parent = node;
    while(parent != nullptr){
      parent = parent->get_parent();
      if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(parent)){
        return SgNodeHelper::getFunctionName(funDec);
      }
    }
    return "$global";
  }
  else return SgNodeHelper::getFunctionName(funDef);
}

//Returns true if the sets intersect
bool setIntersect(const std::set<SgNode*> & set1, const std::set<SgNode*> & set2){
  for(auto i = set2.begin(); i != set2.end(); ++i){
    if(set1.count(*i)) return true;
  }
  return false;
}

//add all elements of set2 to set1
void inPlaceUnion(set<SgNode*> & set1, set<SgNode*> & set2){
  for(auto i = set2.begin(); i != set2.end(); ++i){
    set1.insert(*i);
  }
}

//returns a new set that contains the same elements
set<SgNode*>* copySet(set<SgNode*>* oldSet){
  set<SgNode*>* newSet = new set<SgNode*>;
  for(auto i = oldSet->begin(); i != oldSet->end(); ++i){
    newSet->insert(*i);
  }
  return newSet;
}

#define DEBUG__Analysis__variableSetAnalysis DEBUG__Analysis

//searches for locations where types may be connected through assignment, passing as argument and returns
//then passes the associated node along with the expression to link variables.
int Analysis::variableSetAnalysis(SgProject* project, SgType * matchType) {
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "Analysis::variableSetAnalysis" << std::endl;
  std::cout << "  project   = " << project << std::endl;
  if (matchType != nullptr) {
    std::cout << "  matchType = " << matchType << " ( " << matchType->class_name() << " ) = " << matchType->unparseToString() << std::endl;
  }
#endif

  RoseAst wholeAST(project);

  list<SgVariableDeclaration*> listOfVars = SgNodeHelper::listOfGlobalVars(project);
  listOfVars.splice(listOfVars.end(), SgNodeHelper::listOfGlobalFields(project));
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "  listOfVars.size() = " << listOfVars.size() << std::endl;
#endif
  for (auto varDec : listOfVars) {
    if (!SgNodeHelper::node_can_be_changed(varDec)) {
      continue;
    }

#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    varDec   = " << varDec << " ( " << varDec->class_name() << " )" << std::endl;
#endif

    SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
    if (initName == nullptr) {
      continue;
    }

#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    initName = " << initName << " ( " << initName->class_name() << " ) = " << initName->get_name() << std::endl;
#endif

    SgInitializer* init = initName->get_initializer();
    if (init == nullptr) {
      continue;
    }

#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    init = " << init << " ( " << init->class_name() << " ) = " << init->unparseToString() << std::endl;
#endif

    SgType* keyType = initName->get_type();
    if (keyType == nullptr || !sameType(keyType, matchType)) {
      continue;
    }

#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    keyType = " << keyType << " ( " << keyType->class_name() << " ) = " << keyType->unparseToString() << std::endl;
#endif

    if (!isArrayPointerType(keyType)) {
      continue;
    }

    assert(stack.empty());
    linkVariables(varDec, keyType, init);
    assert(stack.empty());
  }

  list<SgFunctionDefinition*> listOfFunctionDefinitions = SgNodeHelper::listOfFunctionDefinitions(project);
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "  listOfFunctionDefinitions.size() = " << listOfFunctionDefinitions.size() << std::endl;
#endif

  for (auto funDef : listOfFunctionDefinitions) {
#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    funDef   = " << funDef << " ( " << funDef->class_name() << " )" << std::endl;
#endif

    RoseAst ast(funDef);
    for (auto n : ast) {
#if DEBUG__Analysis__variableSetAnalysis
//    std::cout << "      n       = " << n << " ( " << n->class_name() << " )" << std::endl;
#endif
      SgNode* key = nullptr;
      SgType* keyType = nullptr;
      SgExpression* exp = nullptr;
      if (SgAssignOp * assignOp = isSgAssignOp(n)) {
        SgExpression * lhs = assignOp->get_lhs_operand();
#if DEBUG__Analysis__variableSetAnalysis
        std::cout << "      lhs     = " << lhs << " ( " << (lhs ? lhs->class_name() : "") << " )" << std::endl;
#endif

        SgDotExp * dotexp = isSgDotExp(lhs);
        SgArrowExp * arrexp = isSgArrowExp(lhs);
        while (dotexp || arrexp) {
#if DEBUG__Analysis__variableSetAnalysis
          if (dotexp) std::cout << "      dotexp  = " << dotexp << " ( " << (dotexp ? dotexp->class_name() : "") << " )" << std::endl;
          if (arrexp) std::cout << "      arrexp  = " << arrexp << " ( " << (arrexp ? arrexp->class_name() : "") << " )" << std::endl;
#endif
          if (dotexp) lhs = dotexp->get_rhs_operand_i();
          if (arrexp) lhs = arrexp->get_rhs_operand_i();

          dotexp = isSgDotExp(lhs);
          arrexp = isSgArrowExp(lhs);
        }

        if (SgVarRefExp* varRef = isSgVarRefExp(lhs)) {
#if DEBUG__Analysis__variableSetAnalysis
          std::cout << "      varRef  = " << varRef << " ( " << (varRef ? varRef->class_name() : "") << " )" << std::endl;
#endif

          keyType = varRef->get_type();
#if DEBUG__Analysis__variableSetAnalysis
          std::cout << "      keyType = " << keyType << " ( " << (keyType ? keyType->class_name() : "") << " )" << std::endl;
#endif

          if (!isArrayPointerType(keyType)) {
            continue;
          }

          SgVariableSymbol* varSym = varRef->get_symbol();
          assert(varSym != nullptr);

          SgInitializedName * iname = varSym->get_declaration();
          assert(iname != nullptr);

          if (!SgNodeHelper::node_can_be_changed(iname)) {
            continue;
          }

          if (!SgNodeHelper::node_can_be_changed(iname)) {
            continue;
          }

	  key = iname->get_declaration(); // get variable decl if exist
          if (key == nullptr) {
            key = iname; // case of a function parameter (or non-type template parameter)
          }
        }

        if (SgFunctionRefExp * fref = isSgFunctionRefExp(lhs)) {
#if DEBUG__Analysis__variableSetAnalysis
          std::cout << "      fref    = " << fref << " ( " << (fref ? fref->class_name() : "") << " )" << std::endl;
#endif
          keyType = fref->get_type();
#if DEBUG__Analysis__variableSetAnalysis
          std::cout << "      keyType = " << keyType << " ( " << (keyType ? keyType->class_name() : "") << " )" << std::endl;
#endif
          if (!isArrayPointerType(keyType)) {
            continue;
          }

          SgFunctionSymbol * fsym = fref->get_symbol();
          assert(fsym != nullptr);

          if (!SgNodeHelper::node_can_be_changed(fsym->get_declaration())) {
            continue;
          }

	  key = fsym->get_declaration();
        }

        exp = assignOp->get_rhs_operand();
      } else if (SgVariableDeclaration* varDec = isSgVariableDeclaration(n)) {
        SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
        if(!initName) {
          continue;
        }

        if (!SgNodeHelper::node_can_be_changed(initName)) {
          continue;
        }

        SgInitializer* init = initName->get_initializer();
        if (!init) {
          continue;
        }

        keyType = initName->get_type();
        if (!isArrayPointerType(keyType)) {
          continue;
        }
#if DEBUG__Analysis__variableSetAnalysis
        std::cout << "      keyType = " << keyType << " ( " << keyType->class_name() << " )" << std::endl;
#endif

        key = initName->get_declaration();
        exp = init;
      } else if (SgFunctionCallExp* callExp = isSgFunctionCallExp(n)) {
        SgFunctionDefinition* funDef = SgNodeHelper::determineFunctionDefinition(callExp);
        if (!funDef) {
          continue;
        }
#if DEBUG__Analysis__variableSetAnalysis
        std::cout << "      funDef  = " << funDef << " ( " << funDef->class_name() << " )" << std::endl;
#endif

        SgInitializedNamePtrList& initNameList = SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
        SgExpressionPtrList& expList = callExp->get_args()->get_expressions();
        auto initIter = initNameList.begin();
        auto expIter  = expList.begin(); 
        while (initIter != initNameList.end()) {
          if (
              SgNodeHelper::node_can_be_changed(*initIter) &&
              isArrayPointerType((*initIter)->get_type()) &&
              sameType(matchType, (*initIter)->get_type())
          ) {
              assert(stack.empty());
              linkVariables((*initIter), (*initIter)->get_type(), (*expIter));
              assert(stack.empty());
          }
          ++initIter;
          ++expIter;
        }
      } else if(SgReturnStmt* ret = isSgReturnStmt(n)) {
        exp = ret->get_expression();
        keyType = exp->get_type();
        if (!isArrayPointerType(keyType)) {
          continue;
        }

        if (!SgNodeHelper::node_can_be_changed(funDef->get_declaration())) {
          continue;
        }

        key = funDef->get_declaration();
      }

#if DEBUG__Analysis__variableSetAnalysis
      if (key)
        std::cout << "      key     = " << key     << " ( " << key->class_name() << " )" << std::endl;
      if (keyType)
        std::cout << "      keyType = " << keyType << " ( " << keyType->class_name() << " )" << std::endl;
      if (exp)
        std::cout << "      exp     = " << exp     << " ( " << exp->class_name() << " )" << std::endl;
#endif

      if (!sameType(keyType, matchType)) {
        continue;
      }

      if (key && keyType && exp) {
        assert(stack.empty());
        linkVariables(key, keyType, exp);
        assert(stack.empty());
      }
    }
  }
/*
  for (auto i = setMap.begin(); i != setMap.end(); ++i) {
    bool intersect = false;
    set<SgNode*>* found = nullptr;
    for (auto j = listSets.begin(); j != listSets.end(); ++j) {
      intersect = setIntersect(*j, i->second);
      if (j->count(i->first)) {
        intersect = true;
      }

      if (found != nullptr && intersect) {
        inPlaceUnion(*found, i->second);
        inPlaceUnion(*found, *j);
        found->insert(i->first);
        j = listSets.erase(j);
        ++j;
      } else if (intersect) {
        inPlaceUnion(*j, i->second);
        j->insert(i->first);
        found = &(*j);
      }
    }

    if (!intersect) {
      listSets.push_back(i->second); // copy
      listSets.back().insert(i->first);
    }
  }
*/
  return 0;
}

//finds the set containing the given node
set<SgNode*> * Analysis::getSet(SgNode* node){
  for(auto i = listSets.begin(); i != listSets.end(); ++i){
    if(i->count(node)) return &(*i);
  }
  return nullptr;
}

//takes a set of nodes and makes a string representation of their names
string makeSetString(set<SgNode*> & variableSet){
  string setString = "";
  for (auto j = variableSet.begin(); j != variableSet.end(); ++j) {
    string name = "";
    string funName = getFunctionNameOfNode(*j) + ":"; 
    SgSymbol* varSym = nullptr;
    if(SgInitializedName* leftInit = isSgInitializedName(*j)) varSym = SgNodeHelper::getSymbolOfInitializedName(leftInit);
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(*j)) varSym = SgNodeHelper::getSymbolOfFunctionDeclaration(funDec);
    else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(*j)) varSym = SgNodeHelper::getSymbolOfVariableDeclaration(varDec);
    if(varSym) name = SgNodeHelper::symbolToString(varSym);
    
    if(setString != "") setString = setString + "==";
    setString = setString + funName + name; 
  }
  return setString;
}

//writes the sets to a file
void Analysis::writeAnalysis(SgType* type, string toTypeString){
  for(auto i = listSets.begin(); i != listSets.end(); ++i){
    string nameString = makeSetString(*i);
    string handle = TFHandles::getHandleVectorString(*i);
    // TODO ToolConfig::getGlobal()->addReplaceVarBaseType(handle, nameString);
  }
}

void Analysis::appendAnalysis(ToolConfig * tc) {
  for (auto e: edges) {
    auto key = e.first;
    std::ostringstream oss; oss << "set-analysis:" << key;
    std::string label = oss.str();
    tc->addLabel(key, label);
    for (auto target_stack: e.second) {
      auto target = target_stack.first;
//    auto stack = target_stack.second;
      tc->addLabel(target, label);
    }
  }
}

#define DEBUG__getNodeLabel 0

static std::string getNodeLabel(SgNode * n) {
#if DEBUG__getNodeLabel
  std::cout << "getNodeLabel(" << n->class_name() << " * n = " << n << "):" << std::endl;
#endif

  std::string name;
  SgScopeStatement * scope = nullptr;
  if (isSgVariableDeclaration(n)) {
    SgInitializedName * iname = SgNodeHelper::getInitializedNameOfVariableDeclaration((SgVariableDeclaration*)n);
    assert(iname != nullptr);
    name = iname->get_qualified_name().getString();
    scope = iname->get_scope();
  } else if (isSgInitializedName(n)) {
    name = ((SgInitializedName*)n)->get_qualified_name().getString();
    scope = ((SgInitializedName*)n)->get_scope();
  } else if (isSgFunctionDeclaration(n)) {
    name = ((SgFunctionDeclaration*)n)->get_qualified_name().getString();
  } else {
    std::ostringstream oss;
    oss << "(" << n->class_name() << "*)" << n;
    name = oss.str();
  }

#if DEBUG__getNodeLabel
  std::cout << "  name  = " << name << std::endl;
  std::cout << "  scope = " << scope << " (" << ( scope ? scope->class_name() : "") << ")" << std::endl;
#endif

  while (scope != nullptr) {
    SgFunctionDefinition * fdefn = isSgFunctionDefinition(scope);
    SgClassDefinition * xdefn = isSgClassDefinition(scope);
    SgNamespaceDefinitionStatement * ndefn = isSgNamespaceDefinitionStatement(scope);
    SgGlobal * glob = isSgGlobal(scope);
    if (fdefn != nullptr) {
      name = fdefn->get_declaration()->get_qualified_name() + "::" + name;
      scope = nullptr;
    } else if (glob != nullptr) {
//    name = scope + "::" + name;
      scope = nullptr;
    } else if (xdefn != nullptr) {
//    name = xdefn->get_declaration()->get_qualified_name() + "::" + name;
      scope = nullptr;
    } else if (ndefn != nullptr) {
//    name = ndefn->get_declaration()->get_qualified_name() + "::" + name;
      scope = nullptr;
    } else {
      scope = scope->get_scope();
    }

#if DEBUG__getNodeLabel
    if (scope != nullptr) {
      std::cout << "  - scope = " << scope << " (" << scope->class_name() << ")" << std::endl;
    }
#endif
  }

  std::string cname = n->class_name();

  std::string label;
  SgLocatedNode * lnode = isSgLocatedNode(n);
  if (lnode != nullptr) {
    std::ostringstream oss;
    oss << "\\n"  << lnode->get_endOfConstruct()->get_raw_line() << ":" << lnode->get_endOfConstruct()->get_raw_col();
//  oss << "\\n" << lnode->get_endOfConstruct()->get_filenameString() << ":"  << lnode->get_endOfConstruct()->get_raw_line() << ":" << lnode->get_endOfConstruct()->get_raw_col();
    label = oss.str();
  }

#if DEBUG__getNodeLabel
  std::cout << "  label = " << label << std::endl;
#endif

  return name + "\\n" + cname + label;
}

void Analysis::toDot(std::string const & fileName) const {
  fstream dotfile;
  dotfile.open(fileName, ios::out | ios::trunc);

  dotfile << "digraph {" << std::endl;
  dotfile << "  ranksep=5;" << std::endl;

  for (auto n: nodes) {
    assert(n != nullptr);

    dotfile << "  n_" << n << " [label=\"" << getNodeLabel(n) << "\"];" << std::endl;

    auto edges__ = edges.find(n);
    if (edges__ != edges.end()) {
      for (auto target_stack: edges__->second) {
        auto t = target_stack.first;
        auto stacks = target_stack.second;
        dotfile << "    n_" << n << " -> n_" << t << "[label=\"";
#if 0
        for (auto stack__: stacks) {
          for (auto s: stack__) {
            assert(s != nullptr);
            dotfile << s->unparseToString() << " - ";
          }
          dotfile << "";
        }
#endif
        dotfile << "\"];" << std::endl;
      }
    }
  }

  dotfile << "}" << std::endl;

  dotfile.close();
}

//writes a dot graph of the sets to the given file
void Analysis::writeGraph(string fileName){
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> SetGraph;
  SetGraph graph(0);
  map<SgNode*, SetGraph::vertex_descriptor> desMap;
  vector<string> names;
/*
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    string name = "";
    string funName = getFunctionNameOfNode(i->first);
    SgSymbol* varSym = nullptr;
    if(SgInitializedName* leftInit = isSgInitializedName(i->first)) varSym = SgNodeHelper::getSymbolOfInitializedName(leftInit);
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(i->first)) varSym = SgNodeHelper::getSymbolOfFunctionDeclaration(funDec);
    else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(i->first)) varSym = SgNodeHelper::getSymbolOfVariableDeclaration(varDec);
    if(varSym) name = SgNodeHelper::symbolToString(varSym);
    name = funName + "::" +name;
    SetGraph::vertex_descriptor vDes = boost::add_vertex(graph);
    desMap[i->first] = vDes;
    names.push_back(name);
  } 
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    for(auto j = i->second.begin(); j != i->second.end(); ++j){
      if(desMap[i->first] < desMap[*j]) boost::add_edge(desMap[i->first], desMap[*j], graph);
    }
  }
*/
  fstream fileStream;
  fileStream.open(fileName, ios::out | ios::trunc);
  boost::write_graphviz(fileStream, graph, boost::make_label_writer((&names[0])));
  fileStream.close();
}

#define DEBUG__Analysis__linkVariables DEBUG__Analysis

// Searches through the expression for variables of the given type then links them with the key node provided
void Analysis::linkVariables(SgNode * key, SgType * type, SgExpression * expression) {
#if DEBUG__Analysis__linkVariables
  std::cout << "Analysis::linkVariables( key = " << key << " (" << (key != nullptr ? key->class_name() : "") << ")" << std::endl;
  std::cout << "  type       = " << type       << " ( " << (type       != nullptr ? type->class_name()       : "") << " ) = " << type->unparseToString()       << std::endl;
  std::cout << "  expression = " << expression << " ( " << (expression != nullptr ? expression->class_name() : "") << " ) = " << expression->unparseToString() << std::endl;
  std::cout << "  STACK = [ " << std::endl;
  for (auto i: stack) {
    std::cout << "    " << i  << " (" << (i != nullptr ? i->class_name()  : "") << ") = " << i->unparseToString() << std::endl;
  }
  std::cout << " ]" << std::endl;
#endif

  addNode(key);

  stack.push_back(expression);

  RoseAst ast(expression);
  for (auto i = ast.begin(); i != ast.end(); ++i) {
    if (SgExpression * exp = isSgExpression(*i)) {
      SgType * etype = exp->get_type();
      if (etype && sameType(etype, type)) {
        if (exp != expression) {
          stack.push_back(exp);
        }

        if (SgFunctionCallExp* funCall = isSgFunctionCallExp(exp)) {
          SgFunctionDeclaration* funDec = funCall->getAssociatedFunctionDeclaration();
          SgFunctionDefinition* funDef = SgNodeHelper::determineFunctionDefinition(funCall);
          // TV: FIXME:
          //     - why do we need a definition?
          //     - what happen to the expressions used as argument of the function? (they are never seen because of the `skip`)
          if(funDef) {
            funDec = funDef->get_declaration();
            if (key != funDec) {
              addNode(funDec);
              addEdge(key, funDec);
            }
            i.skipChildrenOnForward();
          }
        } else if(SgVarRefExp* varRef = isSgVarRefExp(exp)) {
          SgVariableSymbol* varSym = varRef->get_symbol();
          if (varSym) {
            SgInitializedName * refInitName = varSym->get_declaration();
            SgNode * target = refInitName;
            if (!SgNodeHelper::isFunctionParameterVariableSymbol(varSym)) {
              target = refInitName->get_declaration();
            }
            if (target && key != target) {
              addNode(target);
              addEdge(key, target);
            }
          }
        } else if(SgPntrArrRefExp* refExp = isSgPntrArrRefExp(exp)) {
          linkVariables(key, refExp->get_lhs_operand()->get_type(), refExp->get_lhs_operand());
          i.skipChildrenOnForward(); // FIXME what about the RHS? (aka index)
        } else if(SgPointerDerefExp* refExp = isSgPointerDerefExp(exp)) {
          linkVariables(key, refExp->get_operand()->get_type(), refExp->get_operand());
          i.skipChildrenOnForward();
        }

        if (exp != expression) {
          stack.pop_back();
        }
      }
    }
  }
  stack.pop_back();
}

#define DEBUG__Analysis__addEdge DEBUG__Analysis

void Analysis::addEdge(SgNode * k, SgNode * t) {
#if DEBUG__Analysis__addEdge
  std::cout << "Analysis::addEdge "
            << k << " (" << (k != nullptr ? k->class_name() : "") << ") -> "
            << t << " (" << (t != nullptr ? t->class_name() : "") << ")"
            << std::endl;
  std::cout << "  STACK = [ " << std::endl;
  for (auto i: stack) {
    std::cout << "    " << i  << " (" << (i != nullptr ? i->class_name()  : "") << ") = " << i->unparseToString() << std::endl;
  }
  std::cout << " ]" << std::endl;
#endif

  assert(nodes.find(k) != nodes.end());
  assert(nodes.find(t) != nodes.end());

  edges[k][t].push_back(stack);
}

#define DEBUG__Analysis__addNode DEBUG__Analysis

void Analysis::addNode(SgNode * n) {
  if (nodes.find(n) != nodes.end()) return;

#if DEBUG__Analysis__addNode
  std::cout << "Analysis::addNode:" << std::endl;
  std::cout << "  n = " << n << " ( " << (n != nullptr ? n->class_name() : "") << " )" << std::endl;
#endif

  nodes.insert(n);
}

}

