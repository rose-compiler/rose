
#include "KLT/utils.hpp"

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/iteration-mapper.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "KLT/OpenACC/dlx-openacc.hpp"
#include "KLT/OpenACC/language-opencl.hpp"
#include "KLT/OpenACC/runtime-openacc.hpp"
#include "KLT/OpenACC/mfb-acc-ocl.hpp"

#include "MFB/Sage/function-declaration.hpp"

#include "sage3basic.h"

namespace MFB {

KLT<Kernel_OpenCL_OpenACC>::object_desc_t::object_desc_t(
  unsigned id_,
  Kernel_OpenCL_OpenACC * kernel_,
  unsigned long file_id_
) :
  id(id_),
  kernel(kernel_),
  file_id(file_id_),
  shapes()
{}

SgVariableSymbol * getExistingSymbolOrBuildDecl(
  const std::string & name,
  SgType * type,
  SgScopeStatement * scope
) {
  SgVariableSymbol * sym = scope->lookup_variable_symbol(name);
  if (sym == NULL) {
    SgVariableDeclaration * decl = SageBuilder::buildVariableDeclaration(name, type, NULL, scope);
    SageInterface::appendStatement(decl, scope);
    sym = scope->lookup_variable_symbol(name);
  }
  assert(sym != NULL);
  return sym;
}

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
      SgVariableSymbol * local_sym = getExistingSymbolOrBuildDecl("local_it_" + iter_name, iter_type, kernel_body);
      local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
    }
    else {
      std::cerr << "Create it decl for " << iter_name << " (dist)" << std::endl;

      SgVariableSymbol * local_sym = NULL;

      if (shape->tile_0 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_0";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[0] = local_sym;
      }
      if (shape->gang != 1) {
        std::string name = "local_it_" + iter_name + "_gang";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[1] = local_sym;
      }
      if (shape->tile_1 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_1";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[2] = local_sym;
      }
      if (shape->worker != 1) {
        std::string name = "local_it_" + iter_name + "_worker";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[3] = local_sym;
      }
      if (shape->tile_2 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_2";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[4] = local_sym;
      }
      if (shape->vector == 0)
        assert(false); // Vector cannot have a dynamic size
      else if (shape->vector > 1) {

        assert(false); /// \todo no iterator with static vector length > 1, as vector expressions imply it

        std::string name = "local_it_" + iter_name + "_vector";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[5] = local_sym;
      }

      if (shape->tile_3 != 1) {
        std::string name = "local_it_" + iter_name + "_tile_3";
        local_sym = getExistingSymbolOrBuildDecl(name, iter_type, kernel_body);
        shape->iterators[6] = local_sym;
      }

      assert(local_sym != NULL);

      local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));

    }
  }

  SgVariableSymbol * context_sym = kernel_defn->lookup_variable_symbol("context");
  assert(context_sym != NULL);

  local_symbol_maps.context = context_sym;

  return kernel_body;

}

}

