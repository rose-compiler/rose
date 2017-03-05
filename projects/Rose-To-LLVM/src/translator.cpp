#include <iostream>
#include <vector>
#include <rose.h>

#include "rosetollvm/RoseToLLVM.hpp"

using namespace std;
using namespace boost::program_options;

#define PREFIX "rose2llvm:"

/**
 * Main program for Rose to LLVM translator.
 */
int main(int argc, char *argv[]) {
    options_description desc("");
    desc.add_options()
        ("help","print this help message")
        (PREFIX "translate-external", "translate code from external files");

    command_line_parser parser(argc, argv);

    parsed_options options = parser.allow_unregistered().options(desc).run();
    vector<string> roseOpts = collect_unrecognized(options.options, include_positional);
    roseOpts.insert(roseOpts.begin(), argv[0]);

    variables_map vm;
    store(options, vm);

    if (vm.count("help")) {
        RoseToLLVM::addOptionsToDescription(desc);
        cout << desc << "\n";
        return 1;
    }

    bool translateExternals = vm.count(PREFIX "translate-external");

    // Give RoseToLLVM all of the leftover arguments.
    RoseToLLVM roseToLLVM(roseOpts);

    // Any remaining options should be handled by Rose.
    SgProject *astRoot = frontend(roseOpts);

    if (astRoot && translateExternals) {
        astRoot -> setAttribute(RoseToLLVM::kTranslateExternals, new AstAttribute());
    }

    vector<llvm::Module*> modules = roseToLLVM.translate(astRoot);
    for (vector<llvm::Module*>::iterator itr = modules.begin(), end = modules.begin();  itr != end; ++itr) {
        delete *itr;
    }

    return 0;
}
