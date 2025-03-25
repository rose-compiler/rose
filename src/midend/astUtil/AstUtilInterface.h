#ifndef AST_UTIL_INTERFACE
#define AST_UTIL_INTERFACE

#include <functional>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <AstInterface.h>

class SgNode;
class SgFunctionDeclaration;
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
   //! Interface for saving side effects of operations.
   class SaveOperatorSideEffectInterface {
     public:
       //! Erase existing side effects for the given operator to start over.
       virtual void ClearOperatorSideEffect(SgNode* op) = 0;
    
       //! The operator op accesses the given memory reference in nature of the given relation.
       virtual bool SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, AstUtilInterface::OperatorSideEffect relation, SgNode* sig = 0) = 0;
   };

   //! A simplified interface, which
   //! accumulate all side effects into read and write vectors.
   class SaveOperatorSideEffectIntoVectors : public SaveOperatorSideEffectInterface {
       std::vector<AstNodePtr> *readp, *writep;
       bool _has_unknown = false;
     public:
      SaveOperatorSideEffectIntoVectors(std::vector<AstNodePtr>& readset, std::vector<AstNodePtr>& writeset) : readp(&readset), writep(&writeset), _has_unknown(false) {}

       //! Erase existing side effects for the given operator. Do nothing b/c we accumulate.
       virtual void ClearOperatorSideEffect(SgNode* op) override {} 
    
       //! The operator op accesses the given memory reference in nature of the given relation.
       virtual bool SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, AstUtilInterface::OperatorSideEffect relation, SgNode* sig = 0) override {
           if (varref == AST_UNKNOWN) {
              _has_unknown = true;
           } 
           switch (relation) {
               case OperatorSideEffect::Modify: readp->push_back(varref); break;
               case OperatorSideEffect::Read: writep->push_back(varref); break;
               default: break;
           }
           return true;
       }
       bool has_unknown() const { return _has_unknown; }
   };


   //! Find all the potential side effects of invoking all functions inside ast (the first 
   //! parameter), including what are the variables read/written, which other functions are 
   //! called, etc (see the enumeration type above. 
   //! If the given call-back function parameter collect is not null, it is invoked for each
   //! side effect discovered. If the last parameter save_side_effect is not null, its interface 
   //! member functions are invoked to save a side-effect summary for each operation (function) 
   //! contained inside ast.
    void ComputeAstSideEffects(SgNode* ast, 
                   std::function<bool(const AstNodePtr&, const AstNodePtr&, OperatorSideEffect)>* collect = 0,
                   SaveOperatorSideEffectInterface* save_side_effect = 0);

   //!Collect non-local variables that are read and written within the given ast. This is a wrapper
   //! of the ComputeAstSideEffects function to provide a more convenient user interface.
   //!Returns true if the returned variables are guaranteed to be complete; returns false otherwise.
    inline bool CollectReadWriteVariables(SgNode* ast, std::vector<AstNodePtr>& readVars, std::vector<AstNodePtr>& writeVars) {
        SaveOperatorSideEffectIntoVectors save(readVars, writeVars);
        ComputeAstSideEffects(ast, 0, &save);
        return !save.has_unknown(); 
    }

    //! Instruct the compiler to collect operator side effect annotation. The 
    //! annotations will be stored internally to be used later to assist program 
    //! analysis within the compiler.
    void RegisterOperatorSideEffectAnnotation();

   class DependenceTable;
   //! Read annotation from the given file. The annotations will be stored internally 
   //! to be used later to assist program analysis within the compiler.
   //! Only annotations supported by those already registered can be parsed correctly.
   //! Use the given dependence analysis if it is not 0.
    void ReadAnnotations(std::istream& input, DependenceTable* use_dep_table = 0); 

   //! Write annotation to the given file. The annotations are those stored internally 
   //! collected through other annotation-related calls of this interface.
   //! Use the given dependence analysis if it is not 0.
    void OutputOperatorSideEffectAnnotations(std::ostream& output, DependenceTable* use_dep_analysis = 0); 

    //! Record that op_ast references the given variable with the given relation:
    //! (modify, read, or call). Returns the string representation of the operator and variable.
    void AddOperatorSideEffectAnnotation(SgNode* op_ast, const AstNodePtr& var, OperatorSideEffect relation);

    //! Returns a string that uniquely identifies the given variable.
    std::string GetVariableSignature(const AstNodePtr& variable);

    //! Returns whether the given ref reaches only local data within scope. 
    bool IsLocalRef(SgNode* ref, SgNode* scope);

};
#endif
