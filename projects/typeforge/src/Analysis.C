
#include "sage3basic.h"
#include "Analysis.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include <boost/graph/graphviz.hpp>
#include "ToolConfig.hpp"

#ifndef DEBUG__statics__isArrayPointerType
#  define DEBUG__statics__isArrayPointerType 1
#endif
#ifndef DEBUG__statics__getNodeLabel
#  define DEBUG__statics__getNodeLabel 1
#endif
#ifndef DEBUG__Analysis
#  define DEBUG__Analysis 1
#endif
#ifndef DEBUG__Analysis__variableSetAnalysis
#  define DEBUG__Analysis__variableSetAnalysis DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__linkVariables
#  define DEBUG__Analysis__linkVariables DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__addEdge
#  define DEBUG__Analysis__addEdge DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__addNode
#  define DEBUG__Analysis__addNode DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__getScope
#  define DEBUG__Analysis__getScope DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__getType
#  define DEBUG__Analysis__getType DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__getPosition
#  define DEBUG__Analysis__getPosition DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__getClass
#  define DEBUG__Analysis__getClass DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__getHandle
#  define DEBUG__Analysis__getHandle DEBUG__Analysis
#endif
#ifndef DEBUG__Analysis__getNode
#  define DEBUG__Analysis__getNode DEBUG__Analysis
#endif

