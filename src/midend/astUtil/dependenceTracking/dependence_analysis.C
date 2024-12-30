#include <sstream>
#include <string>

#include "dependence_analysis.h"

#include "CommandOptions.h"
#include "sage3basic.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"
#include "OperatorDescriptors.h"
#include "OperatorAnnotation.h"


namespace AstUtilInterface {

void WholeProgramDependenceAnalysis::CollectPastResults(std::istream& input_file) {
    Log.push("Collect past results of dependence analysis");
    DependenceTable::CollectFromFile(input_file);
    Log.push("Done collecting past results of dependence analysis");
}

WholeProgramDependenceAnalysis:: WholeProgramDependenceAnalysis(int argc, const char** argv) {
  std::vector<std::string> argvList(argv, argv + argc);
  CmdOptions::GetInstance()->SetOptions(argvList);
  sageProject = new SgProject (argvList);
}

void WholeProgramDependenceAnalysis:: ComputeDependences() {
  Log.push("Compute dependences.");
  if (sageProject == 0) {
    return;
  }
  int filenum = sageProject->numberOfFiles();
  for (int i = 0; i < filenum; ++i) {
    SgSourceFile* sageFile = isSgSourceFile(sageProject->get_fileList()[i]);
    ROSE_ASSERT(sageFile != NULL);

    std::string fname = sageFile->get_file_info()->get_raw_filename();
    Log.push("Targeting file:"+fname);

    SgGlobal *root = sageFile->get_globalScope();
    ROSE_ASSERT(root != NULL);

    SgDeclarationStatementPtrList declList = root->get_declarations ();
    for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p)
    {
      SgNode* func = *p;
      if (func == 0) continue;
      std::string defn_file_name;
      if (!AstInterface::get_fileInfo(func,&defn_file_name) || defn_file_name != fname) {
         continue;
      } 
      Log.push("Analyzing declaration " + func->unparseToString() + " in " + fname);
      ComputeDependences(func, root);
    }
  }
}

void WholeProgramDependenceAnalysis:: OutputDependences(std::ostream& output) { 
  if (CmdOptions::GetInstance()->HasOption("-data")) {
     DependenceTable::OutputDataDependences(output);
  } else {
     DependenceTable::OutputDependences(output); 
  }
}

bool WholeProgramDependenceAnalysis::
     SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, AstUtilInterface::OperatorSideEffect relation, SgNode* details) {
  std::string prefix, attr;
  bool save_annot = false;
  switch (relation) {
    case AstUtilInterface::OperatorSideEffect::Modify: prefix = "modify"; save_annot=true; break;
    case AstUtilInterface::OperatorSideEffect::Read:  prefix = "read"; save_annot=true; break;
    case AstUtilInterface::OperatorSideEffect::Call:  prefix = "call"; break;
    case AstUtilInterface::OperatorSideEffect::Parameter:  
             prefix = "parameter"; break;
    case AstUtilInterface::OperatorSideEffect::Return:  
             prefix = "return"; break;
    case AstUtilInterface::OperatorSideEffect::Kill:  return false; 
    case AstUtilInterface::OperatorSideEffect::Decl:  prefix = "construct_destruct"; break;
    case AstUtilInterface::OperatorSideEffect::Allocate:  prefix = "allocate"; break;
    case AstUtilInterface::OperatorSideEffect::Free:  prefix = "free"; break;
    default:
     std::cerr << "Unexpected case:" << relation << "\n";
     assert(0);
  }
  if (save_annot) {
     Log.push("Adding annotation: " + prefix + op->unparseToString());
     AstUtilInterface::AddOperatorSideEffectAnnotation(op, varref, relation);
  }
  if (details != 0) {
       attr = AstUtilInterface::GetVariableSignature(details);
  }
  DependenceEntry e(AstUtilInterface::GetVariableSignature(op), AstUtilInterface::GetVariableSignature(varref), prefix, attr); 
  Log.push("saving dependence: " + e.to_string());
  DependenceTable::SaveDependence(e);
  return true;
}

void WholeProgramDependenceAnalysis::
ClearOperatorSideEffect(SgNode* op) {
  auto sig = AstUtilInterface::GetVariableSignature(op);
  DependenceTable::ClearDependence(sig);
}

