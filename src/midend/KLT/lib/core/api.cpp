
#include "sage3basic.h"

#include "MFB/utils.hpp"

#include "MDCG/Model/model.hpp"
#include "MDCG/Model/function.hpp"
#include "MDCG/Model/class.hpp"

#include "KLT/Core/api.hpp"
#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/utils.hpp"

namespace KLT {

namespace API {

////// KLT::API::kernel_t

void kernel_t::load(const MDCG::Model::model_t & model) {
  bool res = true;

  MDCG::Model::class_t class_;
  res = api_t::load(class_, klt_loop_context_class, model, "klt_loop_context_t", NULL); assert(res == true);
  res = api_t::load(class_, klt_data_context_class, model, "klt_data_context_t", NULL); assert(res == true);

  MDCG::Model::function_t function_;
  res = api_t::load(function_,  get_loop_lower_fnct, model, "klt_get_loop_lower" , NULL); assert(res == true);
  res = api_t::load(function_,  get_loop_upper_fnct, model, "klt_get_loop_upper" , NULL); assert(res == true);
  res = api_t::load(function_, get_loop_stride_fnct, model, "klt_get_loop_stride", NULL); assert(res == true);
  res = api_t::load(function_, get_tile_length_fnct, model, "klt_get_tile_length", NULL); assert(res == true);
  res = api_t::load(function_, get_tile_stride_fnct, model, "klt_get_tile_stride", NULL); assert(res == true);

  loadUser(model);
}

void kernel_t::loadUser(const MDCG::Model::model_t & model) {}

SgInitializedName * kernel_t::buildConstantVariable(const std::string & name, SgType * type, SgInitializer * init) const { return SageBuilder::buildInitializedName(name, type, init); }
SgInitializedName * kernel_t::buildGlobalVariable(const std::string & name, SgType * type, SgInitializer * init) const { return SageBuilder::buildInitializedName(name, type, init); }
SgInitializedName * kernel_t::buildLocalVariable(const std::string & name, SgType * type, SgInitializer * init) const { return SageBuilder::buildInitializedName(name, type, init); }

void kernel_t::applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const {}

SgType * kernel_t::buildKernelReturnType(Descriptor::kernel_t & kernel) const { return SageBuilder::buildVoidType(); }

//////

SgType * kernel_t::getLoopContextPtrType() const { return SageBuilder::buildPointerType(klt_loop_context_class->get_declaration()->get_type()); }

SgExpression * kernel_t::buildGetLoopLower (size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_lower_fnct);
}
SgExpression * kernel_t::buildGetLoopUpper (size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_upper_fnct);
}
SgExpression * kernel_t::buildGetLoopStride(size_t loop_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(loop_id, ctx, get_loop_stride_fnct);
}

SgExpression * kernel_t::buildGetTileLength(size_t tile_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_length_fnct);
}
SgExpression * kernel_t::buildGetTileStride(size_t tile_id, SgVariableSymbol * ctx) const {
  return ::MFB::Utils::buildCallVarIdx(tile_id, ctx, get_tile_stride_fnct);
}

SgType * kernel_t::getDataContextPtrType() const { return SageBuilder::buildPointerType(klt_data_context_class->get_declaration()->get_type()); }

////// KLT::API::host_t

