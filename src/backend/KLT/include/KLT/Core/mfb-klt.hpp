
#ifndef __KLT_MFB_KLT_HPP__
#define __KLT_MFB_KLT_HPP__

#include "KLT/Core/kernel.hpp"
#include "KLT/Core/runtime.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/function-declaration.hpp"

namespace MFB {

/*!
 * \addtogroup grp_klt_mfb
 * @{
*/

template <class Object>
class KLT {};

template <>
class Driver<KLT> {
  protected:
    bool guard_kernel_decl;

  public:
    Driver(bool guard_kernel_decl_);
    virtual ~Driver();

    template <class Object>
    typename KLT<Object>::build_result_t build(typename KLT<Object>::object_desc_t const & object);
};

class KLT_Driver : public Driver<Sage>, public Driver<KLT> {
  public:
    KLT_Driver(SgProject * project_ = NULL, bool guard_kernel_decl = false);
    virtual ~KLT_Driver();
};

template <class Annotation, class Language, class Runtime>
SgBasicBlock * createLocalDeclarations(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
  const typename ::KLT::Kernel<Annotation, Language, Runtime>::arguments_t & arguments,
  const std::map<
    typename ::KLT::LoopTrees<Annotation>::loop_t *,
    typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *
  > & loop_tiling
);

template <class Object>
typename KLT<Object>::build_result_t Driver<KLT>::build(typename KLT<Object>::object_desc_t const & object) {
  typename KLT<Object>::build_result_t result = 
    new typename ::KLT::Kernel<
      typename KLT<Object>::Annotation,
      typename KLT<Object>::Language,
      typename KLT<Object>::Runtime
    >::a_kernel();

  std::map<SgVariableSymbol *, SgVariableSymbol *> param_to_field_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *> coef_to_field_map;
  std::map< ::KLT::Data<typename KLT<Object>::Annotation> *, SgVariableSymbol *> data_to_field_map;

  typename std::list<typename ::KLT::LoopTrees<typename KLT<Object>::Annotation>::loop_t *>::const_iterator it_nested_loop;
  typename std::list<typename ::KLT::LoopTrees<typename KLT<Object>::Annotation>::node_t *>::const_iterator it_body_branch;

  // * Function Declaration *

  std::ostringstream kernel_function_name;
  kernel_function_name << "kernel_" << object.kernel << "_" << object.id;
  result->kernel_name = kernel_function_name.str();

  SgFunctionParameterList * kernel_function_params =
    ::KLT::createParameterList<
      typename KLT<Object>::Annotation,
      typename KLT<Object>::Language,
      typename KLT<Object>::Runtime
    >(object.kernel);

  ::KLT::Runtime::get_exec_config<typename KLT<Object>::Annotation, typename KLT<Object>::Language, typename KLT<Object>::Runtime>(result->config, object.kernel);

  MFB::Sage<SgFunctionDeclaration>::object_desc_t kernel_function_desc(
    result->kernel_name,
    SageBuilder::buildVoidType(),
    kernel_function_params,
    NULL,
    object.file_id,
    object.file_id
  );

  Driver<Sage> * sage_driver = dynamic_cast<Driver<Sage> *>(this);
  assert(sage_driver != NULL);

  MFB::Sage<SgFunctionDeclaration>::build_result_t kernel_result = 
    sage_driver->build<SgFunctionDeclaration>(kernel_function_desc);

  {
    SgFunctionDeclaration * kernel_decl = kernel_result.symbol->get_declaration();
    assert(kernel_decl != NULL);

    SgFunctionDeclaration * first_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_firstNondefiningDeclaration());
    assert(first_kernel_decl != NULL);
    first_kernel_decl->get_functionModifier().setOpenclKernel();

    SgFunctionDeclaration * defn_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_definingDeclaration());
    assert(defn_kernel_decl != NULL);
    defn_kernel_decl->get_functionModifier().setOpenclKernel();

    if (guard_kernel_decl)
      SageInterface::guardNode(defn_kernel_decl, std::string("defined(ENABLE_") + result->kernel_name + ")");
  }

  // * Local Declarations *

  typename ::KLT::Kernel<typename KLT<Object>::Annotation, typename KLT<Object>::Language, typename KLT<Object>::Runtime>::local_symbol_maps_t local_symbol_maps;

  SgBasicBlock * body = createLocalDeclarations<
    typename KLT<Object>::Annotation,
    typename KLT<Object>::Language,
    typename KLT<Object>::Runtime
  > (
    *(Driver<Sage> *)this,
    kernel_result.definition,
    local_symbol_maps,
    object.kernel->getArguments(),
    object.tiling
  );

  std::map<
      typename ::KLT::LoopTrees<typename KLT<Object>::Annotation>::loop_t *,
      typename KLT<Object>::Runtime::a_loop
  > loop_descriptors_map;

  size_t loop_cnt = 0;
  size_t tile_cnt = 0;
  const std::list<typename ::KLT::LoopTrees<typename KLT<Object>::Annotation>::node_t *> & kernel_roots = object.kernel->getRoots();
  typename std::list<typename ::KLT::LoopTrees<typename KLT<Object>::Annotation>::node_t *>::const_iterator it_root;
  for (it_root = kernel_roots.begin(); it_root != kernel_roots.end(); it_root++)
    ::KLT::generateKernelBody<typename KLT<Object>::Annotation, typename KLT<Object>::Language, typename KLT<Object>::Runtime>(
      *it_root, loop_cnt, tile_cnt, loop_descriptors_map, (typename KLT<Object>::Runtime::exec_mode_t)0, result->config, object.tiling, local_symbol_maps, body
    );

  assert(result->loops.empty());
  result->loops.resize(loop_descriptors_map.size());

  typename std::map<
      typename ::KLT::LoopTrees<typename KLT<Object>::Annotation>::loop_t *,
      typename KLT<Object>::Runtime::a_loop
  >::const_iterator it_loop_desc;
  for (it_loop_desc = loop_descriptors_map.begin(); it_loop_desc != loop_descriptors_map.end(); it_loop_desc++)
    result->loops[it_loop_desc->second.id] = it_loop_desc->second;

  SageInterface::setSourcePositionForTransformation(body);

  return result;
}

/** @} */

}

#endif /* __KLT__MFB_KLT_HPP__ */
