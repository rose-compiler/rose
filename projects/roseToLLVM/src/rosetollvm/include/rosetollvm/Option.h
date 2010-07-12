#ifndef OPTION
#define OPTION

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <rose.h>

class Control;

class Option {
public: 
    static std::string roseToLLVMModulePrefix;

    Option(Rose_STL_Container<std::string> &args);

    std::string getOptimize() { return optimize; }
    std::string getOptions() { return options; }
    std::string getOutputFile() { return output_file; }
    bool isCompileOnly() { return compile_only; }
    bool isEmitLLVM() { return emit_llvm; }
    bool isEmitLLVMBitcode() { return emit_llvm_bitcode; }
    bool isDebugPreTraversal() { return debug_pre_traversal; }
    bool isDebugPostTraversal() { return debug_post_traversal; }
    bool isDebugOutput() { return debug_output; }

    bool isQuery() { return query; }
    void setQuery() { query = true; }
    void resetQuery() { query = false; }

    /*
    int numLLVMFiles() { return llvm_file_prefixes.size(); }

    std::string addLLVMFile(std::string);

    void generateOutput(Control &control);
    */
private:

    std::string output_file,
                options,
                optimize;

    bool query,
         compile_only,
         emit_llvm,
         emit_llvm_bitcode,
         debug_pre_traversal,
         debug_post_traversal,
         debug_output;

    //    std::vector<std::string> llvm_file_prefixes;

    void debugOutputConflict(bool conflict, std::string option);
};

#endif
