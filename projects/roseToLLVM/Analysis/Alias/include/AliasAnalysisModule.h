#ifndef _ALIASANALYSISMODULE_H
#define _ALIASANALYSISMODULE_H

#include <rose.h>
#include <pacecc/PaceccModule.hpp>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>
#include <rosetollvm/CodeAttributesVisitor.h>
#include <rosetollvm/CodeGeneratorVisitor.h>
#include <llvm/Module.h>


class AliasAnalysisModule: public PaceccModule
{
    private:
        Option *options;
        Control *control;


    public:
        AliasAnalysisModule(Rose_STL_Container<std::string> &args): PaceccModule(Option::roseToLLVMModulePrefix)
                                                                    , options(NULL)
        {     
            handleOptions(args);
            control = new Control(*options);
        }

        ~AliasAnalysisModule()
        {
            delete options;
            delete control;   // free memory
        }

        int visit(SgProject *project);
        llvm::Module* getModule(int index);
        int getLLVMModuleSize();
        int annotateAST(SgProject *project);

        virtual void handleModuleOptions(Rose_STL_Container<string> &args);
};

#endif
