// -*- mode: c++; c-basic-offset: 4; -*-
// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprTransformer.C,v 1.10 2008-03-26 14:57:53 gergo Exp $

#include "rose.h"
#include "patternRewrite.h"

#include "ExprTransformer.h"
#include "IrCreation.h"

ExprTransformer::ExprTransformer(int node_id_, int procnum_, int expnum_,
                 CFG *cfg_, BasicBlock *after_)
  : node_id(node_id_), procnum(procnum_), expnum(expnum_), cfg(cfg_),
    after(after_), retval(after_), root_var(NULL)
{
}

int 
ExprTransformer::get_node_id() const {
  return node_id;
}

int 
ExprTransformer::get_expnum() const {
    return expnum;
}

BasicBlock*
ExprTransformer::get_after() const {
    return after;
}

BasicBlock*
ExprTransformer::get_last() const {
    return last;
}

BasicBlock* 
ExprTransformer::get_retval() const {
    return retval;
}

SgVariableSymbol*
ExprTransformer::get_root_var() const {
    return root_var;
}

void ExprTransformer::visit(SgNode *node)
{
    /*
     * This traverses the expression bottom-up (i.e., postorder) and
     * successively replaces function call and logical expressions
     * by appropriate sequences of statements.
     * Calls are unfolded as required by PAG, logical expressions
     * are taken apart to simulate the control flow forced by the
     * short-circuit operators &&, ||, and ?: (the comma operator
     * should also be implemented some day).
     */
 // GB (2008-03-10): Replaced all calls to replaceChild by
 // satireReplaceChild. This function handles SgValueExps correctly.
    if (SgThisExp* thisExp=isSgThisExp(node))
    {
      SgVarRefExp* varRefExp=Ir::createVarRefExp("this",thisExp->get_type());
      satireReplaceChild(node->get_parent(), node, varRefExp);
    }
    else if (isSgFunctionCallExp(node))
    {
        SgFunctionCallExp *call = isSgFunctionCallExp(node);
        std::string *name = find_func_name(call);
        const std::vector<CallBlock *> *entries = find_entries(call);
        SgExpressionPtrList elist;
        if (name == NULL)
            name = new std::string("unknown_func");

        if (entries != NULL && !entries->empty()) {
          Procedure *p = (*cfg->procedures)[entries->front()->procnum];
          SgInitializedNamePtrList params = p->params->get_args();
          SgExpressionPtrList &alist
            = call->get_args()->get_expressions();
          SgInitializedNamePtrList::const_iterator ni = params.begin();
          SgExpressionPtrList::const_iterator ei;
          /* if this is a member function, put the this pointer as
           * first argument; this does not appear explicitly
           * anywhere */
          if (find_called_memberfunc(call->get_function())) {
         // GB (2008-03-05): calling_object_address returns the expression
         // that we assign to the "this" pointer. We pass a null pointer
         // constant, that is, a valid SgExpression that represents a null
         // pointer, if the member function is static.
         // calling_object_address should never return NULL since every
         // member function call is either static or associated with an
         // object the member function is invoked on.
            SgExpression *e = calling_object_address(call->get_function());
            if (e != NULL) {
              elist.push_back(e);
            } else {
              std::cout << __FILE__ << ":" << __LINE__
                  << ": unknown object in member function call:" << std::endl
                  << dumpTreeFragmentToString(call)
                  << std::endl
                  << "'" << Ir::fragmentToString(call) << "'" << std::endl;
              exit(EXIT_FAILURE);
            }
          }
          for (ei = alist.begin() ; ei != alist.end(); ++ei) {
            elist.push_back(*ei);
            if (ni != params.end())
                ++ni;
          }
          while (ni != params.end()) {
            if (*ni != NULL)
            {
                SgInitializedName *initname = *ni;
                elist.push_back(isSgAssignInitializer(initname->get_initptr())
                    ->get_operand_i());
            }
            ++ni;
          }
        } else {
          /* evaluate args for external functions */
          SgExpressionPtrList &alist
            = call->get_args()->get_expressions();
          SgExpressionPtrList::const_iterator ei;
          for (ei = alist.begin(); ei != alist.end(); ++ei)
            elist.push_back(*ei);
        }
        /*
         * create:
         * 1. blocks for argument assignments
         * 2. a call block
         * 3. a return block
         * 4. a block for return value assignment
         */
        BasicBlock *first_arg_block = NULL, *last_arg_block = NULL;
        if (!elist.empty())
        {
            int i;
            BasicBlock *prev = NULL;
            for (i = 0; i < elist.size(); i++)
            {
                BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
                cfg->nodes.push_back(b);
                if (first_arg_block == NULL)
                    first_arg_block = b;
                if (prev != NULL)
                    add_link(prev, b, NORMAL_EDGE);
                prev = b;
            }
            last_arg_block = prev;
        }
        BasicBlock *retval_block;
        /* FIXME: if no retval_block is set, links are wrong */
        if (true || !isSgTypeVoid(call->get_type()))
        {
            retval_block = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(retval_block);
        }
        else
            retval_block = NULL;
        CallBlock *call_block = NULL, *return_block = NULL;
        ExternalCall *external_call = NULL;
        if (entries != NULL && !entries->empty())
        {
            call_block = new CallBlock(node_id++, CALL, procnum,
                    new std::vector<SgVariableSymbol *>()
                    /*entries->front()->paramlist*/, name->c_str());
            return_block = new CallBlock(node_id++, RETURN,
                    procnum, new std::vector<SgVariableSymbol *>()
                    /*entries->front()->paramlist*/,
                    name->c_str());
            cfg->nodes.push_back(call_block);
            cfg->calls.push_back(call_block);
            cfg->nodes.push_back(return_block);
            cfg->returns.push_back(return_block);
            call_block->partner = return_block;
            return_block->partner = call_block;

            /* set links */
            std::vector<CallBlock *> *exits = new std::vector<CallBlock *>();
            std::vector<CallBlock *>::const_iterator i;
            for (i = entries->begin(); i != entries->end(); ++i)
                exits->push_back((*i)->partner);
            if (last_arg_block != NULL)
                add_link(last_arg_block, call_block, NORMAL_EDGE);
            for (i = entries->begin(); i != entries->end(); ++i)
                add_link(call_block, *i, CALL_EDGE);
            add_link(call_block, return_block, LOCAL);
            for (i = exits->begin(); i != exits->end(); ++i)
                add_link(*i, return_block, RETURN_EDGE);
            if (retval_block != NULL)
            {
                add_link(return_block, retval_block, NORMAL_EDGE);
                add_link(retval_block, after, NORMAL_EDGE);
                retval = retval_block;
            }
            else
            {
                add_link(return_block, after, NORMAL_EDGE);
                retval = return_block;
            }
            after = call_block;
            last = retval;
        }
        else
        {
      /* external call */
            BasicBlock *call_block
                = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(call_block);
         // GB (2007-10-23): This now records the expression referring to
         // the called function and the parameter list. Because the
         // parameter list has not been computed yet, pass an empty dummy
         // list for now.
            external_call = 
                    Ir::createExternalCall(call->get_function(),
                                           new std::vector<SgVariableSymbol *>,
                                           call->get_type());
            call_block->statements.push_front(external_call);

            /* set links */
            if (last_arg_block != NULL)
                add_link(last_arg_block, call_block, NORMAL_EDGE);
            if (retval_block != NULL)
            {
                add_link(call_block, retval_block, NORMAL_EDGE);
                add_link(retval_block, after, NORMAL_EDGE);
                retval = retval_block;
            }
            else
            {
                add_link(call_block, after, NORMAL_EDGE);
                retval = call_block;
            }
            after = call_block;
            last = retval;
        }
        /* fill blocks */
        if (first_arg_block != NULL)
        {
         // GB (2008-03-26): Generate the "$this" parameter of the function
         // only if this is a non-external member function call.
            bool generateThisParam
                = !external_call && find_called_memberfunc(call->get_function());
            std::vector<SgVariableSymbol *> *params =
                evaluate_arguments(*name, elist, first_arg_block,
                    generateThisParam);
         // Set the parameter list for whatever kind of CFG nodes we
         // computed above.
            if (call_block != NULL)
            {
                call_block->paramlist = params;
                call_block->stmt->update_infolabel();
            }
            if (return_block != NULL)
            {
                return_block->paramlist = params;
                return_block->stmt->update_infolabel();
            }
            if (external_call != NULL)
            {
                external_call->set_params(params);
            }
        }
        /* replace call by its result */
        if (retval_block != NULL)
            assign_retval(*name, call, retval_block);
        if (first_arg_block != NULL)
            after = first_arg_block;
        expnum++;
    }
    else if (isSgConstructorInitializer(node)) {
      SgConstructorInitializer* ci = isSgConstructorInitializer(node);

      SgClassDefinition* class_type = (ci->get_class_decl()
                       ? ci->get_class_decl()->get_definition() : NULL);
      std::vector<CallBlock *> blocks(0);
      SgName name = "";
      SgName mangled_name = "";
      if (ci->get_declaration() != NULL) {
        name = ci->get_declaration()->get_name();
        mangled_name = ci->get_declaration()->get_mangled_name();
        /* find constructor implementations */
        int num = 0;
        std::deque<Procedure *>::const_iterator i;
     // GB (2008-03-13): Default constructors can be called without
     // parentheses, in which case the mangled name of the call is different
     // from the mangled name of the constructor that is called. We thus
     // make a distinction between default and non-default constructors. For
     // non-default constructors, comparing the mangled name is fine; for
     // default constructors, we look at the non-mangled name and the number
     // of args.
     // All of this should be handled by some much smarter logic based an
     // defining declarations or something.
        if (ci->get_declaration()->get_args().empty()) {
            for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
              if (strcmp(Ir::getConstCharPtr(name), (*i)->name) == 0
                  && (*i)->decl->get_args().empty()) {
                blocks.push_back((*cfg->procedures)[num]->entry);
              }
              num++;
            }
        } else {
            for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
              if (strcmp(Ir::getConstCharPtr(mangled_name),
                         (*i)->mangled_name) == 0) {
                blocks.push_back((*cfg->procedures)[num]->entry);
              }
              num++;
            }
        }
     // GB (2008-03-13): Overloading must be uniquely resolvable.
        if (blocks.size() > 1) {
            std::cout << __FILE__ << ":" << __LINE__
                << ": error during ICFG construction: ";
            std::cout << "found more than one "
                << "constructor implementation for initializer '"
                << Ir::fragmentToString(ci) << "'" << std::endl;
            std::cout << "procedures:";
            std::vector<CallBlock *>::iterator block;
            for (block = blocks.begin(); block != blocks.end(); ++block) {
                std::cout << " " << (*block)->procnum;
            }
            std::cout << std::endl;
            std::exit(EXIT_FAILURE);
        }
      }
   // GB (2008-03-25): Oops, we missed setting the type for implicit
   // constructor calls (copy constructors).
      if (name == "" && isSgNamedType(ci->get_type())) {
          SgNamedType *t = isSgNamedType(ci->get_type());
          name = t->get_name();
      }
    
      /* setup argument expressions */
      SgExpressionPtrList elist;
      Procedure *p = NULL;
      SgInitializedNamePtrList params;
      SgExpressionPtrList& alist = ci->get_args()->get_expressions();
      SgInitializedNamePtrList::const_iterator ni;
      SgExpressionPtrList::const_iterator ei;
      if (!blocks.empty()) {
        p = (*cfg->procedures)[blocks.front()->procnum];
        params = p->params->get_args();
        ni = params.begin();
      }
      if (SgInitializedName* initializedName=isSgInitializedName(ci->get_parent())) {
        /* some member is initialized, pass the address
         * of the object as this pointer */
        //initializedName->set_file_info(FILEINFO);
        SgVarRefExp *ref = Ir::createVarRefExp(initializedName);
        SgAddressOfOp* a = Ir::createAddressOfOp(ref,Ir::createPointerType(ref->get_type()));
        elist.push_back(a);
      } else if (SgNewExp* newExp0=isSgNewExp(ci->get_parent())) {
        SgType *t = newExp0->get_type();
        if (isSgPointerType(t))
          t = isSgPointerType(t)->get_base_type();
        if (isSgNamedType(t))
          name = isSgNamedType(t)->get_name();
    
        elist.push_back(newExp0);
      
        RetvalAttribute *ra = (RetvalAttribute *) ci->getAttribute("return variable");
    
        SgVariableSymbol *var = Ir::createVariableSymbol(ra->get_str(),newExp0->get_type());
    
        if (isSgExpression(newExp0->get_parent())) {
          SgVarRefExp* varRefExp=Ir::createVarRefExp(var);
          satireReplaceChild(newExp0->get_parent(),newExp0,varRefExp);
          } else if (root_var == NULL) {
            root_var = var;
          }
      } else {
        RetvalAttribute* ra = (RetvalAttribute *) ci->getAttribute("anonymous variable");
        SgVarRefExp* ref = Ir::createVarRefExp(ra->get_str(), ci->get_type());
        elist.push_back(Ir::createAddressOfOp(ref, Ir::createPointerType(ref->get_type())));
      }
      if (!blocks.empty() && params.size() < alist.size()) {
          std::cout << __FILE__ << ":" << __LINE__
              << ": error during ICFG construction: "
              << "constructor has more arguments than parameters!"
              << std::endl;
#if 0
          std::cout
              << "function call: "
              << Ir::fragmentToString(ci->get_parent()) << std::endl
              << "               "
              << dumpTreeFragmentToString(ci->get_parent()) << std::endl;
          std::cout
              << "candicate function: "
              << (void *) (p) << std::endl
           // << " "<< Ir::fragmentToString(p->decl) << std::endl
              ;
          std::cout << "params: [";
          SgInitializedNamePtrList::iterator pi;
          for (pi = params.begin(); pi != params.end(); ++pi) {
              std::cout << Ir::fragmentToString(*pi);
              if (pi + 1 != params.end())
                  std::cout << ", ";
          }
          std::cout << "]" << std::endl;
          std::cout << "alist:  [";
          SgExpressionPtrList::iterator ai;
          for (ai = alist.begin(); ai != alist.end(); ++ai) {
              std::cout << Ir::fragmentToString(*ai);
              if (ai + 1 != alist.end())
                  std::cout << ", ";
          }
          std::cout << "]" << std::endl;
#endif
          exit(EXIT_FAILURE);
      }
      for (ei = alist.begin(); ei != alist.end(); ++ei) {
        elist.push_back(*ei);
        if (!blocks.empty() && ni != params.end())
          ++ni;
      }
      if (!blocks.empty()) {
        while (ni != params.end()) {
          if (*ni != NULL)
          {
              if (isSgAssignInitializer(*ni))
                  elist.push_back(isSgAssignInitializer((*ni)->get_initptr())
                      ->get_operand_i());
#if 0
           // GB (2008-03-12): This cannot be right. It adds the parameter
           // name (i.e. the name of a variable internal to the called
           // function) to the list of function call arguments.
              else if (isSgInitializedName(*ni))
                  elist.push_back(Ir::createVarRefExp(isSgInitializedName(*ni)));
#endif
              else
              {
                  std::cout
                      << __FILE__ << ":" << __LINE__ << ": error:"
                      << "*ni of type: " << (*ni)->class_name() << std::endl;
                  exit(EXIT_FAILURE);
              }
          }
          ++ni;
        }
      }
      BasicBlock *first_arg_block = NULL, *last_arg_block = NULL;
      if (!elist.empty()) {
        int i;
        BasicBlock *prev = NULL;
        for (i = 0; i < elist.size(); i++) {
          BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
          cfg->nodes.push_back(b);
          if (first_arg_block == NULL)
            first_arg_block = b;
          if (prev != NULL)
            add_link(prev, b, NORMAL_EDGE);
          prev = b;
        }
        last_arg_block = prev;
      }
      /* FIXME: is this correct? */
      BasicBlock *retval_block = NULL;
      CallBlock *call_block = NULL, *return_block = NULL;
      if (!blocks.empty()) {
        call_block = new CallBlock(node_id++, CALL, procnum,
                   blocks.front()->paramlist, name.str());
        return_block = new CallBlock(node_id++, RETURN,
                     procnum, blocks.front()->paramlist, name.str());
        cfg->nodes.push_back(call_block);
        cfg->calls.push_back(call_block);
        cfg->nodes.push_back(return_block);
        cfg->returns.push_back(return_block);
        call_block->partner = return_block;
        return_block->partner = call_block;

        /* set links */
        std::vector<CallBlock *> *exits = new std::vector<CallBlock *>();
        std::vector<CallBlock *>::const_iterator i;
        for (i = blocks.begin(); i != blocks.end(); ++i)
          exits->push_back((*i)->partner);
        if (last_arg_block != NULL)
          add_link(last_arg_block, call_block, NORMAL_EDGE);
        for (i = blocks.begin(); i != blocks.end(); ++i)
          add_link(call_block, *i, CALL_EDGE);
        add_link(call_block, return_block, LOCAL);
        for (i = exits->begin(); i != exits->end(); ++i)
          add_link(*i, return_block, RETURN_EDGE);
        if (retval_block != NULL) {
          add_link(return_block, retval_block, NORMAL_EDGE);
          add_link(retval_block, after, NORMAL_EDGE);
          retval = retval_block;
        } else {
          add_link(return_block, after, NORMAL_EDGE);
          retval = return_block;
        }
        after = call_block;
        last = retval;
      } else {
        /* call to external constructor */
        BasicBlock *call_block
          = new BasicBlock(node_id++, INNER, procnum);
        cfg->nodes.push_back(call_block);
        call_block->statements.push_front(Ir::createConstructorCall(name.str(), ci->get_type()));

        /* set links */
        if (last_arg_block != NULL)
          add_link(last_arg_block, call_block, NORMAL_EDGE);
        if (retval_block != NULL) {
          add_link(call_block, retval_block, NORMAL_EDGE);
          add_link(retval_block, after, NORMAL_EDGE);
          retval = retval_block;
        } else {
          add_link(call_block, after, NORMAL_EDGE);
          retval = call_block;
        }
        after = call_block;
        last = retval;
      }
      /* fill blocks */
      if (first_arg_block != NULL) {
        std::vector<SgVariableSymbol *> *params =
          evaluate_arguments(name, elist, first_arg_block, true);
        if (call_block != NULL) {
          call_block->paramlist = params;
          call_block->stmt->update_infolabel();
        }
     // GB (2008-03-12): We had forgotten to update the list of variables in
     // the return block.
        if (return_block != NULL) {
       // GB (2008-03-13): If this is a constructor call that resulted from
       // application of the 'new' operator, we need to remove the $A$this
       // pointer from the list of variables in the ReturnStmt. The reason
       // is that this pointer is used one more time in the subsequent
       // statement; two occurrences break the general rule of exactly one
       // use of temporary variables.
          if (isSgNewExp(ci->get_parent())) {
              return_block->paramlist
                  = new std::vector<SgVariableSymbol *>(
                          params->begin() + 1, params->end());
          } else {
              return_block->paramlist = params;
          }
          return_block->stmt->update_infolabel();
        }
      }
      /* replace call by its result */
      // if (retval_block != NULL)
      //     assign_retval(name, call, retval_block);
      if (first_arg_block != NULL)
        after = first_arg_block;
      expnum++;
    }
    else if (isSgDeleteExp(node)) {
      SgDeleteExp *de = isSgDeleteExp(node);
      SgPointerType *type = isSgPointerType(de->get_variable()->get_type());
   // GB (2008-03-17): Added a check for more than one layer of pointers.
   // We shouldn't generate destructor calls for those.
      if (!de->get_is_array() && type && !isSgPointerType(type->get_base_type())) {
#if 0
        SgType *delete_type = isSgPointerType(de->get_variable()
                          ->get_type())->get_base_type();
        while (isSgTypedefType(delete_type))
          delete_type = isSgTypedefType(delete_type)->get_base_type();
#endif
        SgType *delete_type = type->findBaseType();
        SgClassType *ct = isSgClassType(delete_type);
        std::string class_name(ct->get_name().str());
        // std::string destructor_name = class_name + "::~" + class_name;
        // std::string this_var_name
        //    = std::string() + "$~" + class_name + "$this";
        const std::vector<CallBlock *> *d_entries
          = find_destructor_entries(ct);
        std::vector<std::string> *d_class_names = find_destructor_names(ct);
        std::vector<std::string> *d_this_names
          = find_destructor_this_names(ct);
        std::vector<std::string>::iterator destr_name
          = d_class_names->begin();
        std::vector<std::string>::iterator this_name
          = d_this_names->begin();
        if (d_entries != NULL && !d_entries->empty()) {
          std::vector<BasicBlock *> afters, lasts;
          std::vector<CallBlock *>::const_iterator d;
          for (d = d_entries->begin(); d != d_entries->end(); ++d) {
            SgVariableSymbol *this_var_sym
              = Ir::createVariableSymbol(*this_name++, de->get_variable()->get_type());

            std::string destructor_name = *destr_name++;
            CallBlock *call_block = new CallBlock(node_id++, CALL,
                          procnum, NULL, strdup(destructor_name.c_str()));
            CallBlock *return_block = new CallBlock(node_id++, RETURN,
                            procnum, NULL, strdup(destructor_name.c_str()));
            cfg->nodes.push_back(call_block);
            cfg->calls.push_back(call_block);
            cfg->nodes.push_back(return_block);
            cfg->returns.push_back(return_block);
            call_block->partner = return_block;
            return_block->partner = call_block;
            BasicBlock *this_block
              = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(this_block);
            this_block->statements.push_back(Ir::createArgumentAssignment(Ir::createVarRefExp(this_var_sym),
                                    de->get_variable()));
            call_block->paramlist = new std::vector<SgVariableSymbol *>();
            call_block->paramlist->push_back(this_var_sym);
            return_block->paramlist = call_block->paramlist;

            /* set links */
            add_link(this_block, call_block, NORMAL_EDGE);

            add_link(call_block, *d, CALL_EDGE);
            add_link((*d)->partner, return_block, RETURN_EDGE);
        
            add_link(call_block, return_block, LOCAL);
            // add_link(return_block, after, NORMAL_EDGE);
            afters.push_back(this_block);
            lasts.push_back(return_block);
            // after = this_block;
                    // last = return_block;
          }
          // after: kill this_vars
          BasicBlock* kill_this_vars = new BasicBlock(node_id++, INNER, procnum);
          cfg->nodes.push_back(kill_this_vars);
          std::vector<SgVariableSymbol *>* this_syms = new std::vector<SgVariableSymbol *>();
          std::vector<std::string>::iterator dtn;
          for (dtn = d_this_names->begin(); dtn != d_this_names->end();
               ++dtn) {
            this_syms->push_back(Ir::createVariableSymbol(*dtn,Ir::createClassType()));
          }
          kill_this_vars->statements.push_back(Ir::createUndeclareStmt(this_syms));
          add_link(kill_this_vars, after, NORMAL_EDGE);
          after = kill_this_vars;
          // set links
          if (afters.size() > 1) {
            BasicBlock *afterblock
              = new BasicBlock(node_id++, INNER, procnum);
            cfg->nodes.push_back(afterblock);
            afterblock->statements.push_back(Ir::createNullStatement());
            std::vector<BasicBlock *>::iterator ai, li;
            ai = afters.begin();
            li = lasts.begin();
            while (ai != afters.end() && li != lasts.end()) {
              add_link(afterblock, *ai++, NORMAL_EDGE);
              add_link(*li++, after, NORMAL_EDGE);
            }
            after = afterblock;
          } else {
            add_link(*lasts.begin(), after, NORMAL_EDGE);
            after = *afters.begin();
          }
        } else {
          /* No destructor found. Generating an external call
           * (there might be a destructor whose implementation
           * is not accessible to us). */
          BasicBlock *b = new BasicBlock(node_id++, INNER, procnum);
          cfg->nodes.push_back(b);
          b->statements.push_front(Ir::createDestructorCall(strdup(class_name.c_str()), ct));
          add_link(b, after, NORMAL_EDGE);
          after = b;
          last = b;
        }
      }
    } else if (isSgAndOp(node) || isSgOrOp(node)) {
      SgBinaryOp *logical_op = isSgBinaryOp(node);
      RetvalAttribute *varnameattr
        = (RetvalAttribute *) logical_op->getAttribute("logical variable");
      SgVariableSymbol *var = Ir::createVariableSymbol(varnameattr->get_str(),
                               logical_op->get_type());
      
      BasicBlock *if_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(if_block);
      if (logical_op->get_lhs_operand_i()->attributeExists("logical variable"))
        {
      RetvalAttribute* vna = (RetvalAttribute *) logical_op->get_lhs_operand_i()->getAttribute("logical variable");
      SgVarRefExp* varexp = Ir::createVarRefExp(vna->get_str(),logical_op->get_lhs_operand_i()->get_type());
      LogicalIf* logicalIf=Ir::createLogicalIf(varexp);
      if_block->statements.push_front(logicalIf);
        } else {
      LogicalIf* logicalIf= Ir::createLogicalIf(logical_op->get_lhs_operand_i());
      if_block->statements.push_front(logicalIf);
    }

      BasicBlock *t_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(t_block);
      BasicBlock *f_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(f_block);
   // GB (2007-10-23): Create blocks for a nested branch to evaluate the rhs
   // operand. These blocks are linked below; they are successors of the
   // true or the false branch depending on whether the operator is && or ||.
      BasicBlock *nested_t_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(nested_t_block);
      nested_t_block->statements.push_front(Ir::createExprStatement(Ir::createAssignOp(Ir::createVarRefExp(var),
                                                                    Ir::createBoolValExp(true))));
      BasicBlock *nested_f_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(nested_f_block);
      nested_f_block->statements.push_front(Ir::createExprStatement(Ir::createAssignOp(Ir::createVarRefExp(var),
                                                                    Ir::createBoolValExp(false))));

   // GB (2007-10-23): Compute the rhs operand of the logical operation.
   // This is the expression in the AST or a reference to the logical
   // variable associated with that expression, if any.
      SgExpression *rhs_operand = logical_op->get_rhs_operand_i();
      if (rhs_operand->attributeExists("logical variable"))
      {
      RetvalAttribute* vna = (RetvalAttribute *) rhs_operand->getAttribute("logical variable");
          rhs_operand = Ir::createVarRefExp(vna->get_str(), rhs_operand->get_type());
      }
      
      if(isSgAndOp(logical_op)) {
        t_block->statements.push_front(Ir::createLogicalIf(rhs_operand));
        add_link(t_block, nested_t_block, TRUE_EDGE);
        add_link(t_block, nested_f_block, FALSE_EDGE);
        
    SgAssignOp* assignOp2=Ir::createAssignOp(Ir::createVarRefExp(var),Ir::createBoolValExp(false));
    f_block->statements.push_front(Ir::createExprStatement(assignOp2));
        add_link(f_block, after, NORMAL_EDGE);
      } else if(isSgOrOp(logical_op)) {
    t_block->statements.push_front(Ir::createExprStatement(Ir::createAssignOp(Ir::createVarRefExp(var),
                                          Ir::createBoolValExp(true))));
        add_link(t_block, after, NORMAL_EDGE);

        f_block->statements.push_front(Ir::createLogicalIf(rhs_operand));
        add_link(f_block, nested_t_block, TRUE_EDGE);
        add_link(f_block, nested_f_block, FALSE_EDGE);
      } else {
    assert(false); // impossible if outer 'if' remains unchanged
      }
      
      add_link(if_block, t_block, TRUE_EDGE);
      add_link(if_block, f_block, FALSE_EDGE);
      add_link(nested_t_block, after, NORMAL_EDGE);
      add_link(nested_f_block, after, NORMAL_EDGE);
      after = if_block;
      
      if (isSgExpression(logical_op->get_parent())) {
        satireReplaceChild(logical_op->get_parent(), logical_op,
                           Ir::createVarRefExp(var));
      }
      else if (root_var == NULL) {
        root_var = var;
      }
      expnum++;
    } else if (isSgConditionalExp(node)) {
      SgConditionalExp* cond = isSgConditionalExp(node);
      RetvalAttribute* varnameattr
        = (RetvalAttribute *) cond->getAttribute("logical variable");
      SgVariableSymbol* var = Ir::createVariableSymbol(varnameattr->get_str(),
                                                       cond->get_type());
      BasicBlock *if_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(if_block);
      if_block->statements.push_front(Ir::createLogicalIf(
                  cond->get_conditional_exp()));
      BasicBlock *t_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(t_block);
      t_block->statements.push_front(Ir::createExprStatement(
        Ir::createAssignOp(Ir::createVarRefExp(var),cond->get_true_exp())));
      BasicBlock *f_block = new BasicBlock(node_id++, INNER, procnum);
      cfg->nodes.push_back(f_block);
      f_block->statements.push_front(Ir::createExprStatement(
        Ir::createAssignOp(Ir::createVarRefExp(var),cond->get_false_exp())));
      
      add_link(if_block, t_block, TRUE_EDGE);
      add_link(if_block, f_block, FALSE_EDGE);
      add_link(t_block, after, NORMAL_EDGE);
      add_link(f_block, after, NORMAL_EDGE);
      after = if_block;
      
      if (isSgExpression(cond->get_parent())) {
        satireReplaceChild(cond->get_parent(), cond, Ir::createVarRefExp(var));
      }
      else if (root_var == NULL) {
        root_var = var;
      }
      expnum++;
    }
}

