// This example code demonstrates a possible error:
// ##### WARNING: statement = 0x2a972387a8 = SgTemplateInstantiationDecl not in child list of parent = 0x62ea10 = SgNamespaceDefinitionStatement
namespace std {
     template<typename _Alloc> class allocator;
     template<typename _CharT, typename _Alloc > class basic_string;

  // Here "allocator<char>" is built as a template argument, but is assigned SgNamespaceDefinition as a parent 
     typedef basic_string<char,allocator<char> > string;
   }

namespace std {
     template<typename _Tp> class allocator {};

  // Error: "extern" keyword is dropped in the unparsed output!
     extern template class allocator<char>;
   }
