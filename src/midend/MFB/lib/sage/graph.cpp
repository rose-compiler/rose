
#include "sage3basic.h"

#include "MFB/Sage/graph.hpp"
#include "MFB/Sage/api.hpp"
#include "MFB/utils.hpp"

namespace MFB {

namespace Graph {

std::string API::graphviz_node_desc(SgNode * p) {
  if (isSgGlobal(p))
    return "GLOBAL";

  SgSymbol * sym = isSgSymbol(p);
  SgType * type = isSgType(p);
  SgValueExp * val = isSgValueExp(p);

  if (sym != NULL) {
    return sym->get_name().str();
  } else if (type != NULL) {
    SgModifierType * mt = isSgModifierType(type);
    SgPointerType * pt = isSgPointerType(type);
    if (mt != NULL) {
      return "Type Modifier";
    } else if (pt != NULL) {
      return "Pointer";
    } else {
      return type->unparseToString();
    }
  } else if (val != NULL) {
    return val->unparseToString();
  } else {
    std::cerr << p->class_name() << std::endl;
    ROSE_ASSERT(false);
  }
}

std::string API::graphviz_node_color(SgNode * p) {
  if (isSgScopeStatement(p) || isSgNamespaceSymbol(p))
    return "black";

  SgSymbol * sym = isSgSymbol(p);
  SgType * type = isSgType(p);
  SgValueExp * val = isSgValueExp(p);

  if (sym != NULL) {
    return "green";
  } else if (type != NULL) {
    return "blue";
  } else if (val != NULL) {
    return "red";
  } else {
    std::cerr << p->class_name() << std::endl;
    ROSE_ASSERT(false);
  }
}

std::string API::graphviz_node_shape(SgNode * p) {
  if (isSgGlobal(p) || isSgNamespaceSymbol(p))
    return "plain";

  SgSymbol * sym = isSgSymbol(p);
  SgType * type = isSgType(p);
  SgValueExp * val = isSgValueExp(p);

  if (type != NULL) {
    return "ellipse";
  } else if (sym != NULL) {
    switch (p->variantT()) {
      case V_SgClassSymbol:
      case V_SgTemplateClassSymbol:
      case V_SgTypedefSymbol:
      case V_SgTemplateTypedefSymbol:
        return "box";

      case V_SgVariableSymbol:
      case V_SgTemplateVariableSymbol:
      case V_SgFunctionSymbol:
      case V_SgTemplateFunctionSymbol:
      case V_SgMemberFunctionSymbol:
      case V_SgTemplateMemberFunctionSymbol:
        return "diamond";

      case V_SgNonrealSymbol:
        return "octagon";

      default: std::cerr << p->class_name() << std::endl; ROSE_ASSERT(false);
    }
  } else if (val != NULL) {
    return "plain";
  } else {
    std::cerr << p->class_name() << std::endl;
    ROSE_ASSERT(false);
  }
}

std::string API::graphviz_link_color(API::link_kind_e kind) {
  switch (kind) {
    case e_link_kind_defines:     return "black";
    case e_link_kind_extends:     return "green";
    case e_link_kind_specializes: return "red";
    case e_link_kind_represents:  return "blue";
    case e_link_kind_param:       return "purple";
    case e_link_kind_arg:         return "orange";
    case e_link_kind_type_base:   return "blue";
    default: ROSE_ASSERT(false);
  }
}

std::string API::graphviz_link_style(API::link_kind_e kind) {
  switch (kind) {
    case e_link_kind_defines:     return "solid";
    case e_link_kind_extends:     return "solid";
    case e_link_kind_specializes: return "dashed";
    case e_link_kind_represents:  return "dashed";
    case e_link_kind_param:       return "dotted";
    case e_link_kind_arg:         return "dotted";
    case e_link_kind_type_base:   return "dotted";
    default: ROSE_ASSERT(false);
  }
}

std::string API::graphviz_link_head(API::link_kind_e kind) {
  switch (kind) {
    case e_link_kind_defines:     return "open";
    case e_link_kind_extends:     return "empty";
    case e_link_kind_specializes: return "empty";
    case e_link_kind_represents:  return "open";
    case e_link_kind_param:       return "normal";
    case e_link_kind_arg:         return "normal";
    case e_link_kind_type_base:   return "normal";
    default: ROSE_ASSERT(false);
  }
}

std::string API::graphviz_link_tail(API::link_kind_e kind) {
  switch (kind) {
    case e_link_kind_defines:     return "diamond";
    case e_link_kind_extends:     return "none";
    case e_link_kind_specializes: return "none";
    case e_link_kind_represents:  return "odiamond";
    case e_link_kind_param:       return "odot";
    case e_link_kind_arg:         return "dot";
    case e_link_kind_type_base:   return "none";
    default: ROSE_ASSERT(false);
  }
}

bool API::graphviz_link_constraint(API::link_kind_e kind) {
  return true; // kind == e_link_kind_defines || kind == e_link_kind_extends || kind == e_link_kind_represents || kind == e_link_kind_param || kind == e_link_kind_arg;
}

void API::write_node(std::ostream & out, SgNode * p) {
  std::string desc = graphviz_node_desc(p);
  std::string color = graphviz_node_color(p);
  std::string shape = graphviz_node_shape(p);
  out << "  n_" << std::hex << p << "[label=\"" << desc << "\", color=\"" << color << "\", shape=\"" << shape << "\"];" << std::endl;
}

void API::write_edge(std::ostream & out, SgNode * src, SgNode * dest, API::link_kind_e kind, unsigned short cnt) {
//if (kind == e_link_kind_defines) return;

  std::string color = graphviz_link_color(kind);
  std::string style = graphviz_link_style(kind);
  std::string head = graphviz_link_head(kind);
  std::string tail = graphviz_link_tail(kind);

  bool constraint = graphviz_link_constraint(kind);

  out << "  n_" << std::hex << src << " -> n_" << std::hex << dest << "[color=\"" << color << "\", style=\"" << style << "\", arrowhead=\"" << head << "\", arrowtail=\"" << tail << "\"";
  if (kind == e_link_kind_param) {
    out << ", label=\"P[" << cnt << "]\"";
  }
  if (kind == e_link_kind_arg) {
    out << ", label=\"A[" << cnt << "]\"";
  }
  if (!constraint) {
    out << ", constraint=false";
  }
  out << "];" << std::endl;
      
//std::cout << std::hex << src << "\t" << std::hex << dest << "\t" << kind << "\t" << cnt << std::endl;
}

void API::collect_symbol_edges(SgType * type, SgSymbol * sym, API::link_kind_e kind, unsigned short cnt) {
  if (type == NULL) return;

  link(sym, type, kind, cnt);
}

void API::collect_symbol_edges(SgExpression * expr, SgSymbol * sym, API::link_kind_e kind, unsigned short cnt) {
  if (expr == NULL) return;

  if (isSgValueExp(expr)) {
    link(sym, expr, kind, cnt);
  } else {
    std::cerr << expr << " (" << expr->class_name() << ")" << std::endl;
    ROSE_ASSERT(false); // when does that happen?
  }
}

void API::collect_symbol_edges(SgDeclarationStatement * templ, SgSymbol * sym, API::link_kind_e kind, unsigned short cnt) {
  if (templ == NULL) return;

  std::cerr << templ << " (" << templ->class_name() << ")" << std::endl;
  ROSE_ASSERT(false); // FIXME NIY
}

void API::collect_symbol_edges(SgInitializedName * iname, SgSymbol * sym, API::link_kind_e kind, unsigned short cnt) {
  if (iname == NULL) return;

  SgSymbol * s = iname->search_for_symbol_from_symbol_table();
  ROSE_ASSERT(s != NULL);

  link(sym, s, kind, cnt);
}

API::API(const std::string & name__) :
  name(name__),
  nodes(),
  edges()
{}

void API::add(SgNode * n) {
  ROSE_ASSERT(n != NULL);

  if (nodes.find(n) == nodes.end()) {
    nodes.insert(n);

    SgSymbol * sym = isSgSymbol(n);
    SgType * type = isSgType(n);
    SgExpression * expr = isSgExpression(n);

    if (sym != NULL) {
      SgScopeStatement * scope = sym->get_scope();
      ROSE_ASSERT(scope != NULL);
      if (!isSgGlobal(scope)) {
        SgDeclarationStatement * scp_parent = isSgDeclarationStatement(scope->get_parent());        
        ROSE_ASSERT(scp_parent != NULL);

        SgSymbol * scp_sym = scp_parent->search_for_symbol_from_symbol_table();
        ROSE_ASSERT(scp_sym != NULL);

        link(scp_sym, sym, e_link_kind_defines);
      }

      SgSymbol * tpl_sym = NULL;
      SgDeclarationScope * nrscope = NULL;
      SgTemplateParameterPtrList * tpl_params = NULL;
      SgTemplateArgumentPtrList * tpl_args = NULL;

      ::MFB::Utils::collect_symbol_template_info(sym, tpl_sym, nrscope, tpl_params, tpl_args);

      if (tpl_sym != NULL) {
        link(sym, tpl_sym, e_link_kind_specializes);
      }
          
      if (tpl_params != NULL && tpl_params->size() > 0) {
        unsigned short cnt = 0;
        for (SgTemplateParameterPtrList::const_iterator it = tpl_params->begin(); it != tpl_params->end(); it++) {
          ROSE_ASSERT( (*it)->get_type() || (*it)->get_expression() || (*it)->get_templateDeclaration() || (*it)->get_initializedName() );

//        std::cout << "P: " << std::hex << sym << ": " << cnt << std::endl;

          collect_symbol_edges((*it)->get_type(), sym, e_link_kind_param, cnt);
          collect_symbol_edges((*it)->get_expression(), sym, e_link_kind_param, cnt);
          collect_symbol_edges((*it)->get_templateDeclaration(), sym, e_link_kind_param, cnt);
          collect_symbol_edges((*it)->get_initializedName(), sym, e_link_kind_param, cnt);
          cnt++;
        }
      }

      if (tpl_args != NULL && tpl_args->size() > 0) {
        unsigned short cnt = 0;
        for (SgTemplateArgumentPtrList::const_iterator it = tpl_args->begin(); it != tpl_args->end(); it++) {
          ROSE_ASSERT( (*it)->get_type() || (*it)->get_expression() || (*it)->get_templateDeclaration() || (*it)->get_initializedName() );

//        std::cout << "A: " << std::hex << sym << ": " << cnt << std::endl;

          collect_symbol_edges((*it)->get_type(), sym, e_link_kind_arg, cnt);
          collect_symbol_edges((*it)->get_expression(), sym, e_link_kind_arg, cnt);
          collect_symbol_edges((*it)->get_templateDeclaration(), sym, e_link_kind_arg, cnt);
          collect_symbol_edges((*it)->get_initializedName(), sym, e_link_kind_arg, cnt);
          cnt++;
        }
      }
    } else if (type != NULL) {
      SgNamedType * nt = isSgNamedType(type);
      SgModifierType * mt = isSgModifierType(type);
      SgPointerType * pt = isSgPointerType(type);
      if (nt != NULL) {
        SgDeclarationStatement * declstmt = nt->get_declaration();
        ROSE_ASSERT(declstmt != NULL);
                  
        SgSymbol * s = declstmt->search_for_symbol_from_symbol_table();
        ROSE_ASSERT(s != NULL);

        link(type, s, e_link_kind_represents);
      } else if (mt != NULL) {
        SgType * bt = mt->get_base_type();
        ROSE_ASSERT(bt != NULL);

        link(type, bt, e_link_kind_type_base);
      } else if (pt != NULL) {
        SgType * bt = pt->get_base_type();
        ROSE_ASSERT(bt != NULL);

        link(type, bt, e_link_kind_type_base);
      } else {
        std::cerr << type->class_name() << std::endl;
      }
    } else if (expr != NULL) {
      SgValueExp * val = isSgValueExp(expr);
      if (val != NULL) {
        // NOP
      } else {
        std::cerr << expr->class_name() << std::endl;
        ROSE_ASSERT(false);
      }
    }
  }
}

void API::link(SgNode* src, SgNode * dst, API::link_kind_e kind, unsigned short cnt) {
  add(src);
  add(dst);
  edges[src][dst].insert(edge_param_t(kind,cnt));
}

void API::toGraphViz(const std::string & filename) const {
  std::ofstream out(filename.c_str());
  toGraphViz(out);
  out.close();
}

void API::toGraphViz(std::ostream & out) const {
  out << "digraph " << name << " {" << std::endl;
  for (nodes_set_t::const_iterator it = nodes.begin(); it != nodes.end(); it ++) {
    write_node(out, *it);
  }
  for (edges_map_t::const_iterator it_src = edges.begin(); it_src != edges.end(); it_src++) {
    for (destination_map_t::const_iterator it_dst = it_src->second.begin(); it_dst != it_src->second.end(); it_dst++) {
      for (std::set<edge_param_t>::const_iterator it_edge = it_dst->second.begin(); it_edge != it_dst->second.end(); it_edge++) {
        write_edge(out, it_src->first, it_dst->first, it_edge->first, it_edge->second);
      }
    }
  }
  out << "}" << std::endl;
}

void API::add(const api_t & api) {

  // Namespaces

  for (std::set<SgNamespaceSymbol *>::const_iterator it = api.namespace_symbols.begin(); it != api.namespace_symbols.end(); it++) {
    SgNamespaceSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);
  }