SgName 
ExprTransformer::find_mangled_func_name(SgFunctionRefExp *fr) const {
  // fr->get_symbol()->get_declaration()->get_mangled_name();  
  SgDeclarationStatement* declaration= fr->get_symbol()->get_declaration();
  ROSE_ASSERT(declaration != NULL);
  SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
  ROSE_ASSERT(functionDeclaration != NULL);
  //cannot use following code because the pointer is NULL
  //SgFunctionDeclaration* nondefiningFunctionDeclaration
  //  = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
  SgName mname=functionDeclaration->get_mangled_name();
  //std::cerr<<mname.str()<<std::endl;
  return mname;
}

//SgName 
//ExprTransformer::find_mangled_memberf_name(SgMemberFunctionRefExp *mfr) const {
//  return mfr->get_symbol()->get_declaration()->get_mangled_name();
//}

CallBlock* 
ExprTransformer::find_entry(SgFunctionCallExp *call)
{
    /* ATTENTION: this is deprecated, do not use */
    std::cerr << "warning: "
        << "call to deprecated function ExprTransformer::find_entry"
        << std::endl;

    SgFunctionRefExp *func_ref = isSgFunctionRefExp(call->get_function());
    SgMemberFunctionRefExp *member_func_ref
        = isSgMemberFunctionRefExp(call->get_function());

    if (func_ref)
    {
        std::string *sgname = find_func_name(call);
        const char *name = (sgname != NULL ? sgname->c_str() : "unknown_func");
        int num = 0;
        std::deque<Procedure *>::const_iterator i;

        for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i)
        {
            if (strcmp(name, (*i)->name) == 0)
                return (*cfg->procedures)[num]->entry;
            num++;
        }
        return NULL;
    }

    return NULL;
} 

