#include "rosetollvm/Control.h"
#include <rosetollvm/LLVMAstAttributes.h>
#include <llvm/ADT/StringExtras.h>

void __rose2llvm_fail (const char *__assertion, const char *__file, unsigned int __line) {
    std::cerr << "*** Bad assertion in file " << __file << " at line " << __line << ": " << __assertion << std::endl;
    std::cerr.flush();
    abort();
}

using namespace std;
using namespace llvm;

const char *Control::LLVM_AST_ATTRIBUTES = "LLVM",
           *Control::LLVM_NAME = "a",
           *Control::LLVM_SIZE = "b",
           *Control::LLVM_TYPE = "c",
           *Control::LLVM_PARAMETER = "d",
           *Control::LLVM_LOCAL_DECLARATIONS = "e",
           *Control::LLVM_REFERENCE_NAME = "f",
           *Control::LLVM_REFERENCE_ONLY = "g",
           *Control::LLVM_FUNCTION_ARGUMENTS = "h",
           *Control::LLVM_GLOBAL_DECLARATION = "i",
           *Control::LLVM_ALIGN_TYPE = "j",
           *Control::LLVM_ALIGN_VAR = "LLVM_ALIGN_VAR",
           *Control::LLVM_IF_LABELS = "k",
           *Control::LLVM_IF_COMPONENT_LABELS = "l",
           *Control::LLVM_WHILE_LABELS = "m",
           *Control::LLVM_DO_LABELS = "n",
           *Control::LLVM_FOR_LABELS = "o",
//           *Control::LLVM_LABEL = "p", // UNUSED, remove
           *Control::LLVM_DEFAULT_VALUE = "q",
           *Control::LLVM_EXPRESSION_RESULT_NAME = "r",
           *Control::LLVM_BUFFERED_OUTPUT = "s",
           *Control::LLVM_TRIVIAL_CAST = "t",
           *Control::LLVM_AGGREGATE = "u",
           *Control::LLVM_NEGATION_NAME = "v",
           *Control::LLVM_IS_BOOLEAN = "w",
           *Control::LLVM_EXTEND_BOOLEAN = "x",
           *Control::LLVM_CLASS_MEMBER = "y",

           *Control::LLVM_SELECT_CONDITIONAL = "A",
           *Control::LLVM_CONDITIONAL_LABELS = "B",
           *Control::LLVM_CONDITIONAL_TEST = "C",
           *Control::LLVM_CONDITIONAL_COMPONENT_LABELS = "D",
           *Control::LLVM_LOGICAL_AND = "E",
           *Control::LLVM_LOGICAL_AND_LHS = "F",
           *Control::LLVM_LOGICAL_AND_RHS = "G",
           *Control::LLVM_LOGICAL_OR = "H",
           *Control::LLVM_LOGICAL_OR_LHS = "I",
           *Control::LLVM_LOGICAL_OR_RHS = "J",
           *Control::LLVM_SWITCH_INFO = "K",
           *Control::LLVM_SWITCH_EXPRESSION = "L",
           *Control::LLVM_CASE_INFO = "M",
           *Control::LLVM_DEFAULT_LABEL = "N",
           *Control::LLVM_STRING_INITIALIZATION = "O",
           *Control::LLVM_STRING_SIZE = "P",
           *Control::LLVM_POINTER_TO_INT_CONVERSION = "Q",
           *Control::LLVM_ARRAY_TO_POINTER_CONVERSION = "R",
           *Control::LLVM_INTEGRAL_PROMOTION = "T",
           *Control::LLVM_INTEGRAL_DEMOTION = "U",
           *Control::LLVM_NULL_VALUE = "V", 
           *Control::LLVM_IGNORE = "W",
           *Control::LLVM_COERCE = "X",
           *Control::LLVM_RETURNED_STRUCTURE_BIT_CAST = "Y",
           *Control::LLVM_BIT_CAST = "Z",

           *Control::LLVM_POINTER_DIFFERENCE_DIVIDER = "0",
           *Control::LLVM_EXPRESSION_RESULT_TYPE = "1",
           *Control::LLVM_ARGUMENT_COERCE = "2",
           *Control::LLVM_ARGUMENT_BIT_CAST = "3",
           *Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME = "4",
           *Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION = "5",
           *Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION = "6",
           *Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION = "LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION",
           *Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION = "LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION",
           *Control::LLVM_OP_AND_ASSIGN_FP_PROMOTION = "LLVM_OP_AND_ASSIGN_FP_PROMOTION",
           *Control::LLVM_OP_AND_ASSIGN_FP_DEMOTION = "LLVM_OP_AND_ASSIGN_FP_DEMOTION",
           *Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION = "9",
           *Control::LLVM_ARGUMENT_INTEGRAL_DEMOTION = "~",
           *Control::LLVM_FUNCTION_VISITED = "`",
           *Control::LLVM_FUNCTION_NEEDS_REVISIT = "!",
           *Control::LLVM_STRUCTURE_PADDING = "@",
           *Control::LLVM_SIZEOF_NODE = "#",
           *Control::LLVM_COMPILER_GENERATED = "$",
           *Control::LLVM_MANAGER_ATTRIBUTE = "%",
           *Control::LLVM_TRANSLATE = "^",
           *Control::LLVM_COST_ANALYSIS = "&",
           *Control::LLVM_BOOLEAN_CAST = "*",
           *Control::LLVM_DECLARATION_TYPE = "(",
           *Control::LLVM_GLOBAL_CONSTANT_NAME = ")",
           *Control::LLVM_FIELD_OFFSET = "LLVM_FIELD_OFFSET"
           ;

