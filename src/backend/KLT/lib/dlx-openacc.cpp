
#include "KLT/dlx-openacc.hpp"
#include "KLT/loop-trees.hpp"
#include "KLT/generator.hpp"
#include "KLT/kernel.hpp"
#include "KLT/data.hpp"
#include "KLT/iteration-mapper.hpp"
#include "KLT/language-opencl.hpp"
#include "KLT/runtime-openacc.hpp"
#include "KLT/mfb-klt.hpp"
#include "KLT/mfb-acc-ocl.hpp"
#include "KLT/utils.hpp"

#include "sage3basic.h"

//! This helper function return non-null value iff the expression found is statically defined integer.
SgExpression * parseExpressionOrLabel() {
  SgExpression * exp = NULL;
  if (AstFromString::afs_match_additive_expression()) {
    exp = isSgExpression(AstFromString::c_parsed_node);
    assert(exp != NULL);
    /// \todo Is it a statically defined integer?
  }
  else if (AstFromString::afs_match_identifier()) {
    SgName * label = dynamic_cast<SgName *>(AstFromString::c_parsed_node);
    assert(label != NULL);
    // We don't save in this case as it implies a dynamically determined value
  }
  else assert(false);

  return exp;
}

namespace DLX {

template <>
void KLT_Annotation<OpenACC::language_t>::parseRegion(std::vector<DLX::KLT_Annotation<OpenACC::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<OpenACC::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case OpenACC::language_t::e_acc_clause_if:
      assert(false); /// \todo
      break;
    case OpenACC::language_t::e_acc_clause_async:
      assert(false); /// \todo
      break;
    case OpenACC::language_t::e_acc_clause_num_gangs:
      KLT::ensure('(');
      ((Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs> *)annotation.clause)->parameters.exp
                = parseExpressionOrLabel();
      KLT::ensure(')');
      break;
    case OpenACC::language_t::e_acc_clause_num_workers:
      KLT::ensure('(');
      ((Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers> *)annotation.clause)->parameters.exp
                = parseExpressionOrLabel();
      KLT::ensure(')');
      break;
    case OpenACC::language_t::e_acc_clause_vector_length:
      KLT::ensure('(');
      ((Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> *)annotation.clause)->parameters.exp
                = parseExpressionOrLabel();
      KLT::ensure(')');
      break;
    case OpenACC::language_t::e_acc_clause_reduction:
      assert(false); /// \todo
      break;
    default:
      assert(false);
  }
}

template <>
void KLT_Annotation<OpenACC::language_t>::parseData(std::vector<DLX::KLT_Annotation<OpenACC::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<OpenACC::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case OpenACC::language_t::e_acc_clause_copy:
    case OpenACC::language_t::e_acc_clause_copyin:
    case OpenACC::language_t::e_acc_clause_copyout:
    case OpenACC::language_t::e_acc_clause_create:
    case OpenACC::language_t::e_acc_clause_present:
    case OpenACC::language_t::e_acc_clause_present_or_copy:
    case OpenACC::language_t::e_acc_clause_present_or_copyin:
    case OpenACC::language_t::e_acc_clause_present_or_copyout:
    case OpenACC::language_t::e_acc_clause_present_or_create:
      // Nothing to do as in the LoopTree format, clauses are applied to each data (in directive format the clause encompass multiple data)
      break;
    default:
      assert(false);
  }
}

template <>
void KLT_Annotation<OpenACC::language_t>::parseLoop(std::vector<DLX::KLT_Annotation<OpenACC::language_t> > & container) {
  parseClause(container);

  DLX::KLT_Annotation<OpenACC::language_t> & annotation = container.back();

  switch (annotation.clause->kind) {
    case OpenACC::language_t::e_acc_clause_gang:
    case OpenACC::language_t::e_acc_clause_worker:
    case OpenACC::language_t::e_acc_clause_vector:
    case OpenACC::language_t::e_acc_clause_seq:
    case OpenACC::language_t::e_acc_clause_independent:
      // None of these clauses take any parameters
      break;
    default:
      assert(false);
  }
}

}

namespace KLT {

template <>
bool LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t::isParallel() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_seq) return false;
  }
  return true;
}

template <>
unsigned long Generator<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC,
  MFB::KLT_Driver
>::createFile() {
  return p_sage_driver.createStandaloneSourceFile(p_file_name, "cl");
}

template <>
unsigned long Kernel<DLX::KLT_Annotation<DLX::OpenACC::language_t>, Language::OpenCL, Runtime::OpenACC>::id_cnt = 0;

template <>
bool Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::isFlowIn() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (   it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copy
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copyin
    ) return true;
  }
  return false;
}

template <>
bool Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::isFlowOut() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (   it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copy
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_copyout
    ) return true;
  }
  return false;
}

template <>
void IterationMapper<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  KLT::Language::OpenCL,
  KLT::Runtime::OpenACC
>::generateShapes(
  Kernel_OpenCL_OpenACC::loop_mapping_t * loop_mapping,
  std::set<ItMap_OpenCL_OpenACC *> & shapes
) const {
  assert(false);
}

}

namespace MFB {

template <>
KLT<Kernel_OpenCL_OpenACC>::build_result_t Driver<KLT>::build<Kernel_OpenCL_OpenACC>(
    KLT<Kernel_OpenCL_OpenACC>::object_desc_t const & object
) {
  assert(false); /// \todo
}

KLT<Kernel_OpenCL_OpenACC>::object_desc_t::object_desc_t(
      Kernel_OpenCL_OpenACC * kernel_,
      Kernel_OpenCL_OpenACC::loop_mapping_t * loop_mapping_,
      ItMap_OpenCL_OpenACC * iteration_map_,
      unsigned long file_id_
) :
  kernel(kernel_),
  loop_mapping(loop_mapping_),
  iteration_map(iteration_map_),
  file_id(file_id_)
{}

}


