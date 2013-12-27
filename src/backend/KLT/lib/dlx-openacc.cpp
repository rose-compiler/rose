
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
    case OpenACC::language_t::e_acc_clause_seq:
    case OpenACC::language_t::e_acc_clause_independent:
      // None of these clauses take any parameters
      break;
    default:
      assert(false);
  }
}

}

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
unsigned long Generator<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC,
  MFB::KLT_Driver
>::createFile() {
  return p_sage_driver.createStandaloneSourceFile(p_file_name, "cl");
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

template <>
Runtime::OpenACC::loop_shape_t * IterationMapper<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
>::createShape(
  LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop
) const {
  if (!loop->isDistributed()) return NULL;

  Runtime::OpenACC::loop_shape_t * shape = new Runtime::OpenACC::loop_shape_t();
  shape->gang   = false;
  shape->worker = false;
  shape->vector = false;

  std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::const_iterator it;
  for (it = loop->annotations.begin(); it != loop->annotations.end(); it++) {
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_gang  ) shape->gang   = true;
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_worker) shape->worker = true;
    if (it->clause->kind == DLX::OpenACC::language_t::e_acc_clause_vector) shape->vector = true;
  }
  return shape;
}

template <>
void IterationMapper<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
>::generateShapeConfigs(
  ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop,
  Runtime::OpenACC::loop_shape_t * shape,
  std::vector<Runtime::OpenACC::shape_config_t *> & shape_configs
) const {
  /// \todo only generating the default (not usable) shape

  Runtime::OpenACC::shape_config_t * shape_config = new Runtime::OpenACC::shape_config_t();
    shape_config->tile_0 = 0;
    shape_config->tile_1 = 0;
    shape_config->tile_2 = 0;
    shape_config->tile_3 = 0;
  shape_configs.push_back(shape_config);
}

template <>
SgFunctionParameterList * createParameterList<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  Language::OpenCL,
  Runtime::OpenACC
>(
  Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>,
    Language::OpenCL,
    Runtime::OpenACC
  > * kernel
) {
  const std::list<SgVariableSymbol *> & params = kernel->getArguments().parameters;
  const std::list<SgVariableSymbol *> & scalars = kernel->getArguments().scalars;
  const std::list<Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *> & datas = kernel->getArguments().datas;
  unsigned long data_type_modifer_ = SgTypeModifier::e_ocl_global__;
  std::string suffix = "";

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list<Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *>::const_iterator it_data;

  SgTypeModifier::type_modifier_enum data_type_modifer = (SgTypeModifier::type_modifier_enum)data_type_modifer_;

  SgFunctionParameterList * result = SageBuilder::buildFunctionParameterList();

  // ******************

  for (it_var_sym = params.begin(); it_var_sym != params.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type =  param_sym->get_type();

    result->append_arg(SageBuilder::buildInitializedName("param_" + param_name + suffix, param_type, NULL));
  }

  // ******************

  for (it_var_sym = scalars.begin(); it_var_sym != scalars.end(); it_var_sym++) {
    SgVariableSymbol * scalar_sym = *it_var_sym;
    std::string scalar_name = scalar_sym->get_name().getString();
    SgType * scalar_type = scalar_sym->get_type();

    result->append_arg(SageBuilder::buildInitializedName("scalar_" + scalar_name + suffix, scalar_type, NULL));
  }

  // ******************

  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name().getString();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    switch (data_type_modifer) {
      case SgTypeModifier::e_default:
        break;
      case SgTypeModifier::e_ocl_global__:
      {
        SgModifierType * modif_type = SageBuilder::buildModifierType(field_type);
        modif_type->get_typeModifier().setOpenclGlobal();
        field_type = modif_type;
        break;
      }
      default:
        assert(false);
    }

    result->append_arg(SageBuilder::buildInitializedName("data_" + data_name + suffix, field_type, NULL));
  }

  /// \todo add "context"
  
  return result;
}

}