/**
 *
 */
Control::~Control() {
    /**
     *
     */
    for (int i = 0; i < llvm_streams.size(); i++) {
        delete llvm_streams[i];
    }

    /**
     *
     */
    for (int i = 0; i < manager_attributes.size(); i++) {
        ManagerAstAttribute *manager = manager_attributes[i];

//
// TODO: REMOVE ME !!!
//
SgNode *owner = manager -> getOwner();
int starting_number_of_attributes = manager -> startingNumberOfAttributes();
//std::cerr << "Class " << owner -> class_name() << " started with " << starting_number_of_attributes << " attributes and ended with " 
//          << owner -> numberOfAttributes() << " attributes." << std::endl;
//std::cerr.flush();

        delete manager;
//
// TODO: REMOVE ME !!!
//
//std::cerr << "Class " << owner -> class_name() << " ended with " << owner -> numberOfAttributes() << " attributes." << std::endl;
//std::cerr.flush();
ROSE2LLVM_ASSERT(owner -> numberOfAttributes() == starting_number_of_attributes);
    }
    for (vector<SgNode*>::iterator itr = owned_nodes.begin(), end = owned_nodes.end(); itr != end; ++itr)
        delete *itr;
}


/**
 *
 */
void Control::SetAttribute(SgNode *n, const char *code) {
    SetAttribute(n, code, new RootAstAttribute());
}

/**
 *
 */
void Control::SetAttribute(SgNode *n, const char *code, RootAstAttribute *attribute) {
    ManagerAstAttribute *manager = (ManagerAstAttribute *) n -> getAttribute(Control::LLVM_MANAGER_ATTRIBUTE);
    if (! manager) {
        manager = new ManagerAstAttribute(n, Control::LLVM_MANAGER_ATTRIBUTE);
        n -> addNewAttribute(Control::LLVM_MANAGER_ATTRIBUTE, manager);
        manager_attributes.push_back(manager);
    }

    if (n -> attributeExists(code)) {
        RemoveAttribute(n, code);
    }

    n -> addNewAttribute(code, attribute);
    attribute -> setOwner(n, code);
    manager -> addAttribute(attribute);
}

/**
 * Create an intermediate file (.ll) for this source file (.c) and add it to the list of intermediate files to be processed later.
 */
ostream *Control::addLLVMFile(string input_file_string) {
    const char *input_file = input_file_string.c_str();
    string llvm_file;
    const char *dot = strrchr(input_file, '.');

    if (dot == NULL) {
        llvm_file = input_file;
    }
    else {
        for (const char *p = input_file;  p < dot; p++) {
            llvm_file += *p;
        }
    }
    llvm_file_prefixes.push_back(llvm_file);
    llvm_streams.push_back(new std::ostringstream);
    return llvm_streams.back();
}

