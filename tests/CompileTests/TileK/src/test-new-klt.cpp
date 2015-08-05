
// ROSE

#include "sage3basic.h"

// KLT

#include "KLT/DLX/compiler.hpp"

// TileK

#include "DLX/TileK/language.hpp"

namespace KLT {

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
    Generator(MFB::Driver<MFB::KLT::KLT> & driver, ::MDCG::Tools::ModelBuilder & model_builder) :
      KLT::Generator(driver, model_builder)
    {}

    virtual void loadExtraModel(const std::string & usr_inc_dir) {
      model_builder.add(tilek_model, "klt-user", usr_inc_dir + "/RTL/Host", "h");
    }
};

} // namespace KLT::TileK

} // namespace KLT

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  KLT::DLX::Compiler<DLX::TileK::language_t, KLT::TileK::Generator> compiler(project, KLT_PATH, TILEK_PATH, basename);

  compiler.compile(project);

  project->unparse();

  return 0;
}

