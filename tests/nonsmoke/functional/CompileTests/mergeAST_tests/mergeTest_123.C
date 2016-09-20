
// This is a version of the above error that can be modified as required.
// This demonstrates the error:
//      SgTemplateArgument is not in parent's child list, node: 0x7de170 = SgTemplateArgument = default name parent: 0x2a974846d8 = SgTemplateInstantiationDecl = _List_base
//      SgTemplateArgument is not in parent's child list, node: 0x7de200 = SgTemplateArgument = default name parent: 0x2a974846d8 = SgTemplateInstantiationDecl = _List_base
// This example also demonstrates extremely long mangled names.

// I expect that the SgTemplateArgument List is being shared after it has been built for
// the defining and non-defining template instantiation and that the SgTemplateArgument
// in one of the list that is deleted is not deleted (and is orphaned).

  template<typename S > class _List_base {};

  class list : public _List_base<int>
    {
      list() : _List_base<int>() { }
    };

