#include "rosetollvm/Control.h"
#include "rosetollvm/ConstantValue.h"
#include "rosetollvm/LLVMAstAttributes.h"
#include "rosetollvm/CodeAttributesVisitor.h"
#include "rosetollvm/CodeGeneratorVisitor.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Target/TargetMachine.h"

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
           *Control::LLVM_DIMENSIONS = "p",
           *Control::LLVM_DEFAULT_VALUE = "q",
           *Control::LLVM_CLASS_MEMBER = "r",
           *Control::LLVM_BUFFERED_OUTPUT = "s",
           *Control::LLVM_TRIVIAL_CAST = "t",
           *Control::LLVM_AGGREGATE = "u",
           *Control::LLVM_NEGATION_NAME = "v",
           *Control::LLVM_IS_BOOLEAN = "w",
           *Control::LLVM_EXTEND_BOOLEAN = "x",
           *Control::LLVM_EXPRESSION_RESULT_NAME = "y",
           *Control::LLVM_IMAGINARY_RESULT_NAME = "z",

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
           *Control::LLVM_STRING_INDEX = "O",
           *Control::LLVM_STRING_INITIALIZATION = "P",
           *Control::LLVM_POINTER_TO_INT_CONVERSION = "Q",
           *Control::LLVM_ARRAY_TO_POINTER_CONVERSION = "R",
           *Control::LLVM_ARRAY_NAME_REFERENCE = "S",
           *Control::LLVM_INTEGRAL_PROMOTION = "T",
           *Control::LLVM_INTEGRAL_DEMOTION = "U",
           *Control::LLVM_NULL_VALUE = "V", 
           *Control::LLVM_IGNORE = "W",
           *Control::LLVM_COERCE = "X",
           *Control::LLVM_RETURNED_STRUCTURE_BIT_CAST = "Y",
           *Control::LLVM_BIT_CAST = "Z",

           *Control::LLVM_POINTER_DIFFERENCE_DIVIDER = "0",
           *Control::LLVM_EXPRESSION_RESULT_TYPE = "1",
           *Control::LLVM_CLASS_COERCE = "2",
           *Control::LLVM_CLASS_BIT_CAST = "3",
           *Control::LLVM_ARGUMENT_EXPRESSION_RESULT_NAME = "4",
           *Control::LLVM_OP_AND_ASSIGN_INTEGRAL_PROMOTION = "5",
           *Control::LLVM_OP_AND_ASSIGN_INTEGRAL_DEMOTION = "6",
           *Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION = "LLVM_OP_AND_ASSIGN_INT_TO_FP_PROMOTION",
           *Control::LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION = "LLVM_OP_AND_ASSIGN_INT_TO_FP_DEMOTION",
           *Control::LLVM_OP_AND_ASSIGN_FP_PROMOTION = "LLVM_OP_AND_ASSIGN_FP_PROMOTION",
           *Control::LLVM_OP_AND_ASSIGN_FP_DEMOTION = "LLVM_OP_AND_ASSIGN_FP_DEMOTION",
           *Control::LLVM_REAL = "7",
           *Control::LLVM_IMAGINARY = "8",
           *Control::LLVM_ARRAY_BIT_CAST = "9",
           *Control::LLVM_CONSTANT_VALUE = "~",
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
           *Control::LLVM_NEEDS_STACK  = "_",
         //  = "-",
         //  = "+",
         //  = "=",
         //  = "{",
         //  = "}",
         //  = "[",
         //  = "]",
         //  = "|",
         //  = "\\",
         //  = ":",
         //  = ";",
         //  = "\"",
         //  = "\'",
         //  = "<",
         //  = ">",
         //  = ",",
         //  = ".",
         //  = "?",
         //  = "/",

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
CodeAttributesVisitor *Control::getAdHocAttributesVisitor(LLVMAstAttributes *attributes) {
    assert(ad_hoc_attributes_visitor);
    ad_hoc_attributes_visitor -> setupAdHocVisitor(attributes);
    return ad_hoc_attributes_visitor;
}

/**
 *
 */
