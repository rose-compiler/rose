
#include "rose-kernel/PolyhedralKernel.hpp"
#include "polygraph.hpp"

void usage(char * name) {
  std::cout << "usage: " << name << " input.c output.poly" << std::endl;
}

void help(char * name) {
  usage(name);
  std::cout << std::endl;
  std::cout << "This application extract the PolyGraph associated to the SCoP contains in a kernel." << std::endl;
  std::cout << "Kernel function name need to have the prefix \"kernel_\" and contains pragma \"scop\" and \"endscop\"" << std::endl;
  std::cout << std::endl;
  std::cout << "Author: Tristan Vanderbruggen (vanderbruggentristan@gmail.com)" << std::endl;
}

int main(int argc, char ** argv) {

    if (argc != 3) {
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

    std::ofstream out_poly;
    out_poly.open(argv[2]);
    if (!out_poly.is_open()) {
      usage(argv[0]);
      return 1;
    }

  // Scan all functions
    std::vector<SgNode*> func_decls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
    std::vector<SgNode*>::iterator it_;
    for (it_ = func_decls.begin(); it_ != func_decls.end(); it_++) {
      SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_);

      try {
        if (!PolyhedricAnnotation::parse(func_decl, "kernel_"))
          continue;
      }
      catch (Exception::ExceptionBase & e) {
        e.print(std::cerr);
        continue;
      }

      PolyhedricAnnotation::FunctionPolyhedralProgram & polyhedral_model = 
          PolyhedricAnnotation::getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(func_decl);

      if (polyhedral_model.getNumberOfGlobals() > 0) {
        std::cerr << "Find a polyhedral modeling but it is parametric..." << std::endl;
        exit(-1);
      }
      else {
        PolyGraph * polygraph = new PolyGraph(polyhedral_model, std::string(argv[1]));
        polygraph->print(out_poly);

        std::ofstream out_dot;
        std::string dot_file = std::string(argv[1]) + ".poly.dot";
        out_dot.open(dot_file.c_str());
        if (!out_dot.is_open()) {
          usage(argv[0]);
          return 1;
        }
        polygraph->toDot(out_dot);
        out_dot.close();

        delete polygraph;
      }
    }

    out_poly.close();

    return 0;
}

