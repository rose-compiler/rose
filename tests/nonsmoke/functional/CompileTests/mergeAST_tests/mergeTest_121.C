
// Simple example demonstrating error (remaining know problem error in handling rose.h within merge):
//      SgTemplateArgument is not in parent's child list, node: 0x248de68 = SgTemplateArgument = default name parent: 0x2a97e76660 = SgTemplateInstantiationDecl = new_allocator
//      SgTemplateArgument is not in parent's child list, node: 0x248df88 = SgTemplateArgument = default name parent: 0x2a97e74f98 = SgTemplateInstantiationDecl = allocator
//      SgTemplateArgument is not in parent's child list, node: 0x248e060 = SgTemplateArgument = default name parent: 0x2a97e708b8 = SgTemplateInstantiationDecl = _List_base
//      SgTemplateArgument is not in parent's child list, node: 0x248e0f0 = SgTemplateArgument = default name parent: 0x2a97e708b8 = SgTemplateInstantiationDecl = _List_base

#include<list>
#include<string>

std::list<std::string> validSuffixes;
