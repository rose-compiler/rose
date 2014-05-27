/*!
 * 
 * \file DLX/OpenACC/language.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_OPENACC_LANGUAGE_HPP__
#define __DLX_OPENACC_LANGUAGE_HPP__

#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"

//#include "transformation-handler.hpp"

#include <vector>
#include <map>
#include <string>

#ifndef OPENACC_MULTIDEV
# define OPENACC_MULTIDEV 1
#endif
#ifdef OPENACC_DATA_ACCESS
# define OPENACC_DATA_ACCESS 1
#endif

class SgScopeStatement;
class SgForStatement;
class SgExpression;

namespace DLX {

namespace OpenACC {

/*!
 * \addtogroup grp_dlx_openacc_language
 *
*/
struct language_t {
  static std::string language_label;

  typedef std::vector<std::string> label_set_t;

  enum construct_kinds_e {
    e_acc_construct_data,      //!<
    e_acc_construct_parallel,  //!<
    e_acc_construct_kernel,    //!<
    e_acc_construct_loop,      //!<
    e_acc_construct_host_data, //!<
    e_acc_construct_declare,   //!<
    e_acc_construct_cache,     //!<
    e_acc_construct_update,    //!<
    e_acc_construct_blank,     //!<
    e_construct_last
  };
  typedef std::map<construct_kinds_e, std::string> construct_label_map_t;
  static construct_label_map_t s_construct_labels;
  static construct_kinds_e s_blank_construct;

  enum clause_kinds_e {
    e_acc_clause_if,                 //!< 
    e_acc_clause_async,              //!< 
    e_acc_clause_num_gangs,          //!< 
    e_acc_clause_num_workers,        //!< 
    e_acc_clause_vector_length,      //!< 
    e_acc_clause_reduction,          //!< 
    e_acc_clause_copy,               //!< 
    e_acc_clause_copyin,             //!< 
    e_acc_clause_copyout,            //!< 
    e_acc_clause_create,             //!< 
    e_acc_clause_present,            //!< 
    e_acc_clause_present_or_copy,    //!< 
    e_acc_clause_present_or_copyin,  //!< 
    e_acc_clause_present_or_copyout, //!< 
    e_acc_clause_present_or_create,  //!< 
    e_acc_clause_deviceptr,          //!< 
    e_acc_clause_private,            //!< 
    e_acc_clause_firstprivate,       //!< 
    e_acc_clause_use_device,         //!< 
    e_acc_clause_device_resident,    //!< 
    e_acc_clause_collapse,           //!< 
    e_acc_clause_auto,               //!< 
    e_acc_clause_gang,               //!< 
    e_acc_clause_worker,             //!< 
    e_acc_clause_vector,             //!< 
    e_acc_clause_seq,                //!< 
    e_acc_clause_independent,        //!< 
    e_acc_clause_host,               //!< 
    e_acc_clause_device,             //!<
#if OPENACC_MULTIDEV
    e_acc_clause_split,
    e_acc_clause_devices,
#endif
#if OPENACC_DATA_ACCESS
    e_acc_clause_read,
    e_acc_clause_write,
#endif
    e_clause_last
  };
  typedef std::map<clause_kinds_e, label_set_t> clause_labels_map_t;
  static clause_labels_map_t s_clause_labels;

  enum directives_relation_e {
    e_child_scope,
    e_parent_scope,
    e_relation_last
  };
  typedef std::map<directives_relation_e, std::string> directives_relation_label_map_t;
  static directives_relation_label_map_t s_directives_relation_labels;

  static void init();
};

}

namespace Directives {

/**
 * \addtogroup grp_dlx_openacc_directives
 * @{
 */

template <>
generic_construct_t<OpenACC::language_t> * buildConstruct<OpenACC::language_t>(OpenACC::language_t::construct_kinds_e kind);

template <>
generic_clause_t<OpenACC::language_t> * buildClause<OpenACC::language_t>(OpenACC::language_t::clause_kinds_e kind);
 
template <>
bool parseClauseParameters<OpenACC::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<OpenACC::language_t> * clause
);

template <>
template <>
struct generic_construct_t<OpenACC::language_t>::assoc_nodes_t<OpenACC::language_t::e_acc_construct_data> {
  SgScopeStatement * parent_scope;
  SgStatement * data_region;
};

template <>
template <>
struct generic_construct_t<OpenACC::language_t>::assoc_nodes_t<OpenACC::language_t::e_acc_construct_parallel> {
  SgScopeStatement * parent_scope;
  SgStatement * parallel_region;
};

template <>
template <>
struct generic_construct_t<OpenACC::language_t>::assoc_nodes_t<OpenACC::language_t::e_acc_construct_kernel> {
  SgScopeStatement * parent_scope;
  SgStatement * kernel_region;
};

template <>
template <>
struct generic_construct_t<OpenACC::language_t>::assoc_nodes_t<OpenACC::language_t::e_acc_construct_loop> {
  SgScopeStatement * parent_scope;
  SgForStatement   * for_loop;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_if> {
  SgExpression * condition; // FIXME is it to be known at compile time or runtime ?
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_async> {
  SgExpression * sync_tag; // FIXME is it to be known at compile time or runtime ?
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_num_gangs> {
  SgExpression * exp;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_num_workers> {
  SgExpression * exp;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_vector_length> {
  SgExpression * exp;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_reduction> {
  /// \todo
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_copy> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_copyin> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_copyout> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_create> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_present> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_present_or_copy> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_present_or_copyin> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_present_or_copyout> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_present_or_create> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_deviceptr> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_private> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_firstprivate> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_use_device> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_device_resident> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_collapse> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_auto> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_gang> {
  size_t dimension_id;
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_worker> {
  size_t dimension_id;
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_vector> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_seq> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_independent> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_host> {
  // empty
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_device> {
  // empty
};

#if OPENACC_MULTIDEV

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_split> {
  enum {
    e_acc_split_contiguous,
    e_acc_split_chunk,
    e_acc_split_unknown
  } kind;
  std::vector<SgExpression *> portions;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_devices> {
  std::vector<std::pair<SgExpression *, SgExpression *> > device_list;
};

#endif

#if OPENACC_DATA_ACCESS

struct access_pattern_t {};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_read> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<OpenACC::language_t>::parameters_t<OpenACC::language_t::e_acc_clause_write> {
  std::vector<Frontend::data_sections_t> data_sections;
};

#endif

 /** @} */
}

}

#endif /* __DLX_OPENACC_LANGUAGE_HPP__ */