void Control::emitLLVM(string &file_prefix, llvm::Module *module) {
    string ll_name = file_prefix;
    ll_name += ".ll";
    std::error_code EC;
    raw_fd_ostream ll_ostream(ll_name.c_str(), EC, sys::fs::F_RW);
    AssemblyAnnotationWriter AAW;
    module -> print(ll_ostream, &AAW);
    ll_ostream.close();
}


/**
 * Process the list of intermediate files and generate the output.
 */
std::vector<llvm::Module*> Control::generateOutput() {
  std::vector<llvm::Module*> res; // TODO: should be a vector of std::unique_ptr<llvm::Module> !
    /**
     * 
     */
    for (int i = 0; i < llvm_file_prefixes.size(); i++ ) {
        string file_prefix = llvm_file_prefixes[i];

        /**
         * Assemble
         */
        ROSE2LLVM_ASSERT(llvm_streams[i]);
	std::string llvm_string = llvm_streams[i] -> str(); // copy the string from the stream.
        StringRef llvm_string_ref(llvm_string); // Wrap it in a string ref.
        std::unique_ptr<llvm::Module> module = parseAssemblyString(llvm_string_ref, error, context);
        if (! module.get()) {
            cerr << "Internal error: failure parsing generated LLVM"
                 << " assembly: ";
            llvm::raw_fd_ostream stderr_stream(STDERR_FILENO, false, true);
            string module_name = string("roseToLLVM:") + file_prefix + ".c";
            error.print(module_name.c_str(), stderr_stream);
            abort();
        }
        module -> setModuleIdentifier(file_prefix + ".c");
        verifyModule(*module);

        /**
         * If bitcode output was requested, take care of it here.
         */
        if (option.isEmitLLVMBitcode()) {
            string bc_name = file_prefix;
            bc_name += ".bc";
            std::error_code EC;
            raw_fd_ostream bc_ostream(bc_name.c_str(), EC, sys::fs::F_RW);
            WriteBitcodeToFile(module.get(), bc_ostream);
            bc_ostream.close();
        }

        /**
         * If LLVM source output was requested, take care of it here.
         */
        if (option.isEmitLLVM()) {
            emitLLVM(file_prefix, module.get());
        }

        /**
         * Keep track of this module
         */
        res.push_back(module.get()); // TODO: Push module here !!!

	//
        // Every time we convert the file to a new form, we immediately
        // delete the last form in order to minimize memory usage.
	//
        delete llvm_streams[i];
        llvm_streams[i] = NULL;
    }
    return res;
}

/**
 * Convert a float to its string representation.
 */
string Control::IntToString(long l) {
    stringstream out;
    out << l;
    return out.str();
}

/**
 * Convert a float to its string representation.
 */
string Control::FloatToString(float x) {
    APFloat f(x);
    string s = FloatToString(f);
    return s;
}

/**
 * Convert a double to its string representation.
 */
string Control::FloatToString(double x) {
    APFloat f(x);
    string s = FloatToString(f);
    return s;
}

/**
 * This code was copied from llvm-3.4 AsmWriter and altered slightly here...
 *
 * AsmWriter.cpp:
 *
 *     static void WriteConstantInternal(raw_ostream &Out, const Constant *CV,
 *                                       TypePrinting &TypePrinter,
 *                                       SlotTracker *Machine,
 *                                       const Module *Context) { ...
 */
