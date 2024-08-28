#ifndef AST_UTIL_INTERFACE
#define AST_UTIL_INTERFACE

#include <functional>
#include <fstream>
#include <iostream>
#include <assert.h>

class SgNode;
class SgFunctionDeclaration;
namespace AstUtilInterface{

   enum OperatorSideEffect {
     Modify, Read, Kill, Call, Decl, Allocate,
   };
   inline std::string OperatorSideEffectName(OperatorSideEffect what) {
     switch (what) {
      case OperatorSideEffect::Modify: return "modify";
      case OperatorSideEffect::Read: return "read";
      case OperatorSideEffect::Kill: return "kill";
      case OperatorSideEffect::Call: return "call";
      case OperatorSideEffect::Decl: return "decl";
      case OperatorSideEffect::Allocate: return "allocate";
      default:
          std::cerr << "Error: Unexpected enum value:" << what << "\n";
          assert(false);
          return "";
     }
   }
   //! Returns the collection of memory references modified, read, and invoked (as 
   //! function calls) as potential side effects of running the given ast, inside the
   //! given scope. The functtion returns true if the collection is known to be
   //! accurate (no unknowns); it returns false otherwise.
    bool ComputeAstSideEffects(SgNode* ast, SgNode* scope,
                   std::function<bool(SgNode*, SgNode*, OperatorSideEffect)>& collect);

    //! Instruct the compiler to collect operator side effect annotation. The 
    //! annotations will be stored internally to be used later to assist program 
    //! analysis within the compiler.
    void RegisterOperatorSideEffectAnnotation();

   //! Read annotation from the given file. The annotations will be stored internally 
   //! to be used later to assist program analysis within the compiler.
   //! Only annotations supported by those already registered can be parsed correctly.
    void ReadAnnotations(std::istream& input); 

   //! Write annotation to the given file. The annotations are those stored internally 
   //! collected through other annotation-related calls of this interface.
    void OutputOperatorSideEffectAnnotations(std::ostream& output); 

    //! Record that op_ast references the given variable with the given relation:
    //! (modify, read, or call). Returns the string representation of the operator and variable.
    std::pair<std::string, std::string>  AddOperatorSideEffectAnnotation(SgNode* op_ast, SgNode* var, OperatorSideEffect relation);

    //! Returns a string that uniquely identifies the given variable.
    std::string GetVariableSignature(SgNode* variable);

    //! Returns whether the given ref reaches only local data within scope. 
    bool IsLocalRef(SgNode* ref, SgNode* scope);

};
#endif
