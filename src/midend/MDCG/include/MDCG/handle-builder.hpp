
#ifndef __MDCG_HANDLE_BUILDER_HPP__
#define __MDCG_HANDLE_BUILDER_HPP__

#include "MDCG/model.hpp"

#include "Handles/hooks.hpp"

#include <vector>
#include <utility>
#include <string>

namespace MDCG {

struct build_model_t {
  enum opcode_e {
    e_opcode_last
  };
  struct fields_t {
    std::vector<std::pair<std::string, bool> > files; // boolean true if system header
  };
  struct return_fields_t {
    Model::model_id_t model_id;
  };
};

struct get_model_t {
  enum opcode_e {
    e_opcode_last
  };
  struct fields_t {
    Model::model_id_t model_id;
  };
  struct return_fields_t {
    Model::model_t model;
  };
};

class ModelBuilder;

}

namespace Handles {

template <>
const unsigned long handle_hook_t<MDCG::ModelBuilder, MDCG::build_model_t>::submit(handle_t *);

template <>
handle_hook_t<MDCG::ModelBuilder, MDCG::build_model_t>::return_handle_t * handle_hook_t<MDCG::ModelBuilder, MDCG::build_model_t>::get_result(const unsigned long);

template <>
const unsigned long handle_hook_t<MDCG::ModelBuilder, MDCG::get_model_t>::submit(handle_t *);

template <>
handle_hook_t<MDCG::ModelBuilder, MDCG::get_model_t>::return_handle_t * handle_hook_t<MDCG::ModelBuilder, MDCG::get_model_t>::get_result(const unsigned long);

}

#endif /* __MDCG_HANDLE_BUILDER_HPP__ */

