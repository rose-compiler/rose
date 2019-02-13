

#ifndef _F2DOT_HPP

#define _F2DOT_HPP 1

namespace f2cxx
{
  enum attr_set
  {
    types     = (1 << 0),
    moretypes = (1 << 1) + types,
    decllinks = (1 << 2),
    symbols   = (1 << 3),
    builtins  = (1 << 4),
    //~ labels    = (1 << 3),
    simple    = 0,
    balanced  = (types    | decllinks),
    full      = (balanced | moretypes | symbols | builtins)
  };

  /// writes graph to a dot file ( https://www.graphviz.org/doc/info/lang.html )
  ///   for the AST rooted in n. The attribute set determine if types and
  ///   decl-backlinks are included in the graph.
  void print_dot(std::ostream& out, SgNode& n, attr_set options = balanced);

  /// opens the file "outfile" and writes out the graph rooted in n.
  void save_dot(std::string outfile, SgNode& n, attr_set options = balanced);
}


#endif /* _F2DOT_HPP */
