
#include "sage3basic.h"

#include "DLX/Core/directives.hpp"
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/TileK/language.hpp"

#include <cassert>

namespace DLX {

namespace TileK {

std::string language_t::language_label;
language_t::construct_label_map_t language_t::s_construct_labels;
language_t::construct_kinds_e language_t::s_blank_construct;
language_t::clause_labels_map_t language_t::s_clause_labels;
language_t::directives_relation_label_map_t language_t::s_directives_relation_labels;

void language_t::init() {
  language_label = "tilek";

  Directives::setConstructLabel<language_t>(e_construct_kernel, "kernel");
  Directives::setConstructLabel<language_t>(e_construct_loop, "loop");

  s_blank_construct = e_construct_last;

  Directives::addClauseLabel<language_t>(e_clause_data, "data");
  Directives::addClauseLabel<language_t>(e_clause_tile, "tile");
#ifdef TILEK_THREADS
  Directives::addClauseLabel<language_t>(e_clause_num_threads, "num_threads");
#endif
#ifdef TILEK_ACCELERATOR
  Directives::addClauseLabel<language_t>(e_clause_num_gangs, "num_gangs");
  Directives::addClauseLabel<language_t>(e_clause_num_workers, "num_workers");
#endif

  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_child_scope, "child-scope"));
  s_directives_relation_labels.insert(std::pair<directives_relation_e, std::string>(e_parent_scope, "parent-scope"));
}

language_t::kernel_construct_t * language_t::isKernelConstruct(construct_t * construct) {
  return construct->kind == language_t::e_construct_kernel ? (language_t::kernel_construct_t *)construct : NULL;
}

SgStatement * language_t::getKernelRegion(kernel_construct_t * kernel_construct) {
  return kernel_construct->assoc_nodes.kernel_region;
}

language_t::loop_construct_t * language_t::isLoopConstruct(construct_t * construct) {
  return construct->kind == language_t::e_construct_loop ? (language_t::loop_construct_t *)construct : NULL;
}

SgForStatement * language_t::getLoopStatement(loop_construct_t * loop_construct) {
  return loop_construct->assoc_nodes.for_loop;
}

language_t::data_clause_t * language_t::isDataClause(clause_t * clause) {
  return clause->kind == language_t::e_clause_data ? (language_t::data_clause_t *)clause : NULL;
}

language_t::tile_clause_t * language_t::isTileClause(clause_t * clause) {
  return clause->kind == language_t::e_clause_tile ? (language_t::tile_clause_t *)clause : NULL;
}

const std::vector<DLX::Frontend::data_sections_t> & language_t::getDataSections(data_clause_t * data_clause) {
  return data_clause->parameters.data_sections;
}

#ifdef TILEK_THREADS
language_t::num_threads_clause_t * language_t::isNumThreadsClause(clause_t * clause) {
  return clause->kind == language_t::e_clause_num_threads ? (language_t::num_threads_clause_t *)clause : NULL;
}
#endif

#ifdef TILEK_ACCELERATOR
language_t::num_gangs_clause_t * language_t::isNumGangsClause(clause_t * clause) {
  return clause->kind == language_t::e_clause_num_gangs ? (language_t::num_gangs_clause_t *)clause : NULL;
}
size_t language_t::getGangID(num_gangs_clause_t * num_gangs_clause) {
  return num_gangs_clause->parameters.gang_id;
}

language_t::num_workers_clause_t * language_t::isNumWorkersClause(clause_t * clause) {
  return clause->kind == language_t::e_clause_num_workers ? (language_t::num_workers_clause_t *)clause : NULL;
}
size_t language_t::getWorkerID(num_workers_clause_t * num_workers_clause) {
  return num_workers_clause->parameters.worker_id;
}
#endif

}

}

