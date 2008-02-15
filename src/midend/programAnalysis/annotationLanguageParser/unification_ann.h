#ifndef UNIFICATION_ANN_H
#define UNIFICATION_ANN_H

#include "c_breeze.h"
#include "unification.h"
#include "broadway.h"

class UnificationBasedPtr_Ann : public UnificationBasedPtr {
private:
  Annotations * _annotations;

  map<annVariable*,Unify_ECR*> global_var_ecr;
  map<annVariable*,typeNode*>  global_var_type;

  set<declNode*>               fields_from_annotation;

  UnificationBasedPtr_Ann(Linker &l, Annotations *ann);

  virtual bool annotation_returns_object(procNode *proc) const;
  virtual void annotation_call_arg(procNode *, int arg, typeNode*, Unify_ECR *);
  virtual void annotation_call_arg(procNode *, int arg, typeNode*, Alpha *);
  virtual void annotation_ret_val(procNode *, Unify_ECR *taoR, unitNode *unit);
  virtual void annotation_init_global(declNode *global);
  virtual void annotation_init_globals();

  void process_struct_rule(annVariable *, declNode *, typeNode *dtype,
                           const structure_list & struct_rules,
                           Unify_ECR *);

public:

  virtual bool isField(declNode *f, bool &from_annotation) const;

  virtual void mark_alloc(stmtNode *stmt, declNode *source_decl,
                          declNode *target_decl);

  static UnificationBasedPtr *analyze_all(Linker &, Annotations *ann);
};

#endif
