
#include "sage3basic.h"
#include "fixupTraversal.h"

namespace Rose {
namespace AST {

#define DEBUG_NameBasedSharing 0

static std::string declaration_position(SgLocatedNode * const lnode) {
  Sg_File_Info* fileInfo = lnode->get_file_info();
  ROSE_ASSERT(fileInfo != NULL);

  return "_F" + StringUtility::numberToString(fileInfo->get_file_id()) + \
         "_L" + StringUtility::numberToString(fileInfo->get_line())    + \
         "_C" + StringUtility::numberToString(fileInfo->get_col())     ;
}

static std::string declaration_is_defining(SgDeclarationStatement * const declstmt) {
  SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(declstmt);
  SgClassDeclaration * xdecl = isSgClassDeclaration(declstmt);
  if (
      ( fdecl != NULL && fdecl->get_definition() != NULL ) ||
      ( xdecl != NULL && xdecl->get_definition() != NULL )
  ) {
    return ":defining";
  } else {
    return "";
  }
}

static std::string generate_sharing_identifier(SgNode * const node) {
  ROSE_ASSERT(node != NULL);

  SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
  SgInitializedName * iname = isSgInitializedName(node);
  SgSymbol * symbol = isSgSymbol(node);
  SgType * type = isSgType(node);
  if (type != NULL) {
    SgNamedType * ntype = isSgNamedType(node);
    if (ntype) {
      SgDeclarationStatement * declaration = ntype->get_declaration();
      ROSE_ASSERT(declaration != NULL);
      return generate_sharing_identifier(declaration);
    } else {
      switch(node->variantT()) {
        case V_SgTemplateType:
        case V_SgModifierType:
          return ""; // Not shared
        default:
          return type->get_mangled();
      }
    }
  } else if (declstmt != NULL) {
    switch (declstmt->variantT()) {
      case V_SgFunctionDeclaration:
      case V_SgVariableDeclaration:
      case V_SgClassDeclaration:
      case V_SgPragmaDeclaration:
      case V_SgTemplateInstantiationDirectiveStatement:
      case V_SgTypedefDeclaration:
      case V_SgEnumDeclaration:
      case V_SgTemplateDeclaration:
      case V_SgUsingDeclarationStatement:
      case V_SgUsingDirectiveStatement:
      case V_SgMemberFunctionDeclaration:
      case V_SgTemplateInstantiationDecl:
      case V_SgTemplateInstantiationFunctionDecl:
      case V_SgTemplateInstantiationMemberFunctionDecl:
        return declstmt->get_mangled_name() + ":" + declaration_position(declstmt) + declaration_is_defining(declstmt);
      default:
        return ""; // Not shared
    }
  } else if (iname != NULL) {
    return iname->get_mangled_name() + ":" + declaration_position(iname);
  } else if (symbol != NULL) { 
    switch (symbol->variantT()) {
      case V_SgClassSymbol:
      case V_SgEnumFieldSymbol:
      case V_SgEnumSymbol:
      case V_SgFunctionSymbol:
      case V_SgMemberFunctionSymbol:
      case V_SgLabelSymbol:
      case V_SgNamespaceSymbol:
      case V_SgTemplateSymbol:
      case V_SgTypedefSymbol:
      case V_SgVariableSymbol:
        return generate_sharing_identifier(symbol->get_symbol_basis());
      default:
        return ""; // Not shared
    }
  } else {
    switch (node->variantT()) {
      case V_SgTemplateArgument:
        return ""; // TODO
      case V_SgTemplateParameter:
        return ""; // TODO
      default:
        return ""; // Not shared
    }
  }
}

struct NameBasedSharing : public ROSE_VisitTraversal {
  std::set<SgNode *> seen;
  std::map<std::string, std::vector<SgNode *> > name_to_nodes;

  void visit(SgNode * n) {
    if (!seen.insert(n).second) return;

    std::string name = generate_sharing_identifier(n);
    if (!name.empty()) {
      name = name + ":" + StringUtility::numberToString(n->variantT()); // Class last => less matches than if first
      name_to_nodes[name].push_back(n);
    }
  }

  void apply() {
    traverseMemoryPool();

#if DEBUG_NameBasedSharing
    std::cout << "#  NameBasedSharing::apply" << std::endl;
#endif

    std::map<SgNode*, SgNode*> replacements;
    for (std::map<std::string, std::vector<SgNode *> >::const_iterator it_map = name_to_nodes.begin(); it_map != name_to_nodes.end(); ++it_map) {
      std::vector<SgNode *> const & nodes = it_map->second;
      ROSE_ASSERT(nodes.size() > 0);

#if DEBUG_NameBasedSharing
      std::cout << "#    " << it_map->first << " -> " << nodes.size() << std::endl;
#endif

      std::vector<SgNode *>::const_iterator it_node = nodes.begin();
      SgNode * reference_node = *(it_node++);
      ROSE_ASSERT(reference_node != NULL);

      // Select proper reference for init-name:
      //   - can be from CG forward decl and have global scope
      if (isSgInitializedName(reference_node) && isSgFunctionParameterList(reference_node->get_parent())) {
        SgInitializedName * iname = (SgInitializedName *)reference_node;
        while (it_node != nodes.end() && !isSgFunctionDefinition(iname->get_scope())) {
          reference_node = *(it_node++);
        }
      }

#if DEBUG_NameBasedSharing
      std::cout << "#      reference_node = " << std::hex << reference_node << " ( " << reference_node->class_name() << " )" << std::endl;
#endif

      // Set reference_node as shared
      if (nodes.size() > 1) {
        if (reference_node->get_file_info() != NULL)
          reference_node->get_startOfConstruct()->setShared();
        if (reference_node->get_endOfConstruct() != NULL)
          reference_node->get_endOfConstruct()->setShared();
      }

      // Deal with the duplicates
      it_node = nodes.begin();
      while (it_node != nodes.end()) {
        SgNode * duplicate_node = *(it_node++);
        ROSE_ASSERT(duplicate_node != NULL);
        if (duplicate_node != reference_node) {
          replacements.insert(std::pair<SgNode*, SgNode*>(duplicate_node, reference_node));
        }
      }
    }

    fixupTraversal(replacements);
  }
};

void shareRedundantNodes(SgProject * project) {
  NameBasedSharing nbs;
  nbs.apply();
}

}
}

