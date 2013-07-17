/*
 * A test translator to process OpenMP Accelerator directives
 *
 * by Liao, 2/8/2013
*/
#include "rose.h"
#include <string>

using namespace std;

int main(int argc, char * argv[])
{
  // This flag can skip insertion of XOMP_init() and XOMP_terminate(), both of which are defined in libxomp.a
  // But somehow nvcc cannot link the entire libxomp.a with the cuda object file
  // TODO: ultimate solution is to outline all CUDA code into a separate file and keep the original file type intact
  OmpSupport::enable_accelerator = true;

  SgProject *project = frontend (argc, argv);


  //TODO: turn this back on once blockDim.x * blockIdx.x + threadIdx.x is built properly in omp_lowering.cpp
  //  AstTests::runAllTests(project);

  // change .c suffix to .cu suffix
  // We only process one single input file at a time
  ROSE_ASSERT (project->get_fileList().size() ==1);
  SgFile * cur_file = project->get_fileList()[0];

  string orig_name = cur_file->get_file_info()->get_filenameString();
  string file_suffix = StringUtility::fileNameSuffix(orig_name);
  // We only allow C file to be compatible with nvcc CUDA compiler
  ROSE_ASSERT (CommandlineProcessing::isCFileNameSuffix(file_suffix));
  orig_name = StringUtility::stripPathFromFileName(orig_name);
  string naked_name = StringUtility::stripFileSuffixFromFileName(orig_name);
  cur_file->set_unparse_output_filename("rose_"+naked_name+".cu");

  return backend(project);
}

