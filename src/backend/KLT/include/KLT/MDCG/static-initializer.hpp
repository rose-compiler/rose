
#include "MDCG/Tools/static-initializer.hpp"
#include "KLT/utils.hpp"

namespace KLT {

namespace MDCG {

SgExpression * createParamSizeOf(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<SgVariableSymbol *> & parameters) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::vector<SgVariableSymbol *>::const_iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++)
    expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->get_type()));

  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(parameters.size()));

  return SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
}

SgExpression * createDataSizeOf(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++)
    expr_list->append_expression(SageBuilder::buildSizeOfOp((*it)->base_type));

  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(data.size()));

  return SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
}

SgExpression * createDataNDims(MFB::Driver<MFB::Sage> & driver, const std::string & decl_name, size_t file_id, const std::vector<Descriptor::data_t *> & data) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();
  SgInitializer * init = SageBuilder::buildAggregateInitializer(expr_list);

  std::vector<Descriptor::data_t *>::const_iterator it;
  for (it = data.begin(); it != data.end(); it++)
    expr_list->append_expression(SageBuilder::buildIntVal((*it)->sections.size()));

  SgType * type = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildUnsignedLongVal(data.size()));

  return SageBuilder::buildVarRefExp(::MDCG::Tools::StaticInitializer::instantiateDeclaration(driver, decl_name, file_id, type, init));
}

