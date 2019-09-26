#ifndef CONTROL
#define CONTROL

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <rosetollvm/Option.h>
#include <rosetollvm/RootAstAttribute.h>
#include <rosetollvm/ManagerAstAttribute.h>
#include <rosetollvm/DimensionAstAttribute.h>
#include <rosetollvm/ConstantValue.h>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/AssemblyAnnotationWriter.h"
#include "llvm/Support/SourceMgr.h" // contains class SMDiagnostic;
#include "llvm/AsmParser/Parser.h"
#include "llvm/Support/raw_ostream.h" // contains raw_fd
#include "llvm/Support/FileSystem.h" // contains raw_fd

// #include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"

#include <llvm/ADT/APFloat.h>

extern void __rose2llvm_fail (const char *__assertion, const char *__file, unsigned int __line);

#define ROSE2LLVM_ASSERT(expr) ((expr) ? __ASSERT_VOID_CAST (0) : __rose2llvm_fail(__STRING(expr), __FILE__, __LINE__))

class CodeAttributesVisitor;
class CodeGeneratorVisitor;
class LLVMAstAttributes;
class FunctionAstAttribute;

class Control {
public: 
    Control(Option &option_) : option(option_),
                               ad_hoc_attributes_visitor(NULL),
                               ad_hoc_generator_visitor(NULL),
                               type_char(NULL),
                               complex_float(NULL),
                               complex_double(NULL),
                               complex_long_double(NULL)
    {
    }

    ~Control();

    void setAdHocAttributesVisitor(CodeAttributesVisitor *ad_hoc_attributes_visitor_) { ad_hoc_attributes_visitor = ad_hoc_attributes_visitor_; }
    CodeAttributesVisitor *getAdHocAttributesVisitor(LLVMAstAttributes *);

    void setAdHocGeneratorVisitor(CodeGeneratorVisitor *ad_hoc_generator_visitor_) { ad_hoc_generator_visitor = ad_hoc_generator_visitor_; }
    CodeGeneratorVisitor *getAdHocGeneratorVisitor(LLVMAstAttributes *);

    int numLLVMFiles() { return llvm_file_prefixes.size(); }

    llvm::LLVMContext &getGlobalContext() { return context; }


    std::ostream *addLLVMFile(std::string);

    std::string getLLVMFile(int i) { return llvm_file_prefixes[i]; }

    /// Caller is responsible for the memory of the resulting modules.
    std::vector<llvm::Module*> generateOutput();

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
                      *LLVM_ALIGN_VAR,                       // IntAstAttribute - specify alignment for variable when different from type's alignment (e.g. from aligned attribute)
                      *LLVM_IF_LABELS,                       // IfAstAttribute - used to associate labels (true, false, end) with an SgIfStmt
                      *LLVM_IF_COMPONENT_LABELS,             // IfComponentAstAttribute - used to associate labels (true, false) with blocks (true, false) of an SgIfStmt
                      *LLVM_WHILE_LABELS,                    // WhileAstAttribute - used to associate labels with an SgWhileStatement
                      *LLVM_DO_LABELS,                       // DoAstAttribute - used to associate labels with a SgDoWhileStmt
                      *LLVM_FOR_LABELS,                      // ForAstAttribute - used to associate labels  with a SgForStatement
                      *LLVM_DIMENSIONS,                      // DimensionAstAttribute - used to associated non-constant expressions used as array dimensions with their declarations.
                      *LLVM_DEFAULT_VALUE,                   // StringAstAttribute - used to associate a default value to with an SgType 
                      *LLVM_CLASS_MEMBER,                    // IntAstAttrbute - used to associate the index position of a member in a class (first element is indexed at 0)
                      *LLVM_BUFFERED_OUTPUT,                 // ForAstAttribute - used to identify a SgExpression as the increment of a for loop and associate the loop labels with it.
                      *LLVM_TRIVIAL_CAST,                    // AstAttribute - identify a SgCastExpression as trivial (not needed).
                      *LLVM_AGGREGATE,                       // AggregateAstAttribute - used to associate an aggregate initializer or a type with an SgInitializedName or an SgAggregateInitializer
                      *LLVM_NEGATION_NAME,                   // StringAstAttribute - used to associate extra temporary name with SgSubtractOp or SgMinusAssignOp when negation computation needed
                      *LLVM_IS_BOOLEAN,                      // AstAttribute - identify a SgExpression as having a boolean result
                      *LLVM_EXTEND_BOOLEAN,                  // StringAstAttribute - used to associate extra temporary name witn an SgExpression that requires casting from boolean to integer
                      *LLVM_EXPRESSION_RESULT_NAME,          // StringAstAttribute - used to associate the name of the final result of an expression evaluation with an SgExpression 
                      *LLVM_IMAGINARY_RESULT_NAME,           // StringAstAttribute - used to associate the name of the final imaginary result of a complex type expression evaluation with an SgExpression 

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
                      *LLVM_STRING_INDEX,
                      *LLVM_STRING_INITIALIZATION,
                      *LLVM_POINTER_TO_INT_CONVERSION,
                      *LLVM_ARRAY_TO_POINTER_CONVERSION,
                      *LLVM_ARRAY_NAME_REFERENCE,
                      *LLVM_INTEGRAL_PROMOTION,
                      *LLVM_INTEGRAL_DEMOTION,
                      *LLVM_NULL_VALUE,
                      *LLVM_IGNORE,
                      *LLVM_COERCE,
                      *LLVM_RETURNED_STRUCTURE_BIT_CAST,
                      *LLVM_BIT_CAST,

