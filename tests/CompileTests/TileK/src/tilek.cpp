
#if defined(TILEK_BASIC)
#  if defined(TILEK_ACCELERATOR) || defined(TILEK_THREADS)
#    error "TileK can only be configured for one of Basic, Threads, or Accelerator."
#  endif
#elif defined(TILEK_THREADS)
#  if defined(TILEK_BASIC) || defined(TILEK_ACCELERATOR)
#    error "TileK can only be configured for one of Basic, Threads, or Accelerator."
#  endif
#elif defined(TILEK_ACCELERATOR)
#  if defined(TILEK_BASIC) || defined(TILEK_THREADS)
#    error "TileK can only be configured for one of Basic, Threads, or Accelerator."
#  endif
#  if !(defined(TILEK_TARGET_OPENCL) xor defined(TILEK_TARGET_CUDA))
#    error "Tilek for Accelerator needs one of OpenCL or CUDA target to be defined!"
#  endif
#else
#  error "TileK need to be configured for one of Basic, Threads, or Accelerator."
#endif

#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation;

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Language
#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
typedef ::KLT::Language::OpenCL Klang; // Kernel Language
#  elif defined(TILEK_TARGET_CUDA)
typedef ::KLT::Language::CUDA Klang; // Kernel Language
#  endif
#else
typedef ::KLT::Language::C Klang; // Kernel Language
#endif

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "MDCG/TileK/model.hpp"
typedef ::MDCG::TileK::KernelDesc<Hlang, Klang> KernelDesc; // Model for Static Initializer

#include "MDCG/TileK/runtime.hpp"

#include "DLX/KLT/compiler.hpp" // Needs Annotation and Runtime to be defined

#include <cassert>

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

#if defined(TILEK_THREADS)
  args.push_back("-DTILEK_THREADS");
#elif defined(TILEK_ACCELERATOR)
  args.push_back("-DTILEK_ACCELERATOR");
#  if defined(TILEK_TARGET_OPENCL)
  args.push_back("-DTILEK_TARGET_OPENCL");
#  elif defined(TILEK_TARGET_CUDA)
  args.push_back("-DTILEK_TARGET_CUDA");
#  endif
#endif

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
  std::string kernel_filename(basename + "-kernel.cl");
#  elif defined(TILEK_TARGET_CUDA)
  std::string kernel_filename(basename + "-kernel.cu");
#  endif
#else
  std::string kernel_filename(basename + "-kernel.c");
#endif

  DLX::KLT::compile<Dlang, Hlang, Klang, Runtime, KernelDesc>(project, std::string(KLT_RTL_INC_PATH), std::string(TILEK_RTL_INC_PATH), kernel_filename, basename + "-data.c");

  project->unparse();

  return 0;
}

