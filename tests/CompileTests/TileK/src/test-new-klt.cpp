
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"

#include "MFB/KLT/driver.hpp"

#include "MDCG/Core/model-builder.hpp"

#include "KLT/runtime.hpp"
#include "KLT/looptree.hpp"
#include "KLT/kernel.hpp"
#include "KLT/descriptor.hpp"
#include "KLT/api.hpp"
#include "KLT/data.hpp"

////////////////////////////////////////////////// Utils

template <class A, class B, class C>
void maps_composition(const std::map<A, B> & map_A_B, const std::map<B, C> & map_B_C, std::map<A, C> & map_A_C) {
  typename std::map<A, B>::const_iterator it_A_B;
  for (it_A_B = map_A_B.begin(); it_A_B != map_A_B.end(); it_A_B++) {
    typename std::map<B, C>::const_iterator it_B_C = map_B_C.find(it_A_B->second);
    if (it_B_C != map_B_C.end())
      map_A_C.insert(std::pair<A, C>(it_A_B->first, it_B_C->second));
  }
}

namespace KLT {

////////////////////////////////////////////////// Extract Data

// Requires:
//   * language_tpl::has_klt_data
template <class language_tpl>
KLT::Descriptor::data_t * convertData(typename language_tpl::data_clause_t * data_clause, const DLX::Frontend::data_sections_t & data_section) {
  SgVariableSymbol * data_sym = data_section.first;

  SgType * base_type = data_sym->get_type();
  std::vector<DLX::Frontend::section_t>::const_iterator it_section;
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
         if (isSgPointerType(base_type)) base_type = ((SgPointerType *)base_type)->get_base_type();
    else if (isSgArrayType  (base_type)) base_type = ((SgArrayType   *)base_type)->get_base_type();
    else assert(false);
    assert(base_type != NULL);
  }
  KLT::Descriptor::data_t * data = new KLT::Descriptor::data_t(data_sym, base_type);
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++)
    data->sections.push_back(new KLT::Descriptor::section_t(it_section->lower_bound, it_section->size));
  return data;
}

// Requires:
//   * language_tpl::has_klt_data
template <class language_tpl>
void convertDataList(const std::vector<typename language_tpl::clause_t *> & clauses, std::vector<KLT::Descriptor::data_t *> & data) {
  typename std::vector<typename language_tpl::clause_t *>::const_iterator it_clause;
  for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
    typename language_tpl::data_clause_t * data_clause = language_tpl::isDataClause(*it_clause);
    if (data_clause != NULL) {
      const std::vector<DLX::Frontend::data_sections_t> & data_sections = language_tpl::getDataSections(data_clause);
      std::vector<DLX::Frontend::data_sections_t>::const_iterator it_data_sections;
      for (it_data_sections = data_sections.begin(); it_data_sections != data_sections.end(); it_data_sections++)
        data.push_back(convertData<language_tpl>(data_clause, *it_data_sections));
    }
    else assert(false);
  }
}

////////////////////////////////////////////////// Extract Kernels & Loops

typedef std::map<SgForStatement *, size_t> loop_id_map_t;
typedef std::pair< ::KLT::Kernel::kernel_t *, loop_id_map_t> extracted_kernel_t;

// Requires:
//   * language_tpl::has_klt_kernel
//   * language_tpl::has_klt_loop
//   * language_tpl::has_klt_data
template <class language_tpl>
void extractLoopsAndKernels(
  const std::vector<typename language_tpl::directive_t *> & directives,
  std::map<typename language_tpl::directive_t *, SgForStatement *> & loop_directive_map,
  std::map<typename language_tpl::directive_t *, extracted_kernel_t> & kernel_directives_map
) {
  typedef typename language_tpl::directive_t directive_t;

  typename std::vector<directive_t *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    directive_t * directive = *it_directive;

    typename language_tpl::kernel_construct_t * kernel_construct = language_tpl::isKernelConstruct(directive->construct);
    typename language_tpl::loop_construct_t * loop_construct = language_tpl::isLoopConstruct(directive->construct);

    if (kernel_construct != NULL) {
      // Where to store the result
      extracted_kernel_t & kernel_directive = kernel_directives_map[directive];
      // Associated code region.
      SgStatement * region_base = language_tpl::getKernelRegion(kernel_construct);
      // Associated data
      std::vector<KLT::Descriptor::data_t *> data;
      convertDataList<language_tpl>(directive->clause_list, data);
      // Extract kernel
      kernel_directive.first = KLT::Kernel::kernel_t::extract(region_base, data, kernel_directive.second);
    }
    else if (loop_construct != NULL) {
      loop_directive_map.insert(std::pair<directive_t *, SgForStatement *>(directive, language_tpl::getLoopStatement(loop_construct)));
    }
    else assert(false);
  }
}

