
#include "ROSE/proposed/sage-interface.hpp"

#include "ROSE/Analysis/Template/instantiation-flow.hpp"

#include "sage3basic.h"

#include <cassert>

namespace SageInterface {

  bool isTemplateDeclaration(SgNode * node) {
    SgTemplateFunctionDeclaration *       tpl_fdecl  = isSgTemplateFunctionDeclaration(node);
    SgTemplateMemberFunctionDeclaration * tpl_mdecl  = isSgTemplateMemberFunctionDeclaration(node);
    SgTemplateClassDeclaration *          tpl_cdecl  = isSgTemplateClassDeclaration(node);
    SgTemplateTypedefDeclaration *        tpl_tddecl = isSgTemplateTypedefDeclaration(node);
    SgTemplateVariableDeclaration *       tpl_vdecl  = isSgTemplateVariableDeclaration(node);
    return (tpl_fdecl || tpl_mdecl || tpl_cdecl || tpl_tddecl || tpl_vdecl);
  }

  SgDeclarationStatement * getEnclosingTemplateDeclaration(SgNode * node) {
    if (node == NULL || isSgGlobal(node)) {
      return NULL;
    }

    SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
    if (declstmt != NULL) {
      SgTemplateFunctionDeclaration *       tpl_fdecl  = isSgTemplateFunctionDeclaration(node);
      SgTemplateMemberFunctionDeclaration * tpl_mdecl  = isSgTemplateMemberFunctionDeclaration(node);
      SgTemplateClassDeclaration *          tpl_cdecl  = isSgTemplateClassDeclaration(node);
      SgTemplateTypedefDeclaration *        tpl_tddecl = isSgTemplateTypedefDeclaration(node);
      SgTemplateVariableDeclaration *       tpl_vdecl  = isSgTemplateVariableDeclaration(node);
      if (tpl_fdecl || tpl_mdecl || tpl_cdecl || tpl_tddecl || tpl_vdecl) {
        return declstmt;
      }
    }

    return getEnclosingTemplateDeclaration(node->get_parent()); // FIXME Use scope?
  }

  bool isTemplateInstantiation(SgNode * node) {
    SgTemplateInstantiationFunctionDecl *       tpl_fdecl  = isSgTemplateInstantiationFunctionDecl(node);
    SgTemplateInstantiationMemberFunctionDecl * tpl_mdecl  = isSgTemplateInstantiationMemberFunctionDecl(node);
    SgTemplateInstantiationDecl *               tpl_cdecl  = isSgTemplateInstantiationDecl(node);
    SgTemplateInstantiationTypedefDeclaration * tpl_tddecl = isSgTemplateInstantiationTypedefDeclaration(node);
    return (tpl_fdecl || tpl_mdecl || tpl_cdecl || tpl_tddecl);
  }

  SgDeclarationStatement * getEnclosingTemplateInstantiation(SgNode * node) {
    if (node == NULL || isSgGlobal(node)) {
      return NULL;
    }

    SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
    if (declstmt != NULL) {
      SgTemplateInstantiationFunctionDecl *       tpl_fdecl  = isSgTemplateInstantiationFunctionDecl(node);
      SgTemplateInstantiationMemberFunctionDecl * tpl_mdecl  = isSgTemplateInstantiationMemberFunctionDecl(node);
      SgTemplateInstantiationDecl *               tpl_cdecl  = isSgTemplateInstantiationDecl(node);
      SgTemplateInstantiationTypedefDeclaration * tpl_tddecl = isSgTemplateInstantiationTypedefDeclaration(node);
//    SgTemplateVariableDeclaration *       tpl_vdecl  = isSgTemplateVariableDeclaration(node);
      if (tpl_fdecl || tpl_mdecl || tpl_cdecl || tpl_tddecl) {
        return declstmt;
      }
    }

    return getEnclosingTemplateInstantiation(node->get_parent()); // FIXME Use scope?
  }

  VariantT getTemplateInstantiationVariant(VariantT variant) {
    switch (variant) {
      case V_SgTemplateFunctionDeclaration:
        return (VariantT)SgTemplateFunctionDeclaration::instantiation_kind_t::static_variant;
      case V_SgTemplateMemberFunctionDeclaration:
        return (VariantT)SgTemplateMemberFunctionDeclaration::instantiation_kind_t::static_variant;
      case V_SgTemplateClassDeclaration:
        return (VariantT)SgTemplateClassDeclaration::instantiation_kind_t::static_variant;
      case V_SgTemplateTypedefDeclaration:
        return (VariantT)SgTemplateTypedefDeclaration::instantiation_kind_t::static_variant;
      case V_SgTemplateVariableDeclaration:
        return (VariantT)SgTemplateVariableDeclaration::instantiation_kind_t::static_variant;
      default:
        ROSE_ASSERT(false);
    }
    return (VariantT)SgNode::static_variant;
  }

