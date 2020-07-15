#ifndef ROSE_SAGE_TREE_BUILDER_H_
#define ROSE_SAGE_TREE_BUILDER_H_

#include "general_language_translation.h"
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

// WARNING: This file has been designed to compile with -std=c++17
// This limits the use of ROSE header files at the moment.
//
class SgBasicBlock;
class SgCaseOptionStmt;
class SgDefaultOptionStmt;
class SgDerivedTypeStatement;
class SgEnumDeclaration;
class SgEnumType;
class SgEnumVal;
class SgExpression;
class SgExprListExp;
class SgExprStatement;
class SgFunctionCallExp;
class SgFunctionDeclaration;
class SgFunctionDefinition;
class SgFunctionParameterList;
class SgFunctionParameterScope;
class SgGlobal;
class SgIfStmt;
class SgInitializedName;
class SgLocatedNode;
class SgNamespaceDeclarationStatement;
class SgProcessControlStatement;
class SgProgramHeaderStatement;
class SgScopeStatement;
class SgSourceFile;
class SgSwitchStatement;
class SgType;
class SgTypedefDeclaration;
class SgVariableDeclaration;
class SgWhileStmt;

// Jovial specific classes
class SgJovialCompoolStatement;
class SgJovialDefineDeclaration;
class SgJovialDirectiveStatement;
class SgJovialOverlayDeclaration;
class SgJovialTableStatement;


namespace Rose {
namespace builder {

// Need std=c++11
//
#ifndef nullptr
#define nullptr NULL
#endif

// This is similar to F18 Fortran::parser::SourcePosition
struct SourcePosition {
   std::string path;  // replaces Fortran::parser::SourceFile
   int line, column;
};

 struct TraversalContext {
    TraversalContext() : type(nullptr), actual_function_param_scope(nullptr) {}
    SgType* type;
    SgScopeStatement* actual_function_param_scope;
 };

//using SourcePositionPair = boost::tuple<SourcePosition, SourcePosition>;
//using SourcePositions    = boost::tuple<SourcePosition, SourcePosition, SourcePosition>;
typedef boost::tuple<SourcePosition, SourcePosition> SourcePositionPair;
typedef boost::tuple<SourcePosition, SourcePosition, SourcePosition> SourcePositions;

// The global must be initialized before using the SageTreeBuilder class
// Consider creating a constructor (may not work well with interface for F18/Flang)
SgGlobal* initialize_global_scope(SgSourceFile* file);

// Create a builder class that does nothing
class SageTreeBuilderNull {
public:
   // Default action for a sage tree node is to do nothing.
   template<typename T, typename... Options> void Enter(T* &, Options...) {}
   template<typename T>                      void Leave(T*) {}
};


class SageTreeBuilder {
public:

   // Default action for a sage tree node is to do nothing.
   template<typename T> void Enter(T* &) {}
   template<typename T> void Leave(T*)   {}

   void Leave(SgScopeStatement* &);

   void Enter(SgBasicBlock* &);
   void Leave(SgBasicBlock*);

   void Enter(SgProgramHeaderStatement* &,
              const boost::optional<std::string> &, const std::list<std::string> &, const SourcePositions &);
   void Leave(SgProgramHeaderStatement*);

   void setFortranEndProgramStmt(SgProgramHeaderStatement*,
                                 const boost::optional<std::string> &,
                                 const boost::optional<std::string> &);

   void Enter(SgFunctionParameterList* &, SgBasicBlock* &);
   void Leave(SgFunctionParameterList*, SgBasicBlock*, const std::list<LanguageTranslation::FormalParameter> &);

   void Enter(SgFunctionDeclaration* &, const std::string &, SgType*, SgFunctionParameterList*,
                                        const LanguageTranslation::FunctionModifierList &);
   void Leave(SgFunctionDeclaration*, SgBasicBlock*);

   void Enter(SgFunctionDefinition* &);
   void Leave(SgFunctionDefinition*);

   void Enter(SgDerivedTypeStatement* &, const std::string &);
   void Leave(SgDerivedTypeStatement*);

   void Enter(SgVariableDeclaration* &, const std::string &, SgType*, SgExpression*);
   void Leave(SgVariableDeclaration*);

   void Enter(SgEnumDeclaration* &, const std::string &, std::list<SgInitializedName*> &);
   void Leave(SgEnumDeclaration*);

