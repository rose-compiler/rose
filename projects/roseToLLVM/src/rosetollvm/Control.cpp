#include <rosetollvm/Control.h>
#include <rosetollvm/Optimize.h>
//#include <rosetollvm/ObjectGenerator.h>

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Assembly/AsmAnnotationWriter.h"
#include "llvm/Analysis/Verifier.h"

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
           *Control::LLVM_UNION_NAME = "z",

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
           *Control::LLVM_DO_NOT_CAST = "S",
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
           *Control::LLVM_LEFT_OPERAND_INTEGRAL_PROMOTION = "7",
           *Control::LLVM_RIGHT_OPERAND_INTEGRAL_PROMOTION = "8",
           *Control::LLVM_ARGUMENT_INTEGRAL_PROMOTION = "9",
           *Control::LLVM_ARGUMENT_INTEGRAL_DEMOTION = "~",
           *Control::LLVM_FUNCTION_VISITED = "`",
           *Control::LLVM_FUNCTION_NEEDS_REVISIT = "!",
           *Control::LLVM_STRUCTURE_PADDING = "@",
           *Control::LLVM_SIZEOF_NODE = "#",
           *Control::LLVM_COMPILER_GENERATED = "$",
           *Control::LLVM_MANAGER_ATTRIBUTE = "%",
           *Control::LLVM_TRANSLATE = "^"
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
    for (int i = 0; i < llvm_modules.size(); i++) {
        delete llvm_modules[i];
    }

    /**
     *
     */
    for (int i = 0; i < manager_attributes.size(); i++) {
        ManagerAstAttribute *manager = manager_attributes[i];
SgNode *owner = manager -> getOwner();
int starting_number_of_attributes = manager -> startingNumberOfAttributes();
//std::cerr << "Class " << owner -> class_name() << " started with " << starting_number_of_attributes << " attributes and ended with " 
//          << owner -> numberOfAttributes() << " attributes." << std::endl;
//std::cerr.flush();

        delete manager;
//std::cerr << "Class " << owner -> class_name() << " ended with " << owner -> numberOfAttributes() << " attributes." << std::endl;
//std::cerr.flush();
assert(owner -> numberOfAttributes() == starting_number_of_attributes);
    }
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


/**
 * Process the list of intermediate files and generte the output.
 */