const std::vector<CallBlock *>*
ExprTransformer::find_entries(SgFunctionCallExp *call)
{
    std::vector<CallBlock *> *blocks = new std::vector<CallBlock *>();
    SgFunctionRefExp* func_ref = find_called_func(call->get_function());
    SgMemberFunctionRefExp* member_func_ref = find_called_memberfunc(call->get_function());
    //std::cout << "FunctionCall: FunctionRefExp:" << func_ref << ", MemberFunctionRef:" << member_func_ref << std::endl;

    if (func_ref)
    {
      // MS: changed for upgrading to ROSE 0.8.10e
      // (auto conversion of SgName to const char* is broken, name must be determined explicitely)
    char *name = const_cast<char*>(find_mangled_func_name(func_ref).getString().c_str()); // the string remains in the AST, c_str should be fine 
        int num = 0;
        std::deque<Procedure *>::const_iterator i;

        for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
        //std::cout<<"B: "<< (char*)((*i)->mangled_name) << std::endl;
        //std::cout<<"A: "<< name <<std::endl;

            if (strcmp(name, (*i)->mangled_name) == 0)
                blocks->push_back((*cfg->procedures)[num]->entry);
            num++;
        }
        return blocks;
    }
    else if (member_func_ref)
    {
        SgMemberFunctionDeclaration *decl
            = isSgMemberFunctionDeclaration(
                    member_func_ref->get_symbol()->get_declaration());
    assert(decl);
        SgMemberFunctionDeclaration *fnddecl
            = isSgMemberFunctionDeclaration(decl
                    ->get_firstNondefiningDeclaration());
        if (fnddecl != NULL && fnddecl != decl)
            decl = fnddecl;
        SgClassDefinition *class_type
            = isSgClassDefinition(member_func_ref
                ->get_symbol_i()->get_declaration()->get_scope());
    // MS: changed following line for upgrading to ROSE 0.8.10e 
        // (auto conversion of SgName to const char* is broken, name must be determined explicitely)
        const char *name = member_func_ref->get_symbol()->get_name().getString().c_str(); // the string remains in the AST, c_str should be fine
        int num = 0;
        std::deque<Procedure *>::const_iterator i;

        for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i)
        {
            if (strcmp(name, (*i)->name) == 0 && (*i)->class_type != NULL
                    && (class_type == (*i)->class_type
                        || (decl->get_functionModifier().isVirtual()
                        && subtype_of((*i)->class_type, class_type)))) {
             // GB (2008-03-13): OK, so the functions have the same name and
             // are on the same class, or the call is virtual and there is
             // an appropriate subtype relation. But to be sure that this is
             // the right function to call, we still have to check the
             // parameter types; the member function might be overloaded.
                bool candidate = true;
             // call_decl_args is the argument list of the declaration found
             // from the call, proc_decl_args is the argument list of the
             // declaration found from our procedure entry in the CFG.
                SgInitializedNamePtrList &call_decl_args = decl->get_args();
                SgInitializedNamePtrList &proc_decl_args = (*i)->decl->get_args();
                if (call_decl_args.size() == proc_decl_args.size()) {
                    SgInitializedNamePtrList::iterator c = call_decl_args.begin(),
                                                       p = proc_decl_args.begin();
                    while (c != call_decl_args.end() && p != proc_decl_args.end()) {
#if 0
                     // Comparing types by comparing pointers, this is
                     // supposedly correct for ROSE as types are shared.
                        if ((*c++)->get_type() != (*p++)->get_type()) {
                            candidate = false;
                            break;
                        }
#endif
                     // GB (2008-03-18): It looks like, at least in ROSE
                     // 0.9.1a, class types are NOT always shared. I had
                     // this problem when running the ICFG builder on
                     // grato/GBackEnd.C and grato/GCocoBackEnd.C at once. I
                     // think this should be solved using Mihai Ghete's AST
                     // matcher (TODO). For now, we use the good old ugly
                     // string comparison technique.
                        if ((*c++)->get_type()->unparseToString()
                                != (*p++)->get_type()->unparseToString())
                        {
                            candidate = false;
                            break;
                        }
                    }
                } else 
                    candidate = false;
                if (candidate) {
                 // Yippie! Looks like this function can be the target of
                 // this function call.
                    blocks->push_back((*cfg->procedures)[num]->entry);
                }
            }
            num++;
        }
        return blocks;
    }
    return NULL;
}

