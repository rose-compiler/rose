/*!
 * 
 * \file lib/openacc/clauses.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "DLX/Core/clauses.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/OpenACC/language.hpp"

#include <cassert>

#ifndef OPENACC_MULTIDEV
# define OPENACC_MULTIDEV 1
#endif
#ifdef OPENACC_DATA_ACCESS
# define OPENACC_DATA_ACCESS 1
#endif

class SgLocatedNode;

namespace DLX {

namespace Directives {

/*!
 * \addtogroup grp_dlx_openacc_directives
 * @{
 */

template <>
generic_clause_t<OpenACC::language_t> * buildClause<OpenACC::language_t>(OpenACC::language_t::clause_kinds_e kind) {
  switch (kind) {
    case OpenACC::language_t::e_acc_clause_if:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_if>();
    case OpenACC::language_t::e_acc_clause_async:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_async>();
    case OpenACC::language_t::e_acc_clause_num_gangs:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs>();
    case OpenACC::language_t::e_acc_clause_num_workers:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers>();
    case OpenACC::language_t::e_acc_clause_vector_length:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length>();
    case OpenACC::language_t::e_acc_clause_reduction:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_reduction>();
    case OpenACC::language_t::e_acc_clause_copy:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copy>();
    case OpenACC::language_t::e_acc_clause_copyin:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyin>();
    case OpenACC::language_t::e_acc_clause_copyout:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyout>();
    case OpenACC::language_t::e_acc_clause_create:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_create>();
    case OpenACC::language_t::e_acc_clause_present:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present>();
    case OpenACC::language_t::e_acc_clause_present_or_copy:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copy>();
    case OpenACC::language_t::e_acc_clause_present_or_copyin:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyin>();
    case OpenACC::language_t::e_acc_clause_present_or_copyout:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyout>();
    case OpenACC::language_t::e_acc_clause_present_or_create:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_create>();
    case OpenACC::language_t::e_acc_clause_deviceptr:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_deviceptr>();
    case OpenACC::language_t::e_acc_clause_private:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_private>();
    case OpenACC::language_t::e_acc_clause_firstprivate:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_firstprivate>();
    case OpenACC::language_t::e_acc_clause_use_device:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device>();
    case OpenACC::language_t::e_acc_clause_device_resident:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device_resident>();
    case OpenACC::language_t::e_acc_clause_collapse:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_collapse>();
    case OpenACC::language_t::e_acc_clause_auto:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_auto>();
    case OpenACC::language_t::e_acc_clause_gang:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_gang>();
    case OpenACC::language_t::e_acc_clause_worker:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_worker>();
    case OpenACC::language_t::e_acc_clause_vector:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector>();
    case OpenACC::language_t::e_acc_clause_seq:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_seq>();
    case OpenACC::language_t::e_acc_clause_independent:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_independent>();
    case OpenACC::language_t::e_acc_clause_host:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_host>();
    case OpenACC::language_t::e_acc_clause_device:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device>();
#if OPENACC_MULTIDEV
    case OpenACC::language_t::e_acc_clause_split:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_split>();
    case OpenACC::language_t::e_acc_clause_devices:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_devices>();
#endif
#if OPENACC_DATA_ACCESS
    case OpenACC::language_t::e_acc_clause_read:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_read>();
    case OpenACC::language_t::e_acc_clause_write:
      return new clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_write>();
#endif
    case OpenACC::language_t::e_clause_last:
      assert(false);
    default:
     assert(false);
  }
}
 
template <>
bool parseClauseParameters<OpenACC::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<OpenACC::language_t> * clause
) {
  switch (clause->kind) {
    case OpenACC::language_t::e_acc_clause_if:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_if>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_if> *)clause
      );
    case OpenACC::language_t::e_acc_clause_async:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_async>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_async> *)clause
      );
    case OpenACC::language_t::e_acc_clause_num_gangs:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_num_gangs>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs> *)clause
      );
    case OpenACC::language_t::e_acc_clause_num_workers:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_num_workers>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers> *)clause
      );
    case OpenACC::language_t::e_acc_clause_vector_length:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_vector_length>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> *)clause
      );
    case OpenACC::language_t::e_acc_clause_reduction:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_reduction>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_reduction> *)clause
      );
    case OpenACC::language_t::e_acc_clause_copy:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copy>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copy> *)clause
      );
    case OpenACC::language_t::e_acc_clause_copyin:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copyin>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyin> *)clause
      );
    case OpenACC::language_t::e_acc_clause_copyout:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_copyout>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyout> *)clause
      );
    case OpenACC::language_t::e_acc_clause_create:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_create>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_create> *)clause
      );
    case OpenACC::language_t::e_acc_clause_present:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present> *)clause
      );
    case OpenACC::language_t::e_acc_clause_present_or_copy:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_copy>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copy> *)clause
      );
    case OpenACC::language_t::e_acc_clause_present_or_copyin:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_copyin>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyin> *)clause
      );
    case OpenACC::language_t::e_acc_clause_present_or_copyout:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_copyout>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyout> *)clause
      );
    case OpenACC::language_t::e_acc_clause_present_or_create:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_present_or_create>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_create> *)clause
      );
    case OpenACC::language_t::e_acc_clause_deviceptr:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_deviceptr>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_deviceptr> *)clause
      );
    case OpenACC::language_t::e_acc_clause_private:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_private>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_private> *)clause
      );
    case OpenACC::language_t::e_acc_clause_firstprivate:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_firstprivate>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_firstprivate> *)clause
      );
    case OpenACC::language_t::e_acc_clause_use_device:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_device>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device> *)clause
      );
    case OpenACC::language_t::e_acc_clause_device_resident:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_device_resident>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device_resident> *)clause
      );
    case OpenACC::language_t::e_acc_clause_collapse:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_collapse>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_collapse> *)clause
      );
    case OpenACC::language_t::e_acc_clause_auto:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_auto>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_auto> *)clause
      );
    case OpenACC::language_t::e_acc_clause_gang:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_gang>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_gang> *)clause
      );
    case OpenACC::language_t::e_acc_clause_worker:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_worker>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_worker> *)clause
      );
    case OpenACC::language_t::e_acc_clause_vector:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_vector>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector> *)clause
      );
    case OpenACC::language_t::e_acc_clause_seq:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_seq>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_seq> *)clause
      );
    case OpenACC::language_t::e_acc_clause_independent:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_independent>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_independent> *)clause
      );
    case OpenACC::language_t::e_acc_clause_host:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_host>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_host> *)clause
      );
    case OpenACC::language_t::e_acc_clause_device:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_device>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_device> *)clause
      );
#if OPENACC_MULTIDEV
    case OpenACC::language_t::e_acc_clause_split:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_split>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_split> *)clause
      );
    case OpenACC::language_t::e_acc_clause_devices:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_devices>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_devices> *)clause
      );
#endif
#if OPENACC_DATA_ACCESS
    case OpenACC::language_t::e_acc_clause_read:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_read>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_read> *)clause
      );
    case OpenACC::language_t::e_acc_clause_write:
      return Frontend::Frontend<OpenACC::language_t>::parseClauseParameters<OpenACC::language_t::e_acc_clause_write>(
        directive_str, directive_node, (clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_write> *)clause
      );
#endif
    case OpenACC::language_t::e_clause_last:
      assert(false);
    default:
      assert(false);
  }
}

/** @} */

}

}

