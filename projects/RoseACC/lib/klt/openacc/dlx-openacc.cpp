
#include "KLT/Core/loop-trees.hpp"
#include "KLT/OpenACC/dlx-openacc.hpp"
#include "KLT/OpenACC/language-opencl.hpp"
#include "KLT/OpenACC/runtime-openacc.hpp"

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
      /// \todo 'dimension_id' and expression parameter
    case OpenACC::language_t::e_acc_clause_auto:
    case OpenACC::language_t::e_acc_clause_seq:
    case OpenACC::language_t::e_acc_clause_independent:
      // None of these clauses take any parameters
      break;
    default:
      assert(false);
  }
}

}