std::vector<SgVariableSymbol *>*
ExprTransformer::evaluate_arguments(std::string name,
                    SgExpressionPtrList &args, 
                    BasicBlock *block,
                    bool member_func) {
  std::vector<SgVariableSymbol *> *params
    = new std::vector<SgVariableSymbol *>();
  SgExpressionPtrList::const_iterator i = args.begin();
  if (member_func) {
    std::string varname = std::string("$") + name + "$this";
    SgExpression *new_expr = *i;
    SgVariableSymbol *varsym = 
      Ir::createVariableSymbol(varname.c_str(), (*i)->get_type());
    params->push_back(varsym);
    block->statements.push_back(Ir::createArgumentAssignment(varsym, new_expr));
    ++i;
    if (block->successors.size() > 0) {
      Edge next = block->successors[0];
      block = next.first;
    }
  }
  int n = 0;
  for ( ; i != args.end(); ++i) {
    std::stringstream varname;
    varname << "$" << name << "$arg_" << n++;
    
    SgExpression *new_expr = *i;
    RetvalAttribute *varnameattr
      = (*i)->attributeExists("logical variable") ?
      (RetvalAttribute *) (*i)->getAttribute("logical variable")
      : (*i)->attributeExists("return variable") ?
      (RetvalAttribute *) (*i)->getAttribute("return variable")
      : NULL;
    
    if (varnameattr != NULL) {
      new_expr = Ir::createVarRefExp(varnameattr->get_str(),(*i)->get_type());
    }
    SgVariableSymbol *varsym=Ir::createVariableSymbol(varname.str(), (*i)->get_type());
    params->push_back(varsym);
    block->statements.push_back(Ir::createArgumentAssignment(varsym, new_expr));
    if (block->successors.size() > 0) {
      Edge next = block->successors[0];
      block = next.first;
    }
  }
  return params;
}

