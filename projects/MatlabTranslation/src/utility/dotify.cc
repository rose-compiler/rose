#include <iostream>
#include <sstream>
#include <string>

//~ #include <boost/filesystem.hpp>

#include <boost/algorithm/string.hpp>

// #include "rose.h"
#include "sageInterface.h"
#include "sageGeneric.h"

#include "dotify.h"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace RoseUtils
{
  SgName nameOf(const SgVarRefExp& var_ref);
}


namespace dotify
{
  /// converts object of type E to T via string conversion
  template <class T, class E>
  static inline
  T conv(const E& el)
  {
    std::stringstream s;
    T                 res;

    s << el;
    s >> res;
    return res;
  }

  void edge(std::ostream& os, const void* src, const void* tgt, std::string lbl, std::string attr)
  {
    os << "  p" << size_t(src) << " -> p" << size_t(tgt)
       << "[ taillabel = \"" << lbl << "\" " << attr << "];" << std::endl;
  }

  void node(std::ostream& os, const void* n, std::string lbl, std::string attr)
  {
    os << "  p" << size_t(n) << "[ label = \"" << lbl << "\" " << attr << "];" << std::endl;
  }

  std::string short_name(SgNode& n)
  {
    std::string s = typeid(n).name();
    size_t      pre = s.find_first_of("Sg");

    ROSE_ASSERT(pre != std::string::npos);
    return s.substr(pre+2);
  }

  std::string short_name(SgNode* n)
  {
    if (n == NULL)
      return "<null>";

    return short_name(*n);
  }


  template <class T>
  static
  bool freshlyDiscovered(std::set<const T*>& aset, const T* elem)
  {
    if (aset.find(elem) != aset.end())
      return false;

    aset.insert(elem);
    return true;
  }

  template <class T>
  static inline
  bool freshlyDiscovered(std::set<const T*>& aset, const T& elem)
  {
    return freshlyDiscovered(aset, &elem);
  }

  static inline
  std::string nameIfNeeded(std::string s)
  {
    if (s.empty()) return "<noname>";

    return s;
  }


  struct DotPrinter
  {
    DotPrinter(std::ostream& s, attr_set options)
    : os(&s), opts(options), has_project(false), known_types(), known_init_names(), known_symbols()
    {}

    bool with_opt(int o) const { return (opts & o) == o; }

    void recurse(SgNode& n)
    {
      *this = sg::traverseChildren(*this, n);
    }

    void descend(SgNode* n)
    {
      //~ *this = sg::forAllNodes(*this, n, preorder);
      *this = sg::dispatch(*this, n);

      // recurse(*n);
    }

    void write_type(SgExpression& n, attr_set opt = ::dotify::types)
    {
      if (!with_opt(opt))
        return;

      prn_node_type(n.get_type());
      edge(*os, &n, n.get_type(), "", type_color);
    }

    void prn_node_type(SgType* n)
    {
      if (!freshlyDiscovered(known_types, n))
        return;

      *this = sg::dispatch(*this, n);
    }

    void prn_decllink(SgNode* src, SgInitializedName* decl, std::string edgelbl, std::string edgeattr)
    {
      edge(*os, src, decl, edgelbl, edgeattr);
    }

    void prn_decllink(SgNode* src, SgVariableSymbol* sy, std::string edgeattr)
    {
      std::string edgelbl = shortcut_lbl;

      if (with_opt(::dotify::symbols))
      {
        // include extra nodes and edges

        if (freshlyDiscovered(known_symbols, static_cast<SgSymbol*>(sy)))
          node(*os, sy, short_name(sy), decl_color);

        edge(*os, src, sy, "", edgeattr);

        edgelbl = "";
        src     = sy;
      }

      prn_decllink(src, sy ? sy->get_declaration() : 0, edgelbl, edgeattr);
    }

    void normal_node(SgLocatedNode& n, std::string lbl, std::string nodeattr)
    {
      //~ if (n.isCompilerGenerated()) return;

      node(*os, &n, lbl, nodeattr);
      edge(*os, n.get_parent(), &n, "", "");
    }

    void normal_node(SgLocatedNode& n, std::string nodeattr)
    {
      normal_node(n, short_name(n), nodeattr);
    }

    void expr_node(SgExpression& n, std::string id, attr_set opt = ::dotify::types)
    {
      node(*os, &n, id, expr_color);
      edge(*os, n.get_parent(), &n, "", "");
      write_type(n, opt);
      recurse(n);
    }
    
    void expr_node(SgExpression& n, attr_set opt = ::dotify::types)
    {
      expr_node(n, short_name(n), opt);
    }

    template <class SgValNode>
    void value_expression(SgValNode& n)
    {
      std::string val = conv<std::string>(n.get_value());
      //~ std::string val = n.get_valueString();

      node(*os, &n, short_name(n) + ": " + val, expr_color);
      edge(*os, n.get_parent(), &n, "", "");

      write_type(n, ::dotify::moretypes);
    }

    void handle(SgNode& n)
    {
      node(*os, &n, short_name(n), udef_color);
    }

    void handle(SgSourceFile& n)
    {
      node(*os, &n, short_name(n), udef_color);

      if (has_project)
        edge(*os, sg::ancestor<SgProject>(&n), &n, shortcut_lbl, "");

      recurse(n);
    }