////////////////////////////////////////////////// Generate Kernels

typedef std::map<KLT::Descriptor::kernel_t *, std::vector<KLT::Descriptor::kernel_t *> > kernel_deps_map_t;

// Requires:
//   * language_tpl::has_klt_kernel
//   * language_tpl::has_klt_loop
//   * language_tpl::has_klt_tile
template <class language_tpl>
void generateAllKernels(
  const std::map<typename language_tpl::directive_t *, SgForStatement *> & loop_directive_map,
  const std::map<typename language_tpl::directive_t *, extracted_kernel_t> & kernel_directives_map,
  std::map<typename language_tpl::directive_t *, std::pair<KLT::Kernel::kernel_t *, std::map<KLT::Generator::tiling_info_t<language_tpl> *, kernel_deps_map_t> > > & kernel_directive_translation_map,
  KLT::Generator * generator
) {
  typedef typename language_tpl::directive_t directive_t;
  typedef KLT::Generator::tiling_info_t<language_tpl> tiling_info_t;

  typename std::map<directive_t *, extracted_kernel_t>::const_iterator it_kernel_directive;
  for (it_kernel_directive = kernel_directives_map.begin(); it_kernel_directive != kernel_directives_map.end(); it_kernel_directive++) {
    directive_t * directive = it_kernel_directive->first;
    assert(language_tpl::isKernelConstruct(directive->construct));

    KLT::Kernel::kernel_t * kernel = it_kernel_directive->second.first;
    std::map<directive_t *, size_t> directive_loop_id_map;
    maps_composition(loop_directive_map, it_kernel_directive->second.second, directive_loop_id_map);

    kernel_directive_translation_map[directive].first = kernel;
    std::map<tiling_info_t *, kernel_deps_map_t> & tiled_generated_kernels = kernel_directive_translation_map[directive].second;

    // Apply tiling
    std::map<tiling_info_t *, std::vector<KLT::Kernel::kernel_t *> > tiled_kernels;
    generator->applyLoopTiling<language_tpl>(kernel, directive_loop_id_map, tiled_kernels);

    typename std::map<tiling_info_t *, std::vector<KLT::Kernel::kernel_t *> >::const_iterator it_tiled_kernel;
    for (it_tiled_kernel = tiled_kernels.begin(); it_tiled_kernel != tiled_kernels.end(); it_tiled_kernel++) {
      std::map<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *> translation_map;
      std::map<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *> rtranslation_map;

      tiling_info_t * tiling_info = it_tiled_kernel->first;
      const std::vector<KLT::Kernel::kernel_t *> & subkernels = it_tiled_kernel->second;

      kernel_deps_map_t & kernels = tiled_generated_kernels[tiling_info];

      // Build all the sub-kernels for one tiling_info
      std::vector<KLT::Kernel::kernel_t *>::const_iterator it_kernel;
      for (it_kernel = subkernels.begin(); it_kernel != subkernels.end(); it_kernel++) {
        KLT::Kernel::kernel_t * subkernel = *it_kernel;

        assert(subkernel != NULL);
        assert(subkernel->root != NULL);
        assert(dynamic_cast<KLT::LoopTree::node_t *>(subkernel->root) != NULL);

        // Descriptor for kernel builder
        MFB::Driver<MFB::KLT::KLT>::kernel_desc_t kernel_desc(subkernel->root, subkernel->parameters, subkernel->data, generator);

        // Call builder
        KLT::Descriptor::kernel_t * result = generator->getDriver().build<KLT::Kernel::kernel_t>(kernel_desc);

        // Insert result in containers
        kernels.insert(std::pair<KLT::Descriptor::kernel_t *, std::vector<KLT::Descriptor::kernel_t *> >(result, std::vector<KLT::Descriptor::kernel_t *>()));
        translation_map.insert(std::pair<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *>(subkernel, result));
        rtranslation_map.insert(std::pair<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *>(result, subkernel));
      }
      // Figures out the dependencies between sub-kernels
      generator->solveDataFlow(kernel, tiling_info, subkernels, kernels, translation_map, rtranslation_map);
    }
  }
}

//////////////////////////////////////