  // Functions

  for (std::set<SgFunctionSymbol *>::const_iterator it = api.function_symbols.begin(); it != api.function_symbols.end(); it++) {
    SgFunctionSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);

    // Declaration

    SgFunctionDeclaration * decl = sym->get_declaration();
    ROSE_ASSERT(decl != NULL);
  }

  // Classes

  for (std::set<SgClassSymbol *>::const_iterator it = api.class_symbols.begin(); it != api.class_symbols.end(); it++) {
    SgClassSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);

    // Declaration

    SgClassDeclaration * decl = sym->get_declaration();
    ROSE_ASSERT(decl != NULL);

    // Definition

    SgClassDefinition * defn = decl->get_definingDeclaration() ? isSgClassDeclaration(decl->get_definingDeclaration())->get_definition() : decl->get_definition();

    // Base Classes

    if (defn != NULL) {
      const SgBaseClassPtrList & inheritances = defn->get_inheritances();
      for (SgBaseClassPtrList::const_iterator it = inheritances.begin(); it != inheritances.end(); it++) {
        SgBaseClass * bclass = *it;
        ROSE_ASSERT(bclass != NULL);

        SgExpBaseClass * ebclass = isSgExpBaseClass(bclass);
        ROSE_ASSERT(ebclass == NULL); // TODO

        SgNonrealBaseClass * nrbclass = isSgNonrealBaseClass(bclass);

        SgDeclarationStatement * bdecl = nrbclass ? (SgDeclarationStatement*)nrbclass->get_base_class_nonreal() : (SgDeclarationStatement*)bclass->get_base_class();
        ROSE_ASSERT(bdecl != NULL);

        SgSymbol * bsym = bdecl->search_for_symbol_from_symbol_table();
        ROSE_ASSERT(bsym != NULL);

        link(sym, bsym, e_link_kind_extends);
      }
    }
  }

  // Variable

  for (std::set<SgVariableSymbol *>::const_iterator it = api.variable_symbols.begin(); it != api.variable_symbols.end(); it++) {
    SgVariableSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);

    // Declaration

    SgInitializedName * decl = sym->get_declaration();
    ROSE_ASSERT(decl != NULL);

    // Template Specialization

    // TODO 
  }

  // Methods

  for (std::set<SgMemberFunctionSymbol *>::const_iterator it = api.member_function_symbols.begin(); it != api.member_function_symbols.end(); it++) {
    SgMemberFunctionSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);

    // Declaration

    SgMemberFunctionDeclaration * decl = sym->get_declaration();
    ROSE_ASSERT(decl != NULL);
  }

  // Typedef

  for (std::set<SgTypedefSymbol *>::const_iterator it = api.typedef_symbols.begin(); it != api.typedef_symbols.end(); it++) {
    SgTypedefSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);

    // Declaration

    SgTypedefDeclaration * decl = sym->get_declaration();
    ROSE_ASSERT(decl != NULL);

    SgType * type = decl->get_base_type();
    ROSE_ASSERT(type != NULL);

    SgNamedType * ntype = isSgNamedType(type);
    if (ntype != NULL) {
      SgDeclarationStatement * declstmt = ntype->get_declaration();
      ROSE_ASSERT(declstmt != NULL);
                  
      SgSymbol * nsym = declstmt->search_for_symbol_from_symbol_table();
      ROSE_ASSERT(nsym != NULL);

      link(sym, nsym, e_link_kind_represents);
    } else {
      link(sym, type, e_link_kind_represents);
    }
  }

  // Nonreal

  for (std::set<SgNonrealSymbol *>::const_iterator it = api.nonreal_symbols.begin(); it != api.nonreal_symbols.end(); it++) {
    SgNonrealSymbol * sym = *it;
    ROSE_ASSERT(sym != NULL);

    add(sym);

    // Declaration

    SgNonrealDecl * decl = sym->get_declaration();
    ROSE_ASSERT(decl != NULL);

    SgNode * parent = decl->get_parent();
    ROSE_ASSERT(parent != NULL);
    ROSE_ASSERT(isSgDeclarationScope(parent) != NULL);
    parent = parent->get_parent();
    ROSE_ASSERT(parent != NULL);

    link(parent, sym, e_link_kind_defines);
  }
}

}

}