void ExprTransformer::assign_retval(std::string name, SgFunctionCallExp *call, BasicBlock *block) {
  std::stringstream varname;
  varname << "$" << name << "$return_" << expnum;
  std::stringstream retname;
  retname << "$" << name << "$return";
  RetvalAttribute *varnameattr = (RetvalAttribute *) call->getAttribute("return variable");
  SgVariableSymbol *var = Ir::createVariableSymbol(varnameattr->get_str(),call->get_type());
  SgVariableSymbol *retvar = Ir::createVariableSymbol(retname.str(),call->get_type());
  block->statements.push_front(Ir::createReturnAssignment(var, retvar));
  if (isSgExpression(call->get_parent())) {
    satireReplaceChild(call->get_parent(), call, Ir::createVarRefExp(var));
  } else if (root_var == NULL) {
    root_var = var;
  }
}

static 
SgMemberFunctionDeclaration*
find_dest_impl(SgClassType *ct) {
  SgClassDefinition *classdef 
    = isSgClassDefinition(isSgClassDeclaration(ct->get_declaration())->get_definition());
  if (classdef == NULL)
      return NULL;
  SgDeclarationStatementPtrList decls = classdef->get_members();
  for (SgDeclarationStatementPtrList::iterator i = decls.begin(); i != decls.end(); ++i) {
    SgMemberFunctionDeclaration* mf = isSgMemberFunctionDeclaration(*i);
    if (mf != NULL) {
      SgMemberFunctionDeclaration *fnddecl
    = isSgMemberFunctionDeclaration(mf->get_firstNondefiningDeclaration());
      if (fnddecl != NULL && fnddecl != mf)
    mf = fnddecl;
      if (mf->get_specialFunctionModifier().isDestructor())
    return mf;
    }
  }
  return NULL;
}

