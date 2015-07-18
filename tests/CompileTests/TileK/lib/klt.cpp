
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation;

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "MDCG/TileK/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "KLT/Core/kernel.hpp"

namespace KLT {

template <> size_t ::KLT::LoopTrees<Annotation>::id_cnt = 0;
template <> size_t ::KLT::Kernel<Annotation, Runtime>::id_cnt = 0;
template <> size_t ::KLT::Kernel<Annotation, Runtime>::kernel_desc_t::id_cnt = 0;

template <>
SgFunctionParameterList * Kernel<Annotation, Runtime>::createParameterList() const {
  return SageBuilder::buildFunctionParameterList(
#ifdef TILEK_THREADS
    SageBuilder::buildInitializedName("tid",  SageBuilder::buildIntType(), NULL),
#endif
    SageBuilder::buildInitializedName("param",  SageBuilder::buildPointerType(SageBuilder::buildIntType()), NULL),
    SageBuilder::buildInitializedName("data",   SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL),
    SageBuilder::buildInitializedName("scalar", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL),
    Runtime::kernel_api.createContext()
  );
}

template <>
void Kernel<Annotation, Runtime>::setRuntimeSpecificKernelField(SgVariableSymbol * kernel_sym, SgBasicBlock * bb) const {
#ifdef TILEK_THREADS
  std::vector<Annotation>::const_iterator it;
  ::DLX::TileK::language_t::num_threads_clause_t * num_threads_clause = NULL;
  for (it = p_loop_tree.annotations.begin(); it != p_loop_tree.annotations.end(); it++) {
    num_threads_clause = ::DLX::TileK::language_t::isNumThreadsClause(it->clause);
    if (num_threads_clause != NULL) break;
  }
  assert(num_threads_clause != NULL);
  assert(kernel_sym != NULL);
  assert(kernel_sym->get_declaration() != NULL);
  assert(Runtime::host_api.user->kernel_num_threads_field != NULL);
  assert(Runtime::host_api.user->kernel_num_threads_field->get_declaration() != NULL);

  SageInterface::appendStatement(SageBuilder::buildExprStatement(
    SageBuilder::buildAssignOp(
      SageBuilder::buildArrowExp(
        SageBuilder::buildVarRefExp(kernel_sym),
        SageBuilder::buildVarRefExp(Runtime::host_api.user->kernel_num_threads_field)
      ),
      SageBuilder::buildIntVal(num_threads_clause->parameters.num_threads)
    )
  ), bb);
#endif
}

} // namespace KLT