string Control::FloatToString(APFloat &f) {
    stringstream out;

    if (&f.getSemantics() == &APFloat::IEEEsingle || &f.getSemantics() == &APFloat::IEEEdouble) {
        // We would like to output the FP constant value in exponential notation,
        // but we cannot do this if doing so will lose precision.  Check here to
        // make sure that we only output it in exponential format if we can parse
        // the value back and get the same value.
        //
        bool ignored;
        bool isHalf = &f.getSemantics()==&APFloat::IEEEhalf;
        bool isDouble = &f.getSemantics()==&APFloat::IEEEdouble;
        bool isInf = f.isInfinity();
        bool isNaN = f.isNaN();
        if (!isHalf && !isInf && !isNaN) {
            double Val = isDouble ? f.convertToDouble() : f.convertToFloat();
            SmallString<128> StrVal;
            raw_svector_ostream(StrVal) << Val;

            // Check to make sure that the stringized number is not some string like
            // "Inf" or NaN, that atof will accept, but the lexer will not.  Check
            // that the string matches the "[-+]?[0-9]" regex.
            //
            if ((StrVal[0] >= '0' && StrVal[0] <= '9') ||
                ((StrVal[0] == '-' || StrVal[0] == '+') &&
                 (StrVal[1] >= '0' && StrVal[1] <= '9'))) {
                 // Reparse stringized version!
                if (APFloat(APFloat::IEEEdouble, StrVal).convertToDouble() == Val) {
                    return StrVal.str().str();
                }
            }
        }
        // Otherwise we could not reparse it to exactly the same value, so we must
        // output the string in hexadecimal format!  Note that loading and storing
        // floating point types changes the bits of NaNs on some hosts, notably
        // x86, so we must not use these types.
        assert(sizeof(double) == sizeof(uint64_t) && "assuming that double is 64 bits!");
        char Buffer[40];
        APFloat apf = f;
        // Halves and floats are represented in ASCII IR as double, convert.
        if (!isDouble) {
            apf.convert(APFloat::IEEEdouble, APFloat::rmNearestTiesToEven, &ignored);
        }
        out << "0x" << utohex_buffer(uint64_t(apf.bitcastToAPInt().getZExtValue()), Buffer+40);
        return out.str();
    }

    // Either half, or some form of long double.
    // These appear as a magic letter identifying the type, then a
    // fixed number of hex digits.
    out << "0x";
    // Bit position, in the current word, of the next nibble to print.
    int shiftcount;

    if (&f.getSemantics() == &APFloat::x87DoubleExtended) {
        out << 'K';
        // api needed to prevent premature destruction
        APInt api = f.bitcastToAPInt();
        const uint64_t* p = api.getRawData();
        uint64_t word = p[1];
        shiftcount = 12;
        int width = api.getBitWidth();
        for (int j=0; j<width; j+=4, shiftcount-=4) {
            unsigned int nibble = (word>>shiftcount) & 15;
            if (nibble < 10)
                out << (unsigned char)(nibble + '0');
            else
                out << (unsigned char)(nibble - 10 + 'A');
            if (shiftcount == 0 && j+4 < width) {
                word = *p;
                shiftcount = 64;
                if (width-j-4 < 64)
                    shiftcount = width-j-4;
            }
        }

        return out.str();
    } else if (&f.getSemantics() == &APFloat::IEEEquad) {
        shiftcount = 60;
        out << 'L';
    } else if (&f.getSemantics() == &APFloat::PPCDoubleDouble) {
        shiftcount = 60;
        out << 'M';
    } else if (&f.getSemantics() == &APFloat::IEEEhalf) {
        shiftcount = 12;
        out << 'H';
    } else
        llvm_unreachable("Unsupported floating point type");

    // api needed to prevent premature destruction
    APInt api = f.bitcastToAPInt();
    const uint64_t* p = api.getRawData();
    uint64_t word = *p;
    int width = api.getBitWidth();
    for (int j=0; j<width; j+=4, shiftcount-=4) {
        unsigned int nibble = (word>>shiftcount) & 15;
        if (nibble < 10)
            out << (unsigned char)(nibble + '0');
        else
            out << (unsigned char)(nibble - 10 + 'A');
        if (shiftcount == 0 && j+4 < width) {
            word = *(++p);
            shiftcount = 64;
            if (width-j-4 < 64)
                shiftcount = width-j-4;
        }
    }

    return out.str();
}

bool Control::isIntegerType(SgType *t) {
    return (t != NULL) &&
           (isSgTypeBool(t) ||
            isSgTypeChar(t) ||
            isSgTypeUnsignedChar(t) ||
            isSgTypeShort(t) ||
            isSgTypeUnsignedShort(t) ||
            isSgTypeInt(t) ||
            isSgTypeUnsignedInt(t) ||
            isSgTypeLong(t) ||
            isSgTypeUnsignedLong(t) ||
            isSgTypeLongLong(t) ||
            isSgTypeUnsignedLongLong(t));
}

bool Control::isFloatingType(SgType *t) {
    return (t != NULL) &&
           (isSgTypeFloat(t) ||
            isSgTypeDouble(t) ||
            isSgTypeLongDouble(t));
}

