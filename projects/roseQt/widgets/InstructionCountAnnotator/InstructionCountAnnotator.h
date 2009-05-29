#ifndef INSTRUCTIONCOUNTANNOTATOR_H
#define INSTRUCTIONCOUNTANNOTATOR_H


class SgProject;

#include <vector>
#include <string>

namespace InstructionCountAnnotator
{
    /// Annotates a Binary AST
    /// @param proj the (binary) SgProject to annotate
    /// @param args array with parameters for the process to trace
    ///             (only parameter i.e. args[0] is NOT the exec name
    void annotate(SgProject * proj, std::vector<std::string> args);

};


#endif