void host_t::load(const MDCG::Model::model_t & model) {
  bool res = true;

  MDCG::Model::class_t class_;
  MDCG::Model::field_t field_;
  MDCG::Model::function_t function_;

    res = api_t::load(class_   , kernel_class        , model, "klt_kernel_t"      , NULL);   assert(res == true);
      res = api_t::load(field_ , kernel_param_field  , model,   "param"           , class_); assert(res == true);
      res = api_t::load(field_ , kernel_data_field   , model,   "data"            , class_); assert(res == true);
      res = api_t::load(field_ , kernel_loops_field  , model,   "loops"           , class_); assert(res == true);
      res = api_t::load(field_ , kernel_config_field , model,   "config"          , class_); assert(res == true);
//    res = api_t::load(field_ , kernel_tiles_field  , model,   "tiles"           , class_); assert(res == true);

    res = api_t::load(class_   , loop_class          , model, "klt_loop_t"        , NULL);   assert(res == true);
      res = api_t::load(field_ , loop_lower_field    , model,   "lower"           , class_); assert(res == true);
      res = api_t::load(field_ , loop_upper_field    , model,   "upper"           , class_); assert(res == true);
      res = api_t::load(field_ , loop_stride_field   , model,   "stride"          , class_); assert(res == true);

//  res = api_t::load(class_   , tile_class          , model, "klt_tile_t"        , NULL);   assert(res == true);
//    res = api_t::load(field_ , tile_length_field   , model,   "length"          , class_); assert(res == true);
//    res = api_t::load(field_ , tile_stride_field   , model,   "stride"          , class_); assert(res == true);

    res = api_t::load(class_   , data_class          , model, "klt_data_t"        , NULL);   assert(res == true);
      res = api_t::load(field_ , data_ptr_field      , model,   "ptr"             , class_); assert(res == true);
      res = api_t::load(field_ , data_sections_field , model,   "sections"        , class_); assert(res == true);

    res = api_t::load(class_   , section_class       , model, "klt_data_section_t", NULL);   assert(res == true);
      res = api_t::load(field_ , section_offset_field, model,   "offset"          , class_); assert(res == true);
      res = api_t::load(field_ , section_length_field, model,   "length"          , class_); assert(res == true);

    res = api_t::load(function_, build_kernel_func   , model, "klt_build_kernel"  , NULL);   assert(res == true);

    res = api_t::load(function_, execute_kernel_func , model, "klt_execute_kernel", NULL);   assert(res == true);

  loadUser(model);
}

void host_t::loadUser(const MDCG::Model::model_t & model) {}

//////

SgVariableSymbol * host_t::insertKernelInstance(const std::string & name, size_t kernel_id, SgScopeStatement * scope) const {
  SgInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
                           build_kernel_func, SageBuilder::buildExprListExp(SageBuilder::buildIntVal(kernel_id))
                         ));
  SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration("kernel", SageBuilder::buildPointerType(kernel_class->get_type()), init, scope);
  SageInterface::appendStatement(kernel_decl, scope);

  SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
  assert(kernel_sym != NULL);

  return kernel_sym;
}

void host_t::insertKernelExecute(SgVariableSymbol * kernel_sym, SgScopeStatement * scope) const {
  SageInterface::appendStatement(
    SageBuilder::buildFunctionCallStmt(
      SageBuilder::buildFunctionRefExp(execute_kernel_func),
      SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(kernel_sym))
    ), scope
  );
}

SgStatement * host_t::buildParamAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_param_field, SageBuilder::buildIntVal(idx), NULL), rhs));
}

SgStatement * host_t::buildDataPtrAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_ptr_field), rhs));
}

SgStatement * host_t::buildDataSectionOffsetAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
           MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_sections_field), SageBuilder::buildIntVal(dim)
         ), SageBuilder::buildVarRefExp(section_offset_field)), rhs));
}

SgStatement * host_t::buildDataSectionLengthAssign(SgVariableSymbol * kernel_sym, size_t idx, size_t dim, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildDotExp(SageBuilder::buildPntrArrRefExp(
           MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_data_field, SageBuilder::buildIntVal(idx), data_sections_field), SageBuilder::buildIntVal(dim)
         ), SageBuilder::buildVarRefExp(section_length_field)), rhs));
}

SgStatement * host_t::buildLoopLowerAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loops_field, SageBuilder::buildIntVal(idx), loop_lower_field), rhs));
}

SgStatement * host_t::buildLoopUpperAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loops_field, SageBuilder::buildIntVal(idx), loop_upper_field), rhs));
}

SgStatement * host_t::buildLoopStrideAssign(SgVariableSymbol * kernel_sym, size_t idx, SgExpression * rhs) const {
  return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(MFB::Utils::buildPtrArrElemField(SageBuilder::buildVarRefExp(kernel_sym), kernel_loops_field, SageBuilder::buildIntVal(idx), loop_stride_field), rhs));
}

////// KLT::API::call_interface_t

