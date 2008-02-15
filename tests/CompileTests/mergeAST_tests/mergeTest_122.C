
// This demonstrates the error:
//      SgTemplateArgument is not in parent's child list, node: 0x7de170 = SgTemplateArgument = default name parent: 0x2a974846d8 = SgTemplateInstantiationDecl = _List_base
//      SgTemplateArgument is not in parent's child list, node: 0x7de200 = SgTemplateArgument = default name parent: 0x2a974846d8 = SgTemplateInstantiationDecl = _List_base
// This example also demonstrates extremely long mangled names.

namespace std
{
  template<typename T >
  class allocator 
  {     
  };

  template<typename T, typename S >
  class _List_base
  {
    public: _List_base(S __a ) { }
  };

  template<typename _Tp , typename _Alloc = allocator<_Tp> >
  class list : protected _List_base<_Tp, _Alloc>
    {
      typedef _List_base<_Tp, _Alloc> _Base;
      typedef _Alloc allocator_type;

    public:

      list(allocator_type __a = allocator_type()) : _Base(__a) { }
   // list() { }

    };

}

namespace std
{
  template<typename _CharT>
    class basic_string
    {
    };

  typedef basic_string<char> string;
}


std::list<std::string> validSuffixes;