const std::vector<CallBlock *>*
ExprTransformer::find_destructor_entries(SgClassType *ct) {
  SgClassDefinition *cd
    = isSgClassDeclaration(ct->get_declaration())->get_definition();
  std::vector<CallBlock *> *blocks = new std::vector<CallBlock *>();

  SgMemberFunctionDeclaration *dimpl = find_dest_impl(ct);
  bool virtual_destructor;
  if (dimpl != NULL && dimpl->get_functionModifier().isVirtual())
    virtual_destructor = true;
  else
    virtual_destructor = false;

  std::deque<Procedure *>::const_iterator p;
  for (p = cfg->procedures->begin(); p != cfg->procedures->end(); ++p) {
    if ((*p)->class_type != NULL
    && (cd == (*p)->class_type
        || (virtual_destructor
        && subtype_of((*p)->class_type, cd)))
    && strchr((*p)->memberf_name, '~') != NULL)
      blocks->push_back((*p)->entry);
  }
  return blocks;
}

std::vector<std::string>*
ExprTransformer::find_destructor_names(SgClassType *ct) {
    SgClassDefinition *cd
        = isSgClassDeclaration(ct->get_declaration())->get_definition();
    std::vector<std::string> *names = new std::vector<std::string>();

    SgMemberFunctionDeclaration *dimpl = find_dest_impl(ct);
    bool virtual_destructor;
    if (dimpl != NULL && dimpl->get_functionModifier().isVirtual())
        virtual_destructor = true;
    else
        virtual_destructor = false;

    std::deque<Procedure *>::const_iterator p;
    for (p = cfg->procedures->begin(); p != cfg->procedures->end(); ++p)
    {
        if ((*p)->class_type != NULL
                && (cd == (*p)->class_type
                    || (virtual_destructor
                        && subtype_of((*p)->class_type, cd)))
                && strchr((*p)->memberf_name, '~') != NULL)
        {
            std::string class_name((*p)->class_type->get_declaration()
                    ->get_name().str());
            std::string destructor_name = class_name + "::~" + class_name;
            names->push_back(destructor_name);
        }
    }

    return names;
}

