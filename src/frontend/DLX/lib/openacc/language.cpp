/*!
 * 
 * \file lib/openacc/language.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "DLX/Core/directives.hpp"
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/OpenACC/language.hpp"

#include <cassert>

#ifndef OPENACC_MULTIDEV
# define OPENACC_MULTIDEV 1
#endif
#ifdef OPENACC_DATA_ACCESS
# define OPENACC_DATA_ACCESS 1
#endif

namespace DLX {

namespace OpenACC {

/*!
 * \addtogroup grp_dlx_openacc_language
 * @{
 */

std::string language_t::language_label;
language_t::construct_label_map_t language_t::s_construct_labels;
language_t::construct_kinds_e language_t::s_blank_construct;
language_t::clause_labels_map_t language_t::s_clause_labels;
language_t::directives_relation_label_map_t language_t::s_directives_relation_labels;

void language_t::init() {
  // Language label

  language_label = "acc";

  // Language Constructs

  Directives::setConstructLabel<language_t>(e_acc_construct_data, "data");
  Directives::setConstructLabel<language_t>(e_acc_construct_parallel, "parallel");
  Directives::setConstructLabel<language_t>(e_acc_construct_kernel, "kernel");
  Directives::setConstructLabel<language_t>(e_acc_construct_loop, "loop");
  Directives::setConstructLabel<language_t>(e_acc_construct_host_data, "host_data");
  Directives::setConstructLabel<language_t>(e_acc_construct_declare, "declare");
  Directives::setConstructLabel<language_t>(e_acc_construct_cache, "cache");
  Directives::setConstructLabel<language_t>(e_acc_construct_update, "update");

  s_blank_construct = e_acc_construct_blank;

  // Language Clauses

  Directives::addClauseLabel<language_t>(e_acc_clause_if, "if");

  Directives::addClauseLabel<language_t>(e_acc_clause_async, "async");

  Directives::addClauseLabel<language_t>(e_acc_clause_num_gangs, "num_gangs");

  Directives::addClauseLabel<language_t>(e_acc_clause_num_workers, "num_workers");

  Directives::addClauseLabel<language_t>(e_acc_clause_vector_length, "vector_length");

  Directives::addClauseLabel<language_t>(e_acc_clause_reduction, "reduction");

  Directives::addClauseLabel<language_t>(e_acc_clause_copy, "copy");

  Directives::addClauseLabel<language_t>(e_acc_clause_copyin, "copyin");

  Directives::addClauseLabel<language_t>(e_acc_clause_copyout, "copyout");

  Directives::addClauseLabel<language_t>(e_acc_clause_create, "create");

  Directives::addClauseLabel<language_t>(e_acc_clause_present, "present");

  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_copy, "present_or_copy");
  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_copy, "pcopy");

  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_copyin, "present_or_copyin");
  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_copyin, "pcopyin");

  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_copyout, "present_or_copyout");
  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_copyout, "pcopyout");

  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_create, "present_or_create");
  Directives::addClauseLabel<language_t>(e_acc_clause_present_or_create, "pcreate");

  Directives::addClauseLabel<language_t>(e_acc_clause_deviceptr, "deviceptr");

  Directives::addClauseLabel<language_t>(e_acc_clause_private, "private");

  Directives::addClauseLabel<language_t>(e_acc_clause_firstprivate, "firstprivate");

  Directives::addClauseLabel<language_t>(e_acc_clause_use_device, "use_device");

  Directives::addClauseLabel<language_t>(e_acc_clause_device_resident, "device_resident");

  Directives::addClauseLabel<language_t>(e_acc_clause_collapse, "collapse");

  Directives::addClauseLabel<language_t>(e_acc_clause_auto, "auto");

  Directives::addClauseLabel<language_t>(e_acc_clause_gang, "gang");

  Directives::addClauseLabel<language_t>(e_acc_clause_worker, "worker");

  Directives::addClauseLabel<language_t>(e_acc_clause_vector, "vector");

  Directives::addClauseLabel<language_t>(e_acc_clause_seq, "seq");

  Directives::addClauseLabel<language_t>(e_acc_clause_independent, "independent");

  Directives::addClauseLabel<language_t>(e_acc_clause_host, "host");

  Directives::addClauseLabel<language_t>(e_acc_clause_device, "device");

#if OPENACC_MULTIDEV
  Directives::addClauseLabel<language_t>(e_acc_clause_split, "split");
  Directives::addClauseLabel<language_t>(e_acc_clause_devices, "devices");
#endif
#if OPENACC_DATA_ACCESS
  Directives::addClauseLabel<language_t>(e_acc_clause_read, "read");
  Directives::addClauseLabel<language_t>(e_acc_clause_write, "write");
#endif

  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_child_scope, "child-scope"));
  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_parent_scope, "parent-scope"));

}

/** @} */

}

}