                      *LLVM_POINTER_DIFFERENCE_DIVIDER,
                      *LLVM_EXPRESSION_RESULT_TYPE,
                      *LLVM_CLASS_COERCE,
                      *LLVM_CLASS_BIT_CAST,
                      *LLVM_ARGUMENT_EXPRESSION_RESULT_NAME,
                      *LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION,
                      *LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION,
                      *LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION,
                      *LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION,
                      *LLVM_OP_AND_ASSIGN_FP_PROMOTION,
                      *LLVM_OP_AND_ASSIGN_FP_DEMOTION,
                      *LLVM_REAL,
                      *LLVM_IMAGINARY,
                      *LLVM_ARRAY_BIT_CAST,
                      *LLVM_CONSTANT_VALUE,
                      *LLVM_FUNCTION_VISITED,
                      *LLVM_FUNCTION_NEEDS_REVISIT,
                      *LLVM_STRUCTURE_PADDING,
                      *LLVM_SIZEOF_NODE,
                      *LLVM_COMPILER_GENERATED,
                      *LLVM_MANAGER_ATTRIBUTE,
                      *LLVM_TRANSLATE,
                      *LLVM_COST_ANALYSIS,
                      *LLVM_BOOLEAN_CAST,
                      *LLVM_DECLARATION_TYPE,
                      *LLVM_GLOBAL_CONSTANT_NAME,
                      *LLVM_NEEDS_STACK,
                      *LLVM_FIELD_OFFSET // IntAstAttribute - specifies, in bytes, the offset of a struct/union field
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

    /**
     *
     */
    int numDimensionAttributes() { return dimensionAttributes.size(); }
    DimensionAstAttribute *getDimensionAttribute(int i) { return dimensionAttributes[i]; }
    void insertDimensionAttribute(DimensionAstAttribute *dimension_attribute) {
        dimensionAttributes.push_back(dimension_attribute);
    }

    /**
     * Store \c n to be destroyed with \c this.
     */
    template <typename T>
    T *ownNode(T *n) { owned_nodes.push_back(n); return n; }

    /**
     * Construct a chartype with the given subtype.
     */
    SgTypeChar *getTypeChar() {
        if (! type_char) {
            type_char = ownNode(new SgTypeChar());
        }
        return type_char;
    }

    /**
     * Construct a complex type with the given subtype.
     */
    SgTypeComplex *getComplexType(SgType *base_type) {
        if (isSgTypeFloat(base_type)) {
            if (! complex_float) {
                complex_float = ownNode(new SgTypeComplex(base_type));
            }
            return complex_float;
        }
        else if (isSgTypeDouble(base_type)) {
            if (! complex_double) {
                complex_double = ownNode(new SgTypeComplex(base_type));
            }
            return complex_double;
        }
        else if (isSgTypeLongDouble(base_type)) {
            if (! complex_long_double) {
                complex_long_double = ownNode(new SgTypeComplex(base_type));
            }
            return complex_long_double;
        }
        else {
cout << "*** I don't know how to construct Complex type with subtype: "
     << base_type -> class_name()
     << endl;
 cout.flush();
            ROSE2LLVM_ASSERT(! ("know how to construct Complex type with subtype: " + base_type -> class_name()).c_str());
        }
    }


    /**
     * Unpack a CREAL or CIMAG macro call and return its expression argument.
     */
    static SgExpression *isComplexMacro(SgFunctionCallExp *n, string macro_name);
    

    /**
     * 
     */
    static std::string APFloatToString(llvm::APFloat &);
    static std::string FloatToString(float f);
    static std::string DoubleToString(double d);
    static std::string LongDoubleToString(long double x);
    static std::string IntToString(long long l);
    static bool isIntegerType(SgType *t);
    static bool isFloatingType(SgType *t);
    static bool isIntegerValue(SgValueExp *n);
    static bool isFloatingValue(SgValueExp *n);
    static bool isPrimitiveValue(SgExpression *n);
    static std::string primitiveSource(SgValueExp *n, SgType *t);
    static std::string floatingValue(SgValueExp *n, SgType *t);
    static std::string integerValueofAPFloat(llvm::APFloat &f);
    static std::string integerValue(SgValueExp *n, SgType *t);
    static std::string primitiveCast(SgValueExp *, SgType *type);
    static std::string primitiveCast(ConstantValue &, SgType *type);

private:

    Option &option;
    llvm::LLVMContext context;
    llvm::SMDiagnostic error;

    CodeAttributesVisitor *ad_hoc_attributes_visitor;
    CodeGeneratorVisitor  *ad_hoc_generator_visitor;

    SgTypeChar *type_char;

    SgTypeComplex *complex_float;
    SgTypeComplex *complex_double;
    SgTypeComplex *complex_long_double;

    std::vector<DimensionAstAttribute *> dimensionAttributes;

    std::vector<ManagerAstAttribute *> manager_attributes;
    std::vector<SgNode *> owned_nodes;

    std::vector<std::string> llvm_file_prefixes;
    std::vector<std::ostringstream *> llvm_streams;

    void emitLLVM(std::string &, llvm::Module *);
};

#endif
