#include "AstUtilInterface.h"
#include "StmtInfoCollect.h"
#include "sage3basic.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"
#include "annotation/OperatorAnnotation.h"
#include "dependenceTracking/dependence_analysis.h"
#include "CommandOptions.h"
#include "SymbolicVal.h"

DebugLog DebugAstUtil("-debugastutil");

void AstUtilInterface::ComputeAstSideEffects(SgNode* ast, SgNode* scope,
              std::function<bool(const AstNodePtr&, const AstNodePtr&, AstUtilInterface::OperatorSideEffect)>& collect,
              DependenceTable* add_to_dep_analysis) {
    AstInterfaceImpl astImpl(scope);
    AstInterface fa(&astImpl);

    OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
    assert(funcAnnot != 0);
    DebugAstUtil([&ast](){ return "ComputeAstSideEffect: " + AstInterface::AstToString(ast); });
    // Should we add annotation? (do_annot)? Have we added any annotation? (done_annot)
    bool do_annot = false, done_annot_modify=false, done_annot_read = false, done_annot_call=0;
    AstInterface::AstNodePtr body;
    AstInterface::AstNodeList ast_params;
    AstInterface::AstTypeList ast_param_types;
    if (AstInterface::IsFunctionDefinition(ast, 0, &ast_params, 0, &body, &ast_param_types)) {
      // Add empty annotations for this function. Details of the side effects will be added later
      // while the body of the function is being analyzed.
      DebugAstUtil([&ast](){ return "Saving side effects for :" + AstInterface::AstToString(ast) + "\n"; });
      if (add_to_dep_analysis != 0) {
         add_to_dep_analysis->ClearOperatorSideEffect(ast);
         assert(ast_params.size() == ast_param_types.size());
         auto pt = ast_param_types.begin();
         for (const auto& p : ast_params) {
            add_to_dep_analysis->SaveOperatorSideEffect(ast,p, OperatorSideEffect::Parameter, (*pt).get_ptr());
            pt++;
         }
      }
      do_annot = true;
    }

    StmtSideEffectCollect collect_operator(fa, funcAnnot);
    std::function<bool(AstNodePtr, AstNodePtr)> save_mod = [&collect, &ast, do_annot, &done_annot_modify, &body, add_to_dep_analysis] (AstNodePtr first, AstNodePtr second) {
      if (!AstInterface::IsMemoryAccess(first)) {
          DebugAstUtil([&first](){ return "Do not save non-memory-access modify:" + AstInterface::AstToString(first); });
          return true;
      }
      if (do_annot && (first == AST_UNKNOWN || !IsLocalRef(first.get_ptr(), body.get_ptr()))) {
         done_annot_modify = true;
         DebugAstUtil([&first](){ return "save modify annotation:" + AstInterface::AstToString(first); });
         if (add_to_dep_analysis != 0) {
            add_to_dep_analysis->SaveOperatorSideEffect(ast, first, OperatorSideEffect::Modify, second.get_ptr()); 
         } else {
             AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Modify);
         }
      }
      DebugAstUtil([&first](){ return "save modify:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Modify);
    };
    std::function<bool(AstNodePtr, AstNodePtr)> save_read = [&collect,&ast, do_annot, &done_annot_read, &body, add_to_dep_analysis] (AstNodePtr first, AstNodePtr second) {
      if (!AstInterface::IsMemoryAccess(first))  {
          DebugAstUtil([&first](){ return "Do not save non-memory-access read:" + AstInterface::AstToString(first); });
          return true;
      }
      if (do_annot && (first == AST_UNKNOWN || !IsLocalRef(first.get_ptr(), body.get_ptr()))) {
         done_annot_read = true;
         DebugAstUtil([&first](){ return "save read annotation:" + AstInterface::AstToString(first); });
         if (add_to_dep_analysis != 0) {
            add_to_dep_analysis->SaveOperatorSideEffect(ast, first, OperatorSideEffect::Read, 0) ;
         } else {
            AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Read);
         }
      }
      DebugAstUtil([&first](){ return "save read:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Read);
    };
    std::function<bool(AstNodePtr, AstNodePtr)> save_kill = [&collect] (AstNodePtr first, AstNodePtr second) {
      if (!AstInterface::IsMemoryAccess(first)) 
          return true;
      DebugAstUtil([&first](){ return "save kill:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Kill);
    };
    std::function<bool(AstNodePtr, AstNodePtr)> save_call = [&collect,&ast, do_annot, &done_annot_call, &body, add_to_dep_analysis] (AstNodePtr first, AstNodePtr second) {
      if (do_annot && (first == AST_UNKNOWN || !IsLocalRef(first.get_ptr(), body.get_ptr()))) {
         done_annot_call = true;
         if (add_to_dep_analysis != 0) {
            add_to_dep_analysis->SaveOperatorSideEffect(ast, first, OperatorSideEffect::Call, second.get_ptr()); 
         } else {
            AddOperatorSideEffectAnnotation(ast, first, OperatorSideEffect::Call);
         }
      }
      DebugAstUtil([&first](){ return "save call:" + AstInterface::AstToString(first); });
      return collect(first, second, OperatorSideEffect::Call);
    };
    std::function<bool(AstNodePtr, AstNodePtr)> save_decl = [&collect] (AstNodePtr var, AstNodePtr init) {
      DebugAstUtil([&var](){ return "save new decl:" + AstInterface::AstToString(var); });
      return collect(var, init, OperatorSideEffect::Decl);
    };
    std::function<bool(AstNodePtr, AstNodePtr)> save_allocate = [&collect] (AstNodePtr var, AstNodePtr init) {
      DebugAstUtil([&var](){ return "save allocate:" + AstInterface::AstToString(var); });
      return collect(var, init, OperatorSideEffect::Allocate);
    };
    std::function<bool(AstNodePtr, AstNodePtr)> save_free = [&collect] (AstNodePtr var, AstNodePtr init) {
      DebugAstUtil([&var](){ return "save free:" + AstInterface::AstToString(var); });
      return collect(var, init, OperatorSideEffect::Free);
    };
    collect_operator.set_modify_collect(save_mod);
    collect_operator.set_read_collect(save_read);
    collect_operator.set_kill_collect(save_kill);
    collect_operator.set_call_collect(save_call);
    collect_operator.set_new_var_collect(save_decl);
    collect_operator.set_allocate_collect(save_allocate);
    collect_operator.set_free_collect(save_free);
    collect_operator(ast);
    // Add empty annot if no annotations have been inserted.
    if (do_annot && add_to_dep_analysis != 0) {
       if (!done_annot_modify) {
          add_to_dep_analysis->SaveOperatorSideEffect(ast, AST_NULL, OperatorSideEffect::Modify, 0) ;
        }
        if (!done_annot_read) {
          add_to_dep_analysis->SaveOperatorSideEffect(ast, AST_NULL, OperatorSideEffect::Read, 0) ;
        }
        if (!done_annot_call) {
           add_to_dep_analysis->SaveOperatorSideEffect(ast, AST_NULL, OperatorSideEffect::Call, 0) ;
        }
    } 
}

void AstUtilInterface::ReadAnnotations(std::istream& input, DependenceTable* use_dep_analysis) {
  if (use_dep_analysis != 0) {
     use_dep_analysis->CollectFromFile(input);
  }
  else {
     ReadAnnotation::get_inst()->read(input);
  }
}

void AstUtilInterface::OutputOperatorSideEffectAnnotations(std::ostream& output, DependenceTable* use_dep_analysis) {
  if (use_dep_analysis != 0) {
     use_dep_analysis->OutputDependences(output);
  }
  else {
    OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
    funcAnnot->write(output);
  }
}

void AstUtilInterface::RegisterOperatorSideEffectAnnotation() {
  OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  funcAnnot->register_annot();
};

void AstUtilInterface::AddOperatorSideEffectAnnotation(
              SgNode* op_ast, const AstNodePtr& var, AstUtilInterface::OperatorSideEffect relation)
{
  std::string varname = GetVariableSignature(var);
  DebugAstUtil([&relation, &varname](){ return "Adding operator annotation: " + OperatorSideEffectName(relation) + ":" + "var is : " + varname; });
  if (!AstInterface::IsFunctionDefinition(op_ast)) {
     DebugAstUtil([&op_ast](){ return "Expecting an operator but getting " + AstInterface::AstToString(op_ast);});
     return;
  }
  AstInterfaceImpl astImpl(op_ast);
  AstInterface fa(&astImpl);

  OperatorSideEffectAnnotation* funcAnnot=OperatorSideEffectAnnotation::get_inst();
  OperatorSideEffectDescriptor *desc = 0;
  switch (relation) {
     case OperatorSideEffect::Modify:
         desc = funcAnnot->get_modify_descriptor(fa, op_ast, true);
          break;
     case OperatorSideEffect::Read:
        desc = funcAnnot->get_read_descriptor(fa, op_ast, true);
          break;
     case OperatorSideEffect::Call:
        desc = funcAnnot->get_call_descriptor(fa, op_ast, true);
          break;
     case OperatorSideEffect::Kill:
     case OperatorSideEffect::Decl:
     case OperatorSideEffect::Allocate:
     case OperatorSideEffect::Free:
          break;
     default: 
        std::cerr << "Unexpected relation: " << relation << "\n";
        assert(0);
  }
  if (desc != 0 && varname != "_NULL_") {
     if (varname == "_UNKNOWN_" || varname == "") {
       desc->set_has_unknown(true);  
     } else {
       desc->push_back(SymbolicValDescriptor(SymbolicValGenerator::GetSymbolicVal(fa, AstNodePtrImpl(var)), varname));
     }
  }
} 


bool AstUtilInterface::IsLocalRef(SgNode* ref, SgNode* scope) {
   return AstInterface::IsLocalRef(ref, scope);
}

std::string AstUtilInterface::GetVariableSignature(const AstNodePtr&  variable) {
  return AstInterface::GetVariableSignature(variable);
}
