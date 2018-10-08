#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>

#include <map>
#include <vector>
#include <string>

#include <sysexits.h>

using namespace std;
using namespace llvm;
using namespace boost::program_options;

#define ROSE_PREFIX "rose2llvm:"
#define LLC_PREFIX  "llc:"
string Option::roseToLLVMModulePrefix = "--" ROSE_PREFIX;
string Option::llcPrefix1 = "-" LLC_PREFIX;
string Option::llcPrefix2 = "--" LLC_PREFIX;

Option::Option(Rose_STL_Container<string> &args) : query(false),
                                                   synthetic_translation(false),
                                                   translating(true),
                                                   emit_llvm(false),
                                                   emit_llvm_bitcode(false),
                                                   debug_pre_traversal(false),
                                                   debug_post_traversal(false),
                                                   debug_output(false),

                                                   compile_only(false),
                                                   output_filename("-o a.out")

{
    options_description desc("");
    addOptionsToDescription(desc);
    parsed_options options = command_line_parser(args).allow_unregistered().options(desc).run();
    variables_map vm;
    store(options, vm);

    vector<string> extraOpts = collect_unrecognized(options.options, include_positional);

    if (vm.count(ROSE_PREFIX"debug-output")) {
        debug_output = true;
        debugOutputConflict(emit_llvm, "emit-llvm");
        debugOutputConflict(emit_llvm_bitcode, "emit-llvm-bitcode");
    }

    if (vm.count(ROSE_PREFIX"debug-pre-traversal")) {
        debug_pre_traversal = true;
    }

    if (vm.count(ROSE_PREFIX"debug-post-traversal")) {
        debug_post_traversal = true;
    }

    if (vm.count(ROSE_PREFIX"debug-traversal")) {
        debug_pre_traversal = true;
        debug_post_traversal = true;
    }

    if (vm.count(ROSE_PREFIX"emit-llvm")) {
        emit_llvm = true;
        debugOutputConflict(debug_output, "emit-llvm");
    }

    if (vm.count(ROSE_PREFIX"emit-llvm-bitcode")) {
        emit_llvm_bitcode = true;
        debugOutputConflict(debug_output, "emit-llvm-bitcode");
    }

    if (vm.count(ROSE_PREFIX"c")) {
        compile_only = true;
    }

    // Remove all rose2llvm options.
    CommandlineProcessing::removeArgs (args, "--" ROSE_PREFIX);

    // Set up llvm llc options
    if (vm.count("help")) {
        llc_options.push_back("-help"); // This is the default for rose2llvm ... The user may reassign (respecify) this option.
    }
    else {
        /**/
        // TODO:  Temporary patch... Do this right !!!
        /**/
        llc_options.push_back("-filetype=obj"); // This is the default for rose2llvm ... The user may reassign (respecify) this option.
        for (int i = 0; i < args.size(); i++) {
            if (args[i].compare("-c") == 0) {
                compile_only = true;
            }
            else if (args[i].compare("-o") == 0) {
                if (i+1 < args.size()) {
                    output_filename = args[i] + " " + args[i + 1];
                }
                else {
                    cout << "***rose2llvm warning: ignoring output filename option with no filename specified: " << args[i]
                         << endl;
                    cout.flush();
                }
            }
            else if (args[i].size() >= 3 && args[i].compare(0, 3, "-o=") == 0) {
                output_filename = "-o " + args[i].substr(3);
            }
            else if (args[i].compare(0, llcPrefix1.size(), llcPrefix1.c_str()) == 0) { // look for llc options
                llc_options.push_back("-" + args[i].substr(llcPrefix1.size()));
            }
            else if (args[i].compare(0, llcPrefix2.size(), llcPrefix2.c_str()) == 0) { // look for llc options
                llc_options.push_back("-" + args[i].substr(llcPrefix2.size()));
            }
        }
        /**/
    }

    // Remove all llc options.
    CommandlineProcessing::removeArgs (args, "--" LLC_PREFIX);

cout << "The arguments left are:" << endl << endl;
    for (int i = 1; i < args.size(); i++) {
cout << "    argument[" << i << "] = " << args[i] << endl;
         if (args[i][0] == '-') {
             other_options += args[i];
             other_options += " ";
        }
    }
cout << "The other options are: " << other_options << endl;
cout.flush();
 
    // Default to llvm bitcode if no option is specified.
    if (!(emit_llvm_bitcode || emit_llvm || debug_output))
        emit_llvm_bitcode = true;

    return;
}


/**
 *
 */
void Option::debugOutputConflict(bool conflict, string option) {
    if (conflict) {
        cerr << "Error: The " << roseToLLVMModulePrefix << option
             << " option is incompatible with the "
             << roseToLLVMModulePrefix << "debug-output option" << endl;
        exit(EX_USAGE);
    }
}

void Option::addOptionsToDescription(options_description& desc)
{
    desc.add_options()
        (ROSE_PREFIX"c", "Compile only")
        (ROSE_PREFIX"debug-output", "")
        (ROSE_PREFIX"debug-pre-traversal","")
        (ROSE_PREFIX"debug-post-traversal", "")
        (ROSE_PREFIX"debug-traversal", "")
        (ROSE_PREFIX"emit-llvm", "")
        (ROSE_PREFIX"emit-llvm-bitcode", "(default)")
        (LLC_PREFIX"XX", "where -XX is a valid llvm compiler option");
}