    void handle(SgProject& n)
    {
      has_project = true;

      node(*os, &n, short_name(n), udef_color);

      for (int sz = n.numberOfFiles(); sz > 0; --sz)
      {
        descend(&n.get_file(sz-1));
      }
    }

    void handle(SgLocatedNode& n)
    {
      normal_node(n, short_name(n), udef_color);
      recurse(n);
    }

    void handle(SgType& n)
    {
      node(*os, &n, short_name(n), type_color);
    }

    void handle(SgArrayType& n)
    {
      std::string arrname = short_name(n);

      arrname = arrname + "[" + conv<std::string>(n.get_number_of_elements()) + "]"
                        + " @" + conv<std::string>(n.get_rank())
                        ;

      node(*os, &n, arrname, type_color);

      prn_node_type(n.get_base_type());
      descend(n.get_index());
      descend(n.get_dim_info());

      edge(*os, &n, n.get_base_type(), "",  type_color);
      //~ edge(*os, &n, n.get_index(),     "index", type_color);
      //~ edge(*os, &n, n.get_dim_info(),  "dim",   type_color);
    }

    void handle(SgInitializedName& n)
    {
      if (!freshlyDiscovered(known_init_names, n))
        return;

      known_init_names.insert(&n);
      normal_node(n, nameIfNeeded(n.get_name()), decl_color);

      if (!with_opt(::dotify::types))
        return;

      prn_node_type(n.get_type());
      edge(*os, &n, n.get_type(), "", type_color);
      recurse(n);
    }

    void handle(SgStatement& n)
    {
      normal_node(n, stmt_color);
      recurse(n);
    }
    
    void handle(SgRenamePair& n)
    {
      normal_node(n, udef_color);
      recurse(n);
    }

    void handle(SgDeclarationStatement& n)
    {
      normal_node(n, decl_color);
      recurse(n);
    }

    void handle(SgFunctionDeclaration& n)
    {
      if (  !with_opt(attr_set::builtins)
         && boost::starts_with(std::string(n.get_name()), "__")
         )
        return;

      normal_node(n, decl_color);
      recurse(n);
    }

    void handle(SgFunctionParameterList& n)
    {
      SgInitializedNamePtrList& args = n.get_args();

      // normal_node(n, short_name(n) + ": " + conv<std::string>(args.size()), udef_color);
      normal_node(n, short_name(n), udef_color);

      for (size_t i = 0; i < args.size(); ++i)
      {
        SgInitializedName& child = sg::deref(args.at(i));

        handle(child);

        // fortran special ..
        if (child.get_parent() != &n)
          edge(*os, &n, &child, unusual_lbl, udef_color);
      }
    }

    void handle(SgIntVal& n)         { value_expression(n); }
    void handle(SgFloatVal& n)       { value_expression(n); }
    void handle(SgDoubleVal& n)      { value_expression(n); }
    void handle(SgBoolValExp& n)     { value_expression(n); }
    // void handle(SgComplexVal& n) { value_expression(n); }

    void handle(SgExpression& n)     { expr_node(n); }
    void handle(SgExprListExp& n)    { expr_node(n, ::dotify::moretypes); }
    void handle(SgNullExpression& n) { expr_node(n, ::dotify::moretypes); }

    void handle(SgVarRefExp& n)
    {
      std::string ident("VarRefExp: ");
      
      ident += RoseUtils::nameOf(n);
      
      expr_node(n, ident);

      if (!with_opt(::dotify::decllinks))
        return;

      prn_decllink(&n, n.get_symbol(), "style = dotted");
    }

    std::ostream*                      os;
    attr_set                           opts;
    bool                               has_project;
    std::set<const SgType*>            known_types;
    std::set<const SgInitializedName*> known_init_names;
    std::set<const SgSymbol*>          known_symbols;

    static constexpr const char* decl_color = "color=gold";
    static constexpr const char* stmt_color = "color=blue";
    static constexpr const char* type_color = "color=red";
    static constexpr const char* expr_color = "color=indigo";
    static constexpr const char* udef_color = "color=green";

    static constexpr const char* shortcut_lbl = "**";
    static constexpr const char* unusual_lbl  = "??";
  };


  void dot_header(std::ostream& os, std::string s = "dotify")
  {
    os << "digraph " << s << " {" << std::endl;
  }

  void dot_footer(std::ostream& os)
  {
    os << "}" << std::endl;
  }

  void dot_noheader(std::ostream& os, SgNode& n, attr_set options)
  {
    // print type nodes
    //~ if ((attributes & attr::types) == attr::types)
    //~ {
      //~ dot_types(os, prog);

      //~ dot_type_relations(os, prog);
    //~ }

    sg::dispatch(DotPrinter(os, options), &n);
  }


  /// generates a dot file ( https://www.graphviz.org/doc/info/lang.html )
  ///   for the AST rooted in p. The attribute set determine if types and
  ///   decl-backlinks are included in the graph.
  void print_dot(std::ostream& os, SgNode& n, attr_set options)
  {
    dot_header(os);
    dot_noheader(os, n, options);
    dot_footer(os);
  }

  void save_dot(std::string s, SgNode& n, attr_set options)
  {
    std::fstream out(s, std::fstream::out);

    print_dot(out, n, options);
  }
}