namespace MFB {

KLT<Kernel_OpenCL_OpenACC>::object_desc_t::object_desc_t(
      Kernel_OpenCL_OpenACC * kernel_,
      unsigned long file_id_
) :
  kernel(kernel_),
  file_id(file_id_),
  shape_configs()
{}

template <>
SgBasicBlock * createLocalDeclarations<
  DLX::KLT_Annotation<DLX::OpenACC::language_t>,
  ::KLT::Language::OpenCL,
  ::KLT::Runtime::OpenACC
>(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  ::KLT::Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>,
    ::KLT::Language::OpenCL,
    ::KLT::Runtime::OpenACC
  >::local_symbol_maps_t & local_symbol_maps,
  const ::KLT::Kernel<
    DLX::KLT_Annotation<DLX::OpenACC::language_t>,
    ::KLT::Language::OpenCL,
    ::KLT::Runtime::OpenACC
  >::arguments_t & arguments,
  const std::map<
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *,
    ::KLT::Runtime::OpenACC::loop_shape_t *
  > & loop_shapes
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_scalar_to_field;
  std::map< ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *, SgVariableSymbol *>::const_iterator it_data_to_field;

  std::map<
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t *,
    ::KLT::Runtime::OpenACC::loop_shape_t *
  >::const_iterator it_loop_shape;
  
  // * Definition *

  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);

  // * Lookup parameter symbols *

  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("param_" + param_name);
    assert(arg_sym != NULL);

    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, arg_sym));
  }

  // * Lookup scalar symbols *

  for (it_var_sym = arguments.scalars.begin(); it_var_sym != arguments.scalars.end(); it_var_sym++) {
    SgVariableSymbol * scalar_sym = *it_var_sym;
    std::string scalar_name = scalar_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("scalar_" + scalar_name);
    assert(arg_sym != NULL);

    local_symbol_maps.scalars.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(scalar_sym, arg_sym));
  }

  // * Lookup data symbols *

  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();;
    std::string data_name = data_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("data_" + data_name);
    assert(arg_sym != NULL);

    local_symbol_maps.datas.insert(
      std::pair< ::KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> > *, SgVariableSymbol *>(data, arg_sym)
    );
  }

  // * Create iterator *

  for (it_loop_shape = loop_shapes.begin(); it_loop_shape != loop_shapes.end(); it_loop_shape++) {
    ::KLT::LoopTrees<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::loop_t * loop = it_loop_shape->first;
    ::KLT::Runtime::OpenACC::loop_shape_t * shape = it_loop_shape->second;
    SgVariableSymbol * iter_sym = loop->iterator;
    std::string iter_name = iter_sym->get_name().getString();
    SgType * iter_type = iter_sym->get_type();

    if (!loop->isDistributed()) {
      assert(shape == NULL);

      SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
        "local_it_" + iter_name, iter_type, NULL, kernel_body
      );
      SageInterface::appendStatement(iter_decl, kernel_body);

      SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_it_" + iter_name);
      assert(local_sym != NULL);
      local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
    }
    else { // for now we add all possible iterators
      /* if (shape->tile_0) */ {
        std::string name = "local_it_" + iter_name + "_tile_0";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
      /* if (shape->gang) */ {
        std::string name = "local_it_" + iter_name + "_gang";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
      /* if (shape->tile_1) */ {
        std::string name = "local_it_" + iter_name + "_tile_1";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
      /* if (shape->worker) */ {
        std::string name = "local_it_" + iter_name + "_worker";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
      /* if (shape->tile_2) */ {
        std::string name = "local_it_" + iter_name + "_tile_2";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
      /* if (shape->vector) */ {
        std::string name = "local_it_" + iter_name + "_vector";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
      /* if (shape->tile_3) */ {
        std::string name = "local_it_" + iter_name + "_tile_3";
        SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
          name, iter_type, NULL, kernel_body
        );
        SageInterface::appendStatement(iter_decl, kernel_body);

        SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol(name);
        assert(local_sym != NULL);
        local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
      }
    }
  }

  /// \todo context

  return kernel_body;

}

}


