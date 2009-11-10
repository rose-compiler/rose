// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ProcTraversal.C,v 1.22 2008-10-21 13:40:40 gergo Exp $

#include <iostream>
#include <string.h>

#include "AstNumberOfNodesStatistics.h"
#include "ProcTraversal.h"
#include "IrCreation.h"
#include "ExprLabeler.h"

#include <satire.h>

ProcTraversal::ProcTraversal(SATIrE::Program *program)
  : procedures(new std::deque<Procedure *>()),
    node_id(0), 
    procnum(0), 
    original_ast_nodes(0), 
    original_ast_statements(0),
    timer(NULL),
    program(program)
{
  setPrintCollectedFunctionNames(false);
// GB (2008-06-25): Some ROSE nodes have tight checks on what types they may
// be associated with, especially concerning pointer base types. Let's hope
// we get away with using void **.
  global_unknown_type
      = Ir::createPointerType(Ir::createPointerType(SgTypeVoid::createType()));
  global_return_variable_symbol
      = Ir::createVariableSymbol("$tmpvar$retvar", global_unknown_type);
  program->global_map["$tmpvar$retvar"]
      = std::make_pair(global_return_variable_symbol,
                       global_return_variable_symbol->get_declaration());
  global_this_variable_symbol
      = Ir::createVariableSymbol("$tmpvar$this", global_unknown_type);
  program->global_map["$tmpvar$this"]
      = std::make_pair(global_this_variable_symbol,
                       global_this_variable_symbol->get_declaration());
}

std::deque<Procedure *>*
ProcTraversal::get_procedures() const {
    return procedures;
}

void 
ProcTraversal::setPrintCollectedFunctionNames(bool pcf) {
  _printCollectedFunctionNames=pcf;
}

bool 
ProcTraversal::getPrintCollectedFunctionNames() {
  return _printCollectedFunctionNames;
}

void
ProcTraversal::atTraversalStart() {
    timer = new TimingPerformance("Collection of procedures:");
}

void
ProcTraversal::atTraversalEnd() {
    if (timer != NULL)
        delete timer;
    timer = NULL;
}

