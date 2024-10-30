#include <sstream>
#include <string>

#include "dependence_analysis.h"

#include "CommandOptions.h"
#include "sage3basic.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"


namespace AstUtilInterface {

void WholeProgramDependenceAnalysis::CollectPastResults(std::ifstream& input_file) {
    Log.push("Collect past results of dependence analysis");
    deptable.CollectFromFile(input_file);
    Log.push("Done collecting past results of dependence analysis");
}

WholeProgramDependenceAnalysis:: WholeProgramDependenceAnalysis(int argc, const char** argv) {
  std::vector<std::string> argvList(argv, argv + argc);
  CmdOptions::GetInstance()->SetOptions(argvList);
  sageProject = new SgProject (argvList);
}

bool WholeProgramDependenceAnalysis:: ComputeDependences() {
  Log.push("Compute dependences.");
  if (sageProject == 0) {
    return false;
  }
  int filenum = sageProject->numberOfFiles();
  bool succ = true;
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
      if (!ComputeDependences(func, root)) {
         succ = false;
      }
    }
  }
  return succ;
}

void WholeProgramDependenceAnalysis:: OutputDependences(std::ostream& output) { 
  if (CmdOptions::GetInstance()->HasOption("-data")) {
     deptable.OutputDataDependences(output);
  } else {
     deptable.OutputDependences(output); 
  }
}

bool WholeProgramDependenceAnalysis::
     SaveOperatorSideEffect(SgNode* op, SgNode* varref, AstUtilInterface::OperatorSideEffect relation, SgNode* details) {
  std::string prefix, attr;
  switch (relation) {
    case AstUtilInterface::OperatorSideEffect::Modify: prefix = "modify"; break;
    case AstUtilInterface::OperatorSideEffect::Read:  prefix = "read"; break;
    case AstUtilInterface::OperatorSideEffect::Call:  prefix = "call"; break;
    case AstUtilInterface::OperatorSideEffect::Kill:  return false; 
    case AstUtilInterface::OperatorSideEffect::Decl:  prefix = "construct_destruct"; break;
    case AstUtilInterface::OperatorSideEffect::Allocate:  prefix = "allocate"; break;
    case AstUtilInterface::OperatorSideEffect::Free:  prefix = "free"; break;
    default:
     std::cerr << "Unexpected case:" << relation << "\n";
     assert(0);
  }
  Log.push("Adding annotation: " + prefix + op->unparseToString());
  auto op_save = AstUtilInterface::AddOperatorSideEffectAnnotation(op, varref, relation);
  if (details != 0) {
       attr = AstUtilInterface::GetVariableSignature(details);
  }
  DependenceEntry e(op_save.first, op_save.second, prefix, attr); 
  Log.push("saving dependence: " + e.to_string());
  deptable.SaveDependence(e);
  return true;
}

bool WholeProgramDependenceAnalysis::ComputeDependences(SgNode* input, SgNode* root) {
  DebugLog DebugSaveDep("-debugdep");
  std::string function_name;
  AstInterface::AstNodeList children;
  AstNodePtr body;
  if (AstInterface::IsFunctionDefinition(input, &function_name, 0, 0, &body, 0, 0,/*use_global_name*/true) && body != 0) {
    Log.push("Computing dependences for " + input->unparseToString());
    std::function<bool(SgNode*, SgNode*, AstUtilInterface::OperatorSideEffect)> save_dep = 
        [this,input,body,&DebugSaveDep] (SgNode* first, SgNode* second, AstUtilInterface::OperatorSideEffect relation) {
        DebugSaveDep([&relation](){return "saving for:" + AstUtilInterface::OperatorSideEffectName(relation); });
        switch (relation) {
          case AstUtilInterface::OperatorSideEffect::Decl: 
          case AstUtilInterface::OperatorSideEffect::Allocate: 
          case AstUtilInterface::OperatorSideEffect::Free:  {
               SgType* t = AstInterface::GetExpressionType(first).get_ptr();
               assert(t != 0);
               second = t;
               break;
          }
          case AstUtilInterface::OperatorSideEffect::Read:  {
               // If the detail is the surrounding statment, skip.
               if (AstInterface::IsStatement(second)) {
                 second = 0;
               }
               if (!AstInterface::IsVarRef(first) || (AstUtilInterface::IsLocalRef(first, body.get_ptr()) && second == 0)) {
                 return true;
               } 
               break; 
           }
          case AstUtilInterface::OperatorSideEffect::Kill: 
               second = 0; break; 
          default: break;
        }
        DebugSaveDep([&first,&second](){return "saving side effect for:" + AstInterface::AstToString(first) + " = " + AstInterface::AstToString(second); });
        if (!SaveOperatorSideEffect(input, first, relation, second)) {
           DebugSaveDep([](){return "Did not save dependene" ; });
        } 
        return true;
      };
    return AstUtilInterface::ComputeAstSideEffects(input, root, save_dep);
  }
  bool succ = true;
  if (AstInterface::IsBlock(input, 0, &children)) {
    for (AstInterface::AstNodeList::const_iterator p = children.begin(); p != children.end(); ++p) {
      AstNodePtr current = *p;
      if (!ComputeDependences(AstNodePtrImpl(current).get_ptr(), root)) {
        succ = false;
      }
    }
  }
  return succ;
}


};
