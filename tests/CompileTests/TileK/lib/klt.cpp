
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation;

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
typedef ::KLT::Language::OpenCL Klang; // Kernel Language
#  elif defined(TILEK_TARGET_CUDA)
typedef ::KLT::Language::CUDA Klang; // Kernel Language
#  endif
#else
typedef ::KLT::Language::C Klang; // Kernel Language
#endif

#include "MDCG/TileK/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "KLT/Core/kernel.hpp"

namespace KLT {

template <> size_t ::KLT::LoopTrees<Annotation>::id_cnt = 0;
template <> size_t ::KLT::Kernel<Annotation, Runtime>::id_cnt = 0;
template <> size_t ::KLT::Kernel<Annotation, Runtime>::kernel_desc_t::id_cnt = 0;

template <>
SgFunctionParameterList * Kernel<Annotation, Runtime>::createParameterList() const {
  SgInitializedName * klt_loop_context = Runtime::kernel_api.createContext();
  SgFunctionParameterList * param_list = NULL;
#if defined(TILEK_ACCELERATOR)
  param_list = SageBuilder::buildFunctionParameterList();
  std::list<SgVariableSymbol *>::const_iterator it_vsym;
  std::list<Data<Annotation> *>::const_iterator it_data;
  for (it_vsym = p_arguments.parameters.begin(); it_vsym != p_arguments.parameters.end(); it_vsym++) {
    param_list->append_arg(SageBuilder::buildInitializedName((*it_vsym)->get_name().getString(),  (*it_vsym)->get_type(), NULL));
  }
  for (it_vsym = p_arguments.scalars.begin(); it_vsym != p_arguments.scalars.end(); it_vsym++) {
    param_list->append_arg(SageBuilder::buildInitializedName((*it_vsym)->get_name().getString(),  (*it_vsym)->get_type(), NULL));
  }
  for (it_data = p_arguments.datas.begin(); it_data != p_arguments.datas.end(); it_data++) {
    std::string name((*it_data)->getVariableSymbol()->get_name().getString());
    SgType * type = SageBuilder::buildPointerType((*it_data)->getBaseType());
#if defined(TILEK_TARGET_OPENCL)
    SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclGlobal();
    type = mod_type;
#elif defined(TILEK_TARGET_CUDA)
    // no modifier
#endif
    param_list->append_arg(SageBuilder::buildInitializedName(name,  type, NULL));
  }
  for (it_data = p_arguments.privates.begin(); it_data != p_arguments.privates.end(); it_data++) {
    std::string name((*it_data)->getVariableSymbol()->get_name().getString());
    SgType * type = SageBuilder::buildPointerType((*it_data)->getBaseType());
#if defined(TILEK_TARGET_OPENCL)
    SgModifierType * mod_type = SageBuilder::buildModifierType(type);
    mod_type->get_typeModifier().setOpenclLocal();
    type = mod_type;
#elif defined(TILEK_TARGET_CUDA)
    assert(false); // TODO
#endif
    param_list->append_arg(SageBuilder::buildInitializedName(name,  type, NULL));
  }
  param_list->append_arg(klt_loop_context);
#else
  param_list = SageBuilder::buildFunctionParameterList(
#if defined(TILEK_THREADS)
    SageBuilder::buildInitializedName("tid",  SageBuilder::buildIntType(), NULL),
#endif
    SageBuilder::buildInitializedName("param",  SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL),
    SageBuilder::buildInitializedName("scalar", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL),
    SageBuilder::buildInitializedName("data",   SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL),
    SageBuilder::buildInitializedName("priv",   SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL),
    klt_loop_context
  );
#endif
  return param_list;
}

template <>
void Kernel<Annotation, Runtime>::setRuntimeSpecificKernelField(SgVariableSymbol * kernel_sym, SgBasicBlock * bb) const {
#if defined(TILEK_THREADS)
  std::vector<Annotation>::const_iterator it;
  ::DLX::TileK::language_t::num_threads_clause_t * num_threads_clause = NULL;
  for (it = p_loop_tree.annotations.begin(); it != p_loop_tree.annotations.end(); it++) {
    num_threads_clause = ::DLX::TileK::language_t::isNumThreadsClause(it->clause);
    if (num_threads_clause != NULL) break;
  }
  assert(num_threads_clause != NULL);

  SageInterface::appendStatement(SageBuilder::buildExprStatement(
    SageBuilder::buildAssignOp(
      MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), Runtime::host_api.user->kernel_num_threads_field, NULL, NULL),
      SageBuilder::buildIntVal(num_threads_clause->parameters.num_threads)
    )
  ), bb);
#elif defined(TILEK_ACCELERATOR)
  std::vector<Annotation>::const_iterator it;
  ::DLX::TileK::language_t::num_gangs_clause_t * num_gangs_clauses[3] = {NULL,NULL,NULL};
  ::DLX::TileK::language_t::num_workers_clause_t * num_workers_clauses[3] = {NULL,NULL,NULL};
  for (it = p_loop_tree.annotations.begin(); it != p_loop_tree.annotations.end(); it++) {
    ::DLX::TileK::language_t::num_gangs_clause_t * num_gangs_clause = ::DLX::TileK::language_t::isNumGangsClause(it->clause);
    if (num_gangs_clause != NULL)
      num_gangs_clauses[::DLX::TileK::language_t::getGangID(num_gangs_clause)] = num_gangs_clause;
    ::DLX::TileK::language_t::num_workers_clause_t * num_workers_clause = ::DLX::TileK::language_t::isNumWorkersClause(it->clause);
    if (num_workers_clause != NULL)
      num_workers_clauses[::DLX::TileK::language_t::getWorkerID(num_workers_clause)] = num_workers_clause;
  }

  int cnt;
  for (cnt = 0; cnt < 3; cnt++) {
    int num_gangs   = num_gangs_clauses[cnt]   != NULL ? num_gangs_clauses[cnt]->parameters.num_gangs     : 1;
    SageInterface::appendStatement(SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
      MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), Runtime::host_api.user->kernel_num_gangs_field, SageBuilder::buildIntVal(cnt), NULL),
      SageBuilder::buildIntVal(num_gangs)
    )), bb);

    int num_workers = num_workers_clauses[cnt] != NULL ? num_workers_clauses[cnt]->parameters.num_workers : 1;
    SageInterface::appendStatement(SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
      MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), Runtime::host_api.user->kernel_num_workers_field, SageBuilder::buildIntVal(cnt), NULL),
      SageBuilder::buildIntVal(num_workers)
    )), bb);
  }
#endif
}

} // namespace KLT