  VariantT getTemplateDeclarationVariant(VariantT variant) {
    switch (variant) {
      case V_SgTemplateInstantiationFunctionDecl:
        return (VariantT)SgTemplateInstantiationFunctionDecl::template_kind_t::static_variant;
      case V_SgTemplateInstantiationMemberFunctionDecl:
        return (VariantT)SgTemplateInstantiationMemberFunctionDecl::template_kind_t::static_variant;
      case V_SgTemplateInstantiationDecl:
        return (VariantT)SgTemplateInstantiationDecl::template_kind_t::static_variant;
      case V_SgTemplateInstantiationTypedefDeclaration:
        return (VariantT)SgTemplateInstantiationTypedefDeclaration::template_kind_t::static_variant;
//    case V_SgTemplateInstantiationVariableDecl:
//      return SgTemplateInstantiationVariableDecl::template_kind_t::static_variant;
      default:
        ROSE_ASSERT(false);
    }
    return (VariantT)SgNode::static_variant; // Never reached
  }

  SgDeclarationStatement * getTemplateDeclaration(SgDeclarationStatement * instantiation) {
    switch (instantiation->variantT()) {
      case V_SgTemplateInstantiationFunctionDecl:
        return isSgTemplateInstantiationFunctionDecl(instantiation)->get_templateDeclaration()->get_firstNondefiningDeclaration();
      case V_SgTemplateInstantiationMemberFunctionDecl:
        return isSgTemplateInstantiationMemberFunctionDecl(instantiation)->get_templateDeclaration()->get_firstNondefiningDeclaration();
      case V_SgTemplateInstantiationDecl:
        return isSgTemplateInstantiationDecl(instantiation)->get_templateDeclaration()->get_firstNondefiningDeclaration();
      case V_SgTemplateInstantiationTypedefDeclaration:
        return isSgTemplateInstantiationTypedefDeclaration(instantiation)->get_templateDeclaration()->get_firstNondefiningDeclaration();
//    case V_SgTemplateInstantiationVariableDecl:
//      return isSgTemplateInstantiationVariableDecl(instantiation)->get_templateDeclaration()->get_firstNondefiningDeclaration();
      case V_SgNonrealDecl: {
        SgDeclarationStatement * tpldecl = isSgNonrealDecl(instantiation)->get_templateDeclaration();
        return tpldecl ? tpldecl->get_firstNondefiningDeclaration() : NULL;
      }
      default:
        return NULL;
    }
    return NULL; // Never reached
  }

} // <<< END SageInterface

/// GraphViz Utils

std::string nodeToLabel(SgNode * node) {
  SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
    inputUnparseInfoPointer->set_SkipComments();
    inputUnparseInfoPointer->set_SkipWhitespaces();
    inputUnparseInfoPointer->set_SkipEnumDefinition();
    inputUnparseInfoPointer->set_SkipClassDefinition();
    inputUnparseInfoPointer->set_SkipFunctionDefinition();
    inputUnparseInfoPointer->set_SkipBasicBlock();
    inputUnparseInfoPointer->set_isTypeFirstPart();
    inputUnparseInfoPointer->set_requiresGlobalNameQualification();
    inputUnparseInfoPointer->set_outputCompilerGeneratedStatements();
    inputUnparseInfoPointer->unset_skipCompilerGeneratedSubExpressions();

  std::ostringstream oss;

  oss << node->class_name();

  SgStatement * stmt = isSgStatement(node);
  SgGlobal * glob = isSgGlobal(node);
  SgDeclarationScope * declscope = isSgDeclarationScope(node);
  SgDeclarationStatement * decl = isSgDeclarationStatement(node);
  SgVariableDeclaration * vdecl = isSgVariableDeclaration(node);
  if (glob || declscope) {
     // NOPE
  } else if (vdecl) {
//   std::cout << "Unparse: " << std::hex << node << std::dec << " : " << node->class_name() << std::endl;
     oss << "\\n" << globalUnparseToString(stmt, inputUnparseInfoPointer);
  } else if (decl) {
     oss << "\\n" << SageInterface::get_name(node);

     if (isSgClassDeclaration(decl) || isSgNonrealDecl(decl)) {
       SgTemplateArgumentPtrList * tpl_args = isSgFunctionParameterList(decl) ? NULL : SageBuilder::getTemplateArgumentList(decl);
       if (tpl_args != NULL && !tpl_args->empty()) {
         oss << "<";
         for (auto tpl_arg: *tpl_args) {
           if (tpl_arg->get_type()) {
//           std::cout << "Unparse: " << std::hex << node << std::dec << " : " << node->class_name() << std::endl;
             oss << globalUnparseToString(tpl_arg->get_type(), inputUnparseInfoPointer) << " , ";
           }
         }
         oss << ">";
       }
    }
  } else if (stmt) {
//  std::cout << "Unparse: " << std::hex << node << std::dec << " : " << node->class_name() << std::endl;
    oss << "\\n" << globalUnparseToString(stmt, inputUnparseInfoPointer);
  }

  SgLocatedNode * lnode = isSgLocatedNode(node);
  if (lnode != NULL) {
    Sg_File_Info * pos = lnode->get_startOfConstruct();
    ROSE_ASSERT(pos != NULL);
    if (!pos->isCompilerGenerated() && pos->get_file_id() >= 0) {
      oss << "\\n" << boost::filesystem::path(pos->get_filenameString()).filename().string() << ":" << pos->get_line();
    }
  }

  delete inputUnparseInfoPointer;

  return oss.str();
}