std::vector<std::string>*
ExprTransformer::find_destructor_this_names(SgClassType *ct) {
    SgClassDefinition *cd
        = isSgClassDeclaration(ct->get_declaration())->get_definition();
    std::vector<std::string> *names = new std::vector<std::string>();

    SgMemberFunctionDeclaration *dimpl = find_dest_impl(ct);
    bool virtual_destructor;
    if (dimpl != NULL && dimpl->get_functionModifier().isVirtual())
        virtual_destructor = true;
    else
        virtual_destructor = false;

    std::deque<Procedure *>::const_iterator p;
    for (p = cfg->procedures->begin(); p != cfg->procedures->end(); ++p)
    {
        if ((*p)->class_type != NULL
                && (cd == (*p)->class_type
                    || (virtual_destructor
                        && subtype_of((*p)->class_type, cd)))
                && strchr((*p)->memberf_name, '~') != NULL)
        {
            std::string class_name((*p)->class_type->get_declaration()
                    ->get_name().str());
            std::string this_var_name 
                = std::string() + "$~" + class_name + "$this";
            names->push_back(this_var_name);
        }
    }

    return names;
}

// GB (2008-03-10): See comment in header file satire/ExprTransformer.h
void satireReplaceChild(SgNode *parent, SgNode *from, SgNode *to)
{
    if (isSgValueExp(parent))
    {
     // Do nothing on value exps.
        return;
    }
    else
        replaceChild(parent, from, to);
}