CodeGeneratorVisitor *Control::getAdHocGeneratorVisitor(LLVMAstAttributes *attributes) {
    assert(ad_hoc_generator_visitor);
    ad_hoc_generator_visitor -> setupAdHocVisitor(attributes);
    return ad_hoc_generator_visitor;
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


extern int llc(vector<string> &, char *);

/**
 * Process the list of intermediate files and generate the output.
 */
std::vector<llvm::Module*> Control::generateOutput() {
    std::vector<llvm::Module*> res; // TODO: should be a vector of std::unique_ptr<llvm::Module> !

    string link_command;
    
    /**
     * 
     */
/*
void setModuleIdentifier (StringRef ID)
     Set the module identifier. More...
 
void setSourceFileName (StringRef Name)
     Set the module's original source file name. More...
 
void setDataLayout (StringRef Desc)
     Set the data layout. More...
 
void setDataLayout (const DataLayout &Other)
 
void setTargetTriple (StringRef T)
     Set the target triple. More...
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
        module -> setSourceFileName(file_prefix + ".c");

// TODO: Remove this !
/*  
cout
  << "Data Layout String = "
  << module -> getDataLayoutStr()
  << endl
  << "Target Triple = "
  << module -> getTargetTriple()
  << endl;
cout.flush();
*/
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

            int ret = llc(option.llcOptions(), (char *) bc_name.c_str()); // system(("llc -filetype=obj " + bc_name).c_str());
            if (ret != 0) {
                cout << "***rose2llvm error: llc could not generate object file." << endl;
                cout.flush();
                exit(ret);
            }

            if (! option.isCompileOnly()) {
                link_command += " ";
                link_command += file_prefix;
                link_command += ".o";
            }
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

    /**
     * If we need to link the program, we do so here.
     */
    if (link_command.size()) {
        link_command = (string) "gcc" + " " + option.otherOptions() + option.outputFilename() + link_command;
// TODO: Remove this !!!
/*
cout << "***Issuing Link command: " << link_command
     << endl
     << "*** Compile only?  ... " << (option.isCompileOnly() ? "yes!" : "no!")
     << endl;
cout.flush();
*/
        int ret = system(link_command.c_str());
        if (ret != 0) {
            cout << "***rose2llvm error: gcc could not generate executable file." << endl;
            cout.flush();
            exit(ret);
        }
    }
    
    return res;
}


/**
 * Unpack a CREAL macro call and return its expression argument.
 */
SgExpression *Control::isComplexMacro(SgFunctionCallExp *n, string macro_name) {
    SgFunctionRefExp *function = isSgFunctionRefExp(n -> get_function());
    if (function) {
        if (function -> get_symbol() && function -> get_symbol() -> get_name() == macro_name) {
            vector<SgExpression *> args = n -> get_args() -> get_expressions();
            if (args.size() == 1 && (args[0] -> get_type())) {
                return args[0];
            }
        }
    }
    return NULL;
}


/**
 * Convert a long to its string representation.
 */
string Control::IntToString(long long l) {
    stringstream out;
    out << l;
    return out.str();
}

/**
 * Convert a float to its string representation.
 */
string Control::FloatToString(float x) {
    APFloat f(x);
    string s = APFloatToString(f);
    return s;
}

/**
 * Convert a double to its string representation.
 */
string Control::DoubleToString(double x) {
    APFloat f(x);
    string s = APFloatToString(f);
    return s;
}

/**
 * Convert a long double to its string representation.
 * 
 * TODO: figure out how to properly convert a "long double" literal.
 *       Currently, APFloat will truncate this value and only process
 *       its integer part. Thus 3.1416 will produce  "3"
 */
string Control::LongDoubleToString(long double x) {
    APFloat f(APFloat::x87DoubleExtended(), x);
    string s = APFloatToString(f);
    return s;
}

/**
 *
 */
string Control::APFloatToString(APFloat &f) {
/*
    SmallString<256> Str;

    f.toString(Str, 0, 0);
    
    return Str.str().str();
*/
  
/**
 * This code was copied from llvm-4.0.1 AsmWriter and altered slightly here...
 *
 * AsmWriter.cpp:
 *
 *     static void WriteConstantInternal(raw_ostream &Out, const Constant *CV,
 *                                       TypePrinting &TypePrinter,
 *                                       SlotTracker *Machine,
 *                                       const Module *Context) { ... }
 */

    string output_string;
    llvm::raw_string_ostream out(output_string);

    if (&(f.getSemantics()) == &APFloat::IEEEsingle() ||
        &(f.getSemantics()) == &APFloat::IEEEdouble()) {
        // We would like to output the FP constant value in exponential notation,
        // but we cannot do this if doing so will lose precision.  Check here to
        // make sure that we only output it in exponential format if we can parse
        // the value back and get the same value.
        //
        bool ignored;
        bool isDouble = &(f.getSemantics()) == &APFloat::IEEEdouble();
        bool isInf = f.isInfinity();
        bool isNaN = f.isNaN();
        if (!isInf && !isNaN) {
            double Val = isDouble ? f.convertToDouble() :
                                    f.convertToFloat();
            //
            // TODO: THIS NEEDS TO BE FIXED !!!
            //
            /*    
            SmallString<256> StrVal;
            raw_svector_ostream(StrVal) << Val;

            // Check to make sure that the stringized number is not some string like
            // "Inf" or NaN, that atof will accept, but the lexer will not.  Check
            // that the string matches the "[-+]?[0-9]" regex.
            //
            if ((StrVal[0] >= '0' && StrVal[0] <= '9') ||
                ((StrVal[0] == '-' || StrVal[0] == '+') &&
                 (StrVal[1] >= '0' && StrVal[1] <= '9'))) {
                // Reparse stringized version!
                if (APFloat(APFloat::IEEEdouble(), StrVal).convertToDouble() == Val) {
                    return StrVal.str().str();
                }
            }
            */

            out << Val;
            string result = out.str(); // call to str() is required in order to flush buffer ...

            // Check to make sure that the stringized number is not some string like
            // "Inf" or NaN, that atof will accept, but the lexer will not.  Check
            // that the string matches the "[-+]?[0-9]" regex.
            //
            if (result.length() >= 2) {
                if ((result[0] >= '0' && result[0] <= '9') ||
                    ((result[0] == '-' || result[0] == '+') &&
                     (result[1] >= '0' && result[1] <= '9'))) {
                    // Reparse stringized version!
                    if (APFloat(APFloat::IEEEdouble(), result/* StrVal*/).convertToDouble() == Val) {
                        return result;
                    }
                }
            }
        }
 
        output_string = ""; // Clear the string...

        // Otherwise we could not reparse it to exactly the same value, so we must
        // output the string in hexadecimal format!  Note that loading and storing
        // floating point types changes the bits of NaNs on some hosts, notably
        // x86, so we must not use these types.
        static_assert(sizeof(double) == sizeof(uint64_t),
                      "assuming that double is 64 bits!");
        APFloat apf = f;
        // Floats are represented in ASCII IR as double, convert.
        if (!isDouble)
            apf.convert(APFloat::IEEEdouble(), APFloat::rmNearestTiesToEven,
                              &ignored);
        out << format_hex(apf.bitcastToAPInt().getZExtValue(), 0, /*Upper=*/true);
        return out.str();
    }

    // Either half, or some form of long double.
    // These appear as a magic letter identifying the type, then a
    // fixed number of hex digits.
    out << "0x";
    APInt API = f.bitcastToAPInt();
    if (&(f.getSemantics()) == &APFloat::x87DoubleExtended()) {
        out << 'K';

        out << format_hex_no_prefix(API.getHiBits(16).getZExtValue(), 4,
                                    /*Upper=*/true);
        out << format_hex_no_prefix(API.getLoBits(64).getZExtValue(), 16,
                                    /*Upper=*/true);
    }
    else if (&(f.getSemantics()) == &APFloat::x87DoubleExtended()) {
        out << 'L';
        out << format_hex_no_prefix(API.getLoBits(64).getZExtValue(), 16,
                                    /*Upper=*/true);
        out << format_hex_no_prefix(API.getHiBits(64).getZExtValue(), 16,
                                    /*Upper=*/true);
    }
    else if (&(f.getSemantics()) == &APFloat::PPCDoubleDouble()) {
        out << 'M';
        out << format_hex_no_prefix(API.getLoBits(64).getZExtValue(), 16,
                                    /*Upper=*/true);
        out << format_hex_no_prefix(API.getHiBits(64).getZExtValue(), 16,
                                    /*Upper=*/true);
    }
    else if (&(f.getSemantics()) == &APFloat::IEEEhalf()) {
        out << 'H';
        out << format_hex_no_prefix(API.getZExtValue(), 4,
                                    /*Upper=*/true);
    }
    else
        llvm_unreachable("Unsupported floating point type");

    return out.str();
}

bool Control::isIntegerType(SgType *t) {
    return (t != NULL) &&
           (isSgTypeBool(t) ||
            isSgTypeChar(t) ||
            isSgTypeSignedChar(t) ||
            isSgTypeUnsignedChar(t) ||
            isSgTypeShort(t) ||
            isSgTypeSignedShort(t) ||
            isSgTypeUnsignedShort(t) ||
            isSgTypeInt(t) ||
            isSgTypeSignedInt(t) ||
            isSgTypeUnsignedInt(t) ||
            isSgTypeLong(t) ||
            isSgTypeSignedLong(t) ||
            isSgTypeUnsignedLong(t) ||
            isSgTypeLongLong(t) ||
            isSgTypeSignedLongLong(t) ||
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
            APFloat f(APFloat::IEEEsingle(), isSgFloatVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
        else if (isSgTypeDouble(t)) {
            APFloat f(APFloat::IEEEdouble(), isSgFloatVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
        else {
            APFloat f(APFloat::x87DoubleExtended(), isSgFloatVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
    }
    else if (isSgDoubleVal(n)) {
        if (isSgTypeFloat(t)) {
            APFloat f(APFloat::IEEEsingle(), isSgDoubleVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
        else if (isSgTypeDouble(t)) {
            APFloat f(APFloat::IEEEdouble(), isSgDoubleVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
        else {
            APFloat f(APFloat::x87DoubleExtended(), isSgDoubleVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
    }
    else if (isSgLongDoubleVal(n)) {
        if (isSgTypeFloat(t)) {
            APFloat f(APFloat::IEEEsingle(), isSgLongDoubleVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
        else if (isSgTypeDouble(t)) {
            APFloat f(APFloat::IEEEdouble(), isSgLongDoubleVal(n) -> get_valueString());
            str = APFloatToString(f);
        }
        else {
            APFloat f(APFloat::x87DoubleExtended(), isSgLongDoubleVal(n) -> get_valueString());
            str = APFloatToString(f);
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
        APFloat f(APFloat::IEEEsingle(), (float) d);
        return APFloatToString(f);
    }
    else if (isSgTypeDouble(t)) {
        APFloat f(APFloat::IEEEdouble(), (double) d);
        return APFloatToString(f);
    }
    
    ROSE2LLVM_ASSERT(isSgTypeLongDouble(t));
    APFloat f(APFloat::x87DoubleExtended(), (long double) d);
    return APFloatToString(f);
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
if(! (isIntegerValue(node) && isFloatingType(type))) {
cout << "About to crash on value " << node -> class_name() << " with type " << type -> class_name() << endl;
cout.flush();
}
        ROSE2LLVM_ASSERT(isIntegerValue(node) && isFloatingType(type));
        return floatingValue(node, type);
    }
}


std::string Control::primitiveCast(ConstantValue &x, SgType *type) {
    if (isSgPointerType(type) && x.hasIntValue() && x.int_value == 0) {
        return "null";
    }
    else if (x.hasIntValue()) {
        return IntToString(x.int_value);
    }
    else if (x.hasFloatValue()) {
        return FloatToString(x.float_value);
    }
    else if (x.hasDoubleValue()) {
        return DoubleToString(x.double_value);
    }
    else if (x.hasLongDoubleValue()) {
        return LongDoubleToString(x.long_double_value);
    }
    else if (x.string_literal) {
        return x.string_literal -> get_value();
    }
    else {
cout << "I don't yet know how to convert from type "
     << (x.hasFloatComplexValue()
             ? "Float Complex"
             : x.hasDoubleComplexValue()
                   ? "Double Complex"
                   : x.hasLongDoubleComplexValue() 
                       ? "Long Double Complex"
                       : x.string_literal
                             ? ("string: " + x.string_literal -> get_value())
                             : "Unknown")
     << " to type "
     << type -> class_name()
     << endl;
cout.flush();
        ROSE2LLVM_ASSERT(false);
    }
}
