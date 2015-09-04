
#include "DLX/Core/compiler.hpp"
#include "DLX/Logger/compiler.hpp"
#include "DLX/Logger/language.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/static-initializer.hpp"
#include "MDCG/model.hpp"

#include "MDCG/Logger/static-initializer.hpp"

#include <cassert>

namespace DLX {

namespace Logger {

compiler_modules_t::compiler_modules_t(SgProject * project, std::string logger_data_file_name, std::string liblogger_inc_dir) :
  driver(project),
  model_builder(driver),
  static_initializer(driver),
  logger_data_file(0),
  api()
{
  logger_data_file = driver.create(boost::filesystem::path(logger_data_file_name));
    driver.setUnparsedFile(logger_data_file);
    driver.setCompiledFile(logger_data_file);

  loadAPI(liblogger_inc_dir);
}

void compiler_modules_t::loadAPI(std::string & liblogger_inc_dir) {
  MDCG::ModelBuilder::model_id_t logger_model = model_builder.create();
    model_builder.add(logger_model, "liblogger", liblogger_inc_dir + "/libLogger", "h");

  const MDCG::Model::model_t & model = model_builder.get(logger_model);

  MDCG::Model::function_t func;

  func = model.lookup<MDCG::Model::function_t>("logger_log_before");
  api.logger_log_before = func->node->symbol;
  assert(api.logger_log_before != NULL);

  func = model.lookup<MDCG::Model::function_t>("logger_log_after");
  api.logger_log_after = func->node->symbol;
  assert(api.logger_log_after != NULL);

  api.logger_data_class = model.lookup<MDCG::Model::class_t>("logger_data_t");
  assert(api.logger_data_class != NULL);
}

}

namespace Compiler {

template <>
bool Compiler<DLX::Logger::language_t, DLX::Logger::compiler_modules_t>::compile(
  const Compiler<DLX::Logger::language_t, DLX::Logger::compiler_modules_t>::directives_ptr_set_t & directives,
  const Compiler<DLX::Logger::language_t, DLX::Logger::compiler_modules_t>::directives_ptr_set_t & graph_entry,
  const Compiler<DLX::Logger::language_t, DLX::Logger::compiler_modules_t>::directives_ptr_set_t & graph_final
) {
  MDCG::Logger::LoggerDataTraversal::input_t logger_data;

  size_t point_cnt = 0;
  Compiler<DLX::Logger::language_t, DLX::Logger::compiler_modules_t>::directives_ptr_set_t::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    if ((*it_directive)->construct->kind == DLX::Logger::language_t::e_logger_construct_log) {
      logger_data.push_back(MDCG::Logger::LoggerPointTraversal::input_t());
      MDCG::Logger::LoggerPointTraversal::input_t & point = logger_data.back();

      Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_where> * where_clause = NULL;
      Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_message> * message_clause = NULL;
      Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_params> * params_clause = NULL;
      std::vector<Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_conds> *> conds_clauses;

      Directives::directive_t<DLX::Logger::language_t>::clause_list_t::const_iterator it_clause;
      for (it_clause = (*it_directive)->clause_list.begin(); it_clause != (*it_directive)->clause_list.end(); it_clause++) {
        Directives::generic_clause_t<DLX::Logger::language_t> * generic_clause = *it_clause;
        switch (generic_clause->kind) {
          case DLX::Logger::language_t::e_logger_clause_where:
          {
            assert(where_clause == NULL);
            where_clause = (Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_where> *)generic_clause;
            break;
          }
          case DLX::Logger::language_t::e_logger_clause_message:
          {
            assert(message_clause == NULL);
            message_clause = (Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_message> *)generic_clause;
            break;
          }
          case DLX::Logger::language_t::e_logger_clause_params:
          {
            assert(params_clause == NULL);
            params_clause = (Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_params> *)generic_clause;
            break;
          }
          case DLX::Logger::language_t::e_logger_clause_conds:
          {
            conds_clauses.push_back((Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_conds> *)generic_clause);
            break;
          }
          default:
            assert(false);
        }
      }
      assert(where_clause != NULL);
      point.where = where_clause->parameters.position;

      assert(message_clause != NULL);
      point.message = message_clause->parameters.message;

      SgExprListExp * expr_list = SageBuilder::buildExprListExp(SageBuilder::buildIntVal(point_cnt));

      if (params_clause != NULL) {
        point.params.insert(point.params.end(), params_clause->parameters.params.begin(), params_clause->parameters.params.end());
        std::vector<SgExpression *>::const_iterator it_param;
        for (it_param = params_clause->parameters.params.begin(); it_param != params_clause->parameters.params.end(); it_param++)
          expr_list->append_expression(SageInterface::copyExpression(*it_param));
      }

      std::vector<Directives::clause_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_clause_conds> *>::const_iterator it_conds_clause;
      for (it_conds_clause = conds_clauses.begin(); it_conds_clause != conds_clauses.end(); it_conds_clause++) {
        point.conds.push_back(std::pair<SgExpression *, SgValueExp *>(
          (*it_conds_clause)->parameters.expr,
          (*it_conds_clause)->parameters.value)
        );
        expr_list->append_expression(SageInterface::copyExpression((*it_conds_clause)->parameters.expr));
      }

      Directives::construct_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_construct_log> * construct =
        (Directives::construct_t<DLX::Logger::language_t, DLX::Logger::language_t::e_logger_construct_log> *)(*it_directive)->construct;

      SageInterface::insertStatementBefore(
        construct->assoc_nodes.logged_region,
        SageBuilder::buildFunctionCallStmt(SageBuilder::buildFunctionRefExp(compiler_modules.api.logger_log_before), expr_list)
      );
      SageInterface::insertStatementAfter(
        construct->assoc_nodes.logged_region,
        SageBuilder::buildFunctionCallStmt(SageBuilder::buildFunctionRefExp(compiler_modules.api.logger_log_after), isSgExprListExp(SageInterface::copyExpression(expr_list)))
      );
      SageInterface::removeStatement(construct->assoc_nodes.pragma_decl);

      point_cnt++;
    }
    else assert(false);
  }

  MDCG::Logger::LoggerDataTraversal::points_cnt = 0;
  compiler_modules.static_initializer.addDeclaration<MDCG::Logger::LoggerDataTraversal>(
                                   compiler_modules.api.logger_data_class,
                                   logger_data,
                                   compiler_modules.logger_data_file,
                                   "logger_data"
                                 );

  return true;
}

}

}