bool Control::isIntegerValue(SgValueExp *n) {
   return (n != NULL) &&
          (isSgBoolValExp(n) ||
           isSgCharVal(n) ||
           isSgUnsignedCharVal(n) ||
           isSgShortVal(n) ||
           isSgUnsignedShortVal(n) ||
           isSgIntVal(n) ||
           isSgEnumVal(n) ||
           isSgUnsignedIntVal(n) ||
           isSgLongIntVal(n) ||
           isSgUnsignedLongVal(n) ||
           isSgLongLongIntVal(n) ||
           isSgUnsignedLongLongIntVal(n));
}

bool Control::isFloatingValue(SgValueExp *n) {
   return (n != NULL) &&
          (isSgFloatVal(n) ||
           isSgDoubleVal(n) ||
           isSgLongDoubleVal(n));
}

bool Control::isPrimitiveValue(SgExpression *n) {
    SgValueExp *value = isSgValueExp(n);
    return isIntegerValue(value) || isFloatingValue(value);
}

std::string Control::primitiveSource(SgValueExp *n, SgType *t) {
    string str;
    std::stringstream out;
    ROSE2LLVM_ASSERT(n != NULL);
    if (isSgBoolValExp(n)) {
        str = (isSgBoolValExp(n) -> get_value() ? "true" : "false");
    }
    else if (isSgCharVal(n)) {
        str = isSgCharVal(n) -> get_valueString();
    }
    else if (isSgUnsignedCharVal(n)) {
        str = isSgUnsignedCharVal(n) -> get_valueString();
    }
    else if (isSgShortVal(n)) {
        out << isSgShortVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgUnsignedShortVal(n)) {
        out << isSgUnsignedShortVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgIntVal(n)) {
        out << isSgIntVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgEnumVal(n)) {
        out << isSgEnumVal(n) -> get_value(); 
        str = out.str();
    }
    else if (isSgUnsignedIntVal(n)) {
        out << isSgUnsignedIntVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgLongIntVal(n)) {
        out << isSgLongIntVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgUnsignedLongVal(n)) {
        out << isSgUnsignedLongVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgLongLongIntVal(n)) {
        out << isSgLongLongIntVal(n) -> get_value();
        str = out.str();
    }
    else if (isSgUnsignedLongLongIntVal(n)) {
        out << isSgUnsignedLongLongIntVal(n)-> get_value();
        str = out.str();
    }
    else if (isSgFloatVal(n)) {
        if (isSgTypeFloat(t)) {
            APFloat f(APFloat::IEEEsingle, isSgFloatVal(n) -> get_valueString());
            str = FloatToString(f);
        }
        else if (isSgTypeDouble(t)) {
            APFloat f(APFloat::IEEEdouble, isSgFloatVal(n) -> get_valueString());
            str = FloatToString(f);
        }
        else {
            APFloat f(APFloat::IEEEquad, isSgFloatVal(n) -> get_valueString());
            str = FloatToString(f);
        }
    }
    else if (isSgDoubleVal(n)) {
        if (isSgTypeFloat(t)) {
            APFloat f(APFloat::IEEEsingle, isSgDoubleVal(n) -> get_valueString());
            str = FloatToString(f);
        }
        else if (isSgTypeDouble(t)) {
            APFloat f(APFloat::IEEEdouble, isSgDoubleVal(n) -> get_valueString());
            str = FloatToString(f);
        }
        else {
            APFloat f(APFloat::IEEEquad, isSgDoubleVal(n) -> get_valueString());
            str = FloatToString(f);
        }
    }
    else if (isSgLongDoubleVal(n)) {
        if (isSgTypeFloat(t)) {
            APFloat f(APFloat::IEEEsingle, isSgLongDoubleVal(n) -> get_valueString());
            str = FloatToString(f);
        }
        else if (isSgTypeDouble(t)) {
            APFloat f(APFloat::IEEEdouble, isSgLongDoubleVal(n) -> get_valueString());
            str = FloatToString(f);
        }
        else {
            APFloat f(APFloat::IEEEquad, isSgLongDoubleVal(n) -> get_valueString());
            str = FloatToString(f);
        }
    }

    return str;
}

