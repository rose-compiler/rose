
#include "unification_ann.h"

using namespace std;

UnificationBasedPtr_Ann::UnificationBasedPtr_Ann(Linker &l, Annotations *ann)
: _annotations(ann), UnificationBasedPtr(l) { }


bool UnificationBasedPtr_Ann::isField(declNode *f, bool &from_annotation) const{
  if(fields_from_annotation.find(f) != fields_from_annotation.end()) {
    from_annotation = true;
    return true;
  }
  return UnificationBasedPtr::isField(f,from_annotation);
} // isField


void UnificationBasedPtr_Ann::mark_alloc(stmtNode *stmt,
                                         declNode *source_decl,
                                         declNode *target_decl) {
  if(!stmt || stmt->typ() != ThreeAddr) return;
  threeAddrNode *ta = (threeAddrNode*) stmt;
  if(! ta->lhs() || !ta->op() || ta->op()->id()!=Operator::FUNC_CALL)
    return;
  assert(source_decl && target_decl);

  Unify_ECR *left;
  Unify_Size s;
  if(source_decl->name()=="return") {
    if(_ecr[target_decl]) _alloc[ta] = _ecr[target_decl];
    if(_alloc[ta]) return; // already done
    // use the left of ta to store the ecr of right (target_decl)
    s = Unify_Size(Unify_Size::sizeOfAssign(ta, linker, this));
    left = ecr(ta->lhs(), s);
  } else {
    if(_ecr[target_decl]) return;
    left = _ecr[source_decl];
  }

  assert(left);

  // reset pending so that they are processed again.
  set<Unify_Pending*> pending = left->pending().set();
  for(Pendings_p a=pending.begin(); a!=pending.end(); a++) {
    (*a)->un_served();
    if((*a)->typ() == Unify_Pending::cjoin &&
       ! ((Unify_Size*) (*a)->arg1())->leq(s)) {
      // in cjoin(), above condition will lead to expand() instad of
      // join/settype, the desired result. set size to top to prevent this.
      s = Unify_Size();
      if(left->type()->objTyp() == BLANK)
        left->type()->blank()->s = s; //do this to ensure_sim_obj use new size
    }
  }

  memoryBlock *m = left->type()->block();
  ensure_sim_obj(left, s);

  /*if(left->type()->block() != m) {
    // changed. need to update.
    m->unifyType()->block(NULL);
    m->unifyType(left->type());
    left->type()->block(m);
  }*/

  _ecr[target_decl] = Sim_Obj_Alpha(left->type())->tao();
  if(source_decl->name()=="return")
    _alloc[ta] = _ecr[target_decl];
} // mark_alloc


// ==============================================================

bool UnificationBasedPtr_Ann::annotation_returns_object(procNode *p) const {
  if(!p) return false;
  procedureAnn * procedure = _annotations->lookup_procedure(p->decl()->name());
  if(procedure && procedure->lookup("return", false))
    return true;
  return false;
} // annotation_returns_object


void UnificationBasedPtr_Ann::annotation_call_arg(procNode *p,
                                                  int arg,
                                                  typeNode *arg_type,
                                                  Alpha *alpha) {
  if(!p) return;
  procedureAnn * procedure = _annotations->lookup_procedure(p->decl()->name());
  if(!procedure) return;
  // process all pointer and struct rules on formal
  var_list formals = procedure->formal_params();
  structure_list struct_rules  = procedure->on_entry();
  pointerrule_list pointer_rules = procedure->pointer_rules();
  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin(); e!=(*r)->effects().end();
        e++)
      struct_rules.push_back(*e);

  int i=0;
  assert(! formals.empty());
  for(var_list_p f=formals.begin(); i<=arg; i++) {
    if(i == arg) {
      Unify_Parents par(false);
      Unify_Size s(sizeof(int*)); // ?
      arg_type = arg_type->follow_tdefs();
      UnifyType *tao = new UnifyType(new Unify_Simple(alpha, L_bottom, s, par));
      process_struct_rule(*f, (*f)->decl(), arg_type, struct_rules,
                          tao->ecr());
    }
    if(*f != formals.back()) f++;
  }
} //annotation_call_arg(Alpha)


