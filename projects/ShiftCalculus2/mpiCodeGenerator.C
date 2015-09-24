// Example MPI code generator
// Liao 9/22/2015
#include "rose.h"

#include "mpiCodeGenerator.h"

using namespace SageInterface;
using namespace SageBuilder;

int main ( int argc, char * argv[] )
{
  std::vector <std::string> argvList (argv, argv + argc);
  SgProject* project = frontend(argvList); //frontendConstantFolding);
  ROSE_ASSERT(project != NULL);

  ROSE_ASSERT (project->get_fileList().size() ==1);
  SgFile * cur_file = project->get_fileList()[0];

  //#include "mpi.h" 
  SageInterface::insertHeader (isSgSourceFile(cur_file), "libxomp_mpi.h", false);
  SageInterface::insertHeader (isSgSourceFile(cur_file), "mpi.h", false);
  SgFunctionDeclaration* main_decl = findMain(cur_file);
  ROSE_ASSERT (main_decl != NULL);
  SgFunctionDefinition* main_def = main_decl->get_definition();
  ROSE_ASSERT (main_def != NULL);
  SgBasicBlock* func_body = main_def->get_body();
  ROSE_ASSERT (func_body != NULL);

  // Setup MPI
  SgStatement* decl_rank = buildStatementFromString("int _xomp_rank;", func_body);
  prependStatement(decl_rank, func_body);

  SgStatement* decl_nprocs= buildStatementFromString("int _xomp_nprocs;", func_body);
  prependStatement(decl_nprocs, func_body);

  // xomp_init_mpi (&argc, &argv, &_xomp_rank, &_xomp_nprocs);
  SgExprListExp * para_list = buildExprListExp (buildAddressOfOp (buildVarRefExp("argc", func_body)),
      buildAddressOfOp (buildVarRefExp("argv", func_body)),
      buildAddressOfOp (buildVarRefExp("_xomp_rank", func_body)),
      buildAddressOfOp (buildVarRefExp("_xomp_nprocs", func_body))
      );
  SgExprStatement* mpi_init_stmt = buildFunctionCallStmt ("xomp_init_mpi", buildIntType(), para_list, func_body);
  //     SgStatement* last_decl = findLastDeclarationStatement (func_body);
  insertStatementAfter (decl_rank, mpi_init_stmt);


  AstTests::runAllTests(project);
  return backend(project);

  return 0;
}