std::string Control::floatingValue(SgValueExp *n, SgType *t) {
    double d;
    ROSE2LLVM_ASSERT(n != NULL);

    if (isSgBoolValExp(n)) {
        d = (isSgBoolValExp(n) -> get_value() ? 1.0 : 0.0);
    }
    else if (isSgCharVal(n)) {
        d = (double) ((int) isSgCharVal(n) -> get_value());
    }
    else if (isSgUnsignedCharVal(n)) {
        d = (double) ((unsigned) isSgUnsignedCharVal(n) -> get_value());
    }
    else if (isSgShortVal(n)) {
        d = (double) ((short) isSgShortVal(n) -> get_value());
    }
    else if (isSgUnsignedShortVal(n)) {
        d = (double) ((unsigned short) isSgUnsignedShortVal(n) -> get_value());
    }
    else if (isSgIntVal(n)) {
        d = (double) ((int) isSgIntVal(n) -> get_value());
    }
    else if (isSgEnumVal(n)) {
        d = (double) ((int) isSgEnumVal(n) -> get_value());
    }
    else if (isSgUnsignedIntVal(n)) {
        d = (double) ((unsigned) isSgUnsignedIntVal(n) -> get_value());
    }
    else if (isSgLongIntVal(n)) {
        d = (double) ((long) isSgLongIntVal(n) -> get_value());
    }
    else if (isSgUnsignedLongVal(n)) {
        d = (double) ((unsigned long) isSgUnsignedLongVal(n) -> get_value());
    }
    else if (isSgLongLongIntVal(n)) {
        d = (double) ((long long) isSgLongLongIntVal(n) -> get_value());
    }
    else if (isSgUnsignedLongLongIntVal(n)) {
        d = (double) ((unsigned long long) isSgUnsignedLongLongIntVal(n)-> get_value());
    }
    else {
        ROSE2LLVM_ASSERT(false);
    }


    if (isSgTypeFloat(t)) {
        APFloat f((float) d);
        return FloatToString(f);
    }

    APFloat f((double) d);
    return FloatToString(f);
}


std::string Control::integerValueofAPFloat(APFloat &f) {
    ROSE2LLVM_ASSERT(f.isFinite());
    SmallVector<char, 128> sv;
    f.toString(sv);
    string string_value = "";
    for (int i = 0; i < sv.size(); i++) {
        string_value += sv[i];
    }
    return string_value;
}

std::string Control::integerValue(SgValueExp *n, SgType *t) {
    ROSE2LLVM_ASSERT(n != NULL);
    if (isSgFloatVal(n)) {
        float fractional_part, integer_part;
        fractional_part = modf (isSgFloatVal(n) -> get_value(), &integer_part);
        APFloat f(integer_part);
        return integerValueofAPFloat(f);
    }
    else if (isSgDoubleVal(n)) {
        double fractional_part, integer_part;
        fractional_part = modf (isSgDoubleVal(n) -> get_value(), &integer_part);
        APFloat f(integer_part);
        return integerValueofAPFloat(f);
    }
    else if (isSgLongDoubleVal(n)) {
        /* long */ double fractional_part, integer_part; // TODO: Error - the type "long double" is not supported by LLVM's APFloat
        fractional_part = modf (isSgLongDoubleVal(n) -> get_value(), &integer_part);
        APFloat f(integer_part);
        return integerValueofAPFloat(f);
    }
    else {
        ROSE2LLVM_ASSERT(false);
    }
    return "";
}

std::string Control::primitiveCast(SgValueExp *node, SgType *type) {
    type = LLVMAstAttributes::getSourceType(type);

    if (isIntegerValue(node) && (isIntegerType(type) || isSgPointerType(type))) {
        string str = primitiveSource(node, type);
        return (isSgPointerType(type) && str.compare("0") == 0 ? "null" : str);
    }
    else if (isFloatingValue(node) && isFloatingType(type)) {
        return primitiveSource(node, type);
    }
    else if (isIntegerType(type)) {
        ROSE2LLVM_ASSERT(isFloatingValue(node));
        return integerValue(node, type);
    }
    else {
        ROSE2LLVM_ASSERT(isIntegerValue(node) && isFloatingType(type));
        return floatingValue(node, type);
    }
}
