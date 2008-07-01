// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprLabeler.C,v 1.9 2008-07-01 09:45:25 gergo Exp $

#include "ExprLabeler.h"

ExprLabeler::ExprLabeler(int expnum_, CFG *cfg, Procedure *proc)
  : expnum(expnum_), cfg(cfg), proc(proc)
{
}

int ExprLabeler::get_expnum() const
{
    return expnum;
}

void ExprLabeler::visit(SgNode *node)
{
    if (isSgThisExp(node))
    {
        SgThisExp *t = isSgThisExp(node);
        RetvalAttribute *retval = new RetvalAttribute(proc->this_sym);
        node->addNewAttribute("return variable", retval);
    }
    else if (isSgFunctionCallExp(node))
    {
        SgFunctionCallExp *call = isSgFunctionCallExp(node);
        std::string *name = find_func_name(call);
        std::stringstream varname;
        varname << "$tmpvar$" << (name != NULL ? *name : "unknown_func")
            << "$return_" << expnum++;
        delete name;
        SgVariableSymbol *varsym
            = Ir::createVariableSymbol(varname.str(),
                                       cfg->global_unknown_type);
     // GB (2008-07-01): This is supposed to be the *unique* symbol for some
     // call site, not the global symbol!
     // SgVariableSymbol *varsym = cfg->global_return_variable_symbol;
        RetvalAttribute *retval = new RetvalAttribute(varsym);
        node->addNewAttribute("return variable", retval);
    }
    else if (isSgConstructorInitializer(node))
    {
        if (isSgNewExp(node->get_parent()))
        {
            SgNewExp *n = isSgNewExp(node->get_parent());
            SgType *type = n->get_type();
         // GB (2008-03-17): Strip pointers, this might include several
         // layers. I know there is a ROSE method for this, but I don't
         // trust it. The more interesting question is whether the variable
         // name can be considered correct if there is more than one layer
         // of pointers. TODO: Investigate this.
         // while (isSgPointerType(type))
         //     type = isSgPointerType(type)->get_base_type();
         // GB (2008-04-29): We really only want to peel off one layer of
         // arrays and after that possibly one layer of pointers. So if we
         // have:
         // - array of class: generate constructor call
         // - array of pointer to whatever: do not generate constructor call
         // - pointer to class: generate constructor call
         // - pointer to something else: do not generate constructor call
         // I hope that's correct.
            if (isSgArrayType(type))
                type = isSgArrayType(type)->get_base_type();
            if (isSgPointerType(type))
                type = isSgPointerType(type)->get_base_type();
#if 0
            SgName name = isSgNamedType(isSgPointerType(n->get_type())
                    ->get_base_type())->get_name();
#endif
         // SgName name = isSgNamedType(type)->get_name();
            SgNamedType *namedType = isSgNamedType(type);
            if (!namedType)
            {
#if 0
             // This is not really an error, regardless of what the message
             // says. ROSE generates constructor initializer nodes for basic
             // types, but we do not want to generate constructor calls for
             // them.
                std::cerr << __FILE__ << ":" << __LINE__ << ": "
                    << "error in 'new' expression: expected a named type, but got "
                    << type->class_name() << " (" << Ir::fragmentToString(type)
                    << ") instead!" << std::endl;
                Sg_File_Info *where = n->get_file_info();
                std::cerr << "offending expression: "
                    << where->get_filenameString() << ":" << where->get_line()
                    << ":" << where->get_col() << ": " << Ir::fragmentToString(n)
                    << std::endl;
#endif
            }
            else
            {
#if 0
                std::string name = namedType->get_name().str();
                std::stringstream varname;
             // varname << "$" << name << "$this";
                RetvalAttribute *retval = new RetvalAttribute(varname.str());
#endif
                SgVariableSymbol *varsym = cfg->global_this_variable_symbol;
                RetvalAttribute *retval = new RetvalAttribute(varsym);
                node->addNewAttribute("return variable", retval);
            }
        }
        else if (!isSgInitializedName(node->get_parent()))
        {
            std::stringstream varname;
            varname << "$tmpvar$anonymous_var_" << expnum++;
            SgVariableSymbol *varsym
                = Ir::createVariableSymbol(varname.str(),
                                           cfg->global_unknown_type);
            RetvalAttribute *retval = new RetvalAttribute(varsym);
            node->addNewAttribute("anonymous variable", retval);
        }
    }
    else if (isSgAndOp(node) || isSgOrOp(node) || isSgConditionalExp(node))
    {
        std::stringstream varname;
        varname << "$tmpvar$logical_" << expnum++;
        SgVariableSymbol *varsym
            = Ir::createVariableSymbol(varname.str(),
                                       cfg->global_unknown_type);
        RetvalAttribute *retval = new RetvalAttribute(varsym);
        node->addNewAttribute("logical variable", retval);
    }
}