void UnificationBasedPtr_Ann::annotation_call_arg(procNode *p,
                                                  int arg,
                                                  typeNode *arg_type,
                                                  Unify_ECR *ecr) {
  if(!p) return;
  procedureAnn * procedure = _annotations->lookup_procedure(p->decl()->name());
  if(!procedure) return;
  // process all pointer and struct rules on formal
  var_list formals = procedure->formal_params();
  structure_list struct_rules  = procedure->on_entry();
  pointerrule_list pointer_rules = procedure->pointer_rules();
  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin(); e!=(*r)->effects().end();
        e++)
      struct_rules.push_back(*e);

  int i=0;
  assert(! formals.empty());
  for(var_list_p f=formals.begin(); i<=arg; i++) {
    if(i == arg) {
      arg_type = arg_type->follow_tdefs();
      process_struct_rule(*f, (*f)->decl(), arg_type, struct_rules, ecr);
      break;
    }
    if(*f != formals.back()) f++;
  }
} //annotation_call_arg(ecr)



void UnificationBasedPtr_Ann::annotation_ret_val(procNode *p, Unify_ECR *taoR,
                                                 unitNode *cur_unit) {
  if(!p || !taoR) return;
  procedureAnn * procedure = _annotations->lookup_procedure(p->decl()->name());
  if(!procedure) return;
  annVariable *ret = procedure->lookup("return", false);
  if(!ret) return;
  int ret_obj = 0;

  structure_list struct_rules  = procedure->on_entry();
  pointerrule_list pointer_rules = procedure->pointer_rules();
  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin(); e!=(*r)->effects().end();
        e++)
      struct_rules.push_back(*e);

  typeNode *ret_type =((funcNode*)p->decl()->type())->returns()->follow_tdefs();

  process_struct_rule(ret, ret->decl(), ret_type, struct_rules, taoR);

  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin(); e!=(*r)->effects().end();
        e++)
      if((*e)->source() == ret && (*e)->target()) {
        ret_obj++;
        if(! (*e)->target()->is_new()) {
          // is it a global?
          string target_name = (*e)->target()->name();
          if(target_name == "null") { ret_obj--; continue; } // TBD?
          Unify_ECR *target_ecr = NULL; // ecr for target.

          // try: is it a global var?
          annVariable *global_var = _annotations->lookup_global(target_name);
          if(global_var) { // yes, it is global
            // make taoR points to this
            target_ecr = global_var_ecr[global_var];
            if(!target_ecr) {
              // the global's decl is not processed yet.
              bool is_synthetic;
              declNode *decl = linker.lookup_symbol(cur_unit, target_name,
                                                    is_synthetic);
              if(decl) {
                at_decl(decl, Postorder);
                target_ecr = global_var_ecr[global_var];
              } // else: proceed to just ensure_sim_obj(taoR).
            }

          } else {
            // no. should be some local parameter of annotated procedure.
            annVariable *ret_var = procedure->lookup(target_name,false);
            assert(ret_var);
            if(_ecr[ret_var->decl()]) // hopefully!
              target_ecr = _ecr[ret_var->decl()];
            else
              // this could happen when, say, in strtok where
              // "return->previous_string" and
              // "strtok_static_pointer->previous_string" and
              // strtok_static_pointer is a synthetic global defined in
              // annocation.
              _ecr.erase(ret_var->decl());
          }

          // may have found target_ecr (return value). do join now.
          Alpha *ret_alpha = NULL;
          Unify_Size s;
          if(target_ecr) {
            ret_alpha = new Alpha(target_ecr,
                                  new Unify_Offset(Unify_Offset::zero));
            assert(target_ecr->var());
            if(target_ecr->var()->type())
              // note: do not use global_var->decl() above because it is
              // synthetic and does not have type().
              s = Unify_Size(Unify_Size::sizeOf(target_ecr->var()->type()));
          }
          ensure_sim_obj(taoR, s);
          UnifyType *tao_type = taoR->type();
          if(! s.leq(Sim_Obj_Size(tao_type)))
            expand(taoR);
          if(ret_alpha)
            join(ret_alpha, Sim_Obj_Alpha(taoR->type()));
        }
      }
  // assert(ret_obj <= 1); // verify. TBD?
} // annotation_ret_val


