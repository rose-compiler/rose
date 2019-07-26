
#include "sage3basic.h"
#include "Analysis.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include <boost/graph/graphviz.hpp>
#include "ToolConfig.hpp"

#ifndef DEBUG__statics__isArrayPointerType
#  define DEBUG__statics__isArrayPointerType 0
#endif
#ifndef DEBUG__statics__getNodeLabel
#  define DEBUG__statics__getNodeLabel 0
#endif
#ifndef DEBUG__ignoreNode
#  define DEBUG__ignoreNode 0
#endif
#ifndef DEBUG__Analysis
#  define DEBUG__Analysis 0
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
#ifndef DEBUG__isTypeBasedOn
#  define DEBUG__isTypeBasedOn DEBUG__Analysis
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

SgType * stripType(SgType * type, bool strip_std_vector) {
  assert(type != NULL);

#if DEBUG__isTypeBasedOn
  std::cout << "Typeforge::stripType" << std::endl;
  std::cout << "  type      = " << type << " ( " << type->class_name() << "): " << type->unparseToString() << "" << std::endl;
#endif

  type = type->stripType(
    SgType::STRIP_ARRAY_TYPE     |
    SgType::STRIP_POINTER_TYPE   |
    SgType::STRIP_MODIFIER_TYPE  |
    SgType::STRIP_REFERENCE_TYPE |
    SgType::STRIP_RVALUE_REFERENCE_TYPE
  );

  SgTypedefType * td_type = isSgTypedefType(type);
  SgClassType * xtype = isSgClassType(type);
  if (td_type != nullptr) {
    type = stripType(td_type->get_base_type(), strip_std_vector);
  } else if (strip_std_vector && xtype != nullptr) {
    SgDeclarationStatement * decl_stmt = xtype->get_declaration();
    assert(decl_stmt != nullptr);

#if DEBUG__isTypeBasedOn
    std::cout << "  decl_stmt = " << decl_stmt << " ( " << decl_stmt->class_name() << "): " << decl_stmt->unparseToString() << "" << std::endl;
#endif

    SgTemplateInstantiationDecl * ti_decl = isSgTemplateInstantiationDecl(decl_stmt);
    if (ti_decl != nullptr) {
#if DEBUG__isTypeBasedOn
      std::cout << "      ->get_qualified_name() = " << ti_decl->get_qualified_name() << std::endl;
      std::cout << "      ->get_name()           = " << ti_decl->get_name() << std::endl;
#endif

      SgTemplateClassDeclaration * td_decl = ti_decl->get_templateDeclaration();
      assert(td_decl != nullptr);

#if DEBUG__isTypeBasedOn
      std::cout << "  td_decl   = " << td_decl << " ( " << td_decl->class_name() << "): " << td_decl->unparseToString() << "" << std::endl;
      std::cout << "      ->get_qualified_name() = " << td_decl->get_qualified_name() << std::endl;
#endif

      if (td_decl->get_qualified_name() == "::std::vector") {
        auto tpl_args = ti_decl->get_templateArguments();
        assert(tpl_args.size() > 0);

        SgType * tpl_type_arg = tpl_args[0]->get_type();
        assert(tpl_type_arg != nullptr);

        type = stripType(tpl_type_arg, false);
      }
    }
  }

  return type;
}

bool isTypeBasedOn(SgType * type, SgType * base, bool strip_type) {
  if (base == nullptr) return true;

  assert(type != NULL);

#if DEBUG__isTypeBasedOn
  std::cout << "Typeforge::isTypeBasedOn" << std::endl;
  std::cout << "  type      = " << type << " ( " << type->class_name() << "): " << type->unparseToString() << "" << std::endl;
  std::cout << "  base      = " << base << " ( " << base->class_name() << "): " << base->unparseToString() << "" << std::endl;
#endif

  if (strip_type) {
    type = stripType(type, true);
  }

  return type == base;
}

