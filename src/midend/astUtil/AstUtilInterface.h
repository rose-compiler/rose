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

   template<class NodeIterator, class EdgeIterator> class WholeProgramDependenceAnalysis;
   class OperatorSideEffect {
     public:
     enum class EnumVariant {Modify, Read, Kill, Alias, Call, CallUnknown, ModifyUnknown, ReadUnknown, Decl, Allocate, Free, Parameter, Return, Unexpected};
     OperatorSideEffect(EnumVariant e, SgNode* details=0) 
              {  enum_ = e;  details_ = details; }

     const EnumVariant& get_enum() const { return enum_; }
     SgNode* get_details() const { return details_; } 

     OperatorSideEffect(const std::string& what, const std::string& attr) { 
       if (attr == "unknown") {  
          if (what == "modify") { enum_ = EnumVariant::ModifyUnknown; }
          else if (what == "read") { enum_ = EnumVariant::ReadUnknown; }
          else if (what == "call") { enum_ = EnumVariant::CallUnknown; }
          else { std::cerr << "unexpected side effect name for unknown:" << what << "\n"; 
                 assert(0); }
       } else if (attr == "kill") { enum_ = EnumVariant::Kill; }
       else if (attr == "decl") { enum_ = EnumVariant::Decl; }
       else if (attr == "alias") { enum_ = EnumVariant::Alias; }
       else if (attr == "allocate") { enum_ = EnumVariant::Allocate; }
       else if (attr == "free") { enum_ = EnumVariant::Free; }
       else {
          assert(attr == ""); 
          if (what == "modify") { enum_ = EnumVariant::Modify; }
          else if (what == "read") { enum_ = EnumVariant::Read; }
          else if (what == "call") { enum_ = EnumVariant::Call; }
          else if (what == "parameter") { enum_ = EnumVariant::Parameter; }
          else if (what == "return") { enum_ = EnumVariant::Return; }
          else { std::cerr << "unexpected side effect name for:" << what << "\n"; assert(0); }
       }
     }
     inline std::string attr_name() const { 
       switch (enum_) {
        case EnumVariant::Kill: return "kill";
        case EnumVariant::CallUnknown: return "unknown";
        case EnumVariant::ModifyUnknown: return "unknown";
        case EnumVariant::ReadUnknown: return "unknown";
        case EnumVariant::Decl: return "decl";
        case EnumVariant::Alias: return "alias";
        case EnumVariant::Allocate: return "allocate";
        case EnumVariant::Free: return "free";
        default:
          return "";
       }
      }
     inline std::string relation_name() const {
       switch (enum_) {
        case EnumVariant::ModifyUnknown: 
        case EnumVariant::Kill: 
        case EnumVariant::Decl: 
        case EnumVariant::Alias: 
        case EnumVariant::Allocate: 
        case EnumVariant::Free: 
        case EnumVariant::Modify: return "modify";
        case EnumVariant::ReadUnknown: 
        case EnumVariant::Read: return "read";
        case EnumVariant::CallUnknown: 
        case EnumVariant::Call: return "call";
        case EnumVariant::Parameter: return "parameter";
        case EnumVariant::Return: return "return";
        default:
          std::cerr << "Error: Unexpected enum value:" << (int)enum_ << "\n";
          assert(false);
          return "";
       }
      }
    private:
      EnumVariant enum_ = EnumVariant::Unexpected; 
      SgNode* details_ = 0;
   };
   
   //! Interface for saving side effects of operations.
   class SaveOperatorSideEffectInterface {
     public:
       //! Erase existing side effects for the given operator to start over.
       virtual void ClearOperatorSideEffect(SgNode* op) = 0;
    
       //! The operator op accesses the given memory reference in nature of the given relation.
       virtual bool SaveOperatorSideEffect(SgNode* op, const AstNodePtr& ref, const AstUtilInterface::OperatorSideEffect& relation) = 0;
   };

   //! A simplified interface, which
   //! accumulate all side effects into read and write vectors.
   class SaveOperatorSideEffectIntoVectors : public SaveOperatorSideEffectInterface {
       std::vector<AstNodePtr> *readp, *writep, *callp;
     public:
      SaveOperatorSideEffectIntoVectors(std::vector<AstNodePtr>* readset = 0, std::vector<AstNodePtr>* writeset = 0, std::vector<AstNodePtr>* callset = 0) : readp(readset), writep(writeset), callp(callset) {}

       //! Erase existing side effects for the given operator. Do nothing b/c we accumulate.
       virtual void ClearOperatorSideEffect(SgNode* /*op*/) override {}
    
       //! The operator op accesses the given memory reference in nature of the given relation.
       virtual bool SaveOperatorSideEffect(SgNode* /*op*/, const AstNodePtr& sig, const AstUtilInterface::OperatorSideEffect& relation) override {
           switch (relation.get_enum()) {
               case OperatorSideEffect::EnumVariant::Modify: if (writep != 0) writep->push_back(sig); break;
               case OperatorSideEffect::EnumVariant::Read: if (readp != 0) readp->push_back(sig); break;
               case OperatorSideEffect::EnumVariant::Call: if (callp != 0) callp->push_back(sig); break;
               default: break;
           }
           return true;
       }
   };


   //! Find all the potential side effects of invoking all functions inside ast (the first 
   //! parameter), including what are the variables read/written, which other functions are 
   //! called, etc (see the enumeration type above. 
   //! If the given call-back function parameter collect is not null, it is invoked for each
   //! side effect discovered. If the last parameter save_side_effect is not null, its interface 
   //! member functions are invoked to save a side-effect summary for each operation (function) 
   //! contained inside ast.
    void ComputeAstSideEffects(SgNode* ast, 
                   std::function<bool(const AstNodePtr&, const AstNodePtr&, const OperatorSideEffect&)>* collect = 0,
                   SaveOperatorSideEffectInterface* save_side_effect = 0);

   //!Collect non-local variables that are read and written within the given ast. This is a wrapper
   //! of the ComputeAstSideEffects function to provide a more convenient user interface.
   //!Returns true if the returned variables are guaranteed to be complete; returns false otherwise.
    inline void CollectSideEffectVariables(SgNode* ast, std::vector<AstNodePtr>* writeVars = 0, std::vector<AstNodePtr>* readVars = 0, std::vector<AstNodePtr>* callVars = 0) {
        SaveOperatorSideEffectIntoVectors save(readVars, writeVars, callVars);
        ComputeAstSideEffects(ast, 0, &save);
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
    void AddOperatorSideEffectAnnotation(SgNode* op_ast, const AstNodePtr& var, const OperatorSideEffect& relation);

    void SetFunctionNameMangling(std::string (*)(const SgFunctionDeclaration*));

    //! Returns a string that uniquely identifies the given variable. 
    //! If provided with a dict_table, save the file name and line number for the signature.
    std::string GetVariableSignature(const AstNodePtr& variable);
    // Set configuration to save the file name and line number for each variable signature.
    void SetSaveVariableDictionary(bool doit);
    // Output a dictionary that maps each signature to its file name and location.. 
    void OutputSignatureDictionary(std::ostream& output); 


    //! Returns whether the given ref reaches only local data within scope. 
    bool IsLocalRef(SgNode* ref, SgNode* scope, bool* has_ptr_deref = 0);

};
#endif