void UnificationBasedPtr_Ann::annotation_init_global(declNode *global) {
  if(!global) return;
  annVariable *var = _annotations->lookup_global(global->name());
  if(!var) return;
  assert(_ecr[global]);
  if(global_var_ecr[var]) return; // already processed.
  global_var_ecr[var] = _ecr[global];
  global_var_type[var] = global->type();

  const procedureAnn *init_proc = _annotations->init();
  structure_list struct_rules  = init_proc->on_entry();
  pointerrule_list pointer_rules = init_proc->pointer_rules();
  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin(); e!=(*r)->effects().end();
        e++)
      struct_rules.push_back(*e);

  process_struct_rule(var, global, global->no_tdef_type(), struct_rules,
                      _ecr[global]);
} // annotation_init_global(declNode)


void UnificationBasedPtr_Ann::annotation_init_globals() {
  var_map globals = _annotations->globals();

  const procedureAnn *init_proc = _annotations->init();
  structure_list struct_rules  = init_proc->on_entry();
  pointerrule_list pointer_rules = init_proc->pointer_rules();
  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin(); e!=(*r)->effects().end();
        e++)
      struct_rules.push_back(*e);

  for(var_map_p g=globals.begin(); g!=globals.end(); g++)
    if(g->second) {
      annVariable *global = g->second;
      if(! global_var_ecr[global]) { // not yet processed
        global_var_ecr[global] = T_bottom->ecr();
        global_var_ecr[global]->var(global->decl());
      }
    }

  for(var_map_p g=globals.begin(); g!=globals.end(); g++)
    if(g->second) {
      annVariable *global = g->second;
      process_struct_rule(global, global->decl(), global_var_type[global],
                          struct_rules, global_var_ecr[global]);
    }
} // annotation_init_global


/*
void UnificationBasedPtr_Ann::process_pointer_rule(annVariable *source,
                                                 declNode *decl,
                                                 typeNode *dtype,
                                                 pointerrule_list pointer_rules,
                                                 structure_list & struct_rules,
                                                 Unify_ECR *ecr) {
  assert(ecr);

  for(pointerrule_list_p r=pointer_rules.begin(); r!=pointer_rules.end(); r++)
    for(structure_list_p e=(*r)->effects().begin();
        e!=(*r)->effects().end(); e++)
      if((*e)->source() == source) {
        process_pointer_rule(source, decl, dtype, pointer_rules,
                             (*r)->effects(), ecr);
        // recurse with current struct_rules
        assert((*e)->target());
        typeNode *subtype = dtype && (dtype->typ()==Array || dtype->typ()==Ptr)?
                            dtype->no_tdef_type() : NULL;
        Unify_ECR *recurse = Sim_Obj_Alpha(ecr->type())->tao();
        process_struct_rule((*e)->target(), (*e)->target()->decl(),
                             subtype, pointer_rules, struct_rules,
                             recurse);
        process_pointer_rule((*e)->target(), (*e)->target()->decl(),
                             subtype, pointer_rules, struct_rules,
                             recurse);
        // break; // each source has only at most one target
      }

  if(ecr->type()->is_bottom())
    expand(ecr); // upgrade it to size top.

  if(decl) {
    if(!_ecr[decl]) {
      _ecr[decl] = ecr;
      ecr->var(decl);
    } else
      join(_ecr[decl], ecr);
  }
} // process_pointer_rule */


