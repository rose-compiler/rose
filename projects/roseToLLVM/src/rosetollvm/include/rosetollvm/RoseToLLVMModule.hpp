#ifndef ROSETOLLVM_ROSETOLLVMMODULE_HPP
#define ROSETOLLVM_ROSETOLLVMMODULE_HPP

#include <rose.h>
#include <pacecc/PaceccModule.hpp>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>
#include <rosetollvm/CodeAttributesVisitor.h>
#include <rosetollvm/CodeGeneratorVisitor.h>

class RoseToLLVMModule : public PaceccModule {

private:
    Option *options;
    Control *control;

public:

    RoseToLLVMModule(Rose_STL_Container<std::string> &args) :  PaceccModule(Option::roseToLLVMModulePrefix), 
                                                               options(NULL)
    {
        handleOptions(args);
        control = new Control(*options);
    }

    ~RoseToLLVMModule() {
        delete options;
        delete control;
    }

    int visit(SgProject *project);
    int queryVisit(SgProject *project, SgFunctionDeclaration *);

    void handleModuleOptions(Rose_STL_Container<string> &args);
};


#endif /* ROSETOLLVM_ROSETOLLVMMODULE_HPP */
