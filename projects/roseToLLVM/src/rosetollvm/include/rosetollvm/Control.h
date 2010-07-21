#ifndef CONTROL
#define CONTROL

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <rosetollvm/Option.h>
#include <rosetollvm/RootAstAttribute.h>
#include <rosetollvm/ManagerAstAttribute.h>

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/SourceMgr.h" // contains class SMDiagnostic;
#include "llvm/Assembly/Parser.h"
#include "llvm/Support/raw_ostream.h" // contains raw_fd

class Control {
public: 
    Control(Option &option_) : option(option_),
                               context(llvm::getGlobalContext())
    {}

    ~Control();

    int numLLVMFiles() { return llvm_file_prefixes.size(); }

    std::ostream *addLLVMFile(std::string);

    std::string getLLVMFile(int i) { return llvm_file_prefixes[i]; }

    void generateOutput();

    void generateModules();

    int getLLVMModuleSize();

    llvm::Module* getModuleRef(int index);


    /**
     * The following are id variables used to map Sage nodes into AstAttribute(s). Attributes are set either in CodeAttributesVisitor.cpp or in LLVMAstAttributes.cpp.
     * When a SgNode is associated with the root AstAttribute, it is an indication that that attribute will only be used as a flag.  In other words, there is no content
     * in the AstAttribute root node.  We only check whether or not the pointer returned by get_ttribute() is null.
     */
    static const char *LLVM_AST_ATTRIBUTES,                  // LLVMAstAttributes associated with Rose class SgSourceFile.
                      *LLVM_NAME,                            // StringAstAttibute used to associate a name to some SgNode 
                      *LLVM_SIZE,                            // IntAstAttribute - used to associate a size with an SgType.
                      *LLVM_TYPE,                            // StringAstAttribute - used to associate a LLVM type with an SgType.
                      *LLVM_PARAMETER,                       // AstAttribute - identify an SgInitializedName as a parameter
                      *LLVM_LOCAL_DECLARATIONS,              // FunctionAstAttibute - used to associate function information with an SgFunctionDeclaration
                      *LLVM_REFERENCE_NAME,                  // StringAstAttribute - a LLVM temporary variable that will be used as a reference to an SgVarRefExp
                      *LLVM_REFERENCE_ONLY,                  // AstAttribute - identify a SgExpression to be used as a reference (to a variable) only
                      *LLVM_FUNCTION_ARGUMENTS,              // StringAstAttribute - SgFunctionType
                      *LLVM_GLOBAL_DECLARATION,              // AstAttribute - identify a SgInitializedName as a global variable declaration
                      *LLVM_ALIGN_TYPE,                      // IntAstAttribute - used to associate an alignment boundary to an SgType
                      *LLVM_IF_LABELS,                       // IfAstAttribute - used to associate labels (true, false, end) with an SgIfStmt
                      *LLVM_IF_COMPONENT_LABELS,             // IfComponentAstAttribute - used to associate labels (true, false) with blocks (true, false) of an SgIfStmt
                      *LLVM_WHILE_LABELS,                    // WhileAstAttribute - used to associate labels with an SgWhileStatement
                      *LLVM_DO_LABELS,                       // DoAstAttribute - used to associate labels with a SgDoWhileStmt
                      *LLVM_FOR_LABELS,                      // ForAstAttribute - used to associate labels  with a SgForStatement
      //                      *LLVM_LABEL, // UNUSED... Remove
                      *LLVM_DEFAULT_VALUE,                   // StringAstAttribute - used to associate a default value to with an SgType 
                      *LLVM_EXPRESSION_RESULT_NAME,          // StringAstAttribute - used to associate the name of the final result of an expression evaluation with an SgExpression 
                      *LLVM_BUFFERED_OUTPUT,                 // ForAstAttribute - used to identify a SgExpression as the increment of a for loop and associate the loop labels with it.
                      *LLVM_TRIVIAL_CAST,                    // AstAttribute - identify a SgCastExpression as trivial (not needed).
                      *LLVM_AGGREGATE,                       // AggregateAstAttribute - used to associate an aggregate initializer or a type with an SgInitializedName or an SgAggregateInitializer
                      *LLVM_NEGATION_NAME,                   // StringAstAttribute - used to associate extra temporary name with SgSubtractOp or SgMinusAssignOp when negation computation needed
                      *LLVM_IS_BOOLEAN,                      // AstAttribute - identify a SgExpression as having a boolean result
                      *LLVM_EXTEND_BOOLEAN,                  // StringAstAttribute - used to associate extra temporary name witn an SgExpression that requires casting from boolean to integer
                      *LLVM_CLASS_MEMBER,                    // IntAstAttrbute - used to associate the index position of a member in a class (first element is indexed at 0)
                      *LLVM_UNION_NAME,                      // StringAstAttribute - 