std::string nodeToTag(SgNode * node) {
  std::ostringstream oss;

  oss << "n_" << std::hex << node << std::dec;

  return oss.str();
}

void toGraphVizNodes(std::ofstream & dot, const std::set<SgStatement*> & nodes) {
  for (auto node: nodes) {
    dot << nodeToTag(node) << "[label=\"" << nodeToLabel(node) << "\"];" << std::endl;
  }
}

/// Position Matching

void getStatementMatchingPositions(const std::vector< std::pair< std::string, size_t > > & positions, std::vector<SgStatement *> & found_stmts, SgNode * top_node_for_search) {
  std::vector<SgStatement *> stmts = SageInterface::querySubTree<SgStatement>(top_node_for_search, V_SgStatement);

  for (auto stmt: stmts) {
    Sg_File_Info * pos = stmt->get_startOfConstruct();
    ROSE_ASSERT(pos != NULL);

    std::string file = pos->get_filenameString();
    size_t line = pos->get_line();

    bool selected = false;
    for (auto pos: positions) {
      size_t fpos = file.find(pos.first);
      selected |= (fpos != std::string::npos && fpos + pos.first.length() == file.length() && line == pos.second);
    }

    if (selected) {
      found_stmts.push_back(stmt);
    }
  }
  std::cout << " * found: " << found_stmts.size() << std::endl;
}

/// References

class DeclarationReferenceMap : public ROSE_VisitTraversal {
  private:
    std::map<SgDeclarationStatement *, std::set<SgStatement *> > map_decl_to_stmt;
    std::vector< std::pair<SgDeclarationStatement *, SgStatement *> > decl_stmt_matches;

  public:
    DeclarationReferenceMap() :
      ROSE_VisitTraversal(),
      map_decl_to_stmt(),
      decl_stmt_matches()
    {
      traverseMemoryPool();
      collectMatches();
    }

  private:

    void collectMatches() {
      while (!decl_stmt_matches.empty()) {
        SgDeclarationStatement * decl = decl_stmt_matches.front().first;
        ROSE_ASSERT(decl != NULL);
        decl = decl->get_firstNondefiningDeclaration();
        ROSE_ASSERT(decl != NULL);

        ROSE_ASSERT(!isSgFunctionParameterList(decl));

        SgStatement * stmt = decl_stmt_matches.front().second;
        ROSE_ASSERT(stmt != NULL);

        ROSE_ASSERT(!isSgDeclarationScope(stmt));
        ROSE_ASSERT(!isSgFunctionParameterList(stmt));
        ROSE_ASSERT(!isSgVariableDefinition(stmt));
        ROSE_ASSERT(!isSgScopeStatement(stmt));

        if (decl->get_definingDeclaration() != NULL) {
          decl = decl->get_definingDeclaration();
        }

        if (decl != stmt) {
          map_decl_to_stmt[decl].insert(stmt);
        }

        SgScopeStatement * scope = decl->get_scope();
        SgClassDefinition * xdefn = isSgClassDefinition(scope);
        SgDeclarationScope * nrscope = isSgDeclarationScope(scope);
        if (xdefn != NULL) {
          SgClassDeclaration * xdecl = xdefn->get_declaration();
          ROSE_ASSERT(xdecl != NULL);
          xdecl = isSgClassDeclaration(xdecl->get_firstNondefiningDeclaration());
          ROSE_ASSERT(xdecl != NULL);

//        std::cout << "Class:" << std::endl;
//        std::cout << "  " << std::hex << stmt  << std::dec << " : " << stmt->class_name()  << std::endl;
//        std::cout << "  " << std::hex << xdecl << std::dec << " : " << xdecl->class_name() << std::endl;

          if (xdecl != stmt) {
            decl_stmt_matches.push_back(std::pair<SgDeclarationStatement *, SgStatement *>(xdecl, stmt));
          }
        } else if (nrscope != NULL) {
          SgDeclarationStatement * declstmt = isSgDeclarationStatement(nrscope->get_parent());
          ROSE_ASSERT(declstmt != NULL);
          declstmt = declstmt->get_firstNondefiningDeclaration();
          ROSE_ASSERT(declstmt != NULL);

          ROSE_ASSERT(!isSgFunctionParameterList(declstmt));

//        std::cout << "Nonreal:" << std::endl;
//        std::cout << "  " << std::hex << stmt     << std::dec << " : " << stmt->class_name()     << std::endl;
//        std::cout << "  " << std::hex << declstmt << std::dec << " : " << declstmt->class_name() << std::endl;

          if (declstmt != stmt) {
            decl_stmt_matches.push_back(std::pair<SgDeclarationStatement *, SgStatement *>(declstmt, stmt));
          }
        }

        decl_stmt_matches.erase(decl_stmt_matches.begin());
      }
    }