Analysis::node_tuple_t::node_tuple_t(SgNode * n) :
  handle(),
  cname(n->class_name()),
  position(),
  scope(nullptr),
  type(nullptr)
{

  // This object is used to unparse type properly
  SgUnparse_Info * uinfo = new SgUnparse_Info();
      uinfo->set_SkipComments();
      uinfo->set_SkipWhitespaces();
      uinfo->set_SkipEnumDefinition();
      uinfo->set_SkipClassDefinition();
      uinfo->set_SkipFunctionDefinition();
      uinfo->set_SkipBasicBlock();
      uinfo->set_isTypeFirstPart();

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
  SgClassDeclaration * xdecl = isSgClassDeclaration(n);
  SgNamespaceDeclarationStatement * ndecl = isSgNamespaceDeclarationStatement(n);

  if (expr != nullptr) {
    handle = "?<" + position + ">";
    type = expr->get_type();
    scope = nullptr; // FIXME do we want to prepend the scope of the expression?

  } else if (ndecl != nullptr) {
    type = ndecl->get_type();
    handle = ndecl->get_qualified_name().getString();
    scope = nullptr;
    ndecl = nullptr;

  } else if (iname != nullptr) {
    SgFunctionParameterList * fplst = isSgFunctionParameterList(iname->get_parent());
    assert(fplst != nullptr);
    fdecl = isSgFunctionDeclaration(fplst->get_parent());
    assert(fdecl != nullptr);

    auto it = std::find(fplst->get_args().begin(), fplst->get_args().end(), iname);
    assert(it != fplst->get_args().end());
    auto pos = it - fplst->get_args().begin();

    std::ostringstream oss;
    oss << pos;
    handle = oss.str();
    type = iname->get_type();
    scope = nullptr;

  } else if (xdecl != nullptr) {
    type = xdecl->get_type();
    handle = xdecl->get_qualified_name().getString();
    scope = nullptr;
    fdecl = nullptr;

  } else if (fdecl != nullptr) {
    type = fdecl->get_type()->get_return_type();
    std::ostringstream oss;
    oss << fdecl->get_qualified_name().getString() << "(";
    for (auto t : fdecl->get_type()->get_argument_list()->get_arguments()) {
      oss << globalUnparseToString(t, uinfo) << ",";
    }
    oss << ")" << handle;
    handle = oss.str();
    scope = nullptr;
    fdecl = nullptr;

  } else if (vdecl != nullptr) {
    iname = SgNodeHelper::getInitializedNameOfVariableDeclaration(vdecl);

    handle = iname->get_name().getString();
    type = iname->get_type();
    scope = vdecl->get_scope();
    assert(scope != nullptr);

  } else {
    assert(false);
  }

#if DEBUG__statics__getNodeLabel
  std::cout << "  type     = " << type  << " (" << ( type  ? type->class_name()  : "") << ") : " << ( type  ? type->unparseToString()  : "") << std::endl;
  std::cout << "  scope    = " << scope << " (" << ( scope ? scope->class_name() : "") << ")" << std::endl;
#endif

  SgScopeStatement * ascope = isSgScopeStatement(scope);
  assert(scope == nullptr || ascope != nullptr);

  while ( ascope && !isSgFunctionDefinition(ascope) && !isSgClassDefinition(ascope) && !isSgNamespaceDefinitionStatement(ascope) && !isSgGlobal(ascope) ) {

#if DEBUG__statics__getNodeLabel
    std::cout << "  -> ascope  = " << ascope << " (" << ascope->class_name() << ")" << std::endl;
#endif

    SgScopeStatement * pscope = ascope->get_scope();

#if DEBUG__statics__getNodeLabel
    std::cout << "  -> pscope  = " << pscope << " (" << ( pscope ? pscope->class_name() : "" ) << ")" << std::endl;
#endif

#define DEBUG__build_qualname_for_non_named_scopes 0
#if DEBUG__build_qualname_for_non_named_scopes
    SgFunctionDefinition * pfdefn = isSgFunctionDefinition(pscope);
    SgClassDefinition * pxdefn = isSgClassDefinition(pscope);
    SgNamespaceDefinitionStatement * pndefn = isSgNamespaceDefinitionStatement(pscope);
    SgGlobal * pglob = isSgGlobal(pscope);
#endif
    SgBasicBlock * pbb = isSgBasicBlock(pscope);
    std::ostringstream oss;
    if (pbb != nullptr) {
      auto stmts = pbb->getStatementList();
      auto it = std::find(stmts.begin(), stmts.end(), ascope);
      assert(it != stmts.end());
      auto pos = it - stmts.begin();
      oss << pos << "::";
#if DEBUG__build_qualname_for_non_named_scopes
    } else if (pfdefn != nullptr) {
      oss << "{}::";
    } else if (pxdefn != nullptr) {
      oss << "##::";
    } else if (pndefn != nullptr) {
      oss << "$$::";
    } else if (pglob != nullptr) {
      oss << "@@::";
    } else {
      oss << "--::";
#endif
    }
    handle = oss.str() + handle;
    ascope = pscope;
  }

  if (SgClassDefinition * xdefn = isSgClassDefinition(ascope)) {
    xdecl = xdefn->get_declaration();
    scope = xdecl;
  } else if (SgFunctionDefinition * fdefn = isSgFunctionDefinition(ascope)) {
    fdecl = fdefn->get_declaration();
    scope = fdecl;
  } else if (SgNamespaceDefinitionStatement * ndefn = isSgNamespaceDefinitionStatement(ascope)) {
    ndecl = ndefn->get_namespaceDeclaration();
    scope = ndecl;
  }

  if (ndecl != nullptr) {
    handle = ndecl->get_qualified_name().getString() + "::" + handle;
  } else if (xdecl != nullptr) {
    handle = xdecl->get_qualified_name().getString() + "::" + handle;
  } else if (fdecl != nullptr) {
    std::ostringstream oss;
    oss << fdecl->get_qualified_name().getString() << "(";
    for (auto t : fdecl->get_type()->get_argument_list()->get_arguments()) {
      oss << globalUnparseToString(t, uinfo) << ",";
    }
    oss << ")::" << handle;
    handle = oss.str();
  }

#if DEBUG__statics__getNodeLabel
  std::cout << "  handle   = " << handle << std::endl;
#endif

  delete uinfo;
}

