#ifndef AST_UTIL_INTERFACE
#define AST_UTIL_INTERFACE

#include <functional>
#include <fstream>
#include <iostream>
#include <assert.h>

class SgNode;
class SgFunctionDeclaration;
class AstNodePtr;
namespace AstUtilInterface{

   class WholeProgramDependenceAnalysis;
   enum OperatorSideEffect {
     Modify, Read, Kill, Call, Decl, Allocate, Free, Parameter, Return
   };
   inline std::string OperatorSideEffectName(OperatorSideEffect what) {
     switch (what) {
      case OperatorSideEffect::Modify: return "modify";
      case OperatorSideEffect::Read: return "read";
      case OperatorSideEffect::Kill: return "kill";
      case OperatorSideEffect::Call: return "call";
      case OperatorSideEffect::Parameter: return "parameter";
      case OperatorSideEffect::Return: return "return";
      case OperatorSideEffect::Decl: return "decl";
      case OperatorSideEffect::Allocate: return "allocate";
      case OperatorSideEffect::Free: return "free";
      default:
          std::cerr << "Error: Unexpected enum value:" << what << "\n";
          assert(false);
          return "";
     }
   }
   //! Returns the collection of memory references modified, read, and invoked (as 
   //! function calls) as potential side effects of running the given ast, inside the
   //! given scope. 
    void ComputeAstSideEffects(SgNode* ast, SgNode* scope,
                   std::function<bool(const AstNodePtr&, const AstNodePtr&, OperatorSideEffect)>& collect,
                   WholeProgramDependenceAnalysis* add_to_dep_analysis = 0);

    //! Instruct the compiler to collect operator side effect annotation. The 
    //! annotations will be stored internally to be used later to assist program 
    //! analysis within the compiler.
    void RegisterOperatorSideEffectAnnotation();

   //! Read annotation from the given file. The annotations will be stored internally 
   //! to be used later to assist program analysis within the compiler.
   //! Only annotations supported by those already registered can be parsed correctly.
   //! Use the given dependence analysis if it is not 0.
    void ReadAnnotations(std::istream& input, WholeProgramDependenceAnalysis* use_dep_analysis = 0); 

   //! Write annotation to the given file. The annotations are those stored internally 
   //! collected through other annotation-related calls of this interface.
   //! Use the given dependence analysis if it is not 0.
    void OutputOperatorSideEffectAnnotations(std::ostream& output, WholeProgramDependenceAnalysis* use_dep_analysis = 0); 

    //! Record that op_ast references the given variable with the given relation:
    //! (modify, read, or call). Returns the string representation of the operator and variable.
    void AddOperatorSideEffectAnnotation(SgNode* op_ast, const AstNodePtr& var, OperatorSideEffect relation);

    //! Returns a string that uniquely identifies the given variable.
    std::string GetVariableSignature(const AstNodePtr& variable);

    //! Returns whether the given ref reaches only local data within scope. 
    bool IsLocalRef(SgNode* ref, SgNode* scope);

};
#endif