                      *LLVM_SELECT_CONDITIONAL,
                      *LLVM_CONDITIONAL_LABELS,
                      *LLVM_CONDITIONAL_TEST,
                      *LLVM_CONDITIONAL_COMPONENT_LABELS,
                      *LLVM_LOGICAL_AND,
                      *LLVM_LOGICAL_AND_LHS,
                      *LLVM_LOGICAL_AND_RHS,
                      *LLVM_LOGICAL_OR,
                      *LLVM_LOGICAL_OR_LHS,
                      *LLVM_LOGICAL_OR_RHS,
                      *LLVM_SWITCH_INFO,
                      *LLVM_SWITCH_EXPRESSION,
                      *LLVM_CASE_INFO,
                      *LLVM_DEFAULT_LABEL,
                      *LLVM_STRING_INITIALIZATION,
                      *LLVM_STRING_SIZE,
                      *LLVM_POINTER_TO_INT_CONVERSION,
                      *LLVM_ARRAY_TO_POINTER_CONVERSION,
                      *LLVM_DO_NOT_CAST,
                      *LLVM_INTEGRAL_PROMOTION,
                      *LLVM_INTEGRAL_DEMOTION,
                      *LLVM_NULL_VALUE,
                      *LLVM_IGNORE,
                      *LLVM_COERCE,
                      *LLVM_RETURNED_STRUCTURE_BIT_CAST,
                      *LLVM_BIT_CAST,

                      *LLVM_POINTER_DIFFERENCE_DIVIDER,
                      *LLVM_EXPRESSION_RESULT_TYPE,
                      *LLVM_ARGUMENT_COERCE,
                      *LLVM_ARGUMENT_BIT_CAST,
                      *LLVM_ARGUMENT_EXPRESSION_RESULT_NAME,
                      *LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION,
                      *LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION,
                      *LLVM_LEFT_OPERAND_INTEGRAL_PROMOTION,
                      *LLVM_RIGHT_OPERAND_INTEGRAL_PROMOTION,
                      *LLVM_ARGUMENT_INTEGRAL_PROMOTION,
                      *LLVM_ARGUMENT_INTEGRAL_DEMOTION,
                      *LLVM_FUNCTION_VISITED,
                      *LLVM_FUNCTION_NEEDS_REVISIT,
                      *LLVM_STRUCTURE_PADDING,
                      *LLVM_SIZEOF_NODE,
                      *LLVM_COMPILER_GENERATED,
                      *LLVM_MANAGER_ATTRIBUTE,
                      *LLVM_TRANSLATE
                      ;

    /**
     *
     */
    void SetAttribute(SgNode *, const char *);

    /**
     *
     */
    void SetAttribute(SgNode *, const char *, RootAstAttribute *);

    /**
     *
     */
    void UpdateAttribute(SgNode *n, const char *code, RootAstAttribute *attribute) {
        SetAttribute(n, code, attribute);
    }

    /**
     * Remove the code attribute from the SgNode n if it exists.
     */
    void RemoveAttribute(SgNode *n, const char *code) {
        RootAstAttribute *attribute = (RootAstAttribute *) n -> getAttribute(code);
        if (attribute) {
            n -> removeAttribute(code);
            attribute -> removeOwner();
	}
    }

private:

    Option &option;
    llvm::LLVMContext &context;
    llvm::SMDiagnostic error;

    std::vector<ManagerAstAttribute *> manager_attributes;

    std::vector<std::string> llvm_file_prefixes;
    std::vector<std::ostringstream *> llvm_streams;
    std::vector<llvm::Module *> llvm_modules;
};

#endif