void Control::generateOutput() {
    string command;

    /**
     * 
     */
    for (int i = 0; i < llvm_file_prefixes.size(); i++ ) {
        string file_prefix = llvm_file_prefixes[i];

//cerr << "Starting Assembly"
//     << endl;
//cerr.flush();
        /**
         * Assemble
         */
        // Every time we convert the file to a new form, we immediately
        // delete the last form in order to minimize memory usage.
        assert(llvm_streams[i]);
        char *llvm_cstr;
        {
            std::string llvm_string = llvm_streams[i]->str();
            delete llvm_streams[i];
            llvm_streams[i] = NULL;
            llvm_cstr = new char[llvm_string.size() + 1];
            llvm_string.copy(llvm_cstr, llvm_string.size());
            llvm_cstr[llvm_string.size()] = '\0';
        }
        Module *module = ParseAssemblyString(llvm_cstr, NULL, error, context);
        if (! module) {
            cerr << "Internal error: failure parsing generated LLVM"
                 << " assembly: ";
            llvm::raw_stderr_ostream stderr_stream;
            string module_name = string("roseToLLVM:") + file_prefix + ".c";
            error.Print(module_name.c_str(), stderr_stream);
            abort();
        }
        module -> setModuleIdentifier(file_prefix + ".c");
        delete [] llvm_cstr;
        verifyModule(*module);
//cerr << "Ending Assembly"
//     << endl;
//cerr.flush();

        /**
         * Optimize
         */
        if (option.getOptimize().size() > 0) {
//cerr << "Starting Optimization"
//     << endl;
//cerr.flush();
            Optimize opt(option);
            opt.process(module);
//cerr << "Ending Optimization"
//     << endl;
//cerr.flush();
        }

        /**
         * If bitcode output was requested, take care of it here.
         */
        if (option.isEmitLLVMBitcode()) {
            string bc_name = file_prefix;
            bc_name += ".bc";
            std::string ErrorInfo;
            raw_fd_ostream bc_ostream(bc_name.c_str(), ErrorInfo, raw_fd_ostream::F_Binary);
            WriteBitcodeToFile(module, bc_ostream);
            bc_ostream.close();
        }

        /**
         * If LLVM source output was requested, take care of it here.
         */
//        if (option.isEmitLLVM()) {
            string ll_name = file_prefix;
            ll_name += ".ll";
            std::string ErrorInfo;
            raw_fd_ostream ll_ostream(ll_name.c_str(), ErrorInfo);
            AssemblyAnnotationWriter AAW;
            module -> print(ll_ostream, &AAW);
            ll_ostream.close();
//        }

        /**
         * generate .o files.
         */
        //        ObjectGenerator og(option);
	//        if (! og.process(module)) {
	//            // Scream!
	//        }

	llvm_modules.push_back(module);
    }
    
    /**
     * Generate executable.
     */
    if (! option.isDebugOutput()) {
        command += "llvmc -lm ";
        command += option.getOptions();
        for (int i = 0; i < llvm_file_prefixes.size(); i++ ) {
            command +=  " ";
            command +=  llvm_file_prefixes[i];
            command +=  ".ll ";
        }
        system(command.c_str());
    }

    /**
     * Remove .o files.
     */
    if (! option.isCompileOnly()) {
        for (int i = 0; i < llvm_file_prefixes.size(); i++ ) {
            string file_prefix = llvm_file_prefixes[i];
            string oname = file_prefix;
            oname += ".o";
//            remove(oname.c_str());
        }
    }

    /**
     * Remove .ll files.
     */
    if (! option.isEmitLLVM()) {
        for (int i = 0; i < llvm_file_prefixes.size(); i++ ) {
            string file_prefix = llvm_file_prefixes[i];
            string ll_name = file_prefix;
            ll_name += ".ll";
            remove(ll_name.c_str());
        }
    }
}

void Control::generateModules()
{
     string command;

    /**
     * 
     */
    for (int i = 0; i < llvm_file_prefixes.size(); i++ ) {
        string file_prefix = llvm_file_prefixes[i];

        /**
         * Assemble
         */
        // Every time we convert the file to a new form, we immediately
        // delete the last form in order to minimize memory usage.
        assert(llvm_streams[i]);
        char *llvm_cstr;
        {
            std::string llvm_string = llvm_streams[i]->str();
            delete llvm_streams[i];
            llvm_streams[i] = NULL;
            llvm_cstr = new char[llvm_string.size() + 1];
            llvm_string.copy(llvm_cstr, llvm_string.size());
            llvm_cstr[llvm_string.size()] = '\0';
        }
        Module *module = ParseAssemblyString(llvm_cstr, NULL, error, context);
        if (! module) {
            cerr << "Internal error: failure parsing generated LLVM"
                 << " assembly: ";
            llvm::raw_stderr_ostream stderr_stream;
            string module_name = string("roseToLLVM:") + file_prefix + ".c";
            error.Print(module_name.c_str(), stderr_stream);
            abort();
        }
        module -> setModuleIdentifier(file_prefix + ".c");
        delete [] llvm_cstr;
        verifyModule(*module);

	llvm_modules.push_back(module);
    }     
}

Module* Control::getModuleRef(int index)
{
    assert(llvm_modules.size() > 0 && index < llvm_modules.size());
    return llvm_modules[index];
}

int Control::getLLVMModuleSize()
{
    assert(llvm_modules.size() > 0);
    return llvm_modules.size();
}
