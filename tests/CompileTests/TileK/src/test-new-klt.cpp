
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

////////////////////////////////////////////////// Extract Data

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

template <class language_tpl>
void generateAllKernels(
  const std::map<typename language_tpl::directive_t *, SgForStatement *> & loop_directive_map,
  const std::map<typename language_tpl::directive_t *, extracted_kernel_t> & kernel_directives_map,
  std::map<typename language_tpl::directive_t *, std::pair<KLT::Kernel::kernel_t *, std::map<KLT::Runtime::tiling_info_t<language_tpl> *, kernel_deps_map_t> > > & kernel_directive_translation_map,
  KLT::Runtime * runtime
) {
  typedef typename language_tpl::directive_t directive_t;
  typedef KLT::Runtime::tiling_info_t<language_tpl> tiling_info_t;

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
    runtime->applyLoopTiling<language_tpl>(kernel, directive_loop_id_map, tiled_kernels);

    typename std::map<tiling_info_t *, std::vector<KLT::Kernel::kernel_t *> >::const_iterator it_tiled_kernel;
    for (it_tiled_kernel = tiled_kernels.begin(); it_tiled_kernel != tiled_kernels.end(); it_tiled_kernel++) {
      std::map<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *> translation_map;
      std::map<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *> rtranslation_map;

      kernel_deps_map_t & kernels = tiled_generated_kernels[it_tiled_kernel->first];

      std::vector<KLT::Kernel::kernel_t *>::const_iterator it_kernel;
      for (it_kernel = it_tiled_kernel->second.begin(); it_kernel != it_tiled_kernel->second.end(); it_kernel++) {
//      assert(dynamic_cast<KLT::LoopTree::node_t *>((*it_kernel)->root) != NULL);
        // Descriptor for kernel builder
        MFB::Driver<MFB::KLT::KLT>::kernel_desc_t kernel_desc((*it_kernel)->root, (*it_kernel)->parameters, (*it_kernel)->data, runtime);
        // Call builder
        KLT::Descriptor::kernel_t * result = runtime->getDriver().build<KLT::Kernel::kernel_t>(kernel_desc);
        // Insert result in containers
        kernels.insert(std::pair<KLT::Descriptor::kernel_t *, std::vector<KLT::Descriptor::kernel_t *> >(result, std::vector<KLT::Descriptor::kernel_t *>()));
        translation_map.insert(std::pair<KLT::Kernel::kernel_t *, KLT::Descriptor::kernel_t *>(*it_kernel, result));
        rtranslation_map.insert(std::pair<KLT::Descriptor::kernel_t *, KLT::Kernel::kernel_t *>(result, *it_kernel));
      }
      // Figures out the dependencies between sub-kernels
      runtime->solveDataFlow(kernel, it_tiled_kernel->first, it_tiled_kernel->second, kernels, translation_map, rtranslation_map);
    }
  }
}

////////////////////////////////////// 

template <class language_tpl, class runtime_tpl, class host_api_tpl, class kernel_api_tpl, class call_interface_tpl>
void compile(SgProject * project, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & basefilename) {
  typedef typename language_tpl::directive_t directive_t;
  typedef typename runtime_tpl::template tiling_info_t<language_tpl> tiling_info_t;
  typedef std::map<tiling_info_t *, kernel_deps_map_t> tiling_choice_map_t;
  typedef std::map<directive_t *, std::pair<KLT::Kernel::kernel_t *, tiling_choice_map_t> > kernel_directive_translation_map_t;

  ::DLX::Frontend::Frontend<language_tpl> frontend;
  ::MFB::Driver< ::MFB::KLT::KLT> driver(project);
  ::MDCG::ModelBuilder model_builder(driver);

  language_tpl::init();

  std::string klt_inc_dir( KLT_RTL + "/include");
  std::string usr_inc_dir(USER_RTL + "/include");

  runtime_tpl * runtime = KLT::Runtime::build<runtime_tpl, host_api_tpl, kernel_api_tpl, call_interface_tpl>(driver, model_builder, klt_inc_dir, usr_inc_dir, basefilename);

  if (!frontend.parseDirectives(project)) {
    std::cerr << "Error in FrontEnd !!!" << std::endl;
    exit(1);
  }

  std::map<directive_t *, SgForStatement *> loop_directive_map;
  std::map<directive_t *, extracted_kernel_t> kernel_directives_map;

  extractLoopsAndKernels<language_tpl>(frontend.directives, loop_directive_map, kernel_directives_map);

  kernel_directive_translation_map_t kernel_directive_translation_map;

  generateAllKernels<language_tpl>(loop_directive_map, kernel_directives_map, kernel_directive_translation_map, runtime);

  // TODO
}

//////////////////////////////////////////////////

namespace TileK {

typedef KLT::API::host_t   host_t;
typedef KLT::API::kernel_t kernel_t;

#if 1
typedef KLT::API::array_args_interface_t call_interface_t;
#else
typedef KLT::API::individual_args_interface_t call_interface_t;
#endif

class Runtime : public KLT::Runtime {
  friend class KLT::Runtime;
  protected:
    Runtime(MFB::Driver<MFB::KLT::KLT> & driver, MDCG::ModelBuilder & model_builder) :
      KLT::Runtime(driver, model_builder)
    {}

    virtual void loadExtraModel(const std::string & usr_inc_dir) {
      model_builder.add(tilek_model, "kernel", usr_inc_dir + "/RTL/Host", "h");
    }
};

}

//////////////////////////////////////

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  compile<DLX::TileK::language_t, TileK::Runtime, TileK::host_t, TileK::kernel_t, TileK::call_interface_t>(project, KLT_PATH, TILEK_PATH, basename);

  project->unparse();

  return 0;
}