void UnificationBasedPtr_Ann::process_struct_rule(annVariable *source,
                                            declNode *decl,
                                            typeNode *dtype,
                                            const structure_list & struct_rules,
                                            Unify_ECR *ecr) {
  assert(ecr);

  for(structure_list_cp r=struct_rules.begin(); r!=struct_rules.end(); r++)
    if((*r)->source() == source && (*r)->target()->name() != "null") {
//cout << "rule " << **r << " line " << (*r)->line() << endl;
      Unify_Size s;
      if(decl && decl->type())
        s = Unify_Size(Unify_Size::sizeOf(decl->type()));

      Unify_ECR *recurse = NULL;
      typeNode *subtype = NULL;
      if((*r)->field_name().empty()) { // operator is "-->" ie. dereference.
        ensure_sim_obj(ecr, s);
        recurse = Sim_Obj_Alpha(ecr->type())->tao();
        subtype = dtype && (dtype->typ() == Array || dtype->typ() == Ptr) ?
                  dtype->no_tdef_type() : NULL;

      } else { // field access
        declNode *field = (*r)->target()->decl();
        fields_from_annotation.insert(field);
        bool field_from_annotation = true;

        suespecNode *sue = NULL;
        if(dtype && (dtype->typ() == Struct || dtype->typ() == Union)) {
          sue = ((sueNode*) dtype)->spec();
          string field_name = (*r)->field_name();
          for(decl_list_p f=sue->fields().begin(); f!=sue->fields().end(); f++)
            if((*f)->name() == field_name) {
              field = _unique_field_defn[*f];
              assert(field);
              subtype = field->no_tdef_type();
              field_from_annotation = false;
              break;
            }
        }
        recurse = ecrField(ecr->type(), field, field_from_annotation);
        if(!recurse && (*r)->target()->is_global()) {
          assert( recurse = global_var_ecr[(*r)->target()] );
          if(decl && sue) {
            Unify_ECR *source_ecr = _ecr[decl];
            if(!source_ecr) source_ecr = ecr;
            assert(source_ecr);
            ensure_struct_obj(source_ecr, sue);
            assert(source_ecr->type()->objTyp() == STRUCTURE); // TBD?
            Unify_ECR *field_ecr =
              source_ecr->type()->structure()->get(field, source_ecr);
            if(!field_ecr) {
              declSet ds;  ds.insert(field);
              make_compatible(ds, source_ecr->type()->structure(), source_ecr,
                              true);
              field_ecr =
                source_ecr->type()->structure()->get(field, source_ecr);
            }
            join(field_ecr, recurse);
          }
        }
      }

      if(recurse && (*r)->target())
        process_struct_rule((*r)->target(), (*r)->target()->decl(), subtype,
                            struct_rules, recurse);
    }

  if(ecr->type()->is_bottom())
    expand(ecr); // upgrade it to size top.

  if(decl) {
    if(!_ecr[decl]) {
      _ecr[decl] = ecr;
      ecr->var(decl);
    } else
      join(_ecr[decl], ecr);

    if(source->is_global()) {
      if(!global_var_ecr[source]) global_var_ecr[source] = _ecr[decl];
      if(!global_var_type[source]) global_var_type[source] = dtype;
    }
    _uses[decl] = true;
  }
} // process_struct_rule


// ==============================================================

UnificationBasedPtr *UnificationBasedPtr_Ann::analyze_all(Linker &linker,
                                                          Annotations *ann) {
  cbzTimer unify_timer;
  unify_timer.start();
  UnificationBasedPtr_Ann *U = new UnificationBasedPtr_Ann(linker, ann);
  for(unit_list_p u=CBZ::Program.begin(); u!=CBZ::Program.end(); u++)
    (*u)->walk(*U);

//cout << "annotation_init_globals\n";
  U->annotation_init_globals(); // new

  U->finalize();
  unify_timer.stop();
  double t(unify_timer);
  cout << "STAT-unification-time              " << t << endl;
  return U;
} // analyze_all