// Requires:
//   * language_tpl::has_klt_loop
//   * language_tpl::has_klt_tile
template <class language_tpl>
struct Generator::tiling_info_t {
  typedef typename ::DLX::Directives::generic_clause_t<language_tpl>::template parameters_t<language_tpl::e_clause_tile> tile_parameter_t;
  std::map<size_t, std::map<size_t, tile_parameter_t *> > tiling_map;
};

typedef std::vector<LoopTree::node_t *> node_list_t;
typedef node_list_t::const_iterator node_list_citer_t;

template <class language_tpl>
LoopTree::node_t * applyTiling(LoopTree::node_t * node, const Generator::tiling_info_t<language_tpl> & tiling_info) {
  switch (node->kind) {
    case LoopTree::e_block:
    {
      LoopTree::block_t * block = new LoopTree::block_t();
      const node_list_t & children = ((LoopTree::block_t*)node)->children;
      for (node_list_citer_t it = children.begin(); it != children.end(); it++)
        block->children.push_back(applyTiling<language_tpl>(*it, tiling_info));
      break;
    }
    case LoopTree::e_cond:
    {
      LoopTree::cond_t * cond = new LoopTree::cond_t(*(LoopTree::cond_t *)node);
      cond->branch_true = applyTiling<language_tpl>(((LoopTree::cond_t *)node)->branch_true, tiling_info);
      cond->branch_false = applyTiling<language_tpl>(((LoopTree::cond_t *)node)->branch_false, tiling_info);
      return cond;
    }
    case LoopTree::e_loop:
    {
      typedef typename Generator::tiling_info_t<language_tpl>::tile_parameter_t tile_parameter_t;
      typename std::map<size_t, std::map<size_t, tile_parameter_t *> >::const_iterator it = tiling_info.tiling_map.find(((LoopTree::loop_t *)node)->id);
      if (it != tiling_info.tiling_map.end()) {
        assert(false); // TODO
      }
      else {
        LoopTree::loop_t * loop = new LoopTree::loop_t(*(LoopTree::loop_t *)node);
        loop->body = applyTiling<language_tpl>(((LoopTree::loop_t *)node)->body, tiling_info);
        return loop;
      }
      assert(false);
    }
    case LoopTree::e_stmt:
      return new LoopTree::stmt_t(*(LoopTree::stmt_t *)node);
    case LoopTree::e_tile:
    case LoopTree::e_ignored:
    case LoopTree::e_unknown:
    default:
      assert(false);
  }
  assert(false);
}

template <class language_tpl>
void splitKernelRoot(Kernel::kernel_t * kernel, const Generator::tiling_info_t<language_tpl> & tiling_info, std::vector<Kernel::kernel_t *> & kernels) {
  kernels.push_back(new Kernel::kernel_t(applyTiling<language_tpl>(kernel->root, tiling_info), kernel->parameters, kernel->data));
}

// Requires:
//   * language_tpl::has_klt_loop
//   * language_tpl::has_klt_tile
template <class language_tpl>
void Generator::applyLoopTiling(
  Kernel::kernel_t * kernel,
  const std::map<typename language_tpl::directive_t *, size_t> & directive_loop_id_map,
  std::map<tiling_info_t<language_tpl> *, std::vector<Kernel::kernel_t *> > & tiled_kernels
) const {
  typedef typename language_tpl::directive_t directive_t;
  typedef typename language_tpl::loop_construct_t loop_construct_t;
  typedef typename language_tpl::clause_t clause_t;
  typedef typename language_tpl::tile_clause_t tile_clause_t;
  typedef typename tiling_info_t<language_tpl>::tile_parameter_t tile_parameter_t;

  tiling_info_t<language_tpl> * tiling_info = new tiling_info_t<language_tpl>();

  typename std::map<directive_t *, size_t>::const_iterator it_loop;
  for (it_loop = directive_loop_id_map.begin(); it_loop != directive_loop_id_map.end(); it_loop++) {
    directive_t * directive = it_loop->first;
    size_t loop_id = it_loop->second;

    loop_construct_t * loop_construct = language_tpl::isLoopConstruct(directive->construct);
    assert(loop_construct != NULL);

    typename std::vector<clause_t *>::const_iterator it_clause;
    size_t tile_cnt = 0;
    for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
      tile_clause_t * tile_clause = language_tpl::isTileClause(*it_clause);
      if (tile_clause != NULL) {
        tiling_info->tiling_map[loop_id][tile_cnt++] = new tile_parameter_t(tile_clause->parameters);
      }
    }
  }

  splitKernelRoot<language_tpl>(kernel, *tiling_info, tiled_kernels[tiling_info]);
}