void Analysis::initialize(SgProject * p) {
  if (p != nullptr) {
    assert(::Typeforge::project == nullptr || ::Typeforge::project == p);
    ::Typeforge::project = p;
  }
  assert(::Typeforge::project != nullptr);

  for (auto g : SgNodeHelper::listOfSgGlobal(project)) {
    traverse(g);
  }
}

template <class T> 
bool is_not_disjoint(std::set<T> const & set1, std::set<T> const & set2) {
    if (set1.empty() || set2.empty()) return false;

    auto it1 = set1.begin();
    auto it1End = set1.end();
    auto it2 = set2.begin();
    auto it2End = set2.end();

    if (*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return false;

    while (it1 != it1End && it2 != it2End) {
        if (*it1 == *it2)
          return true;

        if (*it1 < *it2)
          it1++;
        else
          it2++;
    }

    return false;
}

static bool ignoreNode(SgNode * n) {
  assert(n != nullptr);
#if DEBUG__ignoreNode
  std::cout << "::ignoreNode()" << std::endl;
  std::cout << "  n = " << n << " ( " << n->class_name() << " )" << std::endl;
#endif

  bool ignore = true;

  SgLocatedNode * lnode = isSgLocatedNode(n);
  if (lnode != nullptr) {
    ignore = !SgNodeHelper::nodeCanBeChanged(lnode);
  }

#if DEBUG__ignoreNode
  std::cout << "  => " << ignore << std::endl;
#endif

  return ignore;
}

void Analysis::traverseVariableDeclarations(SgGlobal * g) {
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "Analysis::traverseVariableDeclarations" << std::endl;
  std::cout << "  g   = " << g << std::endl;
#endif

  std::list<SgVariableDeclaration *> listOfVars = SgNodeHelper::listOfGlobalVars(g);
  listOfVars.splice(listOfVars.end(), SgNodeHelper::listOfGlobalFields(g));
  for (auto varDec : listOfVars) {
    if (ignoreNode(varDec)) {
      continue;
    }
    SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
    if (initName == nullptr) {
      continue;
    }
    addNode(varDec);
  }
  for (auto varDec : listOfVars) {
    SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
    if (initName == nullptr) {
      continue;
    }

    SgInitializer* init = initName->get_initializer();
    if (init == nullptr) {
      continue;
    }

    SgType* keyType = initName->get_type();
    if (keyType == nullptr) {
      continue;
    }

    linkVariables(varDec, init);
  }
}

void Analysis::traverseFunctionDeclarations(SgGlobal * g) {
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "Analysis::traverseFunctionDeclarations" << std::endl;
  std::cout << "  g   = " << g << std::endl;
#endif

  std::set<SgFunctionDeclaration *> fdecls;
  for (auto fdecl : SgNodeHelper::listOfFunctionDeclarations(g)) {
    if (ignoreNode(fdecl)) {
      continue;
    }
    if (isSgTemplateFunctionDeclaration(fdecl) || isSgTemplateMemberFunctionDeclaration(fdecl)) {
      continue;
    }
    if (fdecl->get_qualified_name() == "::SQRT" || fdecl->get_qualified_name() == "::FABS" || fdecl->get_qualified_name() == "::CBRT") {
      continue;
    }

    SgFunctionDeclaration * fd = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
    assert(fd != nullptr);
    fdecls.insert(fd);
  }
  for (auto fdecl : fdecls) {
#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "  fdecl = " << fdecl << " (" << fdecl->class_name() << ")" << std::endl;
#endif
    addNode(fdecl);

    SgFunctionDeclaration * pfdecl = isSgFunctionDeclaration(fdecl->get_definingDeclaration());
    if (pfdecl == nullptr) {
      pfdecl = fdecl;
    }
    for (auto iname : pfdecl->get_args()) {
      addNode(iname);
    }
  }
}

void Analysis::traverseFunctionDefinitions(SgGlobal * g) {
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "Analysis::traverseFunctionDefinitions" << std::endl;
  std::cout << "  g   = " << g << std::endl;
#endif

  list<SgFunctionDefinition*> listOfFunctionDefinitions = SgNodeHelper::listOfFunctionDefinitions(g);
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "  listOfFunctionDefinitions.size() = " << listOfFunctionDefinitions.size() << std::endl;
#endif

  for (auto funDef : listOfFunctionDefinitions) {
    if (ignoreNode(funDef))
      continue;
    if (isSgTemplateFunctionDefinition(funDef))
      continue;

#if DEBUG__Analysis__variableSetAnalysis
    std::cout << "    funDef   = " << funDef << " ( " << funDef->class_name() << " )" << std::endl;
#endif

    RoseAst ast(funDef);
    for (auto i = ast.begin(); i != ast.end(); ++i) {
      auto n = *i;

      if (ignoreNode(n)) {
        continue;
      }

#if DEBUG__Analysis__variableSetAnalysis
      std::cout << "      n       = " << n << " ( " << n->class_name() << " )" << std::endl;
#endif

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
          SgVariableSymbol* varSym = varRef->get_symbol();
          assert(varSym != nullptr);

          SgInitializedName * iname = varSym->get_declaration();
          assert(iname != nullptr);
          if (ignoreNode(iname)) {
            continue;
          }

	  SgVariableDeclaration * vdecl = isSgVariableDeclaration(iname->get_declaration());
          if (vdecl != nullptr && ignoreNode(vdecl)) {
            continue;
          }

          if (vdecl != nullptr) {
            linkVariables(vdecl, assignOp->get_rhs_operand());
          } else {
            linkVariables(iname, assignOp->get_rhs_operand());
          }
        } else if (SgFunctionRefExp * fref = isSgFunctionRefExp(lhs)) {
#if DEBUG__Analysis__variableSetAnalysis
          std::cout << "      fref    = " << fref << " ( " << (fref ? fref->class_name() : "") << " )" << std::endl;
#endif
          SgFunctionSymbol * fsym = fref->get_symbol();
          assert(fsym != nullptr);

          SgFunctionDeclaration * fdecl = fsym->get_declaration();
          assert(fdecl != nullptr);

          if (ignoreNode(fdecl)) {
            continue;
          }

          fdecl = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
          assert(fdecl != nullptr);

          linkVariables(fdecl, assignOp->get_rhs_operand());
        } else {
          // TODO other cases such as operation + * / ...
        }
      } else if (SgVariableDeclaration* varDec = isSgVariableDeclaration(n)) {
        SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
        if (initName == nullptr) {
          continue;
        }

        SgLocatedNode * lnode = isSgLocatedNode(n);
        if (lnode != nullptr && ignoreNode(lnode)) {
          continue;
        }

        addNode(varDec);

        SgInitializer* init = initName->get_initializer();
        if (!init) {
          continue;
        }

        linkVariables(varDec, init);

      } else if (SgFunctionCallExp* callExp = isSgFunctionCallExp(n)) {
        SgExpression * callee = callExp->get_function();
        assert(callee != nullptr);
        SgFunctionRefExp * fref = isSgFunctionRefExp(callee);
        SgMemberFunctionRefExp * mfref = isSgMemberFunctionRefExp(callee);
        while (callee != nullptr && fref == nullptr && mfref == nullptr) {
#if DEBUG__Analysis__variableSetAnalysis
          std::cout << "      callee = " << callee << " ( " << callee->class_name() << " )" << std::endl;
#endif
          SgBinaryOp * bop = isSgBinaryOp(callee);
          if (bop != nullptr) {
            callee = bop->get_rhs_operand_i();
          } else {
            assert(false);
          }
          fref = isSgFunctionRefExp(callee);
          mfref = isSgMemberFunctionRefExp(callee);
        }
#if DEBUG__Analysis__variableSetAnalysis
        std::cout << "      fref  = " << fref  << " ( " << ( fref  ? fref->class_name()  : "" ) << " )" << std::endl;
        std::cout << "      mfref = " << mfref << " ( " << ( mfref ? mfref->class_name() : "" ) << " )" << std::endl;
#endif
        SgFunctionSymbol * fsym = nullptr;
        if (fref != nullptr) {
          fsym = fref->get_symbol_i();
        } else if (mfref != nullptr) {
          fsym = mfref->get_symbol_i();
        } else {
          continue;
        }
        assert(fsym != nullptr);

        SgFunctionDeclaration * fdecl = fsym->get_declaration();
        assert(fdecl != nullptr);

        if (fdecl->get_qualified_name() == "::SQRT" || fdecl->get_qualified_name() == "::FABS" || fdecl->get_qualified_name() == "::CBRT") {
          continue;
        }

        SgFunctionDeclaration * dfdecl = isSgFunctionDeclaration(fdecl->get_definingDeclaration());
        if (dfdecl == nullptr) {
          dfdecl = fdecl;
        }

	if (ignoreNode(dfdecl)) {
          continue;
        }

        auto const & initNameList = dfdecl->get_parameterList()->get_args();
        auto const & expList = callExp->get_args()->get_expressions();
        auto initIter = initNameList.begin();
        auto expIter  = expList.begin(); 
        while (initIter != initNameList.end()) {
          addNode(*initIter);
          linkVariables(*initIter, *expIter);
          ++initIter;
          ++expIter;
        }
      } else if(SgReturnStmt* ret = isSgReturnStmt(n)) {
        SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(funDef->get_declaration()->get_firstNondefiningDeclaration());
        assert(fdecl != nullptr);
        addNode(fdecl);
        linkVariables(fdecl, ret->get_expression());
      }
    }
  }
}

