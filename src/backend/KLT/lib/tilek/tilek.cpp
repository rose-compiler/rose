
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation;

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "KLT/Core/kernel.hpp"

namespace KLT {

template <>
size_t ::KLT::LoopTrees<Annotation>::id_cnt = 0;

template <>
unsigned long ::KLT::Kernel<Annotation, Runtime>::id_cnt = 0;

template <>
unsigned long ::KLT::Kernel<Annotation, Runtime>::kernel_desc_t::id_cnt = 0;

template <>
SgFunctionParameterList * Kernel<Annotation, Runtime>::createParameterList() const {
  const std::list<SgVariableSymbol *> & params = getArguments().parameters;
  const std::list<SgVariableSymbol *> & scalars = getArguments().scalars;
  const std::list<Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *> & datas = getArguments().datas;

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list<Data<DLX::KLT::Annotation<DLX::TileK::language_t> > *>::const_iterator it_data;

  SgFunctionParameterList * result = SageBuilder::buildFunctionParameterList();

  result->append_arg(SageBuilder::buildInitializedName("param",  SageBuilder::buildPointerType(SageBuilder::buildIntType()), NULL));
  result->append_arg(SageBuilder::buildInitializedName("data",   SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
  result->append_arg(SageBuilder::buildInitializedName("scalar", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));

  result->append_arg(Runtime::kernel_api.createContext());

  return result;
}
/*
template <>
void printAnnotations<Annotation>(const std::vector<Annotation> & annotations, std::ostream & out, std::string indent) {
  out << Dlang::language_label << "(";
  if (!annotations.empty()) { 
    std::vector<Annotation>::const_iterator it_annotation = annotations.begin();
    out << "[" << it_annotation->clause->kind;
    it_annotation++;
    for (; it_annotation != annotations.end(); it_annotation++)
      out << "], [" << it_annotation->clause->kind;
    out << "]";
  }
  out << "), " << std::endl;
}
*/
} // namespace KLT

