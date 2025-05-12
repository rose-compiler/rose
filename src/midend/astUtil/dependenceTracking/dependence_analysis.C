#include "sage3basic.h"

#include <sstream>
#include <string>

#include "dependence_analysis.h"

#include "CommandOptions.h"
#include "AstInterface.h"

namespace AstUtilInterface {

void WholeProgramDependenceAnalysis::CollectPastResults(std::istream& dep_file, std::istream* annot_file) {
    Log.push("Collect past results of dependence analysis");
    main_table.CollectFromFile(dep_file);
    if (annot_file != 0) {
      Log.push("Reading existing dependence table as annotations.");
      AstUtilInterface::RegisterOperatorSideEffectAnnotation();
      AstUtilInterface::ReadAnnotations(*annot_file, &annot_table);
      Log.push("Done reading existing whole application dependence table.");
    }
    Log.push("Done collecting past results of dependence analysis");
}

WholeProgramDependenceAnalysis:: WholeProgramDependenceAnalysis(int argc, const char** argv) : main_table(false), annot_table(true) {
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
     main_table.OutputDataDependences(output);
  } else {
     main_table.OutputDependences(output); 
  }
}

void WholeProgramDependenceAnalysis:: OutputAnnotations(std::ostream& output) { 
  if (CmdOptions::GetInstance()->HasOption("-annot")) {
     annot_table.OutputDependences(output);
  }
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
        if (!annot_table.SaveOperatorSideEffect(input, p.get_ptr(), AstUtilInterface::OperatorSideEffect::Parameter, 0)) {
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
        if (!main_table.SaveOperatorSideEffect(input, first.get_ptr(), relation, details)) {
           DebugSaveDep([](){return "Did not save dependene" ; });
        } 
        return true;
      };
     AstUtilInterface::ComputeAstSideEffects(input, &save_dep, &annot_table);
  }
  if (AstInterface::IsBlock(input, 0, &children)) {
    for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
      AstNodePtr current = *p;
      ComputeDependences(current.get_ptr(), root);
    }
  }
}

};