call_interface_t::call_interface_t(::MFB::Driver< ::MFB::Sage> & driver_, kernel_t * kernel_api_) : driver(driver_), kernel_api(kernel_api_) {}

void call_interface_t::addKernelArgsForContext(SgFunctionParameterList * param_list) const {
  param_list->append_arg(kernel_api->buildConstantVariable("loop_ctx", kernel_api->getLoopContextPtrType(), NULL));
  param_list->append_arg(kernel_api->buildConstantVariable("data_ctx", kernel_api->getDataContextPtrType(), NULL));
}

SgFunctionParameterList * call_interface_t::buildKernelParamList(Descriptor::kernel_t & kernel) const {
  SgFunctionParameterList * res = SageBuilder::buildFunctionParameterList();

  prependUserArguments(res);
  addKernelArgsForParameter(res, kernel.parameters);
  addKernelArgsForData     (res, kernel.data);
  addKernelArgsForContext  (res);

  return res;
}

void call_interface_t::getContextSymbol(SgFunctionDefinition * func_defn, Utils::symbol_map_t & symbol_map) const {
  symbol_map.loop_context = func_defn->lookup_variable_symbol("loop_ctx"); assert(symbol_map.loop_context != NULL);
  symbol_map.data_context = func_defn->lookup_variable_symbol("data_ctx"); assert(symbol_map.data_context != NULL);
}

void call_interface_t::createLoopIterator(const std::vector<Descriptor::loop_t *> & loops, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  std::vector<Descriptor::loop_t *>::const_iterator it;
  for (it = loops.begin(); it != loops.end(); it++) {
    std::ostringstream oss; oss << "l_" << (*it)->id;
    SgVariableSymbol * symbol = MFB::Utils::getExistingSymbolOrBuildDecl(oss.str(), (*it)->iterator->get_type(), bb);
    symbol_map.iter_loops.insert(std::pair<size_t, SgVariableSymbol *>((*it)->id, symbol));
    symbol_map.orig_loops.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>((*it)->iterator, symbol));
  }
}

SgExpression * call_interface_t::getTileIdx(const Descriptor::tile_t & tile) const {
  assert(false);
  return NULL;
}

void call_interface_t::createTileIterator(const std::vector<Descriptor::tile_t *> & tiles, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
  std::vector<Descriptor::tile_t *>::const_iterator it;
  for (it = tiles.begin(); it != tiles.end(); it++) {
    std::ostringstream oss; oss << "t_" << (*it)->id;
    SgVariableSymbol * iter_sym = MFB::Utils::getExistingSymbolOrBuildDecl(oss.str(), SageBuilder::buildIntType(), bb);
    symbol_map.iter_tiles.insert(std::pair<size_t, SgVariableSymbol *>((*it)->id, iter_sym));
    if ((*it)->kind > 1) {
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildVarRefExp(iter_sym),
        SageBuilder::buildMultiplyOp(
          getTileIdx(**it),
          kernel_api->buildGetTileStride((*it)->id, symbol_map.loop_context)
        )
      ), bb);
    }
  }
}

SgBasicBlock * call_interface_t::generateKernelBody(Descriptor::kernel_t & kernel, SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map) {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  kernel_defn->set_body(bb);
  bb->set_parent(kernel_defn);

  getSymbolForUserArguments(kernel_defn, symbol_map, bb);

  getContextSymbol(kernel_defn, symbol_map);

  getSymbolForParameter(kernel_defn, kernel.parameters, symbol_map, bb);

  getSymbolForData(kernel_defn, kernel.data, symbol_map, bb);

  createLoopIterator(kernel.loops, symbol_map, bb);

  createTileIterator(kernel.tiles, symbol_map, bb);

  std::cerr << "[Info] (KLT::call_interface_t::generateKernelBody) Found " << kernel.loops.size() << " loops and " << kernel.tiles.size() << " tiles." << std::endl;

  return bb;
}

void call_interface_t::prependUserArguments(SgFunctionParameterList * param_list) const {}

void call_interface_t::getSymbolForUserArguments(SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) {}

} // namespace KLT::API

} // namespace KLT

