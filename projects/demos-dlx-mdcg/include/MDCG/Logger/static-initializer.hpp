
#ifndef __LOGGER_STATIC_INITIALIZER_HPP__
#define __LOGGER_STATIC_INITIALIZER_HPP__

#include "DLX/Logger/language.hpp"

#include "MDCG/static-initializer.hpp"
#include "MDCG/model-field.hpp"

#include <vector>

class SgExpression;

namespace MDCG {

namespace Logger {

struct LoggerParamTraversal {
  typedef SgExpression * input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    MFB::Driver<MFB::Sage>::file_id_t file_id
  );
};

struct LoggerCondTraversal {
  static size_t cond_cnt;

  typedef std::pair<SgExpression *, SgValueExp *> input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    MFB::Driver<MFB::Sage>::file_id_t file_id
  );
};

struct LoggerPointTraversal {
  static size_t params_cnt;
  static size_t conds_cnt;

  struct input_t {
    DLX::Directives::generic_clause_t<DLX::Logger::language_t>::parameters_t<DLX::Logger::language_t::e_logger_clause_where>::log_position_e where;
    std::string message;
    std::vector<LoggerParamTraversal::input_t> params;
    std::vector<LoggerCondTraversal::input_t> conds;
  };

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    MFB::Driver<MFB::Sage>::file_id_t file_id
  );
};

struct LoggerDataTraversal {
  static size_t points_cnt;

  typedef std::vector<LoggerPointTraversal::input_t> input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::StaticInitializer & static_initializer,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    MFB::Driver<MFB::Sage>::file_id_t file_id
  );
};

}

}

#endif /* __LOGGER_STATIC_INITIALIZER_HPP__ */