void 
ProcTraversal::visit(SgNode *node) {
  if (isSgFunctionDeclaration(node)) {
    SgFunctionDeclaration *decl = isSgFunctionDeclaration(node);
    if (decl->get_definition() != NULL) {
      /* collect statistics */
      //AstNumberOfNodesStatistics anons;
      //anons.traverse(decl, postorder);
      //original_ast_nodes += anons.get_numberofnodes();
      //original_ast_statements += anons.get_numberofstatements();
            
      /* do the real work */
      Procedure *proc = new Procedure();
      proc->procnum = procnum++;
      proc->decl = decl;
      proc->funcsym
          = isSgFunctionSymbol(decl->get_symbol_from_symbol_table());
      if (proc->funcsym == NULL)
      {
#if 0
          std::cout
              << std::endl
              << "*** NULL function symbol for declaration "
                << decl->unparseToString()
              << std::endl
              << "symbol: "
                << (void *) decl->get_symbol_from_symbol_table()
                << (decl->get_symbol_from_symbol_table() != NULL ?
                        decl->get_symbol_from_symbol_table()->class_name()
                      : "")
              << std::endl
              << "first nondef decl: "
                << (void *) decl->get_firstNondefiningDeclaration()
                << " sym: "
                << (decl->get_firstNondefiningDeclaration() != NULL ?
                        (void *) decl->get_firstNondefiningDeclaration()
                                      ->get_symbol_from_symbol_table()
                      : (void *) NULL)
              << std::endl;
#endif
          if (decl->get_firstNondefiningDeclaration() != NULL)
          {
              proc->funcsym = isSgFunctionSymbol(decl
                      ->get_firstNondefiningDeclaration()
                      ->get_symbol_from_symbol_table());
          }
      }
      assert(proc->funcsym != NULL);
   // GB (2008-05-14): We need two parameter lists: One for the names of the
   // variables inside the function definition, which is
   // decl->get_parameterList(), and one that contains any default arguments
   // the function might have. The default arguments are supposedly
   // associated with the first nondefining declaration.
      proc->params = decl->get_parameterList();
      SgDeclarationStatement *fndstmt = decl->get_firstNondefiningDeclaration();
      SgFunctionDeclaration *fnd = isSgFunctionDeclaration(fndstmt);
      if (fnd != NULL && fnd != decl)
          proc->default_params = fnd->get_parameterList();
      else
          proc->default_params = proc->params;

      SgMemberFunctionDeclaration *mdecl
        = isSgMemberFunctionDeclaration(decl);
      if (mdecl) {
        proc->class_type = isSgClassDefinition(mdecl->get_scope());
        std::string name = proc->class_type->get_mangled_name().str();
        name += "::"; name += decl->get_name().str();
        std::string mname = proc->class_type->get_mangled_name().str();
        mname += "::"; mname += decl->get_mangled_name().str();
        proc->memberf_name = name;
        proc->mangled_memberf_name = mname;
        proc->name = decl->get_name().str();
        proc->mangled_name = decl->get_mangled_name().str();
     // GB (2008-05-26): Computing a single this symbol for each
     // procedure. Thus, this symbol can also be compared by pointer
     // equality (as is the case for all other symbols). While we're at it,
     // we also build a VarRefExp for this which can be used everywhere the
     // this pointer occurs.
        proc->this_type = Ir::createPointerType(
                proc->class_type->get_declaration()->get_type());
        proc->this_sym = Ir::createVariableSymbol("this", proc->this_type);
        proc->this_exp = Ir::createVarRefExp(proc->this_sym);
      } else {
        proc->name = decl->get_name().str();
        proc->mangled_name = decl->get_mangled_name().str();
        proc->class_type = NULL;
        proc->memberf_name = proc->mangled_memberf_name = "";
        proc->this_type = NULL;
        proc->this_sym = NULL;
        proc->this_exp = NULL;
     // GB (2008-07-01): Better resolution of calls to static functions.
     // This only makes sense for non-member functions.
        SgStorageModifier &sm =
            (fnd != NULL ? fnd : decl)->get_declarationModifier().get_storageModifier();
        proc->isStatic = sm.isStatic();
     // Note that we query the first nondefining declaration for the
     // static modifier, but we save the file of the *defining*
     // declaration. This is because the first declaration might be in
     // some header file, but for call resolution, the actual source
     // file with the definition is relevant.
     // Trace back to the enclosing file node. The definition might be
     // included in foo.c from bar.c, in which case the Sg_File_Info
     // would refer to bar.c; but for function call resolution, foo.c is
     // the relevant file.
        SgNode *p = decl->get_parent();
        while (p != NULL && !isSgFile(p))
            p = p->get_parent();
        proc->containingFile = isSgFile(p);
      }
      proc_map.insert(std::make_pair(proc->name, proc));
      mangled_proc_map.insert(std::make_pair(proc->mangled_name, proc));
      std::vector<SgVariableSymbol* >* arglist
        = new std::vector<SgVariableSymbol* >();
      SgVariableSymbol *this_var = NULL, *this_temp_var = NULL;
      if (mdecl 
          || decl->get_parameterList() != NULL
          && !decl->get_parameterList()->get_args().empty()) {
        proc->arg_block
          = new BasicBlock(node_id, INNER, proc->procnum);
        if (mdecl) {
       // GB (2008-05-26): We now compute the this pointer right at the
       // beginning of building the procedure.
       // this_var = Ir::createVariableSymbol("this", this_type);
          this_var = proc->this_sym;
       // std::string varname
       //   = std::string("$") + proc->name + "$this";
       // this_temp_var = Ir::createVariableSymbol(varname, proc->this_type);
          this_temp_var = global_this_variable_symbol;
          ParamAssignment* paramAssignment
            = Ir::createParamAssignment(this_var, this_temp_var);
          proc->arg_block->statements.push_back(paramAssignment);
          arglist->push_back(this_var);
          if (proc->name.find('~') != std::string::npos) {
            arglist->push_back(this_temp_var);
          }
        }
        SgInitializedNamePtrList params
          = proc->params->get_args();
        SgInitializedNamePtrList::const_iterator i;
#if 0
        int parnum = 0;
        for (i = params.begin(); i != params.end(); ++i) {
          SgVariableSymbol *i_var = Ir::createVariableSymbol(*i);
          std::stringstream varname;
       // varname << "$" << proc->name << "$arg_" << parnum++;
          SgVariableSymbol* var =
            Ir::createVariableSymbol(varname.str(),(*i)->get_type());
          proc->arg_block->statements.push_back(Ir::createParamAssignment(i_var, var));
          arglist->push_back(i_var);
        }
#else
     // GB (2008-06-23): Trying to replace all procedure-specific argument
     // variables by a global list of argument variables. This means that at
     // this point, we do not necessarily need to build a complete list but
     // only add to the CFG's argument list if it is not long enough.
        size_t func_params = params.size();
        size_t global_args = global_argument_variable_symbols.size();
        std::stringstream varname;
        while (global_args < func_params)
        {
            varname.str("");
            varname << "$tmpvar$arg_" << global_args++;
            SgVariableSymbol *var
                = Ir::createVariableSymbol(varname.str(),
                        global_unknown_type);
            program->global_map[varname.str()]
                = std::make_pair(var, var->get_declaration());
            global_argument_variable_symbols.push_back(var);
        }
     // now create the param assignments
        size_t j = 0;
        for (i = params.begin(); i != params.end(); ++i)
        {
            SgVariableSymbol *i_var = Ir::createVariableSymbol(params[j]);
            SgVariableSymbol *var = global_argument_variable_symbols[j];
            j++;
            proc->arg_block->statements.push_back(
                    Ir::createParamAssignment(i_var, var));
            arglist->push_back(i_var);
        }
#if 0
     // replace the arglist allocated above by the new one; this must be
     // fixed for this pointers!
        delete arglist;
        arglist = &global_argument_variable_symbols;
#endif
#endif
      } else {
        proc->arg_block = NULL;
      }
      /* If this is a constructor, call default constructors
       * of all base classes. If base class constructors are
       * called manually, these calls will be removed later. */
      if (mdecl
          && strcmp(mdecl->get_name().str(), 
                    proc->class_type->get_declaration()->get_name().str()) == 0
          && proc->class_type != NULL) {
        SgBaseClassPtrList::iterator base;
        for (base = proc->class_type->get_inheritances().begin();
             base != proc->class_type->get_inheritances().end();
             ++base) {
          SgClassDeclaration* baseclass = (*base)->get_base_class();
          SgVariableSymbol *lhs
            = Ir::createVariableSymbol("$tmpvar$" + baseclass->get_name(),
                                       baseclass->get_type());
          program->global_map["$tmpvar$" + baseclass->get_name()]
              = std::make_pair(lhs, lhs->get_declaration());
          SgMemberFunctionDeclaration* fd=get_default_constructor(baseclass);
          assert(fd);
          SgType* basetype=baseclass->get_type();
          assert(basetype);
          SgConstructorInitializer *sci
            = Ir::createConstructorInitializer(fd,basetype);
          ArgumentAssignment* a
            = Ir::createArgumentAssignment(lhs, sci);
          proc->arg_block->statements.push_back(a);

       // std::string this_called_varname
       //   = std::string("$") + baseclass->get_name() + "$this";
          SgVariableSymbol *this_called_var
         // = Ir::createVariableSymbol(this_called_varname,
         //                            baseclass->get_type());
            = global_this_variable_symbol;
          ReturnAssignment* this_ass
            = Ir::createReturnAssignment(this_var, this_called_var);
          proc->arg_block->statements.push_back(this_ass);
        }
      }
      if (mdecl && mdecl->get_CtorInitializerList() != NULL
          && !mdecl->get_CtorInitializerList()->get_ctors().empty()) {
        SgInitializedNamePtrList cis
          = mdecl->get_CtorInitializerList()->get_ctors();
        SgInitializedNamePtrList::const_iterator i;
        if (proc->arg_block == NULL) {
          proc->arg_block = new BasicBlock(node_id, INNER, proc->procnum);
        }
        for (i = cis.begin(); i != cis.end(); ++i) {
          SgVariableSymbol* lhs = Ir::createVariableSymbol(*i);
          SgAssignInitializer *ai
            = isSgAssignInitializer((*i)->get_initializer());
          SgConstructorInitializer *ci
            = isSgConstructorInitializer((*i)->get_initializer());
          /* TODO: other types of initializers */
          if (ai) {
            SgClassDeclaration *class_decl
              = proc->class_type->get_declaration();
         // GB (2008-05-26): We now compute the this pointer right at the
         // beginning of building the procedure.
         // SgVarRefExp* this_ref 
         //   = Ir::createVarRefExp("this",
         //                         Ir::createPointerType(class_decl->get_type()));
            SgVarRefExp* this_ref = proc->this_exp;
            SgArrowExp* arrowExp
              = Ir::createArrowExp(this_ref,Ir::createVarRefExp(lhs));
         // GB (2008-03-17): We need to handle function calls in
         // initializers. In order to be able to build an argument
         // assignment, we need to know the function's return variable, so
         // the expression labeler must be called on it. The expression
         // number is irrelevant, however, as it does not appear in the
         // return variable.
            if (isSgFunctionCallExp(ai->get_operand_i())) {
#if 0
                ExprLabeler el(0 /*expnum*/);
                el.traverse(ai->get_operand_i(), preorder);
             // expnum = el.get_expnum();
#endif
             // GB (2008-06-25): There is now a single global return
             // variable. This may or may not mean that we can simply ignore
             // the code above. I don't quite understand why this labeling
             // couldn't be done later on, and where its result was used.
            }
            ArgumentAssignment* argumentAssignment 
              = Ir::createArgumentAssignment(arrowExp,ai->get_operand_i());
            proc->arg_block->statements.push_back(argumentAssignment);
          } else if (ci) {
            /* if this is a call to a base class's
             * constructor, remove the call we generated
             * before */
            SgStatement* this_a = NULL;
            SgClassDeclaration* cd = ci->get_class_decl();
            std::deque<SgStatement *>::iterator i;
            for (i = proc->arg_block->statements.begin();
                 i != proc->arg_block->statements.end();
                 ++i) {
              ArgumentAssignment* a
                = dynamic_cast<ArgumentAssignment *>(*i);
              if (a && isSgConstructorInitializer(a->get_rhs())) {
                SgConstructorInitializer* c
                  = isSgConstructorInitializer(a->get_rhs());
                std::string c_decl_name = c->get_class_decl()->get_name().str();
                std::string cd_name = cd->get_name().str();
             // if (c->get_class_decl()->get_name() == cd->get_name()) {
                if (c_decl_name == cd_name) {
#if 0
                  // erase the following assignment
                  // of the this pointer as well
                  this_a = *proc->arg_block->statements.erase(i+1);
                  proc->arg_block->statements.erase(i);
#endif
               // GB (2008-03-28): That's an interesting piece of code, but
               // it might be very mean to iterators. At least it is hard to
               // see whether it is correct. So let's try it like this:
               // erase i; we get an iterator back, which refers to the next
               // element. Save that element as this_a, and then erase.
                  std::deque<SgStatement *>::iterator this_pos;
                  this_pos = proc->arg_block->statements.erase(i);
                  this_a = *this_pos;
                  proc->arg_block->statements.erase(this_pos);
               // Good. Looks like this fixed a very obscure bug.
                  break;
                }
              }
            }
            /* now add the initialization */
            proc->arg_block->statements.push_back(Ir::createArgumentAssignment(lhs, ci));
            if (this_a != NULL)
              proc->arg_block->statements.push_back(this_a);
          }
        }
      }
      proc->entry = new CallBlock(node_id++, START, proc->procnum,
                                  new std::vector<SgVariableSymbol *>(*arglist),
                                  (proc->memberf_name != ""
                                    ? proc->memberf_name
                                    : proc->name));
      proc->exit = new CallBlock(node_id++, END, proc->procnum,
                                 new std::vector<SgVariableSymbol *>(*arglist),
                                  (proc->memberf_name != ""
                                    ? proc->memberf_name
                                    : proc->name));
      proc->entry->partner = proc->exit;
      proc->exit->partner = proc->entry;
      proc->entry->call_target = Ir::createFunctionRefExp(proc->funcsym);
      proc->exit->call_target = Ir::createFunctionRefExp(proc->funcsym);
      /* In constructors, insert an assignment $A$this = this
       * at the end to make sure that the 'this' pointer can be
       * passed back to the calling function uncobbled. */
      proc->this_assignment = NULL;
      if (mdecl) {
        SgMemberFunctionDeclaration* cmdecl
          = isSgMemberFunctionDeclaration(mdecl->get_firstNondefiningDeclaration());
     // if (cmdecl && cmdecl->get_specialFunctionModifier().isConstructor()) {
          proc->this_assignment
            = new BasicBlock(node_id++, INNER, proc->procnum);
          ReturnAssignment* returnAssignment 
            = Ir::createReturnAssignment(this_temp_var, this_var);
          proc->this_assignment->statements.push_back(returnAssignment);
          add_link(proc->this_assignment, proc->exit, NORMAL_EDGE);
     // }
      }
      std::stringstream varname;
   // varname << "$" << proc->name << "$return";
   // proc->returnvar = Ir::createVariableSymbol(varname.str(),
   //                                            decl->get_type()->get_return_type());
      proc->returnvar = global_return_variable_symbol;
      procedures->push_back(proc);
      if(getPrintCollectedFunctionNames()) {
        std::cout << (proc->memberf_name != ""
                        ? proc->memberf_name
                        : proc->name)
                  << " " /*<< proc->decl << std::endl*/;
      }
      if (proc->arg_block != NULL)
      {
          proc->arg_block->call_target
              = Ir::createFunctionRefExp(proc->funcsym);
      }
   // delete arglist;
    }
  }
}

SgMemberFunctionDeclaration*
get_default_constructor(SgClassDeclaration *c) {
  SgClassDefinition *d = c->get_definition();
  if (d != NULL) {
    SgDeclarationStatementPtrList::iterator i;
    for (i = d->get_members().begin(); i != d->get_members().end(); ++i) {
      if (SgMemberFunctionDeclaration* m=isSgMemberFunctionDeclaration(*i)) {
        if (m->get_name() == c->get_name() && m->get_args().empty())
          return m;
      }
    }
  }
  return Ir::createMemberFunctionDeclaration(c->get_name());
}
