/** 
 * \file MFB/include/MFB/Sage/graph.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef _MFB_SAGE_GRAPH_HPP_
#define _MFB_SAGE_GRAPH_HPP_

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <utility>

class SgNode;
class SgSymbol;
class SgType;
class SgExpression;
class SgDeclarationStatement;
class SgInitializedName;
class SgNonrealSymbol;

namespace MFB {

class api_t;

namespace Graph {

class API {
  public:
    typedef std::set<SgNode *> nodes_set_t;

    enum link_kind_e {
      e_link_kind_unknown = 0,
      e_link_kind_defines,
      e_link_kind_extends,
      e_link_kind_specializes,
      e_link_kind_represents,
      e_link_kind_param,
      e_link_kind_arg,
      e_link_kind_type_base,
      e_link_kind_last
    };

    typedef std::pair<link_kind_e, unsigned short> edge_param_t;
    typedef std::map<SgNode *, std::set<edge_param_t> > destination_map_t;
    typedef std::map<SgNode *, destination_map_t > edges_map_t;

  protected:
    std::string name;

    nodes_set_t nodes;

    edges_map_t edges;

  protected:
    static std::string graphviz_node_desc(SgNode * p);

    static std::string graphviz_node_color(SgNode * p);

    static std::string graphviz_node_shape(SgNode * p);

    static std::string graphviz_link_color(link_kind_e kind);

    static std::string graphviz_link_style(link_kind_e kind);

    static std::string graphviz_link_head(link_kind_e kind);

    static std::string graphviz_link_tail(link_kind_e kind);

    static bool graphviz_link_constraint(link_kind_e kind);

    static void write_node(std::ostream & out, SgNode * p);

    static void write_edge(std::ostream & out, SgNode * src, SgNode * dest, link_kind_e kind, unsigned short cnt);

    void collect_symbol_edges(SgType * type, SgSymbol * sym, link_kind_e kind, unsigned short cnt = 0);

    void collect_symbol_edges(SgExpression * expr, SgSymbol * sym, link_kind_e kind, unsigned short cnt = 0);

    void collect_symbol_edges(SgDeclarationStatement * templ, SgSymbol * sym, link_kind_e kind, unsigned short cnt = 0);

    void collect_symbol_edges(SgInitializedName * iname, SgSymbol * sym, link_kind_e kind, unsigned short cnt = 0);

  public:
    API(const std::string & name__ = "api");

    void add(const api_t & api);

    void add(SgNode * n);

    void link(SgNode* src, SgNode * dst, link_kind_e kind, unsigned short cnt = 0);

    void toGraphViz(const std::string & filename) const;
    void toGraphViz(std::ostream & out) const;
};

}

}

#endif