////////////////////////////////////// 

// Requires:
//   * language_tpl::has_klt_kernel
//   * language_tpl::has_klt_loop
//   * language_tpl::has_klt_tile
//   * language_tpl::has_klt_data
template <class language_tpl, class generator_tpl>
void compile(SgProject * project, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & basefilename) {
  typedef typename language_tpl::directive_t directive_t;
  typedef typename generator_tpl::template tiling_info_t<language_tpl> tiling_info_t;
  typedef std::map<tiling_info_t *, kernel_deps_map_t> tiling_choice_map_t;
  typedef std::map<directive_t *, std::pair<KLT::Kernel::kernel_t *, tiling_choice_map_t> > kernel_directive_translation_map_t;

  ::DLX::Frontend::Frontend<language_tpl> frontend;
  ::MFB::Driver< ::MFB::KLT::KLT> driver(project);
  ::MDCG::ModelBuilder model_builder(driver);

  language_tpl::init();

  std::string klt_inc_dir( KLT_RTL + "/include");
  std::string usr_inc_dir(USER_RTL + "/include");

  generator_tpl * generator = KLT::Generator::build<generator_tpl>(driver, model_builder, klt_inc_dir, usr_inc_dir, basefilename);

  if (!frontend.parseDirectives(project)) {
    std::cerr << "Error in FrontEnd !!!" << std::endl;
    exit(1);
  }

  std::map<directive_t *, SgForStatement *> loop_directive_map;
  std::map<directive_t *, extracted_kernel_t> kernel_directives_map;

  extractLoopsAndKernels<language_tpl>(frontend.directives, loop_directive_map, kernel_directives_map);

  kernel_directive_translation_map_t kernel_directive_translation_map;

  generateAllKernels<language_tpl>(loop_directive_map, kernel_directives_map, kernel_directive_translation_map, generator);

  // TODO
}

/* TODO
template <class language_tpl>
class Compiler {
  public:
    typedef std::map<SgForStatement *, size_t> loop_id_map_t;
    typedef std::pair< ::KLT::Kernel::kernel_t *, loop_id_map_t> extracted_kernel_t;
    typedef std::map< ::KLT::Descriptor::kernel_t *, std::vector< ::KLT::Descriptor::kernel_t *> > kernel_deps_map_t;

    typedef typename language_tpl::directive_t directive_t;

  protected:
    static KLT::Descriptor::data_t * convertData(typename language_tpl::data_clause_t * data_clause, const DLX::Frontend::data_sections_t & data_section);

    static void convertDataList(const std::vector<typename language_tpl::clause_t *> & clauses, std::vector<KLT::Descriptor::data_t *> & data)

    static void extractLoopsAndKernels(
      const std::vector<directive_t *> & directives,
      std::map<directive_t *, SgForStatement *> & loop_directive_map,
      std::map<directive_t *, extracted_kernel_t> & kernel_directives_map
    );

    static void generateAllKernels(
      const std::map<directive_t *, SgForStatement *> & loop_directive_map,
      const std::map<directive_t *, extracted_kernel_t> & kernel_directives_map,
      std::map<directive_t *, std::pair<KLT::Kernel::kernel_t *, std::map<KLT::Generator::tiling_info_t<language_tpl> *, kernel_deps_map_t> > > & kernel_directive_translation_map,
      KLT::Generator * generator
    );

  public:
    template <class generator_tpl>
    void compile(SgProject * project, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & basefilename)
};

*/

//////////////////////////////////////////////////

namespace TileK {

class Generator : public KLT::Generator {
  friend class KLT::Generator;

  public:
    typedef KLT::API::host_t   host_t;
    typedef KLT::API::kernel_t kernel_t;
#if 1
    typedef KLT::API::array_args_interface_t call_interface_t;
#else
    typedef KLT::API::individual_args_interface_t call_interface_t;
#endif

  protected:
    Generator(MFB::Driver<MFB::KLT::KLT> & driver, MDCG::ModelBuilder & model_builder) :
      KLT::Generator(driver, model_builder)
    {}

    virtual void loadExtraModel(const std::string & usr_inc_dir) {
      model_builder.add(tilek_model, "kernel", usr_inc_dir + "/RTL/Host", "h");
    }
};

} // namespace KLT::TileK

//////////////////////////////////////

} // namespace KLT

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  KLT::compile<DLX::TileK::language_t, KLT::TileK::Generator>(project, KLT_PATH, TILEK_PATH, basename);

  project->unparse();

  return 0;
}