void WholeProgramDependenceAnalysis::ComputeDependences(SgNode* input, SgNode* root) {
  DebugLog DebugSaveDep("-debugdep");
  std::string function_name;
  AstInterface::AstNodeList params, children;
  AstNodePtr body;
  if (AstInterface::IsFunctionDefinition(input, &function_name, &params, 0, &body, 0, 0,/*use_global_name*/true) && body != 0) {
    Log.push("Computing dependences for " + input->unparseToString());
    for (const auto& p : params) {
        DebugSaveDep([&p](){return "saving for function parameter:" + AstInterface::AstToString(p); });
        if (!SaveOperatorSideEffect(input, p.get_ptr(), AstUtilInterface::OperatorSideEffect::Parameter, 0)) {
           DebugSaveDep([](){return "Did not save dependene" ; });
        }
     }
    std::function<bool(const AstNodePtr&, const AstNodePtr&, AstUtilInterface::OperatorSideEffect)> save_dep = 
        [this,input,body,&DebugSaveDep] (const AstNodePtr& first, const AstNodePtr& second, AstUtilInterface::OperatorSideEffect relation) {
        DebugSaveDep([&relation](){return "saving for:" + AstUtilInterface::OperatorSideEffectName(relation); });
        SgNode* details = second.get_ptr();
        switch (relation) {
          case AstUtilInterface::OperatorSideEffect::Decl: 
          case AstUtilInterface::OperatorSideEffect::Allocate: 
          case AstUtilInterface::OperatorSideEffect::Free:  {
               SgType* t = AstInterface::GetExpressionType(first).get_ptr();
               assert(t != 0);
               details = t;
               break;
          }
          case AstUtilInterface::OperatorSideEffect::Read:  {
               // If the detail is the surrounding statment, skip.
               if (AstInterface::IsStatement(second)) {
                 details = 0;
               }
               if (!AstInterface::IsVarRef(first) || (AstUtilInterface::IsLocalRef(first.get_ptr(), body.get_ptr()) && details == 0)) {
                 return true;
               } 
               break; 
           }
          case AstUtilInterface::OperatorSideEffect::Kill: 
               details = 0; break; 
          default: break;
        }
        DebugSaveDep([&first,details](){return "saving side effect for:" + AstInterface::AstToString(first) + " = " + AstInterface::AstToString(details); });
        if (!SaveOperatorSideEffect(input, first.get_ptr(), relation, details)) {
           DebugSaveDep([](){return "Did not save dependene" ; });
        } 
        return true;
      };
     AstUtilInterface::ComputeAstSideEffects(input, root, save_dep);
  }
  if (AstInterface::IsBlock(input, 0, &children)) {
    for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
      AstNodePtr current = *p;
      ComputeDependences(AstNodePtrImpl(current).get_ptr(), root);
    }
  }
}

void WholeProgramDependenceAnalysis:: save_dependence(const DependenceEntry& e) {
  // Save inside the dependence table (base class).
  DependenceTable::SaveDependence(DependenceEntry(e));
  DebugLog DebugSaveDep("-debugdep");

  // Save into annotation  if necessary.
  if (e.type_entry() == "parameter") {
    OperatorSideEffectAnnotation* funcAnnot = OperatorSideEffectAnnotation::get_inst();
    OperatorSideEffectDescriptor* desc1 = funcAnnot->get_modify_descriptor(e.first_entry(), true);
    assert(desc1 != 0);
    desc1->get_param_decl().add_param( /*param type*/ e.attr_entry(),  /* param name*/ e.second_entry());
    OperatorSideEffectDescriptor* desc2 = funcAnnot->get_read_descriptor(e.first_entry(), true);
    assert(desc2 != 0);
    desc2->get_param_decl().add_param( /*param type*/ e.attr_entry(),  /* param name*/ e.second_entry());
    DebugSaveDep([&e](){ return "Saving parameter " + e.second_entry(); });
  }
  else if (e.type_entry() == "modify") {
    OperatorSideEffectAnnotation* funcAnnot = OperatorSideEffectAnnotation::get_inst();
    OperatorSideEffectDescriptor* desc = funcAnnot->get_modify_descriptor(e.first_entry(), true);
    assert(desc != 0);
    SymbolicVal var = SymbolicValGenerator::GetSymbolicVal(e.second_entry());
    desc->push_back(var);
    DebugSaveDep([&var](){ return "Saving modify " + var.toString(); });
    
  } else if (e.type_entry() == "read") {
    OperatorSideEffectAnnotation* funcAnnot = OperatorSideEffectAnnotation::get_inst();
    OperatorSideEffectDescriptor* desc = funcAnnot->get_read_descriptor(e.first_entry(), true);
    assert(desc != 0);
    SymbolicVal var = SymbolicValGenerator::GetSymbolicVal(e.second_entry());
    desc->push_back(var);
    DebugSaveDep([&var](){ return "Saving read " + var.toString(); });
  }
}

};
