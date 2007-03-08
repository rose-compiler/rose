// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprLabeler.C,v 1.2 2007-03-08 15:36:48 markus Exp $

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
        SgName name = find_func_name(call);
        std::stringstream varname;
        varname << "$" << name.str() << "$return_" << expnum++;
        RetvalAttribute *retval = new RetvalAttribute(varname.str());
        node->addNewAttribute("return variable", retval);
    }
    else if (isSgConstructorInitializer(node))
    {
        if (isSgNewExp(node->get_parent()))
        {
            SgNewExp *n = isSgNewExp(node->get_parent());
            SgName name = isSgNamedType(isSgPointerType(n->get_type())
                    ->get_base_type())->get_name();
            std::stringstream varname;
            varname << "$" << name.str() << "$this";
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
