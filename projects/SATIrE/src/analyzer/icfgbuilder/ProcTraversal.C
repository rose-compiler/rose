// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ProcTraversal.C,v 1.3 2007-03-08 15:36:48 markus Exp $

#include <iostream>
#include <string.h>

#include "AstNumberOfNodesStatistics.h"
#include "ProcTraversal.h"

ProcTraversal::ProcTraversal() : procedures(new std::deque<Procedure *>()),
    node_id(0), procnum(0), original_ast_nodes(0), original_ast_statements(0)
{
}

std::deque<Procedure *> *ProcTraversal::get_procedures() const
{
    return procedures;
}

void ProcTraversal::visit(SgNode *node)
{
    if (isSgFunctionDeclaration(node))
    {
        SgFunctionDeclaration *decl = isSgFunctionDeclaration(node);
        if (decl->get_definition() != NULL)
        {
            /* collect statistics */
            AstNumberOfNodesStatistics anons;
            anons.traverse(decl, postorder);
            original_ast_nodes += anons.get_numberofnodes();
            original_ast_statements += anons.get_numberofstatements();
            
            /* do the real work */
            Procedure *proc = new Procedure();
            proc->procnum = procnum++;
            proc->params = decl->get_parameterList();
            proc->decl = decl;
            SgMemberFunctionDeclaration *mdecl
                = isSgMemberFunctionDeclaration(decl);
            if (mdecl)
            {
                proc->class_type = isSgClassDefinition(mdecl->get_scope());
                std::string name = proc->class_type->get_mangled_name().str();
                name += "::"; name += decl->get_name().str();
                std::string mname = proc->class_type->get_mangled_name().str();
                mname += "::"; mname += decl->get_mangled_name().str();
                proc->memberf_name = strdup(name.c_str());
                proc->mangled_memberf_name = strdup(mname.c_str());
                proc->name = strdup(decl->get_name().str());
                proc->mangled_name = strdup(decl->get_mangled_name().str());
            }
            else
            {
                proc->name = strdup(decl->get_name().str());
                proc->mangled_name = strdup(decl->get_mangled_name().str());
                proc->class_type = NULL;
                proc->memberf_name = proc->mangled_memberf_name = NULL;
            }
            std::list<SgVariableSymbol *> *arglist
                = new std::list<SgVariableSymbol *>();
            SgVariableSymbol *this_var = NULL, *this_temp_var = NULL;
            if (mdecl || decl->get_parameterList() != NULL
                    && !decl->get_parameterList()->get_args().empty())
            {
                proc->arg_block
                    = new BasicBlock(node_id, INNER, proc->procnum);
                if (mdecl)
                {
                    SgType *this_type = new SgPointerType(proc
                            ->class_type->get_declaration()->get_type());
                    this_var = new SgVariableSymbol(
                            new SgInitializedName("this", this_type));
                    std::string varname
                        = std::string("$") + proc->name + "$this";
                    this_temp_var = new SgVariableSymbol(
                            new SgInitializedName(varname, this_type));
                    proc->arg_block->statements.push_back(
                            new ParamAssignment(this_var, this_temp_var));
                    arglist->push_back(this_var);
                    if (strchr(proc->name, '~') != NULL)
                        arglist->push_back(this_temp_var);
                }
                SgInitializedNamePtrList params
                    = decl->get_parameterList()->get_args();
                SgInitializedNamePtrList::const_iterator i;
                int parnum = 0;
                for (i = params.begin(); i != params.end(); ++i)
                {
                    SgVariableSymbol *i_var = new SgVariableSymbol(*i);
                    std::stringstream varname;
                    varname << "$" << proc->name << "$arg_" << parnum++;
                    SgVariableSymbol *var = new SgVariableSymbol(
                            new SgInitializedName(varname.str(),
                                (*i)->get_type()));
                    proc->arg_block->statements.push_back(
                            new ParamAssignment(i_var, var));
                    arglist->push_back(i_var);
                }
            }
            else
                proc->arg_block = NULL;
            /* If this is a constructor, call default constructors
             * of all base classes. If base class constructors are
             * called manually, these calls will be removed later. */
            if (mdecl
                    && strcmp(mdecl->get_name().str(),
                        proc->class_type->get_declaration()->get_name().str())
                        == 0
                    && proc->class_type != NULL)
            {
                std::list<SgBaseClass *>::iterator base;
                for (base = proc->class_type->get_inheritances().begin();
                     base != proc->class_type->get_inheritances().end();
                     ++base)
                {
                    SgClassDeclaration *baseclass = (*base)->get_base_class();
                    SgVariableSymbol *lhs
                        = new SgVariableSymbol(new SgInitializedName(
                                baseclass->get_name(),
                                baseclass->get_type()));
                    SgConstructorInitializer *sci
                        = new SgConstructorInitializer(
                            new Sg_File_Info(),
                            get_default_constructor(baseclass),
                            new SgExprListExp(new Sg_File_Info()),
                            baseclass);
                    ArgumentAssignment *a
                        = new ArgumentAssignment(lhs, sci);
                    proc->arg_block->statements.push_back(a);

                    std::string this_called_varname
                        = std::string("$") + baseclass->get_name() + "$this";
                    SgVariableSymbol *this_called_var
                        = new SgVariableSymbol(new SgInitializedName(
                                    this_called_varname,
                                    new SgPointerType(baseclass->get_type())));
                    ReturnAssignment *this_ass
                        = new ReturnAssignment(this_var, this_called_var);
                    proc->arg_block->statements.push_back(this_ass);
                }
            }
            if (mdecl && mdecl->get_CtorInitializerList() != NULL
                    && !mdecl->get_CtorInitializerList()->get_ctors().empty())
            {
                SgInitializedNamePtrList cis
                    = mdecl->get_CtorInitializerList()->get_ctors();
                SgInitializedNamePtrList::const_iterator i;
                if (proc->arg_block == NULL)
                    proc->arg_block
                        = new BasicBlock(node_id, INNER, proc->procnum);
                for (i = cis.begin(); i != cis.end(); ++i)
                {
                    SgVariableSymbol *lhs = new SgVariableSymbol(*i);
                    SgAssignInitializer *ai
                        = isSgAssignInitializer((*i)->get_initializer());
                    SgConstructorInitializer *ci
                        = isSgConstructorInitializer((*i)->get_initializer());
                    /* TODO: other types of initializers */
                    if (ai)
                    {
                        SgClassDeclaration *class_decl
                            = proc->class_type->get_declaration();
                        SgVarRefExp *this_ref =
                            new SgVarRefExp(new Sg_File_Info(),
                                    new SgVariableSymbol(
                                        new SgInitializedName(SgName("this"),
                                            new SgPointerType(
                                                class_decl->get_type()))));
                        proc->arg_block->statements.push_back(
                                new ArgumentAssignment(
                                new SgArrowExp(new Sg_File_Info(),
                                    this_ref,
                                    new SgVarRefExp(new Sg_File_Info(), lhs)),
                                    ai->get_operand_i()));
                    }
                    else if (ci)
                    {
                        /* if this is a call to a base class's
                         * constructor, remove the call we generated
                         * before */
                        SgStatement *this_a = NULL;
                        SgClassDeclaration *cd = ci->get_class_decl();
                        std::deque<SgStatement *>::iterator i;
                        for (i = proc->arg_block->statements.begin();
                             i != proc->arg_block->statements.end();
                             ++i)
                        {
                            ArgumentAssignment *a
                                = dynamic_cast<ArgumentAssignment *>(*i);
                            if (a && isSgConstructorInitializer(a->get_rhs()))
                            {
                                SgConstructorInitializer *c
                                    = isSgConstructorInitializer(a->get_rhs());
                                if (c->get_class_decl()->get_name()
                                        == cd->get_name())
                                {
                                    // erase the following assignment
                                    // of the this pointer as well
                                    this_a = *proc->arg_block
                                        ->statements.erase(i+1);
                                    proc->arg_block->statements.erase(i);
                                    break;
                                }
                            }
                        }
                        /* now add the initialization */
                        proc->arg_block->statements.push_back(
                                new ArgumentAssignment(lhs, ci));
                        if (this_a != NULL)
                            proc->arg_block->statements.push_back(this_a);
                    }
                }
            }
            proc->entry = new CallBlock(node_id++, START, proc->procnum,
                    arglist,
                    strdup(proc->memberf_name
                        ? proc->memberf_name : proc->name));
            proc->exit = new CallBlock(node_id++, END, proc->procnum,
                    arglist,
                    strdup(proc->memberf_name
                        ? proc->memberf_name : proc->name));
            proc->entry->partner = proc->exit;
            proc->exit->partner = proc->entry;
            /* In constructors, insert an assignment $A$this = this
             * at the end to make sure the this pointer can be
             * passed back to the calling function uncobbled. */
            proc->this_assignment = NULL;
            if (mdecl)
            {
                SgMemberFunctionDeclaration *cmdecl
                    = isSgMemberFunctionDeclaration(
                            mdecl->get_firstNondefiningDeclaration());
                if (cmdecl && cmdecl->get_specialFunctionModifier()
                        .isConstructor())
                {
                    proc->this_assignment
                        = new BasicBlock(node_id++, INNER, proc->procnum);
                    proc->this_assignment->statements.push_back(
                            new ReturnAssignment(this_temp_var, this_var));
                    add_link(proc->this_assignment, proc->exit, NORMAL_EDGE);
                }
            }
            std::stringstream varname;
            varname << "$" << proc->name << "$return";
            proc->returnvar = new SgVariableSymbol(
                    new SgInitializedName(SgName(varname.str()),
                        decl->get_type()->get_return_type()));

            procedures->push_back(proc);
            std::cout << (proc->memberf_name ? proc->memberf_name : proc->name)
                << " " /*<< proc->decl << std::endl*/;
        }
    }
}

SgMemberFunctionDeclaration *get_default_constructor(
        SgClassDeclaration *c)
{
    SgClassDefinition *d = c->get_definition();
    if (d != NULL)
    {
        std::list<SgDeclarationStatement *>::iterator i;
        for (i = d->get_members().begin(); i != d->get_members().end(); ++i)
        {
            if (isSgMemberFunctionDeclaration(*i))
            {
                SgMemberFunctionDeclaration *m
                    = isSgMemberFunctionDeclaration(*i);
                if (m->get_name() == c->get_name() && m->get_args().empty())
                    return m;
            }
        }
    }
    return new SgMemberFunctionDeclaration(new Sg_File_Info(), c->get_name());
}
