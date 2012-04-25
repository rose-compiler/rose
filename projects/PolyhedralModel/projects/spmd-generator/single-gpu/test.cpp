
#include "polydriver/polyhedral-driver.hpp"
#include "single-gpu/depth-placement.hpp"
#include "single-gpu/single-gpu.hpp"
#include "opencl-generator/opencl-generator.hpp"

void usage(char * name) {
  std::cout << "usage: " << name << " input.c" << std::endl;
}

void help(char * name) {
  usage(name);
  std::cout << "This application copy the SCoP contains in a kernel." << std::endl;
  std::cout << "Kernel function name need to have the prefix \"kernel_\" and contains pragma \"scop\" and \"endscop\"" << std::endl;
  std::cout << std::endl;
  std::cout << "Author: Tristan Vanderbruggen (vanderbruggentristan@gmail.com)" << std::endl;
}

int main(int argc, char ** argv) {

  if (argc != 2) {
    if (argc == 2) {
      if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        help(argv[0]);
      else
        usage(argv[0]);
    }
    else
      usage(argv[0]);
    return 1;
  }

  char * args[3] = {argv[0], (char *)"-DPARAMETRIC", argv[1]};
  SgProject * project = frontend ( 3 , args );

  PolyDriver poly_driver(new DepthPlacement(new SingleGPU_SingleCPU()));
  OpenCL_Generator ocl_generator(poly_driver);

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

    SageBuilder::pushScopeStack(func_body);

    SgStatement * insert_init_after = NULL;
    {
      std::vector<SgPragmaDeclaration *> pragmas = SageInterface::querySubTree<SgPragmaDeclaration>(func_body);
      std::vector<SgPragmaDeclaration *>::iterator it;
      for (it = pragmas.begin(); it != pragmas.end(); it++) {
        if ((*it)->get_pragma()->get_pragma() == "scop")
          insert_init_after = SageInterface::getPreviousStatement(*it);
      }
    }
    ROSE_ASSERT(insert_init_after != NULL);

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

    SgStatement * insert_final_after = *(func_body->get_statements().end() - 1);

    SgStatement * res = ocl_generator.generate(insert_init_after, insert_final_after, first, last, NULL);

    SgStatement * current = first;
    while (current != last) {
      SgStatement * next = SageInterface::getNextStatement(current);
      SageInterface::removeStatement(current);
      current = next;
    }
    SageInterface::replaceStatement(last, res);

    SageBuilder::popScopeStack();

  }

  project->unparse();

  return 0;
}

