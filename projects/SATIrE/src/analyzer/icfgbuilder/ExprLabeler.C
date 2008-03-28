// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprLabeler.C,v 1.5 2008-03-28 15:55:32 gergo Exp $

#include "ExprLabeler.h"

ExprLabeler::ExprLabeler(int expnum_) : expnum(expnum_)
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
        RetvalAttribute *retval = new RetvalAttribute("this");
        node->addNewAttribute("return variable", retval);
    }
    else if (isSgFunctionCallExp(node))
    {
        SgFunctionCallExp *call = isSgFunctionCallExp(node);
        std::string *name = find_func_name(call);
        std::stringstream varname;
        varname << "$" << (name != NULL ? *name : "unknown_func")
            << "$return_" << expnum++;
        delete name;
        RetvalAttribute *retval = new RetvalAttribute(varname.str());
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
            while (isSgPointerType(type))
                type = isSgPointerType(type)->get_base_type();
#if 0
            SgName name = isSgNamedType(isSgPointerType(n->get_type())
                    ->get_base_type())->get_name();
#endif
         // SgName name = isSgNamedType(type)->get_name();
            std::string name = isSgNamedType(type)->get_name().str();
            std::stringstream varname;
            varname << "$" << name << "$this";
            RetvalAttribute *retval = new RetvalAttribute(varname.str());
            node->addNewAttribute("return variable", retval);
        }
        else if (!isSgInitializedName(node->get_parent()))
        {
            std::stringstream varname;
            varname << "$anonymous_var_" << expnum++;
            RetvalAttribute *retval = new RetvalAttribute(varname.str());
            node->addNewAttribute("anonymous variable", retval);
        }
    }
    else if (isSgAndOp(node) || isSgOrOp(node) || isSgConditionalExp(node))
    {
        std::stringstream varname;
        varname << "$logical_" << expnum++;
        RetvalAttribute *retval = new RetvalAttribute(varname.str());
        node->addNewAttribute("logical variable", retval);
    }
}