namespace Typeforge {

using namespace std;

//Returns the base type of the given type or nullptr if it is the base type
static SgType * getBaseType(SgType* type){
  if(SgArrayType* arrayType = isSgArrayType(type)) return arrayType->get_base_type();
  if(SgPointerType* ptrType = isSgPointerType(type)) return ptrType->get_base_type();
  if(SgTypedefType* typeDef = isSgTypedefType(type)) return typeDef->get_base_type();
  if(SgReferenceType* refType = isSgReferenceType(type)) return refType->get_base_type();
  if(SgModifierType* modType = isSgModifierType(type)) return modType->get_base_type();
  return nullptr;
}

//Method to compare if two types could be interconnected currently just looks at base type
static bool sameType(SgType* typeOne, SgType* typeTwo){
  return typeOne == nullptr || typeTwo == nullptr || typeOne == typeTwo || typeOne->findBaseType() == typeTwo->findBaseType();
}

//returns true if the type contains a pointer or array
static bool isArrayPointerType(SgType* type) {
  if(type == nullptr) return false;

#if DEBUG__statics__isArrayPointerType
  std::cout << "isArrayPointerType(" << type->class_name() << " * type = " << type << ")" << std::endl;
#endif

  if (SgClassType * xtype = isSgClassType(type)) {
    SgDeclarationStatement * decl_stmt = xtype->get_declaration();
    assert(decl_stmt != nullptr);

#if DEBUG__statics__isArrayPointerType
  std::cout << "  decl_stmt = " << decl_stmt << " (" << decl_stmt->class_name() << ")" << std::endl;
#endif

    SgTemplateInstantiationDecl * ti_decl = isSgTemplateInstantiationDecl(decl_stmt);
    if (ti_decl == nullptr) return false;

#if DEBUG__statics__isArrayPointerType
  std::cout << "  ti_decl   = " << ti_decl << " (" << ti_decl->class_name() << ")" << std::endl;
#endif
 
    SgTemplateClassDeclaration * td_decl = ti_decl->get_templateDeclaration();
    assert(td_decl != nullptr);

#if DEBUG__statics__isArrayPointerType
  std::cout << "  td_decl   = " << td_decl << " (" << td_decl->class_name() << ")" << std::endl;
  std::cout << "      ->get_qualified_name() = " << td_decl->get_qualified_name() << std::endl;
#endif

    return td_decl->get_qualified_name() == "::std::vector";
  }

  if(isSgArrayType(type)) return true;
  if(isSgPointerType(type)) return true;

  return isArrayPointerType(getBaseType(type));
}

Analysis::node_tuple_t::node_tuple_t(SgNode * n) :
  handle(),
  cname(n->class_name()),
  position(),
  scope(nullptr),
  type(nullptr)
{

#if DEBUG__statics__getNodeLabel
  std::cout << "Analysis::node_tuple_t::node_tuple_t(" << n->class_name() << " * n = " << n << "):" << std::endl;
#endif

  SgLocatedNode * lnode = isSgLocatedNode(n);
  assert(lnode != nullptr);

  {
    std::ostringstream oss;
    oss << lnode->get_endOfConstruct()->get_filenameString()
        << ":" << lnode->get_startOfConstruct()->get_raw_line()
        << ":" << lnode->get_startOfConstruct()->get_raw_col()
        << ":" << lnode->get_endOfConstruct()->get_raw_line()
        << ":" << lnode->get_endOfConstruct()->get_raw_col();
    position = oss.str();
  }

#if DEBUG__statics__getNodeLabel
  std::cout << "  position   = " << position << std::endl;
#endif

  SgExpression * expr = isSgExpression(n);
  SgInitializedName * iname = isSgInitializedName(n);
  SgVariableDeclaration * vdecl = isSgVariableDeclaration(n);
  SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(n);
  if (vdecl != nullptr) {
    iname = SgNodeHelper::getInitializedNameOfVariableDeclaration(vdecl);
  }

  if (expr != nullptr) {
    handle = "?z<" + position + ">";
    type = expr->get_type();
    scope = iname->get_scope();
  } else if (iname != nullptr) {
    handle = iname->get_qualified_name().getString();
    scope = iname->get_scope();
    type = iname->get_type();
  } else if (fdecl != nullptr) {
    type = fdecl->get_type();
    handle = fdecl->get_qualified_name().getString();
  } else {
    assert(false);
  }

#if DEBUG__statics__getNodeLabel
  std::cout << "  handle   = " << handle << std::endl;
  std::cout << "  scope    = " << scope << " (" << ( scope ? scope->class_name() : "") << ")" << std::endl;
  std::cout << "  type     = " << type  << " (" << ( type  ? type->class_name()  : "") << ") : " << ( type  ? type->unparseToString()  : "") << std::endl;
#endif

  while (scope != nullptr) {
    SgFunctionDefinition * fdefn = isSgFunctionDefinition(scope);
    SgClassDefinition * xdefn = isSgClassDefinition(scope);
    SgNamespaceDefinitionStatement * ndefn = isSgNamespaceDefinitionStatement(scope);
    SgGlobal * glob = isSgGlobal(scope);
    if (fdefn != nullptr) {
      handle = fdefn->get_declaration()->get_qualified_name() + "::" + handle;
      scope = nullptr;
    } else if (glob != nullptr) {
//    handle = scope + "::" + handle;
      scope = nullptr;
    } else if (xdefn != nullptr) {
//    handle = xdefn->get_declaration()->get_qualified_name() + "::" + handle;
      scope = nullptr;
    } else if (ndefn != nullptr) {
//    handle = ndefn->get_declaration()->get_qualified_name() + "::" + handle;
      scope = nullptr;
    } else {
      scope = scope->get_scope();
    }

#if DEBUG__statics__getNodeLabel
    if (scope != nullptr) {
      std::cout << "  - scope  = " << scope << " (" << scope->class_name() << ")" << std::endl;
    }
#endif
  }
}

void Analysis::initialize(SgProject * p) {
  if (p != nullptr) {
    assert(::Typeforge::project == nullptr || ::Typeforge::project == p);
    ::Typeforge::project = p;
  }
  assert(::Typeforge::project != nullptr);

  for (auto g : SgNodeHelper::listOfSgGlobal(project)) {
    Analysis::traverse(g);
  }
}

void Analysis::traverse(SgGlobal * g) {
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "Analysis::variableSetAnalysis" << std::endl;
  std::cout << "  g   = " << g << std::endl;
#endif

  RoseAst wholeAST(g);

  list<SgVariableDeclaration*> listOfVars = SgNodeHelper::listOfGlobalVars(g);
  listOfVars.splice(listOfVars.end(), SgNodeHelper::listOfGlobalFields(g));
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "  listOfVars.size() = " << listOfVars.size() << std::endl;
#endif
  for (auto varDec : listOfVars) {

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

    addNode(varDec);

    SgInitializer* init = initName->get_initializer();
    if (init == nullptr) {
      continue;
    }

#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    init = " << init << " ( " << init->class_name() << " ) = " << init->unparseToString() << std::endl;
#endif

    SgType* keyType = initName->get_type();
    if (keyType == nullptr) {
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

  list<SgFunctionDefinition*> listOfFunctionDefinitions = SgNodeHelper::listOfFunctionDefinitions(g);
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

	  key = fsym->get_declaration();
        }

        exp = assignOp->get_rhs_operand();
      } else if (SgVariableDeclaration* varDec = isSgVariableDeclaration(n)) {
        SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
        if(!initName) {
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
          addNode(*initIter);
          if ( isArrayPointerType((*initIter)->get_type()) ) {
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
      if (key) {
        addNode(key);
      }

      if (key && keyType && exp) {
        assert(stack.empty());
        linkVariables(key, keyType, exp);
        assert(stack.empty());
      }
    }
  }
}

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

  assert(node_map.find(k) != node_map.end());
  assert(node_map.find(t) != node_map.end());

  edges[k][t].push_back(stack);
}

std::string Analysis::addNode(SgNode * n) {
  assert(n != nullptr);

#if DEBUG__Analysis__addNode
  std::cout << "Analysis::addNode:" << std::endl;
  std::cout << "  n = " << n << " ( " << n->class_name() << " )" << std::endl;
#endif

  auto i = node_map.find(n);
  if (i != node_map.end()) {
    std::string h = i->second.handle;
#if DEBUG__Analysis__addNode
    std::cout << "  h = " << h <<  " (found)" << std::endl;
#endif
    return h;
  } else {
    node_tuple_t nt(n);

    std::string const & h = nt.handle;
#if DEBUG__Analysis__addNode
    std::cout << "  h = " << h << " (created)" << std::endl;
#endif
    assert(handle_map.find(h) == handle_map.end());

    node_map.insert(std::pair<SgNode *, node_tuple_t>(n, nt));
    handle_map.insert(std::pair<std::string, SgNode *>(h, n));

    return h;
  }
}

SgNode * Analysis::getNode(std::string const & h) const {
  SgNode * n = nullptr;

#if DEBUG__Analysis__getNode
  std::cout << "Analysis::getNode:" << std::endl;
  std::cout << "  h = " << h << std::endl;
#endif

  auto i = handle_map.find(h);
  if (i != handle_map.end()) {
    n = i->second;
  }

#if DEBUG__Analysis__getNode
  std::cout << "  n = " << n << " ( " << ( n != nullptr ? n->class_name() : "" ) << " )" << std::endl;
#endif

  return n;
}

std::string Analysis::getHandle(SgNode * n) const {
  std::string h;

#if DEBUG__Analysis__getHandle
  std::cout << "Analysis::getHandle:" << std::endl;
  std::cout << "  n = " << n << " ( " << ( n != nullptr ? n->class_name() : "" ) << " )" << std::endl;
#endif

  auto i = node_map.find(n);
  if (i != node_map.end()) {
    h = i->second.handle;
  }

#if DEBUG__Analysis__getHandle
  std::cout << "  h = " << h << std::endl;
#endif

  return h;
}

std::string Analysis::getClass(SgNode * n) const {
  std::string r;

#if DEBUG__Analysis__getClass
  std::cout << "Analysis::getClass:" << std::endl;
  std::cout << "  n = " << n << " ( " << ( n != nullptr ? n->class_name() : "" ) << " )" << std::endl;
#endif

  auto i = node_map.find(n);
  if (i != node_map.end()) {
    r = i->second.cname;
  }

#if DEBUG__Analysis__getClass
  std::cout << "  r = " << r << std::endl;
#endif

  return r;
}

std::string Analysis::getPosition(SgNode * n) const {
  std::string r;

#if DEBUG__Analysis__getPosition
  std::cout << "Analysis::getPosition:" << std::endl;
  std::cout << "  n = " << n << " ( " << ( n != nullptr ? n->class_name() : "" ) << " )" << std::endl;
#endif

  auto i = node_map.find(n);
  if (i != node_map.end()) {
    r = i->second.position;
  }

#if DEBUG__Analysis__getPosition
  std::cout << "  r = " << r << std::endl;
#endif

  return r;
}

SgType * Analysis::getType(SgNode * n) const {
  SgType * t = nullptr;

#if DEBUG__Analysis__getType
  std::cout << "Analysis::getType:" << std::endl;
  std::cout << "  n = " << n << " ( " << ( n != nullptr ? n->class_name() : "" ) << " )" << std::endl;
#endif

  auto i = node_map.find(n);
  if (i != node_map.end()) {
    t = i->second.type;
  }

#if DEBUG__Analysis__getType
  std::cout << "  t = " << t << " ( " << ( t != nullptr ? t->class_name() : "" ) << " )" << std::endl;
#endif

  return t;
}

SgScopeStatement * Analysis::getScope(SgNode * n) const {
  SgScopeStatement * s = nullptr;

#if DEBUG__Analysis__getScope
  std::cout << "Analysis::getScope:" << std::endl;
  std::cout << "  n = " << n << " ( " << ( n != nullptr ? n->class_name() : "" ) << " )" << std::endl;
#endif

  auto i = node_map.find(n);
  if (i != node_map.end()) {
    s = i->second.scope;
  }

#if DEBUG__Analysis__getScope
  std::cout << "  s = " << s << " ( " << ( s != nullptr ? s->class_name() : "" ) << " )" << std::endl;
#endif

  return s;
}

void Analysis::toDot(std::string const & fileName) const {
  fstream dotfile;
  dotfile.open(fileName, ios::out | ios::trunc);

  dotfile << "digraph {" << std::endl;
  dotfile << "  ranksep=5;" << std::endl;

  for (auto n: node_map) {
    assert(n.first != nullptr);

    dotfile << "  n_" << n.first << " [label=\"" << n.second.handle << "\\n" << n.second.cname << "\\n" << n.second.position;
    if (n.second.type != nullptr) {
      dotfile << "\\n" << n.second.type->unparseToString();
    }
    dotfile << "\"];" << std::endl;

    auto edges__ = edges.find(n.first);
    if (edges__ != edges.end()) {
      for (auto target_stack: edges__->second) {
        auto t = target_stack.first;
        auto stacks = target_stack.second;
        dotfile << "    n_" << n.first << " -> n_" << t << "[label=\"";
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

SgProject * project;
Analysis typechain;

}