void Analysis::traverse(SgGlobal * g) {
#if DEBUG__Analysis__variableSetAnalysis
  std::cout << "Analysis::traverse" << std::endl;
  std::cout << "  g   = " << g << std::endl;
#endif

  traverseVariableDeclarations(g);
  traverseFunctionDeclarations(g);
  traverseFunctionDefinitions(g);
}

// Searches through the expression for variables of the given type then links them with the key node provided
//   TODO traverse expression instead: we need the full path (stack is not enough for complex expressions)
void Analysis::linkVariables(SgNode * key, SgExpression * expression) {
#if DEBUG__Analysis__linkVariables
  std::cout << "Analysis::linkVariables():" << std::endl;
  std::cout << "  key        = " << key << " (" << (key != nullptr ? key->class_name() : "") << ")" << std::endl;
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
      if (exp != expression) {
        stack.push_back(exp);
      }

      if (SgFunctionCallExp * funCall = isSgFunctionCallExp(exp)) {
        SgFunctionDeclaration* fdecl = funCall->getAssociatedFunctionDeclaration();
        assert(fdecl != nullptr);

        fdecl = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
        assert(fdecl != nullptr);

        SgTemplateInstantiationFunctionDecl * ti_fdecl = isSgTemplateInstantiationFunctionDecl(fdecl);
        if (ti_fdecl != nullptr) {
          SgTemplateFunctionDeclaration * t_fdecl = ti_fdecl->get_templateDeclaration();
          assert(t_fdecl != nullptr);
          SgFunctionType * ftype = t_fdecl->get_type();
          assert(ftype != nullptr);
          SgType * r_ftype = ftype->get_return_type();
          assert(r_ftype != nullptr);
          SgNonrealType * nrtype = isSgNonrealType(::Typeforge::stripType(r_ftype, true));
          if (nrtype != nullptr) {
            SgNonrealDecl * nrdecl = isSgNonrealDecl(nrtype->get_declaration());
            assert(nrdecl != nullptr);
            if (nrdecl->get_is_template_param()) {
              fdecl = nullptr;
            }
          }
        }

        if (fdecl != nullptr) {
          addNode(fdecl);
          addEdge(key, fdecl);
        } else {
          addNode(funCall);
          addEdge(key, funCall);
        }

        i.skipChildrenOnForward(); // TODO: expressions used as argument of the function? (they are never seen because of the `skip`)
      } else if (SgVarRefExp* varRef = isSgVarRefExp(exp)) {
        SgVariableSymbol* varSym = varRef->get_symbol();
        if (varSym) {
          SgInitializedName * refInitName = varSym->get_declaration();
          SgNode * target = refInitName;
          if (!SgNodeHelper::isFunctionParameterVariableSymbol(varSym)) {
            target = refInitName->get_declaration();
          }
          if (target != nullptr) {
            addNode(target);
            addEdge(key, target);
          }
        }
      } else if (SgPntrArrRefExp* refExp = isSgPntrArrRefExp(exp)) {
        linkVariables(key, refExp->get_lhs_operand());
        i.skipChildrenOnForward(); // FIXME what about the RHS? (aka index)
      } else if (SgPointerDerefExp* refExp = isSgPointerDerefExp(exp)) {
        linkVariables(key, refExp->get_operand());
        i.skipChildrenOnForward();
      } else if (SgCommaOpExp* commaExp = isSgCommaOpExp(exp)) {
        linkVariables(key, commaExp->get_rhs_operand());
        i.skipChildrenOnForward();
      }

      if (exp != expression) {
        stack.pop_back();
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

SgNode * Analysis::getScope(SgNode * n) const {
  SgNode * s = nullptr;

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

void Analysis::buildChildSets(std::map<SgNode *, std::set<SgNode *> > & childsets, SgType * base) const {
  for (auto e: edges) {
    auto s = e.first;

    if (base != nullptr) {
      auto i = node_map.find(s);
      if (i != node_map.end()) {
        if ( !isTypeBasedOn(i->second.type, base, true) ) continue;
      }
    }
    childsets[s].insert(s);
    for (auto i : e.second) {
      auto t = i.first;
      childsets[t].insert(s);
    }
  }
}

template < typename T, typename S=std::set<T> >
void computeClustering(std::map<T, S> const & childsets, std::vector<S> & clusters) {
  using P = std::pair< S , S >;
  std::vector<P> clustering;
  for (auto p : childsets) {
    clustering.push_back(P({p.first},p.second));
  }

  while (clustering.size() > 0) {
    auto & nodeset = clustering[0].first;
    auto & tagset = clustering[0].second;
    bool has_changed = true;
    while (has_changed) {
      has_changed = false;
      size_t i = 1;
      while (i < clustering.size()) {
        if (is_not_disjoint(tagset, clustering[i].second)) {
          nodeset.insert(clustering[i].first.begin(), clustering[i].first.end());
          tagset.insert(clustering[i].second.begin(), clustering[i].second.end());
          clustering.erase(clustering.begin() + i);
          if (i > 1) {
            has_changed = true;
          }
        } else {
          ++i;
        }
      }
    }
    clusters.push_back(nodeset);
    clustering.erase(clustering.begin());
  }
}

void Analysis::buildClusters(std::vector<std::set<SgNode *> > & clusters, SgType * base) const {
  std::map<SgNode *, std::set<SgNode *> > childsets;
  buildChildSets(childsets, base);
  computeClustering(childsets, clusters);
}

void Analysis::toDot(std::string const & fileName, SgType * base) const {
  SgUnparse_Info* uinfo = new SgUnparse_Info();
      uinfo->set_SkipComments();
      uinfo->set_SkipWhitespaces();
      uinfo->set_SkipEnumDefinition();
      uinfo->set_SkipClassDefinition();
      uinfo->set_SkipFunctionDefinition();
      uinfo->set_SkipBasicBlock();
      uinfo->set_isTypeFirstPart();

  std::map<std::string, std::string> node_color_map = {
    { "SgInitializedName",                         "lightsalmon"    },
    { "SgVariableDeclaration",                     "cyan"  },
    { "SgFunctionDeclaration",                     "mediumpurple"   },
    { "SgMemberFunctionDeclaration",               "wheat"          },
    { "SgTemplateInstantiationFunctionDecl",       "palegreen"      },
    { "SgTemplateInstantiationMemberFunctionDecl", "lightcoral"     },
    { "SgClassDeclaration",                        "palevioletred"  },
    { "SgFunctionCallExp",                         "lightsteelblue" }
  };

  fstream dotfile;
  dotfile.open(fileName, ios::out | ios::trunc);

  dotfile << "digraph {" << std::endl;
  dotfile << "  ranksep=5;" << std::endl;

  std::vector< std::set<SgNode *> > clusters;
  buildClusters(clusters, base);

  size_t num_nodes = 0;
  for (auto C : clusters) {
    num_nodes += C.size();
  }

  dotfile << "  title=\"" << clusters.size() << " clusters with " << num_nodes << " possible transformations.\";" << std::endl;

  for (size_t i = 0; i < clusters.size(); ++i) {
    auto C = clusters[i];
    dotfile << "  subgraph cluster_" << i << " {" << std::endl;

    dotfile << "    title=\"Cluster #" << i << " with " << C.size() << " possible transformations.\";" << std::endl;

    std::set<SgNode *> seen;
    for (auto n: C) {
      assert(n != nullptr);

      auto d = node_map.find(n);
      assert(d != node_map.end());

      auto edges__ = edges.find(n);
      if (edges__ != edges.end()) {
        for (auto target_stack: edges__->second) {
          auto t = target_stack.first;
          auto i = node_map.find(t);
          assert(i != node_map.end());

          seen.insert(n);
          seen.insert(t);

#if 1
          dotfile << "    n_" << n << " -> n_" << t << ";" << std::endl;
#else
// TODO make so that one can choose to expand the stacks (nodes)
//   Following code only unparse the stacks in the edge's labels (not readable)
          auto stacks = target_stack.second;
          dotfile << "[label=\"";
          for (auto stack__: stacks) {
            for (auto s: stack__) {
              assert(s != nullptr);
              dotfile << s->unparseToString() << " - ";
            }
            dotfile << "";
          }
          dotfile << "\"];" << std::endl;
#endif
        }
      }
    }

    // TODO get paths through:
    //  - automated: analyze all positions in graph [eventually]
    //  - cmd-line [good first step]
    //  - environment [I don't like that one much]
    std::vector<std::string> paths{
      "/workspace/pipeline-tests/typeforge-tests/",
      "/opt/rose/vanderbrugge1/typeforge/native/release/install/include/edg/g++_HEADERS/hdrs7/"
    };
    for (auto n : seen) {
      auto d = node_map.find(n);
      assert(d != node_map.end());

      auto position = d->second.position;
      for (auto p : paths) {
        auto i = position.find(p);
        if (i == 0) {
          position = position.substr(p.size());
        }
      }

      dotfile << "    n_" << n << " [label=\"" << d->second.handle;
//      dotfile << "\\n" << d->second.cname;
        dotfile << "\\n" << position;
        dotfile << "\\n" << globalUnparseToString(d->second.type, uinfo);
      dotfile << "\"";
      dotfile << ", fillcolor=" << node_color_map[d->second.cname] << ", style=filled";
      if ( isTypeBasedOn(d->second.type, base, true) ) {
        dotfile << ", penwidth=3";
      }
      dotfile << "];" << std::endl;
    }

    dotfile << "}" << std::endl; // end cluster
  }

  dotfile << "}" << std::endl; // end graph

  dotfile.close();

  delete uinfo;
}

void Analysis::getGlobals(std::vector<SgVariableDeclaration *> & decls, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgVariableDeclaration * vdecl = isSgVariableDeclaration(n);
    if (vdecl != nullptr && isSgGlobal(vdecl->get_scope())) {
      decls.push_back(vdecl);
    }
  }
}

void Analysis::getLocals(std::vector<SgVariableDeclaration *> & decls, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgVariableDeclaration * vdecl = isSgVariableDeclaration(n);
    if (vdecl != nullptr) {
      decls.push_back(vdecl); // TODO compare `location`
    }
  }
}

void Analysis::getFields(std::vector<SgVariableDeclaration *> & decls, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgVariableDeclaration * vdecl = isSgVariableDeclaration(n);
    if (vdecl != nullptr && false) {
      decls.push_back(vdecl); // TODO compare `location`
    }
  }
}

