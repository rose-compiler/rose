#ifndef LLVM_ATTRIBUTES
#define LLVM_ATTRIBUTES

#include <map>
#include <vector>
#include <string>

#include <rosetollvm/CodeEmitter.h>

#include "CodeEmitter.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

#include <rosetollvm/RootAstAttribute.h>
#include <rosetollvm/ManagerAstAttribute.h>
#include <rosetollvm/SgCastAstAttribute.h>
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
    std::vector<SgNode *> global_declaration;
    bool needs_memcopy;
    bool needs_stacksave;
    std::vector<SgInitializedName *> remote_global_declarations;

    std::string intPointerTarget;
    SgType *pointerSizeIntegerType;
    SgTypeInt *typeInt;
    SgTypeVoid *typeVoid;
    SgPointerType *voidStarType;

    Control &control;
    Option &option;

    CodeEmitter codeOut;
    llvm::LLVMContext &context;
    llvm::Module *module;
    llvm::IRBuilder<> builder;

    std::vector<SgFunctionDeclaration *> additionalFunctions;
    std::vector<FunctionAstAttribute *> additionalFunctionAttributes;
    std::vector<llvm::MDNode *> mdNodes;
    std::map<llvm::MDNode *, int> mdIndices;
    std::map<std::string, int> functionPragmaMetadataIndices;

    /**
     * This map is used to map each label into a unique number.
     */
    std::map<std::string, int> label_map;

    bool isHex(char c) { return isdigit(c) || ((c >= 'a' && c <= 'f') || (c >= 'a' && c <= 'f')); }

    void processClassDeclaration(SgClassType *);
    void prepFor2DigitHex(std::ostream &s) {
        s << std::setfill('0') << std::setw(2) << std::hex;
    }

public:

    LLVMAstAttributes(Option &option_, Control &control_, std::string input_file) : codeOut(option_, control, input_file),
                                                                                    option(option_),
                                                                                    control(control_),
                                                                                    needs_memcopy(false),
                                                                                    needs_stacksave(false),
                                                                                    context(control.getGlobalContext()),
                                                                                    builder(context)
    {
        int byte_size = sizeof(void *);

        typeInt = SgTypeInt::createType();
        setLLVMTypeName(typeInt);
    
        typeVoid = SgTypeVoid::createType();
        voidStarType = SgPointerType::createType(typeVoid);
        setLLVMTypeName(voidStarType);
    
        if (byte_size == sizeof(int)) {
            pointerSizeIntegerType = SgTypeInt::createType();
        }
        else if (byte_size == sizeof(long)) {
            pointerSizeIntegerType = SgTypeLong::createType();
        }
        else if (byte_size == sizeof(long long)) {
            pointerSizeIntegerType = SgTypeLongLong::createType();
        }
        else ROSE2LLVM_ASSERT(! "capable of processing pointer type on this machine");
        setLLVMTypeName(pointerSizeIntegerType);

        intPointerTarget = ((StringAstAttribute *) pointerSizeIntegerType -> getAttribute(Control::LLVM_TYPE)) -> getValue();
    }

    Control &getControl() { return control; }
    Option &getOption() { return  option;}

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
        TEMP_POINTER_DIFFERENCE_DIVISION,
        TEMP_LABEL
    };

    static long tmp_count,
                tmp_int_count;
    static void resetIntCount() { tmp_int_count = 0; }

    const std::string getTemp(TEMP_KIND k);
    const std::string getFunctionTemp(std::string, std::string);

    std::string findLabel(SgLabelStatement *);

    bool needsMemcopy() { return needs_memcopy; }
    void setNeedsMemcopy() { needs_memcopy = true; }

    bool needsStacksave() { return needs_stacksave; }
    void setNeedsStacksave() { needs_stacksave = true; }

    class StringLiteral {
    public:
        std::string value;
        int length;
        int size;
    };
    
    StringLiteral preprocessString(SgStringVal *, int);

    int insertString(SgStringVal *); 
    
    int insertString(SgStringVal *, int);
    
// TODO: Remove this !!!
/*
    int insertString(std::string s) {
        return string_table.insert(filter(s).c_str());
    }

    int insertString(std::string s, int size) {
        return string_table.insert(filter(s, size).c_str());
    }
*/
    
    int numStrings() { return string_table.size(); }

    const char *getString(int i) { return string_table[i] -> Name(); }

    int getStringLength(int i) { return string_table[i] -> Size(); }