template <class language_tpl>
struct DataContainer {
  typedef Kernel::kernel_t input_t;

/* IN:
      struct kernel_t {
        node_t * root;
        vsym_list_t parameters;
        data_list_t data;
      };                                            */

/* OUT:
      struct klt_data_container_t {
        int num_param;
        int * sizeof_param;

        int num_data;
        int * sizeof_data;
        int * ndims_data;
      };                            */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int num_param;
        return SageBuilder::buildIntVal(input.parameters.size());
      }
      case 1:
      { // int * sizeof_param;
        std::ostringstream decl_name; decl_name << "sizeof_param_" << &input;
        return createParamSizeOf(driver, decl_name.str(), file_id, input.parameters);
      }
      case 2:
      { // int num_data;
        return SageBuilder::buildIntVal(input.data.size());
      }
      case 3:
      { // int * sizeof_data;
        std::ostringstream decl_name; decl_name << "sizeof_data_" << &input;
        return createDataSizeOf(driver, decl_name.str(), file_id, input.data);
      }
      case 4:
      { // int * ndims_data;
        std::ostringstream decl_name; decl_name << "ndims_data_" << &input;
        return createDataNDims(driver, decl_name.str(), file_id, input.data);
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct TileDesc {
  typedef Descriptor::tile_t * input_t;

/* IN: 
      struct tile_t {
        size_t id;
        tile_kind_e kind;
        size_t order;
        SgExpression * param;
      };                             */

/* OUT:
      struct klt_tile_desc_t {
        int idx;
        enum tile_kind_e kind;
        int param;
      };                                    */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int idx;
        return SageBuilder::buildIntVal(input->id);
      }
      case 1:
      { // enum tile_kind_e kind;
        return SageBuilder::buildIntVal(input->kind);
      }
      case 2:
      { // struct klt_tile_desc_t * tile_desc;
        return SageInterface::copyExpression(input->param);
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct LoopDesc {
  typedef Descriptor::loop_t * input_t;

/* IN: 
      struct loop_t {
        size_t id;
        SgExpression * lb;
        SgExpression * ub;
        SgExpression * stride;
        std::vector<tile_t *> tiles;
        SgVariableSymbol * iterator;
      };                             */

/* OUT:
      struct klt_loop_desc_t {
        int idx;
        int num_tiles;
        struct klt_tile_desc_t * tile_desc;
      };                                    */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int idx;
        return SageBuilder::buildIntVal(input->id);
      }
      case 1:
      { // int num_tiles;
        return SageBuilder::buildIntVal(input->tiles.size());
      }
      case 2:
      { // struct klt_tile_desc_t * tile_desc;
        std::ostringstream decl_name; decl_name << "tile_desc_" << input;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("tile_desc", "klt_tile_desc_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createArrayPointer<TileDesc<language_tpl> >(
                   driver, field_class, input->tiles.size(), input->tiles.begin(), input->tiles.end(), file_id, decl_name.str()
               );
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct TopLoopContainer {
  typedef std::vector<Descriptor::loop_t *> input_t;

/* IN: std::vector<Descriptor::loop_t *> */

/* OUT:
      struct klt_loop_container_t {
        int num_loops;
        int num_tiles;
        struct klt_loop_desc_t * loop_desc;
      };                                    */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int num_loops;
        return SageBuilder::buildIntVal(input.size());
      }
      case 1:
      { // int num_tiles;
        return SageBuilder::buildIntVal(0); // klt_loop_container_t for the original loop-tree => no tile
      }
      case 2:
      { // struct klt_loop_desc_t * loop_desc;
        std::ostringstream decl_name; decl_name << "top_loop_desc_" << &input;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc<language_tpl> >(
                   driver, field_class, input.size(), input.begin(), input.end(), file_id, decl_name.str()
               );
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct LoopContainer {
  typedef Descriptor::kernel_t input_t;

/* IN:
      struct kernel_t {
        size_t id;
        std::string kernel_name;
        std::vector<loop_t *> loops;
        std::vector<tile_t *> tiles;
        std::vector<SgVariableSymbol *> parameters;
        std::vector<data_t *> data;
      };                                            */

/* OUT:
      struct klt_loop_container_t {
        int num_loops;
        int num_tiles;
        struct klt_loop_desc_t * loop_desc;
      };                                    */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // int num_loops;
        return SageBuilder::buildIntVal(input.loops.size());
      }
      case 1:
      { // int num_tiles;
        return SageBuilder::buildIntVal(input.tiles.size());
      }
      case 2:
      { // struct klt_loop_desc_t * loop_desc;
        std::ostringstream decl_name; decl_name << "loop_desc_" << &input;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("loop_desc", "klt_loop_desc_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createArrayPointer<LoopDesc<language_tpl> >(
                   driver, field_class, input.loops.size(), input.loops.begin(), input.loops.end(), file_id, decl_name.str()
               );
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct SubKernelDesc {
  typedef std::pair<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> > input_t;

/* IN: 
       input.first  = Descriptor::kernel_t *
       input.second = std::vector<Descriptor::kernel_t *>      */

/* OUT:
      struct klt_subkernel_desc_t {
        struct klt_loop_container_t loop;
        int num_params;
        int * param_ids;
        int num_data;
        int * data_ids;
        int num_loops;
        int * loop_ids;
        int num_deps;
        int * deps_ids;
        struct klt_subkernel_config_t * config;
      };                                                  */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { // struct klt_loop_container_t loop;
        ::MDCG::Model::class_t field_class = element->node->getBaseClass("loop", "klt_loop_container_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createInitializer<LoopContainer<language_tpl> >(driver, field_class, *input.first, file_id);
      }
      case 1:
      { // int num_params;
        return SageBuilder::buildIntVal(input.first->parameters.size());
      }
      case 2:
      { // int * param_ids;
        return SageBuilder::buildIntVal(0); // TODO
      }
      case 3:
      { // int num_data;
        return SageBuilder::buildIntVal(input.first->data.size());
      }
      case 4:
      { // int * data_ids;
        return SageBuilder::buildIntVal(0); // TODO
      }
      case 5:
      { // int num_loops;
        return SageBuilder::buildIntVal(input.first->loops.size());
      }
      case 6:
      { // int * loop_ids;
        return SageBuilder::buildIntVal(0); // TODO
      }
      case 7:
      { // int num_deps;
        return SageBuilder::buildIntVal(input.second.size());
      }
      case 8:
      { // int * deps_ids;
        return SageBuilder::buildIntVal(0); // TODO
      }
      case 9:
      { // struct klt_subkernel_config_t * config;
        return SageBuilder::buildIntVal(0); // TODO
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct VersionDesc {
  typedef std::pair<Utils::tiling_info_t<language_tpl> *, Utils::kernel_deps_map_t> input_t;

/* IN: 
       input.first  = Utils::tiling_info_t<language_tpl> * = [ LANGUAGE DEPENDENT ]
       input.second = std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> >      */

/* OUT:
      struct klt_version_desc_t {
        int num_subkernels;
        struct klt_subkernel_desc_t * subkernels;
        struct klt_version_selector_t * version_selector;
      };                                                  */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    switch (field_id) {
      case 0:
      { //  int num_subkernels;
        return SageBuilder::buildIntVal(input.second.size());
      }
      case 1:
      { // struct klt_subkernel_desc_t * subkernels;
        std::ostringstream decl_name; decl_name << "subkernels_" << input.first;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("subkernels", "klt_subkernel_desc_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createArrayPointer<SubKernelDesc<language_tpl> >(
                   driver, field_class, input.second.size(), input.second.begin(), input.second.end(), file_id, decl_name.str()
               );
      }
      case 2:
      { // struct klt_version_selector_t * version_selector;
        return SageBuilder::buildIntVal(0); // TODO [ LANGUAGE DEPENDENT ]
      }
      default:
        assert(false);
    }
  }
};

template <class language_tpl>
struct KernelContainer {
  typedef std::pair<typename language_tpl::directive_t *, Utils::subkernel_result_t<language_tpl> > input_t;

/* IN:
      struct subkernel_result_t {
        Kernel::kernel_t * original;
        std::vector<Descriptor::loop_t *> loops;
        std::map<tiling_info_t<language_tpl> *, kernel_deps_map_t> tiled;
      };                                                                  */

/* OUT:
      struct klt_kernel_desc_t {
        struct klt_data_container_t data;
        struct klt_loop_container_t loop;
        int num_versions;
        struct klt_version_desc_t * versions;
      };                                      */

  static SgExpression * createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
  ) {
    const Utils::subkernel_result_t<language_tpl> & subkernel_result = input.second;
    switch (field_id) {
      case 0:
      { // struct klt_data_container_t data;
        ::MDCG::Model::class_t field_class = element->node->getBaseClass("data", "klt_data_container_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createInitializer<DataContainer<language_tpl> >(driver, field_class, *(subkernel_result.original), file_id);
      }
      case 1:
      { // struct klt_loop_container_t loop;
        ::MDCG::Model::class_t field_class = element->node->getBaseClass("loop", "klt_loop_container_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createInitializer<TopLoopContainer<language_tpl> >(driver, field_class, subkernel_result.loops, file_id);
      }
      case 2:
      { // int num_versions;
        return SageBuilder::buildIntVal(subkernel_result.tiled.size());
      }
      case 3:
      { // struct klt_version_desc_t * versions;
        std::ostringstream decl_name; decl_name << "versions_" << subkernel_result.original;
        ::MDCG::Model::class_t field_class = element->node->getBaseClassForPointerOnClass("versions", "klt_version_desc_t"); assert(field_class != NULL);
        return ::MDCG::Tools::StaticInitializer::createArrayPointer<VersionDesc<language_tpl> >(
                   driver, field_class, subkernel_result.tiled.size(), subkernel_result.tiled.begin(), subkernel_result.tiled.end(), file_id, decl_name.str()
               );
      }
      default:
        assert(false);
    }
  }
};

} // namespace KLT::MDCG

} // namespace KLT