   void Enter(SgTypedefDeclaration* &, const std::string &, SgType*);
   void Leave(SgTypedefDeclaration*);

// Statements
//
   void Enter(SgNamespaceDeclarationStatement* &, const std::string &, const SourcePositionPair &);
   void Leave(SgNamespaceDeclarationStatement*);

   void Enter(SgExprStatement* &, SgExpression* &, const std::vector<SgExpression*> &, const std::string &);
   void Leave(SgExprStatement*);

   void Enter(SgIfStmt* &, SgExpression*, SgBasicBlock*, SgBasicBlock*);
   void Leave(SgIfStmt*);

   void Enter(SgProcessControlStatement* &, const std::string &, const boost::optional<SgExpression*> &);
   void Enter(SgProcessControlStatement* &, const std::string &, const boost::optional<SgExpression*> &,
                                                                 const boost::optional<SgExpression*> &);
   void Leave(SgProcessControlStatement*);

   void Enter(SgSwitchStatement* &, SgExpression*, const SourcePositionPair &);
   void Leave(SgSwitchStatement*);

   void Enter(SgCaseOptionStmt* &, SgExprListExp*);
   void Leave(SgCaseOptionStmt*);

   void Enter(SgDefaultOptionStmt* &);
   void Leave(SgDefaultOptionStmt*);

   void Enter(SgWhileStmt* &, SgExpression*);
   void Leave(SgWhileStmt*);

   SgEnumVal* ReplaceEnumVal(SgEnumType*, const std::string &);

// Expressions
//
   void Enter(SgFunctionCallExp* &, std::string &name, SgExprListExp* params);

// Jovial specific nodes
//
   void Enter(SgJovialDefineDeclaration* &, const std::string &define_string);
   void Leave(SgJovialDefineDeclaration*);

   void Enter(SgJovialDirectiveStatement* &, const std::string &directive_string, bool is_compool=false);
   void Leave(SgJovialDirectiveStatement*);

   void Enter(SgJovialForThenStatement* &);
   void Leave(SgJovialForThenStatement*);

   void Enter(SgJovialCompoolStatement* &, const std::string &, const SourcePositionPair &);
   void Leave(SgJovialCompoolStatement*);

   void Enter(SgJovialOverlayDeclaration* &, SgExpression* address, SgExprListExp* overlay);
   void Leave(SgJovialOverlayDeclaration*);

   void Enter(SgJovialTableStatement* &, const std::string &, const SourcePositionPair &, bool is_block=false);
   void Leave(SgJovialTableStatement*);

private:
   TraversalContext context_;

   void setSourcePosition(SgLocatedNode* node, const SourcePosition &start, const SourcePosition &end);
   void importModule(const std::string &module_name);

public:
   const TraversalContext & get_context(void) {return context_;}
   void setContext(SgType* type) {context_.type = type;}
   void setActualFunctionParameterScope(SgScopeStatement* scope) {context_.actual_function_param_scope = scope;}

// Helper function
   bool list_contains(const std::list<LanguageTranslation::FunctionModifier>& lst, const LanguageTranslation::FunctionModifier& item)
     {
        return (std::find(lst.begin(), lst.end(), item) != lst.end());
     }

};

// Temporary wrappers for SageInterface functions (needed until ROSE builds with C++17)
//
namespace SageBuilderCpp17 {

   SgType* buildBoolType();
   SgType* buildIntType();
   SgExpression*  buildBoolValExp_nfi(bool value);
   SgExpression*  buildIntVal_nfi(int);
   SgExpression*  buildStringVal_nfi(std::string);
   SgExpression*  buildExprListExp_nfi();
   SgExpression*  buildVarRefExp_nfi(std::string &name, SgScopeStatement* scope = NULL);
   SgExpression*  buildAddOp_nfi(SgExpression* lhs, SgExpression* rhs);
   SgExpression*  buildSubtractOp_nfi(SgExpression* lhs, SgExpression* rhs);
   SgExpression*  buildMultiplyOp_nfi(SgExpression* lhs, SgExpression* rhs);
   SgExpression*  buildDivideOp_nfi(SgExpression* lhs, SgExpression* rhs);
   SgExpression*  buildSubscriptExpression_nfi(SgExpression* lower_bound, SgExpression* upper_bound, SgExpression* stride);
   SgExpression*  buildNullExpression_nfi();
   SgExprListExp* buildExprListExp_nfi(const std::list<SgExpression*> &);

} // namespace SageBuilderCpp17
} // namespace builder
} // namespace Rose

#endif  // ROSE_SAGE_TREE_BUILDER_H_
