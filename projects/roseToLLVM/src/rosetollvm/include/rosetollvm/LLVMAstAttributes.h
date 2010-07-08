#ifndef LLVM_ATTRIBUTES
#define LLVM_ATTRIBUTES

#include <map>
#include <vector>
#include <string>

#include <rosetollvm/CodeEmitter.h>

#include "CodeEmitter.h"
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"

using namespace llvm;

#include <rosetollvm/RootAstAttribute.h>
#include <rosetollvm/ManagerAstAttribute.h>
#include <rosetollvm/SgTypeAstAttribute.h>
#include <rosetollvm/IntAstAttribute.h>
#include <rosetollvm/StringAstAttribute.h>
#include <rosetollvm/IfAstAttribute.h>
#include <rosetollvm/IfComponentAstAttribute.h>
#include <rosetollvm/ConditionalAstAttribute.h>
#include <rosetollvm/ConditionalComponentAstAttribute.h>
#include <rosetollvm/LogicalAstAttribute.h>
#include <rosetollvm/WhileAstAttribute.h>
#include <rosetollvm/DoAstAttribute.h>
#include <rosetollvm/ForAstAttribute.h>
#include <rosetollvm/SwitchAstAttribute.h>
#include <rosetollvm/CaseAstAttribute.h>
#include <rosetollvm/AggregateAstAttribute.h>
#include <rosetollvm/DeclarationsAstAttribute.h>
#include <rosetollvm/FunctionAstAttribute.h>
#include <rosetollvm/SizeOfAstAttribute.h>

class LLVMAstAttributes : public RootAstAttribute {
    StringSet string_table,
              used_function_table,
              defined_function_table;
    std::vector<int> length;
    int getLength(const char *);
    std::vector<SgNode *> global_declaration;
    bool needs_memcopy;
    std::vector<SgInitializedName *> remote_global_declarations;

    std::string intPointerTarget;
    SgType *pointerSizeIntegerType;

    Control &control;
    CodeEmitter codeOut;
    LLVMContext &context;
    Module *module;
    IRBuilder<> builder;

    std::vector<SgFunctionDeclaration *> additionalFunctions;
    std::vector<FunctionAstAttribute *> additionalFunctionAttributes;