void Analysis::getFunctions(std::vector<SgFunctionDeclaration *> & decls, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(n);
    SgMemberFunctionDeclaration * mdecl = isSgMemberFunctionDeclaration(n);
    if (fdecl != nullptr && mdecl == nullptr) {
      decls.push_back(fdecl); // TODO compare `location`
    }
  }
}

void Analysis::getMethods(std::vector<SgFunctionDeclaration *> & decls, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgMemberFunctionDeclaration * mdecl = isSgMemberFunctionDeclaration(n);
    if (mdecl != nullptr) {
      decls.push_back(mdecl); // TODO compare `location`
    }
  }
}

void Analysis::getParameters(std::vector<SgInitializedName *> & decls, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgInitializedName * iname = isSgInitializedName(n);
    if (iname != nullptr) {
      decls.push_back(iname); // TODO compare `location`
    }
  }
}

void Analysis::getCallExp(std::vector<SgFunctionCallExp *> & exprs, std::string const & location) const {
  for (auto p : node_map) {
    auto n = p.first;
    auto d = p.second;

    SgFunctionCallExp * expr = isSgFunctionCallExp(n);
    if (expr != nullptr) {
      exprs.push_back(expr); // TODO compare `location`
    }
  }
}

SgProject * project;
Analysis typechain;

}