    void visit(SgNode * node) {
      if (isSgFunctionDeclaration(node) || isSgClassDeclaration(node) || isSgTypedefDeclaration(node) || isSgVariableDeclaration(node)) {
        SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
        ROSE_ASSERT(declstmt != NULL);
        if (declstmt->get_firstNondefiningDeclaration() == NULL || declstmt == declstmt->get_firstNondefiningDeclaration()) {
          if (declstmt->get_definingDeclaration() != NULL) {
            declstmt = declstmt->get_definingDeclaration();
          }

          map_decl_to_stmt[declstmt];
        }
        return;
      }

      SgDeclarationStatement * decl = NULL;
      SgType * type = NULL;

      switch (node->variantT()) {
        case V_SgVarRefExp: {
          SgVariableSymbol * vsym = ((SgVarRefExp *)node)->get_symbol();
          ROSE_ASSERT(vsym != NULL);

          SgInitializedName * iname = vsym->get_declaration();
          ROSE_ASSERT(iname != NULL);

          decl = isSgVariableDeclaration(iname->get_parent());
          if (decl == NULL) return; // Not interrested into function parameters and template parameters

          break;
        }
        case V_SgFunctionRefExp: {
          SgFunctionSymbol * fsym = ((SgFunctionRefExp *)node)->get_symbol();
          ROSE_ASSERT(fsym != NULL);
          decl = fsym->get_declaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgMemberFunctionRefExp: {
          SgMemberFunctionSymbol * mfsym = ((SgMemberFunctionRefExp *)node)->get_symbol();
          ROSE_ASSERT(mfsym != NULL);
          decl = mfsym->get_declaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgTemplateFunctionRefExp: {
          SgTemplateFunctionSymbol * tfsym = ((SgTemplateFunctionRefExp *)node)->get_symbol();
          ROSE_ASSERT(tfsym != NULL);
          decl = tfsym->get_declaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgTemplateMemberFunctionRefExp: {
          SgTemplateMemberFunctionSymbol * tmfsym = ((SgTemplateMemberFunctionRefExp *)node)->get_symbol();
          ROSE_ASSERT(tmfsym != NULL);
          decl = tmfsym->get_declaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgLambdaRefExp: {
          decl = ((SgLambdaRefExp *)node)->get_functionDeclaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgNonrealRefExp: {
          SgNonrealSymbol * nrsym = ((SgNonrealRefExp *)node)->get_symbol();
          ROSE_ASSERT(nrsym != NULL);
          decl = nrsym->get_declaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgThisExp: {
          SgClassSymbol * csym = ((SgThisExp *)node)->get_class_symbol();
          ROSE_ASSERT(csym != NULL);
          decl = csym->get_declaration();
          ROSE_ASSERT(decl != NULL);
          break;
        }
        case V_SgInitializedName: {
          type = ((SgInitializedName *)node)->get_typeptr();
          break;
        }
        case V_SgFunctionDeclaration: {
          SgFunctionType * ftype = isSgFunctionType(((SgFunctionDeclaration *)node)->get_type());
          ROSE_ASSERT(ftype != NULL);
          type = ftype->get_return_type();
          break;
        }
        case V_SgTemplateArgument: {
          type = ((SgTemplateArgument *)node)->get_type();
          decl = ((SgTemplateArgument *)node)->get_templateDeclaration();
          if ( !( (decl != NULL) xor (type != NULL) ) ) return;
          break;
        }
        default: {
          return;
        }
      }

      if (isSgTemplateArgument(node) && ( isSgGlobal(node->get_parent()) || isSgNamespaceDefinitionStatement(node->get_parent()) ) ) {
        std::cout << "WARNING: Found a template argument whose parent is SgGlobal or SgNamespaceDefinitionStatement..." << std::endl;
        return;
      }

//    if (isSgTemplateArgument(node)) {
//      std::cout << "WARNING: Found a template argument whose parent is: " << ( node->get_parent() ? node->get_parent()->class_name() : "NULL") << std::endl;
//      return;
//    }

//    std::cout << "Node: " << std::hex << node << std::dec << " : " << node->class_name() << std::endl;
//    std::cout << "Decl: " << std::hex << decl << std::dec << " : " << (decl ? decl->class_name() : "") << std::endl;
//    std::cout << "Type: " << std::hex << type << std::dec << " : " << (type ? type->class_name() : "") << std::endl;

      ROSE_ASSERT( (decl != NULL) xor (type != NULL) );

      SgStatement * stmt = SageInterface::getEnclosingStatement(node);
      ROSE_ASSERT(stmt != NULL);

//    std::cout << "Stmt: " << std::hex << stmt << std::dec << " : " << stmt->class_name() << std::endl;

      if (isSgFunctionParameterList(stmt)) {
        ROSE_ASSERT(isSgInitializedName(node));
        stmt = SageInterface::getEnclosingStatement(stmt->get_parent());
        ROSE_ASSERT(isSgFunctionDeclaration(stmt));
      }

      if (isSgDeclarationScope(stmt)) {
        ROSE_ASSERT(isSgTemplateArgument(node));
        stmt = SageInterface::getEnclosingStatement(stmt->get_parent());
        ROSE_ASSERT(isSgDeclarationStatement(stmt));
      }

      if (isSgClassDefinition(stmt)) { // Some template argument get into the wrong scope
        ROSE_ASSERT(isSgTemplateArgument(node));
        stmt = SageInterface::getEnclosingStatement(stmt->get_parent());
        ROSE_ASSERT(isSgDeclarationStatement(stmt));
      }

      ROSE_ASSERT(!isSgDeclarationScope(stmt));
      ROSE_ASSERT(!isSgFunctionParameterList(stmt));

      ROSE_ASSERT(!isSgClassDefinition(stmt));
      ROSE_ASSERT(!isSgFunctionDefinition(stmt));
      ROSE_ASSERT(!isSgVariableDefinition(stmt));
      ROSE_ASSERT(!isSgNamespaceDefinitionStatement(stmt));
      ROSE_ASSERT(!isSgGlobal(stmt));

      if (decl != NULL) {
        decl = decl->get_firstNondefiningDeclaration();
        ROSE_ASSERT(decl != NULL);

//      std::cout << "Direct:" << std::endl;
//      std::cout << "  " << std::hex << stmt << std::dec << " : " << stmt->class_name() << std::endl;
//      std::cout << "  " << std::hex << decl << std::dec << " : " << decl->class_name() << std::endl;

        // FIXME Are we interrested by declarations local to function? I believe that the following predicate is too broad...
        // if (SageInterface::getEnclosingFunctionDefinition(decl) != NULL) return;

        decl_stmt_matches.push_back(std::pair<SgDeclarationStatement *, SgStatement *>(decl, stmt));
      }

      if (type != NULL) {
        type = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
        SgTypedefType * tdtype = isSgTypedefType(type);
        while (tdtype != NULL) {
          SgDeclarationStatement * decl = tdtype->get_declaration();
          ROSE_ASSERT(decl != NULL);
          decl = decl->get_firstNondefiningDeclaration();
          ROSE_ASSERT(decl != NULL);
          decl_stmt_matches.push_back(std::pair<SgDeclarationStatement *, SgStatement *>(decl, stmt));

          type = tdtype->get_base_type();
          ROSE_ASSERT(type != NULL);
          type = type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
          ROSE_ASSERT(type != NULL);
          tdtype = isSgTypedefType(type);
        }

        SgNamedType * ntype = isSgNamedType(type);
        if (ntype) {
          SgDeclarationStatement * decl = ntype->get_declaration();
          ROSE_ASSERT(decl != NULL);
          decl = decl->get_firstNondefiningDeclaration();
          ROSE_ASSERT(decl != NULL);
          decl_stmt_matches.push_back(std::pair<SgDeclarationStatement *, SgStatement *>(decl, stmt));
        }
      }
    }

  public:
    const std::set<SgStatement *> & getReferencingStatements(SgDeclarationStatement * declstmt) const {
      std::cout << std::hex << declstmt << std::dec << " : " << declstmt->class_name() << std::endl;

      ROSE_ASSERT(declstmt != NULL);
      declstmt = declstmt->get_firstNondefiningDeclaration();
      ROSE_ASSERT(declstmt != NULL);

      if (declstmt->get_definingDeclaration() != NULL) {
        declstmt = declstmt->get_definingDeclaration();
      }

      auto statements = map_decl_to_stmt.find(declstmt);
      ROSE_ASSERT(statements != map_decl_to_stmt.end());
      return statements->second;
    }

    friend void toGraphvizReferencingStatementMap(std::ofstream & dot, const DeclarationReferenceMap & decl_refs_map, std::set<SgStatement*> & nodes);
};

void toGraphvizReferencingStatementMap(std::ofstream & dot, const DeclarationReferenceMap & decl_refs_map, std::set<SgStatement*> & nodes) {
  for (auto ref_stmt: decl_refs_map.map_decl_to_stmt) {
    SgDeclarationStatement * declstmt = ref_stmt.first;
    nodes.insert(declstmt);
    for (auto stmt: ref_stmt.second) {

      ROSE_ASSERT(!isSgDeclarationScope(stmt));
      ROSE_ASSERT(!isSgFunctionParameterList(stmt));
      ROSE_ASSERT(!isSgVariableDefinition(stmt));

      nodes.insert(stmt);
      dot << nodeToTag(stmt) << " -> " << nodeToTag(declstmt) << "[label=\"references\", style=\"dashed\", color=blue, constraint=true];" << std::endl;
    }
  }
}

/// Instantiations

class TemplateInstantiationMap : public ROSE_VisitTraversal {
  private:
    std::map<SgDeclarationStatement *, std::set<SgDeclarationStatement *> > map_decl_to_inst;
    std::map<SgStatement *, SgStatement *> map_cg_inst_stmt_to_decl_stmt;

  public:
    TemplateInstantiationMap() :
      ROSE_VisitTraversal(),
      map_decl_to_inst()
    {
      traverseMemoryPool();
      for (auto decl_to_inst: map_decl_to_inst) {
        for (auto inst: decl_to_inst.second) {
          Sg_File_Info * pos = inst->get_startOfConstruct();
          ROSE_ASSERT(pos != NULL);
          if (pos->isCompilerGenerated() || pos->get_file_id() < 0) {
            mapCompilerGeneratedStatement(decl_to_inst.first, inst);
          }
        }
      }
    }

  private:
    void visit(SgNode * node) {
      SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
      if (declstmt != NULL) {
        declstmt = declstmt->get_firstNondefiningDeclaration() ? declstmt->get_firstNondefiningDeclaration() : declstmt;

        if (declstmt->get_definingDeclaration() != NULL) {
          declstmt = declstmt->get_definingDeclaration();
        }

        SgDeclarationStatement * tpldecl = SageInterface::getTemplateDeclaration(declstmt);
        if (tpldecl != NULL) {
          tpldecl = tpldecl->get_firstNondefiningDeclaration();
          ROSE_ASSERT(tpldecl != NULL);

          if (tpldecl->get_definingDeclaration() != NULL) {
            tpldecl = tpldecl->get_definingDeclaration();
          }

          map_decl_to_inst[tpldecl].insert(declstmt);
        }
      }
    }

    void mapCompilerGeneratedStatement(SgDeclarationStatement * decl, SgDeclarationStatement * inst) {
      std::cout << "mapCompilerGeneratedStatement:" << std::endl;
      std::cout << "  Decl: " << std::hex << decl << std::dec << " : " << decl->class_name() << std::endl;
      std::cout << "  Inst: " << std::hex << inst << std::dec << " : " << inst->class_name() << std::endl;

     decl = decl->get_definingDeclaration();
     inst = inst->get_definingDeclaration();

     if (decl == NULL || inst == NULL)
      return;

     // TODO TODO TODO
    }

  public:
    const std::set<SgDeclarationStatement *> & getInstantiationFromDeclaration(SgDeclarationStatement * declstmt) const {
      ROSE_ASSERT(declstmt != NULL);
      declstmt = declstmt->get_firstNondefiningDeclaration();
      ROSE_ASSERT(declstmt != NULL);

      if (declstmt->get_definingDeclaration() != NULL) {
        declstmt = declstmt->get_definingDeclaration();
      }

      auto instantiations = map_decl_to_inst.find(declstmt);
      ROSE_ASSERT(instantiations != map_decl_to_inst.end());
      return instantiations->second;
    }

    SgStatement * getStatementDeclarationFromInstantiatedStatement(SgStatement * stmt) const {
      auto inst = map_cg_inst_stmt_to_decl_stmt.find(stmt);
      if (inst != map_cg_inst_stmt_to_decl_stmt.end()) {
        return inst->second;
      } else {
        return NULL;
      }
    }

    friend void toGraphvizInstantiation(std::ofstream & dot, const TemplateInstantiationMap & tpl_inst_map, std::set<SgStatement*> & nodes);
};

void toGraphvizInstantiation(std::ofstream & dot, const TemplateInstantiationMap & tpl_inst_map, std::set<SgStatement*> & nodes) {
  for (auto decl_to_inst: tpl_inst_map.map_decl_to_inst) {
    auto tpldecl = decl_to_inst.first;
    nodes.insert(tpldecl);
    for (auto tplinst: decl_to_inst.second) {
      nodes.insert(tplinst);
      dot << nodeToTag(tplinst) << " -> " << nodeToTag(tpldecl) << "[label=\"instantiates\", style=\"dashed\", color=green, constraint=false];" << std::endl;
    }
  }
}

// Scopes

void toGraphvizScopes(std::ofstream & dot, std::set<SgStatement *> & nodes) {
  std::set<SgDeclarationStatement *> seen;

  std::set< std::pair<SgStatement *, SgStatement *> > edges;

  std::vector<SgStatement *> queue(nodes.begin(), nodes.end());
  while (!queue.empty()) {
    SgStatement * stmt = queue.front();
    ROSE_ASSERT(stmt != NULL);
    queue.erase(queue.begin());

    SgDeclarationStatement * declstmt = NULL;
    SgScopeStatement * scope = stmt->get_scope();

    while (scope != NULL) {
      SgGlobal *                       global  = isSgGlobal(scope);
      SgNamespaceDefinitionStatement * ndefn   = isSgNamespaceDefinitionStatement(scope);
      SgClassDefinition *              xdefn   = isSgClassDefinition(scope);
      SgFunctionDefinition *           fdefn   = isSgFunctionDefinition(scope);
      SgDeclarationScope *             nrscope = isSgDeclarationScope(scope);
      if (global || ndefn) {
        break;
      } else if (xdefn) {
        declstmt = xdefn->get_declaration();
        declstmt = declstmt->get_firstNondefiningDeclaration();
        break;
      } else if (fdefn) {
        declstmt = fdefn->get_declaration();
        declstmt = declstmt->get_firstNondefiningDeclaration();
        break;
      } else if (nrscope) {
        declstmt = isSgDeclarationStatement(nrscope->get_parent());
        declstmt = declstmt->get_firstNondefiningDeclaration();
        ROSE_ASSERT(!isSgFunctionParameterList(declstmt));
        break;
      } else {
        scope = scope->get_scope();
      }
    }

    if (declstmt == NULL) continue;

    if (declstmt->get_definingDeclaration() != NULL) {
      declstmt = declstmt->get_definingDeclaration();
    }

    if (seen.find(declstmt) == seen.end()) {
      queue.push_back(declstmt);
      seen.insert(declstmt);
      edges.insert(std::pair<SgStatement *, SgStatement *>(declstmt, stmt));
    }
  }
  for (auto edge: edges) {
    nodes.insert(edge.first);
    nodes.insert(edge.second);
    dot << nodeToTag(edge.first) << " -> " << nodeToTag(edge.second) << "[label=\"declares\", style=\"dashed\", color=black, constraint=true];" << std::endl;
  }
}

std::string printLocatedNode(SgLocatedNode * node) {
  SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
    inputUnparseInfoPointer->set_SkipComments();
    inputUnparseInfoPointer->set_SkipWhitespaces();
    inputUnparseInfoPointer->set_SkipEnumDefinition();
    inputUnparseInfoPointer->set_SkipClassDefinition();
    inputUnparseInfoPointer->set_SkipFunctionDefinition();
    inputUnparseInfoPointer->set_SkipBasicBlock();
    inputUnparseInfoPointer->set_isTypeFirstPart();

  std::string unparsed = globalUnparseToString(node, inputUnparseInfoPointer);

  Sg_File_Info * pos = node->get_startOfConstruct();
  ROSE_ASSERT(pos != NULL);

  std::string file = pos->get_filenameString();
  size_t line = pos->get_line();

  std::ostringstream oss;
  oss << SageInterface::get_name(node) << " ( " << std::hex << node << std::dec << " | " << node->class_name() << " ) @ " << file << ":" << line;

  return oss.str();
}

void inspect(SgStatement * stmt, const TemplateInstantiationMap & tpl_inst_map, const DeclarationReferenceMap & decl_refs_map, std::ofstream & dot, std::set<SgStatement*> & nodes) {
  dot << nodeToTag(stmt) << "[label=\"" << nodeToLabel(stmt) << "\", color=red];" << std::endl;

  static std::set<SgStatement *> traversed;

  std::vector< SgStatement * > stmts;
  stmts.push_back( stmt );

  while (!stmts.empty()) {
    stmt = stmts.front();
    stmts.erase(stmts.begin());

    if (traversed.find(stmt) != traversed.end()) continue;
    traversed.insert(stmt);

    std::cout << "Inspecting: " << printLocatedNode(stmt) << std::endl;

    SgDeclarationStatement * declstmt = SageInterface::getEnclosingTemplateDeclaration(stmt);
    if (declstmt == NULL) {
      std::cout << "  Not declared inside a template declaration" << std::endl;

      declstmt = SageInterface::getEnclosingTemplateInstantiation(stmt);
      if (declstmt != NULL) {
        Sg_File_Info * pos = declstmt->get_startOfConstruct();
        ROSE_ASSERT(pos != NULL);
        if (!pos->isCompilerGenerated()) {
          declstmt = NULL;
        }
      }

      if (declstmt == NULL) {
        std::cout << "  Not declared inside a compiler generated template instantiation" << std::endl;
        continue;
      }
    }
    declstmt = declstmt->get_firstNondefiningDeclaration();
    ROSE_ASSERT(declstmt != NULL);

    if (declstmt->get_definingDeclaration() != NULL) {
      declstmt = declstmt->get_definingDeclaration();
    }

    std::cout << "  defined in: " << printLocatedNode(declstmt) << std::endl;

    nodes.insert(declstmt);
    dot << nodeToTag(stmt) << " -> " << nodeToTag(declstmt) << "[label=\"defined in\", color=red, constraint=true];" << std::endl;

    std::cout << "  instantiations:" << std::endl;

    std::set<SgDeclarationStatement *> instantiations;
    if (SageInterface::isTemplateDeclaration(declstmt)) {
      instantiations = tpl_inst_map.getInstantiationFromDeclaration(declstmt);
    } else {
      instantiations.insert(declstmt);
    }
    for (auto instantiation: instantiations) {
      if (isSgNonrealDecl(instantiation)) continue;

      if (declstmt != instantiation) {
        nodes.insert(instantiation);
        dot << nodeToTag(declstmt) << " -> " << nodeToTag(instantiation) << "[label=\"instantiated as\", color=red, constraint=true];" << std::endl;
      }

      std::set<SgStatement *> references;

      SgDeclarationStatement * curr = instantiation;
      SgDeclarationStatement * prev = NULL;
      while (curr != NULL) {
        if (curr->get_definingDeclaration() != NULL) {
          curr = curr->get_definingDeclaration();
        }

        std::cout << "  -> " << printLocatedNode(curr) << std::endl;

        nodes.insert(curr);

        const auto & references__ = decl_refs_map.getReferencingStatements(curr);
        references.insert(references__.begin(), references__.end());

        if (prev != NULL) {
          dot << nodeToTag(prev) << " -> " << nodeToTag(curr) << "[label=\"declared in\", color=red, constraint=true];" << std::endl;
        }
        prev = curr;

        // if instantiation had its own template arguments, it is not instantiated by a reference to its parent.
        SgTemplateArgumentPtrList * tpl_args = SageBuilder::getTemplateArgumentList(curr);
        if (tpl_args != NULL && !tpl_args->empty()) {
          curr = NULL;
          break;
        }

        // This loop handle cases such as:
        // 
        // template <typename T>
        // struct A {
        //   struct B {
        //     int foo();
        //   };
        // };
        // 
        // where a reference to A<int>::B or A<int> would instantiate A<int>::B::foo()

        SgScopeStatement * scope = curr->get_scope();
        SgFunctionDefinition * fdefn = isSgFunctionDefinition(scope);
        SgClassDefinition * xdefn = isSgClassDefinition(scope);
        SgDeclarationScope * nrscope = isSgDeclarationScope(scope);
        if (fdefn != NULL) {
          curr = fdefn->get_declaration();
          curr = curr->get_firstNondefiningDeclaration();
        } else if (xdefn != NULL) {
          curr = xdefn->get_declaration();
          curr = curr->get_firstNondefiningDeclaration();
        } else if (nrscope != NULL) {
          curr = isSgDeclarationStatement(nrscope->get_parent());
          curr = curr->get_firstNondefiningDeclaration();
        } else {
          curr = NULL;
          break;
        }
      }

      for (auto reference: references) {
        std::cout << "  -->> " << printLocatedNode(reference) << std::endl;
        nodes.insert(reference);
        dot << nodeToTag(instantiation) << " -> " << nodeToTag(reference) << "[label=\"referenced by\", color=red, constraint=true];" << std::endl;
        stmts.push_back(reference);

        Sg_File_Info * pos = reference->get_startOfConstruct();
        ROSE_ASSERT(pos != NULL);
        if (pos->isCompilerGenerated() || pos->get_file_id() < 0) {
          SgStatement * original = tpl_inst_map.getStatementDeclarationFromInstantiatedStatement(reference);
          if (original != NULL) {
            nodes.insert(original);
            dot << nodeToTag(reference) << " -> " << nodeToTag(original) << "[label=\"originally\", color=red, constraint=true];" << std::endl;
          }
        }
      }
    }
  }
}

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv+argc);

  std::set<std::string> nsp_filter_graphviz;

  std::vector< std::pair< std::string, size_t > > positions;
  bool add_references_to_graph = false;
  bool add_instantiations_to_graph = false;
  bool add_scopes_to_graph = false;

  auto arg = args.begin();
  while (arg != args.end()) {
    if (arg->find("--positions=") == 0) {

      std::istringstream stream_posarg(arg->substr(12));
      std::string posarg;
      while (std::getline(stream_posarg, posarg, ',')) {

        size_t column = posarg.find(':');

        std::string file = posarg.substr(0, column);
        
        std::stringstream line__(posarg.substr(column+1));
        size_t line;
        line__ >> line;

        positions.push_back(std::pair<std::string, size_t>(file, line));
      }
      arg = args.erase(arg);
    } else if (arg->find("--refgraph") == 0) {
      add_references_to_graph = true;
      arg = args.erase(arg);
    } else if (arg->find("--instgraph") == 0) {
      add_instantiations_to_graph = true;
      arg = args.erase(arg);
    } else if (arg->find("--scopegraph") == 0) {
      add_scopes_to_graph = true;
      arg = args.erase(arg);
    } else {
      arg++;
    }
  }

  SgProject * project = new SgProject(args);

  auto files = project->get_files();
  ROSE_ASSERT(files.size() == 1);
  std::string filename = files[0]->get_sourceFileNameWithoutPath();

  // GraphViz file and header

  std::ofstream dot(filename + ".dot");
  std::set<SgStatement*> graphviz_nodes;

  dot << "digraph refs {" << std::endl;
//dot << "  rankdir=\"LR\";" << std::endl;
  dot << "  graph [pad=\"0.5\", nodesep=\"1\", ranksep=\"2\"];" << std::endl;

  // Instantiations map
  std::cout << "Maps each template declaration to al of its instantiations" << std::endl;

  TemplateInstantiationMap tpl_inst_map;

  if (add_instantiations_to_graph)
    toGraphvizInstantiation(dot, tpl_inst_map, graphviz_nodes);

  // References map
  std::cout << "Maps statements to the declarations they are referencing" << std::endl;

  DeclarationReferenceMap decl_refs_map;

  if (add_references_to_graph)
    toGraphvizReferencingStatementMap(dot, decl_refs_map, graphviz_nodes);

  // Statement of interest
  std::cout << "Find statements for the given source positions" << std::endl;

  std::vector<SgStatement *> stmts;
  getStatementMatchingPositions(positions, stmts, files[0]);

  for (auto stmt: stmts) {
    inspect(stmt, tpl_inst_map, decl_refs_map, dot, graphviz_nodes);
  }

  if (add_scopes_to_graph)
    toGraphvizScopes(dot, graphviz_nodes);

  for (auto stmt: stmts) {
    graphviz_nodes.erase(stmt);
  }

  toGraphVizNodes(dot, graphviz_nodes);

  dot << "}" << std::endl;

  return 0;
}

