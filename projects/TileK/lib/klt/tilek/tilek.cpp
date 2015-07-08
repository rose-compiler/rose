
#include "sage3basic.h"

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/kernel.hpp"

#include "MFB/KLT/tilek.hpp"

namespace KLT {

template <>
size_t LoopTrees<DLX::KLT::Annotation<DLX::TileK::language_t> >::id_cnt = 0;

template <>
unsigned long Kernel<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::id_cnt = 0;

template <>
unsigned long Kernel<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK>::kernel_desc_t::id_cnt = 0;

template <>
SgFunctionParameterList * createParameterList<
  DLX::KLT::Annotation<DLX::TileK::language_t>,
  Language::None,
  Runtime::TileK
>(
  Kernel<
    DLX::KLT::Annotation<DLX::TileK::language_t>,
    Language::None,
    Runtime::TileK
  > * kernel
) {
  const std::list<SgVariableSymbol *> & params = kernel->getArguments().parameters;
  const std::list<SgVariableSymbol *> & scalars = kernel->getArguments().scalars;
  const std::list<Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *> & datas = kernel->getArguments().datas;

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list<Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *>::const_iterator it_data;

  SgFunctionParameterList * result = SageBuilder::buildFunctionParameterList();

  result->append_arg(SageBuilder::buildInitializedName("param", SageBuilder::buildPointerType(SageBuilder::buildIntType()), NULL));	
  result->append_arg(SageBuilder::buildInitializedName("data",  SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));

  assert(Runtime::TileK::kernel_api.context_class != NULL);
  result->append_arg(SageBuilder::buildInitializedName(
    "context", SageBuilder::buildModifierType(SageBuilder::buildPointerType(Runtime::TileK::kernel_api.context_class->get_declaration()->get_type())), NULL
  ));

  return result;
}

template <>
void printAnnotations<DLX::KLT::Annotation<DLX::TileK::language_t> >(
  const std::vector<DLX::KLT::Annotation<DLX::TileK::language_t> > & annotations,
  std::ostream & out,
  std::string indent
) {
  out << DLX::TileK::language_t::language_label << "(";
  if (!annotations.empty()) { 
    std::vector<DLX::KLT::Annotation<DLX::TileK::language_t> >::const_iterator it_annotation = annotations.begin();
    out << "[" << it_annotation->clause->kind;
    it_annotation++;
    for (; it_annotation != annotations.end(); it_annotation++)
      out << "], [" << it_annotation->clause->kind;
    out << "]";
  }
  out << "), " << std::endl;
}

} // namespace KLT

