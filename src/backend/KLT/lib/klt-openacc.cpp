
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

#include "MFB/Sage/function-declaration.hpp"

#include "sage3basic.h"

namespace KLT {

template <>
bool LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t::isDistributed() const {
  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = annotations.begin(); it != annotations.end(); it++) {
    if (   it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_gang
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_worker
        || it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_vector
    ) return true;
  }
  return false;
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



}

