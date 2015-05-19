#include <rose.h>

#include <iostream>
#include <sawyer/CommandLine.h>
#include <string>

struct Settings {
    size_t indentWidth;                                 // Indentation per nesting level
    size_t binaryTreeDepth;
    size_t sequenceLength;
    Settings()
        : indentWidth(2), binaryTreeDepth(4), sequenceLength(2) {}
} settings;

static void
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    Parser parser;
    parser
        .purpose("generate a program with a certain kind of structure")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "RSOE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}]")
        .doc("Description",
             "Generates a program, written in C, on standard output, that has a certain structure specified by the "
             "command-line switches described here.");

    SwitchGroup gen = CommandlineProcessing::genericSwitches();
    gen.insert(Switch("tree-depth", 't')
               .argument("n", nonNegativeIntegerParser(settings.binaryTreeDepth))
               .doc("Width in bits of the 'if' tree."));
    gen.insert(Switch("sequence-length", 'l')
               .argument("n", nonNegativeIntegerParser(settings.sequenceLength))
               .doc("Number of times to repeat the sub-program."));

    if (!parser.with(gen).parse(argc, argv).apply().unreachedArgs().empty())
        throw std::runtime_error("incorrect usage; see --help");
}

std::string
indentation(size_t nestingLevel) {
    return std::string(nestingLevel * settings.indentWidth, ' ');
}

// Generate nested "if" statements whose CFG is a binary tree.  Each level of the tree uses a different bit of the control
// expression.
void
generateBinaryTree(const std::string &ctrlExpr, const std::string &resultVar, size_t nestingLevel, size_t depth=0) {
    if (depth >= settings.binaryTreeDepth) {
        std::cout <<indentation(nestingLevel) <<resultVar <<" = " <<ctrlExpr <<";\n";
    } else {
        std::cout <<indentation(nestingLevel) <<"if ((" <<ctrlExpr <<" >> " <<depth <<") & 1) {\n";
        generateBinaryTree(ctrlExpr, resultVar, nestingLevel+1, depth+1);
        std::cout <<indentation(nestingLevel) <<"} else {\n";
        generateBinaryTree(ctrlExpr, resultVar, nestingLevel+1, depth+1);
        std::cout <<indentation(nestingLevel) <<"}\n";
    }
}

// Generate a linear sequence of "if" statements where each "if" is controlled by the inverse expression of the previous
// "if". The body of each "if" will be generated via generateBinaryTree.
void
generateSequence(const std::string &ctrlExpr, const std::string &resultVar, size_t nestingLevel) {
    if (0 == settings.sequenceLength) {
        generateBinaryTree(ctrlExpr, resultVar, nestingLevel);
    } else {
        for (size_t i=0; i<settings.sequenceLength; ++i) {
            std::cout <<indentation(nestingLevel) <<"if ((" <<ctrlExpr <<" & 1) == " <<(i%2) <<") {\n";
            generateBinaryTree(ctrlExpr, resultVar, nestingLevel+1);
            std::cout <<indentation(nestingLevel) <<"} else {\n";
            generateBinaryTree(ctrlExpr, resultVar, nestingLevel+1);
            std::cout <<indentation(nestingLevel) <<"}\n";
        }
    }
}

int
main(int argc, char *argv[]) {
    parseCommandLine(argc, argv, settings);

    std::cout <<indentation(0) <<"static void trip_breaker(void) {\n"
              <<indentation(1) <<  "return;\n"
              <<indentation(0) <<"}\n"
              <<indentation(0) <<"\n";

    std::cout <<indentation(0) <<"int main(int argc, char *argv[]) {\n"
              <<indentation(1) <<"int a;\n"
              <<indentation(1) <<"\n";

    generateSequence("argc", "a", 1);

    int desiredPath = 0xaaaaaaaa & IntegerOps::genMask<int>(settings.binaryTreeDepth);
    std::cout <<indentation(1) <<"if (a == 0x" <<std::hex <<desiredPath <<std::dec <<")\n"
              <<indentation(2) <<  "trip_breaker();\n";
    std::cout <<indentation(1) <<"return 0;\n"
              <<indentation(0) <<"}\n";

}
