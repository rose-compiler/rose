#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>

#include <map>
#include <vector>
#include <string>

using namespace std;
using namespace llvm;

string Option::roseToLLVMModulePrefix="--rose2llvm:";

Option::Option(Rose_STL_Container<string> &args) : query(false),
                                                   compile_only(false),
                                                   emit_llvm(false),
                                                   emit_llvm_bitcode(false),
                                                   debug_pre_traversal(false),
                                                   debug_post_traversal(false),
                                                   debug_output(false)
{

    /**
     * Process the options. Note that options that are specific to pace-cc are processed first and not added to the "options" lines.
     */

    if (CommandlineProcessing::isOption(args, "--rose2llvm:", "debug-output", true)) {
    	debug_output = true;
        debugOutputConflict(emit_llvm, "-emit-llvm");
        debugOutputConflict(emit_llvm_bitcode, "-emit-llvm-bitcode");
        debugOutputConflict(optimize.size() > 0, optimize);
        debugOutputConflict(output_file.size() > 0, "-o");
        debugOutputConflict(compile_only, "-c");
    }

    if (CommandlineProcessing::isOption(args, "--rose2llvm:", "debug-pre-traversal", true)) {
        debug_pre_traversal = true;
    }

    if (CommandlineProcessing::isOption(args, "--rose2llvm:", "debug-post-traversal", true)) {
        debug_post_traversal = true;
    }

    if (CommandlineProcessing::isOption(args, "--rose2llvm:", "debug-traversal", true)) {
        debug_pre_traversal = true;
        debug_post_traversal = true;
    }

    if (CommandlineProcessing::isOption(args, "--rose2llvm:", "emit-llvm", true)) {
        emit_llvm = true;
        debugOutputConflict(debug_output, "emit-llvm");
    }

    if (CommandlineProcessing::isOption(args, "--rose2llvm:", "emit-llvm-bitcode", true)) {
        emit_llvm_bitcode = true;
        debugOutputConflict(debug_output, "emit-llvm-bitcode");
    }


    //TODO(vc8) these option processing will probably be revisited when we integrate LLVM correctly as a library
    bool dashO = false;

    SgStringList::iterator argIter;
    // we pass on options to the back end
    for (argIter = args.begin (); argIter != args.end (); ++argIter) {
        const char * optString = (*argIter).c_str ();

        if (strcmp(optString, "-O") == 0 || strcmp(optString, "-O1") == 0 || strcmp(optString, "-O2") == 0 || strcmp(optString, "-O3") == 0 || strcmp(optString, "-O4") == 0) {
            optimize = optString;
            options += optimize;
            options += " ";
            debugOutputConflict(debug_output, optString);
        } else {
            int length = strlen(optString);
            if (strcmp(&(optString[length - 2]), ".c") == 0) {
            } else {
                if (strcmp(optString, "-o") == 0) {
                    dashO = true;
                } else {
                    if (dashO) {
                        // processing -o argument
                        if (optString[0] != '-') {
                            debugOutputConflict(debug_output, "-o");
                            output_file = optString;
                            options += "-o ";
                            options += optString;
                            options += " ";
                            dashO = false;
                        } else {
                            cerr << "Error: The -o option must be followed by a filename" << endl;
                            assert(0);
                        }
                    }
                    else if (strcmp(optString, "-c") == 0) {
                        compile_only = true;
                        debugOutputConflict(debug_output, optString);
                        options += optString;
                        options += " ";
                    }
                }
            }
        }
    }

    if (dashO) {
        cerr << "Error: The -o option must be followed by a filename" << endl;
        assert(0);
    }

    // removing rose2llvm options
    CommandlineProcessing::removeArgs (args, "--rose2llvm");
    return;
} 


/**
 * Create an intermediate file (.ll) for this source file (.c) and add it to the list of intermediate files to be processed later.
 */
/*
string Option::addLLVMFile(string input_file_string) {
    const char *input_file = input_file_string.c_str();
    string llvm_file;
    char *dot = strrchr(input_file, '.');

    if (dot == NULL) {
        llvm_file = input_file;
    }
    else {
        for (const char *p = input_file;  p < dot; p++) {
            llvm_file += *p;
        }
    }
    llvm_file_prefixes.push_back(llvm_file);

    llvm_file += (optimize.size() > 0 ? ".llvm" : ".ll");

    return llvm_file;
}
*/

/**
 * Process the list of intermediate files and generte the output.
 */
/*
void Option::generateOutput(Control &control) {
    string command;

    //
    // If needed, add a command to the command line to optimize each of the intermediate .ll files in turn.
    // TODO: THERE MUST BE A BETTER WAY TO DO THIS !!!
    //
    if (optimize.size() > 0) {
        for (int i = 0; i < control.numLLVMFiles(); i++ ) {
            string file_prefix = control.getLLVMFile(i);

            //
            // Assemble
            //
            command = "llvm-as -f -o ";
            command += file_prefix;
            command += ".bc ";
            command += file_prefix;
            command += ".llvm;";

            system(command.c_str());

            //
            // Optimize
            //
            command = "opt -f -o ";
            command += file_prefix;
            command += ".opt.bc ";
            command += optimize;
            command += " ";
            command += file_prefix;
            command += ".bc;";

            system(command.c_str());

            //
            // Disassemble
            //
            command = "llvm-dis -f -o ";
            command += file_prefix;
            command += ".ll ";
            command += file_prefix;
            command += ".opt.bc; ";

            system(command.c_str());
        }
    }

    //
    // If needed, add final command to generate output for intermediate .ll files.
    //
    if (! debug_output) {
        command += "llvmc -lm ";
        command += options;
        for (int i = 0; i < control.numLLVMFiles(); i++ ) {
            command +=  " ";
            command +=  control.getLLVMFile(i);
            command +=  ".ll ";
        }
        system(command.c_str());
    }

    //
    // If the user did not request that the intermediate files be kept, erase them.
    //
    if (! emit_llvm) {
        for (int i = 0; i < control.numLLVMFiles(); i++ ) {
            string file_prefix = control.getLLVMFile(i),
                   file;
            if (optimize.size() > 0) {
                file = file_prefix;
                file += ".llvm";
                remove(file.c_str());
                file = file_prefix;
                file += ".bc";
                remove(file.c_str());
                file = file_prefix;
                file += ".opt.bc";
                remove(file.c_str());
            }
            file = file_prefix;
            file += ".ll";
            remove(file.c_str());
        }
    }
}
*/


/**
 *
 */
void Option::debugOutputConflict(bool conflict, string option) {
    if (conflict) {
        cerr << "Error: The " << option << " option is incompatible with the -debug-output option" << endl;
        assert(0);
    }
}