    bool isHex(char c) { return isdigit(c) || ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')); }

    void processClassDeclaration(SgClassType *);

public:

    LLVMAstAttributes(Option &option, Control &control_, std::string input_file) : codeOut(option, control, input_file),
                                                                                   control(control_),
                                                                                   needs_memcopy(false),
                                                                                   tmp_count(0),
                                                                                   tmp_int_count(0),
                                                                                   context(getGlobalContext()),
                                                                                   builder(context)
    {
        int byte_size = sizeof(void *);

        if (byte_size == sizeof(int)) {
            pointerSizeIntegerType = SgTypeInt::createType();
        }
        else if (byte_size == sizeof(long)) {
            pointerSizeIntegerType = SgTypeLong::createType();
        }
        else if (byte_size == sizeof(long long)) {
            pointerSizeIntegerType = SgTypeLongLong::createType();
        }
        else assert(! "capable of processing pointer type on this machine");
        setLLVMTypeName(pointerSizeIntegerType);

        intPointerTarget = ((StringAstAttribute *) pointerSizeIntegerType -> getAttribute(Control::LLVM_TYPE)) -> getValue();
    }

    CodeEmitter &getCodeOut() {
        return codeOut;
    }

    enum TEMP_KIND {
        TEMP_GENERIC,
        TEMP_INT,
        TEMP_INCREMENT,
        TEMP_DECREMENT,
        TEMP_NEGATION,
        TEMP_ADD,
        TEMP_ADD_PTR,
        TEMP_SUBTRACT,
        TEMP_SUBTRACT_PTR,
        TEMP_MULTIPLY,
        TEMP_DIVIDE,
        TEMP_REMAINDER,
        TEMP_COMPARE,
        TEMP_ARRAY,
        TEMP_ARRAY_INDEX,
        TEMP_CALL,
        TEMP_IF_TRUE,
        TEMP_IF_FALSE,
        TEMP_IF_END,
        TEMP_WHILE_CONDITION,
        TEMP_WHILE_BODY,
        TEMP_WHILE_END,
        TEMP_DO_CONDITION,
        TEMP_DO_BODY,
        TEMP_DO_END,
        TEMP_FOR_CONDITION,
        TEMP_FOR_BODY,
        TEMP_FOR_INCREMENT,
        TEMP_FOR_END,
        TEMP_SWITCH_END_LABEL,
        TEMP_CASE_LABEL,
        TEMP_DEFAULT_LABEL,
        TEMP_RETURN,
        TEMP_CONVERT,
        TEMP_NOT,
        TEMP_AND,
        TEMP_OR,
        TEMP_XOR,
        TEMP_COND,
        TEMP_COND_TRUE,
        TEMP_COND_FALSE,
        TEMP_COND_END,
        TEMP_LOGICAL_AND_RHS,
        TEMP_LOGICAL_AND_END,
        TEMP_LOGICAL_OR_RHS,
        TEMP_LOGICAL_OR_END,
        TEMP_SHIFT_LEFT,
        TEMP_SHIFT_RIGHT,
        TEMP_ANONYMOUS,
        TEMP_LHS_POINTER_TO_INT,
        TEMP_RHS_POINTER_TO_INT,
        TEMP_COERCE,
        TEMP_AGGREGATE,
        TEMP_POINTER_DIFFERENCE_ARITHMETIC_SHIFT_RIGHT,
        TEMP_POINTER_DIFFERENCE_DIVISION 
    };
    long tmp_count,
         tmp_int_count;
    void resetIntCount() { tmp_int_count = 0; }
    const std::string getTemp(TEMP_KIND k);
    const std::string getFunctionTemp(std::string, std::string);

    bool needsMemcopy() { return needs_memcopy; }
    void setNeedsMemcopy() { needs_memcopy = true; }

    const std::string filter(const std::string);

    const std::string filter(const std::string, int);

    int insertString(std::string s) {
        return string_table.insert(filter(s).c_str());
    }

    int insertString(std::string s, int size) {
        return string_table.insert(filter(s, size).c_str());
    }

    int numStrings() { return string_table.size(); }

    const char *getString(int i) { return string_table[i]; }

    int getStringLength(int i) {
        int start = length.size();
        if (length.size() < string_table.size()) {
            length.resize(string_table.size());
            for (int i = start; i < length.size(); i++)
                length[i] = getLength(string_table[i]);
        }

        return length[i];
    }

    void insertFunction(std::string f) {
        used_function_table.insert(f.c_str());
    }

    int numFunctions() { return used_function_table.size(); }

    const char* getFunction(int i) { return used_function_table[i]; }

    void insertAdditionalFunction(SgFunctionDeclaration *function) {
        additionalFunctions.push_back(function);
    }

    int numAdditionalFunctions() { return additionalFunctions.size(); }

    SgFunctionDeclaration *getAdditionalFunction(int i) { return additionalFunctions[i]; }

    void insertAdditionalFunctionAttribute(FunctionAstAttribute *function) {
        additionalFunctionAttributes.push_back(function);
    }

    int numAdditionalFunctionAttributes() { return additionalFunctionAttributes.size(); }

    FunctionAstAttribute *getAdditionalFunctionAttribute(int i) { return additionalFunctionAttributes[i]; }

    void insertGlobalDeclaration(SgNode *n) {
        global_declaration.push_back(n);
    }

    int numGlobalDeclarations() { return global_declaration.size(); }

    SgNode *getGlobalDeclaration(int i) { return global_declaration[i]; }

    void insertDefinedFunction(std::string f) { defined_function_table.insert(f.c_str()); }
    bool isDefinedFunction(const char *fname) { return defined_function_table.contains(fname); }

    int numRemoteGlobalDeclarations() { return remote_global_declarations.size(); }
    SgInitializedName *getRemoteGlobalDeclaration(int i) { return remote_global_declarations[i]; }
    void insertRemoteGlobalDeclaration(SgInitializedName *decl) {
        remote_global_declarations.push_back(decl);
    }

    SgType *getSourceType(SgType *type) {
      /*
        if (dynamic_cast<SgTypedefType *>(type)) {
            return getSourceType(isSgTypedefType(type) -> get_base_type());
        }
        else if (dynamic_cast<SgModifierType *>(type)) {
            return getSourceType(isSgModifierType(type) -> get_base_type());
        }
        return type;
      */
        return type -> stripTypedefsAndModifiers();
    }

    const std::string getFunctionName(SgFunctionSymbol *);
    const std::string getFunctionSignature(SgFunctionSymbol *);
    const std::string getGlobalStringConstantName(int);
    const std::string getGlobalStringReference(int);

    /**
     * This map is only required because there is a bad bug in the Rose compiler - The declaration
     * of structures (SgClassDeclaration) and their associated class types (SgClassType) is not unique.
     * Therefore, we create this map to keep track of LLVM attributes associated with any given structure.
     */
    std::map<std::string, DeclarationsAstAttribute *> class_map;

    /**
     * Map each global variable name into its declaration. If both a definition and a declaration
     * is specified then map the name into the declaration,
     */
    std::map<std::string, std::vector<SgInitializedName *> > global_variable_declaration_map;

    const std::string setLLVMTypeName(SgType *n);

    std::string getIntegerPointerTarget() {
        return intPointerTarget;
    }

    SgType *getPointerSizeIntegerType() {
        return pointerSizeIntegerType;
    }

    /**
     *
     */
    SgType *getExpressionType(SgExpression *expression) {
        assert(expression -> attributeExists(Control::LLVM_EXPRESSION_RESULT_TYPE));
        return ((SgTypeAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE)) -> getType();
    }

    /**
     * Compute the LLVM name of an integer with the given number of bytes.
     */
    std::string getIntegerBitSize(int num_bytes) {
        std::ostringstream out;
        out << "i" << (num_bytes * 8);
        return out.str();
    }

    /**
     * An integral type is structure type that can be stored in an integral type: char, short, int, long.
     */
    int integralStructureType(SgType *type) {
      int size = (isSgClassType(getSourceType(type))
                        ? ((IntAstAttribute *) type -> getAttribute(Control::LLVM_SIZE)) -> getValue()
                        : 0);
        return (size == 1 || size == 2 || size == 4 || size == 8 ? size : 0);
    }
};

#endif
