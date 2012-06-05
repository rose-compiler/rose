
#include "rose-kernel/PolyhedralKernel.hpp"
#include "opencl-from-loops.hpp"
#include "rose-utils.hpp"

void usage() {
  std::cout << "usage: loop-ocl input.c dim_out dim_used" << std::endl;
  std::cout << "          dim_out >= 0" << std::endl;
  std::cout << "          dim_used = [1,3]" << std::endl;
  std::cout << "       kernel function name need to have the prefix \"kernel_\" and contains pragma \"scop\" and \"endscop\"" << std::endl;
}

int main(int argc, char ** argv) {

    if (argc != 4) {
      usage();
      return 1;
    }

    unsigned dim_out = atoi(argv[2]);
    unsigned dim_used = atoi(argv[3]);

    if (dim_used == 0 || dim_used > 3) {
      usage();
      return 1;
    }

    char * args[3] = {argv[0], (char *)"-DPARAMETRIC", argv[1]};
    SgProject * project = frontend ( 3 , args );
    SgGlobal * global_scope = isSgSourceFile((*project)[0])->get_globalScope();

    std::string ocl_file_name = "rose_" + isSgSourceFile((*project)[0])->get_sourceFileNameWithoutPath() + "l";
    SgSourceFile * ocl_file = buildOpenCLfile(ocl_file_name, project);

  // Insert OpenCL-wrapper header

    SageInterface::insertHeader("opencl-wrapper.h", PreprocessingInfo::after, true, global_scope);

  // Scan all functions
    std::vector<SgNode*> func_decls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
    std::vector<SgNode*>::iterator it_;
    for (it_ = func_decls.begin(); it_ != func_decls.end(); it_++) {
        SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_);

        // Look for the kernel function
        std::string name = func_decl->get_name().getString();
        if (name.compare(0, 7, "kernel_") != 0) continue;
        SgFunctionDefinition * func_def = func_decl->get_definition();
        if (func_def == NULL) continue;
        SgBasicBlock * func_body = func_def->get_body();
        if (func_body == NULL) continue;

        AnalysisContainer analysis;
        analysis.base_func = func_decl;

       // Polyhedral analysis
#define APPLY_POLY_ANALYSIS
#ifdef APPLY_POLY_ANALYSIS
        {
          try {
            analysis.polyhedral_analysis_success = PolyhedricAnnotation::parse(func_decl, "kernel_");
            if (analysis.polyhedral_analysis_success) {
              PolyhedricAnnotation::FunctionPolyhedralProgram & polyhedral_model = 
                  PolyhedricAnnotation::getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(func_decl);

              polyhedral_model.print(std::cout);

              if (polyhedral_model.getNumberOfGlobals() > 0) {
                std::cerr << "Find a polyhedral modeling but it is parametric..." << std::endl;
                analysis.polyhedral_analysis_success = false;
              }
              else {
                analysis.dependencies = new std::vector<PolyhedricDependency::FunctionDependency *>();
                std::vector<PolyhedricDependency::FunctionDependency *> * tmp;
		std::cerr << "RaW" << std::endl;
                tmp = PolyhedricDependency::ComputeRaW<SgFunctionDeclaration, SgExprStatement, RoseVariable>(polyhedral_model, false);
                analysis.dependencies->insert(analysis.dependencies->end(), tmp->begin(), tmp->end());
                delete tmp;
		std::cerr << "WaW" << std::endl;
                tmp = PolyhedricDependency::ComputeWaW<SgFunctionDeclaration, SgExprStatement, RoseVariable>(polyhedral_model, false);
                analysis.dependencies->insert(analysis.dependencies->end(), tmp->begin(), tmp->end());
                delete tmp;
		std::cerr << "WaR" << std::endl;
                tmp = PolyhedricDependency::ComputeWaR<SgFunctionDeclaration, SgExprStatement, RoseVariable>(polyhedral_model, false);
                analysis.dependencies->insert(analysis.dependencies->end(), tmp->begin(), tmp->end());
                delete tmp;
              }
            }
          }
          catch (Exception::ExceptionBase & e) {
            e.print(std::cerr);
            analysis.polyhedral_analysis_success = false;
          }
          if (!analysis.polyhedral_analysis_success) {
             std::cerr << "Polyhedral modeling failed: Fine communication optimizations and accurate loop dependencies disable..." << std::endl;
          }
          else  std::cerr << "Success of Polyhedral modeling" << std::endl;
        }
#endif /* APPLY_POLY_ANALYSIS */

        // Parameters analysis
        {
          std::vector<SgInitializedName *> params = func_decl->get_parameterList()->get_args();
          std::vector<SgInitializedName *>::iterator it;
          for (it = params.begin(); it != params.end(); it++) {
            findVariableType(*it, analysis);
          }
          std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(func_body);
          findVariableAccessType(var_refs, analysis);
        }

   // *****************************************************
   // * Start the actual C loops to OpenCL transformation *
   // *****************************************************
#define APPLY_OCL_TRANSFORM
#ifdef APPLY_OCL_TRANSFORM
        SageBuilder::pushScopeStack(func_body);

        SgStatement * previous = NULL;
        {
          std::vector<SgPragmaDeclaration *> pragmas = SageInterface::querySubTree<SgPragmaDeclaration>(func_body);
          std::vector<SgPragmaDeclaration *>::iterator it;
          for (it = pragmas.begin(); it != pragmas.end(); it++) {
            if ((*it)->get_pragma()->get_pragma() == "scop")
              previous = SageInterface::getPreviousStatement(*it);
          }
        }
        ROSE_ASSERT(previous != NULL);

     // Insert common OpenCL initialization (devices, programs, ...)
        createOclInit(&previous, ocl_file_name, analysis);

     // Scan the function's loops to generate kernel objects
     // It also transforms the loop nest by placing the kernel calls
        std::vector<Kernel *> generated_kernel;
        {
          SgStatement * first = NULL;             
          SgStatement * last = NULL;
          std::vector<SgStatement *>::iterator it;
          bool proceed = false;
          for (it = func_body->get_statements().begin(); it != func_body->get_statements().end(); it++) {
            if (isSgPragmaDeclaration(*it))
              if (isSgPragmaDeclaration(*it)->get_pragma()->get_pragma() == "scop") {
                proceed = true;
                continue;
              }
              else if (isSgPragmaDeclaration(*it)->get_pragma()->get_pragma() == "endscop") {
                proceed = false;
                break;
              }
            if (proceed) {
              if (first == NULL) first = *it;
              last = *it;
            }
          }
          GenerateKernel(first, last, dim_out, dim_used, generated_kernel, analysis);
        }

      // Insert kernels initialization
        std::vector<Kernel *>::iterator it;
        for (it = generated_kernel.begin(); it != generated_kernel.end(); it++)
          (*it)->createKernel(&previous, ocl_file);

      // Insert OpenCL release code
        SgStatement * last = *(func_body->get_statements().end() - 1);
        createOclRelease(&last, analysis);

        SageBuilder::popScopeStack();
#endif
    }

    project->unparse();

    return 0;
}

