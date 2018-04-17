#ifndef OPTION
#define OPTION

#include <iostream>
#include <vector>
#include <string>
#include <rose.h>

#include <boost/program_options.hpp>

class Control;

class Option {
public:
    static std::string roseToLLVMModulePrefix;

    Option(Rose_STL_Container<std::string> &args);

    bool isEmitLLVM() { return emit_llvm; }
    bool isEmitLLVMBitcode() { return emit_llvm_bitcode; }
    bool isDebugPreTraversal() { return debug_pre_traversal; }
    bool isDebugPostTraversal() { return debug_post_traversal; }
    bool isDebugOutput() { return debug_output; }

    bool isQuery() { return query; }
    void setQuery() { query = true; }
    void resetQuery() { query = false; }

    bool isSyntheticTranslation() { return synthetic_translation; }
    void setSyntheticTranslation() { synthetic_translation = true; }
    void resetSyntheticTranslation() { synthetic_translation = false; }

    bool isTranslating() { return translating; }
    void setTranslating() { translating = true; }
    void resetTranslating() { translating = false; }

    bool isCompileOnly() { return compile_only; }
    void setCompileOnly() { compile_only = true; }
    void resetCompileOnly() { compile_only = false; }

    std::string optimizationLevel() { return optimization_level; }
    void setOptimizationLevel(std::string level) { optimization_level = level; }

    std::string outputFilename() { return output_filename; }
    void setOutputFilename(std::string filename) { output_filename = filename; }


    /*
    int numLLVMFiles() { return llvm_file_prefixes.size(); }

    std::string addLLVMFile(std::string);

    void generateOutput(Control &control);
    */

    // static void printOptions();
    static void addOptionsToDescription(
            boost::program_options::options_description& desc);

private:

    bool query,
         synthetic_translation,
         translating,
         emit_llvm,
         emit_llvm_bitcode,
         debug_pre_traversal,
         debug_post_traversal,
         debug_output;

    bool compile_only;
    std::string optimization_level;
    std::string output_filename;

    //    std::vector<std::string> llvm_file_prefixes;

    void debugOutputConflict(bool conflict, std::string option);
};

#endif
