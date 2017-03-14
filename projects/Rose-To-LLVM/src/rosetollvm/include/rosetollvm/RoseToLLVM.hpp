#ifndef ROSETOLLVM_ROSETOLLVM_HPP
#define ROSETOLLVM_ROSETOLLVM_HPP

#include <rose.h>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>
#include <rosetollvm/CodeAttributesVisitor.h>
#include <rosetollvm/CodeGeneratorVisitor.h>
#include "llvm/IR/Module.h"

#include <boost/program_options.hpp>

class RoseToLLVM {

private:
    Option *options;
    Control *control;

    void astVisit(SgProject *);
    llvm::Module *translate(SgFunctionDeclaration *, bool);

public:

    /**
     *
     */
    RoseToLLVM(Rose_STL_Container<std::string> &args)
      : options(new Option(args)), control(new Control(*options))
    {
    }

    ~RoseToLLVM() {
        delete options;
        delete control;
    }

    static SgFunctionDeclaration *findEnclosingFunction(SgNode *);
    static SgFile *findEnclosingFile(SgNode *);
    static SgProject *findEnclosingProject(SgNode *);

    llvm::Module *translate(SgForStatement *);
    llvm::Module *translate(SgFunctionDeclaration *function_decl) {
      return translate(function_decl, true);
    }
    std::vector<llvm::Module *> translate(SgProject *);

    /// Prints the accepted command line options to standard error.
    // static void printOptions();
    static void addOptionsToDescription(
            boost::program_options::options_description& desc);

    static const std::string kTranslateExternals;
};


#endif /* ROSETOLLVM_ROSETOLLVM_HPP */
