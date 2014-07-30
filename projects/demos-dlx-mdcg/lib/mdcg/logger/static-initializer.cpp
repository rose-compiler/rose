
#include "MFB/Sage/variable-declaration.hpp"

#include "MDCG/Logger/static-initializer.hpp"

#include "sage3basic.h"

namespace MDCG {

namespace Logger {

/**
 * 
 * struct logger_param_t {
 *   size_t size;
 * };
 *
 */
SgExpression * LoggerParamTraversal::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  MFB::Driver<MFB::Sage>::file_id_t file_id
) {
  switch (field_id) {
    case 0:
    {
      SgType * type = input->get_type();
      assert(type != NULL);
      return SageBuilder::buildSizeOfOp(type);
    }
    default:
      assert(false);
  }
}

size_t LoggerCondTraversal::cond_cnt = 0;

/**
 * 
 * struct logger_cond_t {
 *   size_t size;
 *   void * value;
 * };
 *
 */
SgExpression * LoggerCondTraversal::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  MFB::Driver<MFB::Sage>::file_id_t file_id
) {
  switch (field_id) {
    case 0:
    {
      SgType * type = input.second->get_type();
      assert(type != NULL);
      return SageBuilder::buildSizeOfOp(type);
    }
    case 1:
    {
      std::ostringstream decl_name;
        decl_name << "cond_" << cond_cnt++;

      SgInitializer * init = SageBuilder::buildAssignInitializer(SageInterface::copyExpression(input.second));

      MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name.str(), input.second->get_type(), init, NULL, file_id, false, true);

      MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = static_initializer.getDriver().build<SgVariableDeclaration>(var_decl_desc);

      return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(var_decl_res.symbol));
    }
    default:
      assert(false);
  }
}

size_t LoggerPointTraversal::params_cnt = 0;
size_t LoggerPointTraversal::conds_cnt = 0;

/**
 * 
 * struct logger_point_t {
 *   enum { e_before, e_after, e_both } where;
 *   char * message;
 *   size_t num_params;
 *   struct logger_param_t * params;
 *   size_t num_conds;
 *   struct logger_cond_t * conds;
 * };
 *
 */
SgExpression * LoggerPointTraversal::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  MFB::Driver<MFB::Sage>::file_id_t file_id
) {
  switch (field_id) {
    case 0:
    {
      /// enum { e_before, e_after, e_both } where;
      return SageBuilder::buildIntVal(input.where); /// \todo lookup the enumerators and instantiate the good one...
    }
    case 1:
    {
      /// char * message;
      return SageBuilder::buildStringVal(input.message);
    }
    case 2:
    {
      /// size_t num_params;
      return SageBuilder::buildIntVal(input.params.size());
    }
    case 3:
    {
      /// struct logger_param_t * params;
      std::ostringstream decl_name;
        decl_name << "params_" << params_cnt++;
      Model::class_t base_class = MDCG::StaticInitializer::getBaseClassForPointerOnClass(element, "params", "logger_param_t");
      return static_initializer.createArrayPointer<LoggerParamTraversal>(base_class, input.params.size(), input.params.begin(), input.params.end(), file_id, decl_name.str());
    }
    case 4:
    {
      /// size_t num_conds;
      return SageBuilder::buildIntVal(input.conds.size());
    }
    case 5:
    {
      /// struct logger_cond_t * conds;
      std::ostringstream decl_name;
        decl_name << "conds_" << conds_cnt++;
      Model::class_t base_class = MDCG::StaticInitializer::getBaseClassForPointerOnClass(element, "conds", "logger_cond_t");
      LoggerCondTraversal::cond_cnt = 0;
      return static_initializer.createArrayPointer<LoggerCondTraversal>(base_class, input.conds.size(), input.conds.begin(), input.conds.end(), file_id, decl_name.str());
    }
    default:
      assert(false);
  }
}

size_t LoggerDataTraversal::points_cnt = 0;

/**
 * 
 * struct logger_data_t {
 *   size_t num_points;
 *   struct logger_point_t * points;
 * };
 *
 */
SgExpression * LoggerDataTraversal::createFieldInitializer(
  const MDCG::StaticInitializer & static_initializer,
  MDCG::Model::field_t element,
  unsigned field_id,
  const input_t & input,
  MFB::Driver<MFB::Sage>::file_id_t file_id
) {
  switch (field_id) {
    case 0:
    {
      /// size_t num_points;
      return SageBuilder::buildIntVal(input.size());
    }
    case 1:
    {
      /// struct logger_point_t * points;
      std::ostringstream decl_name;
        decl_name << "points_" << points_cnt++;
      Model::class_t base_class = MDCG::StaticInitializer::getBaseClassForPointerOnClass(element, "points", "logger_point_t");
      LoggerPointTraversal::params_cnt = 0;
      LoggerPointTraversal::conds_cnt = 0;
      return static_initializer.createArrayPointer<LoggerPointTraversal>(base_class, input.size(), input.begin(), input.end(), file_id, decl_name.str());
    }
    default:
      assert(false);
  }
}

}

}

