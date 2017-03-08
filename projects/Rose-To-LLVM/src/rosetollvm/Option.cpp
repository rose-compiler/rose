#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>

#include <map>
#include <vector>
#include <string>

#include <sysexits.h>

using namespace std;
using namespace llvm;
using namespace boost::program_options;

#define PREFIX "rose2llvm:"
string Option::roseToLLVMModulePrefix = "--" PREFIX;

Option::Option(Rose_STL_Container<string> &args) : query(false),
                                                   synthetic_translation(false),
                                                   translating(true),
                                                   emit_llvm(false),
                                                   emit_llvm_bitcode(false),
                                                   debug_pre_traversal(false),
                                                   debug_post_traversal(false),
                                                   debug_output(false)
{
    options_description desc("");
    addOptionsToDescription(desc);
    parsed_options options
        = command_line_parser(args).allow_unregistered().options(desc).run();
    variables_map vm;
    store(options, vm);

    vector<string> extraOpts
        = collect_unrecognized(options.options, include_positional);

    if (vm.count(PREFIX"debug-output")) {
        debug_output = true;
        debugOutputConflict(emit_llvm, "emit-llvm");
        debugOutputConflict(emit_llvm_bitcode, "emit-llvm-bitcode");
    }

    if (vm.count(PREFIX"debug-pre-traversal")) {
        debug_pre_traversal = true;
    }

    if (vm.count(PREFIX"debug-post-traversal")) {
        debug_post_traversal = true;
    }

    if (vm.count(PREFIX"debug-traversal")) {
        debug_pre_traversal = true;
        debug_post_traversal = true;
    }

    if (vm.count(PREFIX"emit-llvm")) {
        emit_llvm = true;
        debugOutputConflict(debug_output, "emit-llvm");
    }

    if (vm.count(PREFIX"emit-llvm-bitcode")) {
        emit_llvm_bitcode = true;
        debugOutputConflict(debug_output, "emit-llvm-bitcode");
    }

    // Remove all rose2llvm options.
    CommandlineProcessing::removeArgs (args, "--" PREFIX);

    // Default to llvm bitcode is no option is specified.
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
        (PREFIX"debug-output", "")
        (PREFIX"debug-pre-traversal","")
        (PREFIX"debug-post-traversal", "")
        (PREFIX"debug-traversal", "")
        (PREFIX"emit-llvm", "")
        (PREFIX"emit-llvm-bitcode", "(default)");
}