// TODO: Remove this !!!
/*
    {
        int start = length.size();
        if (length.size() < string_table.size()) {
            length.resize(string_table.size());
            for (int i = start; i < length.size(); i++)
                length[i] = getLength(string_table[i]);
        }

        return length[i];
    }
*/

    int numFunctions() { return used_function_table.size(); }
    const char* getFunction(int i) { return used_function_table[i] -> Name(); }
    void insertFunction(std::string f) {
        used_function_table.insert(f.c_str(), f.size());
    }

    int numAdditionalFunctions() { return additionalFunctions.size(); }
    SgFunctionDeclaration *getAdditionalFunction(int i) { return additionalFunctions[i]; }
    void insertAdditionalFunction(SgFunctionDeclaration *function) {
        additionalFunctions.push_back(function);
    }

    int numAdditionalFunctionAttributes() { return additionalFunctionAttributes.size(); }
    FunctionAstAttribute *getAdditionalFunctionAttribute(int i) { return additionalFunctionAttributes[i]; }
    void insertAdditionalFunctionAttribute(FunctionAstAttribute *function) {
        additionalFunctionAttributes.push_back(function);
    }

    int numGlobalDeclarations() { return global_declaration.size(); }
    SgNode *getGlobalDeclaration(int i) { return global_declaration[i]; }
    void insertGlobalDeclaration(SgNode *n) {
        global_declaration.push_back(n);
    }

    void insertDefinedFunction(std::string f) { defined_function_table.insert(f.c_str(), f.size()); }
    bool isDefinedFunction(const char *fname) { return defined_function_table.contains(fname); }

    int numRemoteGlobalDeclarations() { return remote_global_declarations.size(); }
    SgInitializedName *getRemoteGlobalDeclaration(int i) { return remote_global_declarations[i]; }
    void insertRemoteGlobalDeclaration(SgInitializedName *decl) {
        remote_global_declarations.push_back(decl);
    }

    static SgType *getSourceType(SgType *type) {
        type = type -> stripTypedefsAndModifiers();
        SgTypeOfType *type_of_type = isSgTypeOfType(type);
        return (type_of_type ? getSourceType(type_of_type -> get_base_type()) : type);
    }

    const std::string getFunctionName(SgFunctionSymbol *);
    const std::string getFunctionSignature(SgFunctionSymbol *);
    const std::string getGlobalStringConstantName(int);
    const std::string getGlobalStringReference(int);
    const std::string getGlobalStringReference(int, string);

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

    SgType *getTypeInt() {
        return typeInt;
    }

    SgType *getVoidStarType() {
        return voidStarType;
    }


    /**
     * Check whether or not type1 is identical to type2.
     */
    static bool isTrivialCast(SgType *type1, SgType *type2);


    /**
     *
     */
    SgType *getExpressionType(SgExpression *expression) {
        SgTypeAstAttribute *attribute = (SgTypeAstAttribute *) expression -> getAttribute(Control::LLVM_EXPRESSION_RESULT_TYPE);
        if (! attribute) {
            SgType *expression_type = getSourceType(expression -> get_type());
            attribute = new SgTypeAstAttribute(expression_type);
            control.SetAttribute(expression, Control::LLVM_EXPRESSION_RESULT_TYPE, attribute);
            setLLVMTypeName(expression_type);
        }
        return attribute -> getType();
    }

    bool isValignType(SgType *);
    bool isUnsignedType(SgType *);
    bool isFloatType(SgType *);
    bool isIntegerType(SgType *);
    bool isBooleanType(SgType *);

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

    int addMetadata(llvm::MDNode *node);
    std::string addDebugMetadata(SgNode const *node,
                                 FunctionAstAttribute *current_function_decls,
                                 bool just_index = false);

    /** Adds bundle metadata if the node has a bundleAttribute attached.
     */
    std::string addBundleMetadata(SgNode *node);

    /** Adds the vector alignment string if the node has an alignmentAttribute
     * attached.
     */
    std::string addVectorAlignment(SgNode *node);

    /** Vector alignment in bytes.
     * TODO: obtain this value from resource characterization
     */
    unsigned int vectorMemAlignment() { return 16; }

    /** Get the vector alignment as a string.
     */
    std::string getVectorAlignmentStr();

    /** Get the specified alignment as a string.
     */
    std::string getAlignmentStr(int alignment);

    /** Adds noivdep metadata if the node has a SageForInfo attribute
     * attached and its isParallel field is true.
     */
    std::string addIsParallelMetadata(SgNode *node);

    /**
     * \brief Record in metadata the pragmas for a particular function.
     *
     * The form of the metadata is a bit experimental.  Currently, there
     * is a named metadata "!function_pragmas" that contains a list of
     * metadata nodes, one per each function for which there is at least
     * one pragma.  Each such node has the following format:
     *
     *   metadata !{
     *     metadata !"FUNCTION_NAME",
     *     metadata !"PRAGMA_1", metadata !PRAGMA_1_DEBUG_MD_INDEX,
     *     metadata !"PRAGMA_2", metadata !PRAGMA_2_DEBUG_MD_INDEX,
     *     ...
     *     metadata !"PRAGMA_N", metadata !PRAGMA_N_DEBUG_MD_INDEX,
     *     null
     *   }
     *
     * The named metadata is generated by generateMetadataNodes.
     */
    void addFunctionPragmaMetadata(
        std::string const &name, std::vector<SgPragma *> const &pragmas
    );
    void generateMetadataNodes();
};

#endif
