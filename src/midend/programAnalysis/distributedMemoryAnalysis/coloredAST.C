// Distributed memory traversal demo application.
// Author: Gergo Barany
// $Id: coloredAST.C,v 1.1 2008/01/08 02:55:52 dquinlan Exp $
#include <rose.h>
#include "DistributedMemoryAnalysis.h"


static const char *colors[] = {
    "red4", "green4", "blue4", "yellow4", "red3", "green3", "blue3", "yellow3",
    "red2", "green2", "blue2", "yellow2", "red1", "green1", "blue1", "yellow1"
};

// --------------------------------------------------------
// ColoredDotGraphTraversal
// --------------------------------------------------------

class ColoredDotGraphTraversal: public AstTopDownProcessing<SgNode *> {
public:
    ColoredDotGraphTraversal(std::string color): myColor(color) {
    }

    std::string get_dotSource() const {
        return dotSource;
    }

    void clearDotSource() {
        dotSource = "";
    }

    static std::string edgeDot(SgNode *source, std::string sourceColor,
                               SgNode *dest, std::string destColor) {
        std::stringstream sts;
        sts << "node_" << sourceColor << "_" << (void *) source
            << " -> "
            << "node_" << destColor << "_" << (void *) dest
            << " [color = \"" << destColor << "\"];"
            << std::endl;
        return sts.str();
    }

protected:
    SgNode *evaluateInheritedAttribute(SgNode *node, SgNode *parent) {
        if (!inNondefiningFunctionDeclaration(node)) {
            dotSource += nodeDot(node);
            if (parent != NULL)
                dotSource += edgeDot(parent, node);
        }
        return node;
    }

private:
    bool inNondefiningFunctionDeclaration(SgNode *node) const {
        while (node != NULL && !isSgProject(node) && !isSgFunctionDeclaration(node))
            node = node->get_parent();
        if (isSgFunctionDeclaration(node) &&
            isSgFunctionDeclaration(node)->get_definition() == NULL)
            return true;
        return false;
    }

    std::string nodeDot(SgNode *node) const {
        std::stringstream sts;
        sts << "node_" << myColor << "_" << (void *) node
            << " [label = \"" << node->class_name() // abbreviate(node->class_name())
            << "\", color = \"" << myColor
            << "\", fontcolor = \"" << myColor << "\"];"
            << std::endl;
        return sts.str();
    }

    std::string edgeDot(SgNode *source, SgNode *dest) const {
        return edgeDot(source, myColor, dest, myColor);
    }

    static std::string abbreviate(std::string s) {
        size_t size = s.size();
        std::string result;
        for (size_t i = 1; i < size; i++)
            if (isupper(s[i]))
                result += s[i];
        return result;
    }

    std::string dotSource;
    std::string myColor;
};



// --------------------------------------------------------
// DotGraphPostTraversal
// --------------------------------------------------------

class DotGraphPostTraversal: public AstBottomUpProcessing<std::string> {
protected:
    std::string evaluateSynthesizedAttribute(SgNode *, SynthesizedAttributesList synAttrs) {
        SynthesizedAttributesList::iterator s;
        std::string result = "";
        for (s = synAttrs.begin(); s != synAttrs.end(); ++s)
            result += *s;
        return result;
    }

    std::string defaultSynthesizedAttribute() {
        return "";
    }
};



// --------------------------------------------------------
// DistributedDotGraph
// --------------------------------------------------------

class DistributedDotGraph: public DistributedMemoryTraversal<SgNode *, std::string> {
public:
    static void dumpDot(std::string filename, std::string preamble, std::string source) {
        std::ofstream f(filename.c_str());
        f   << "digraph coloredAST {" << std::endl
            << "rankdir = \"LR\"" << std::endl
            << preamble
            << source
            << "}" << std::endl;
    }

protected:
    std::string analyzeSubtree(SgFunctionDeclaration *funcDecl, SgNode *parent) {
        ColoredDotGraphTraversal dotTraversal(colors[myID()]);
        // compute colored dot source for this subtree
        dotTraversal.traverse(funcDecl, NULL);
        // store the source
        std::string dotSource = dotTraversal.get_dotSource();
        // connect the colored subtree to its black parent
        dotSource += ColoredDotGraphTraversal::edgeDot(parent, "black", funcDecl, colors[myID()]);
        return dotSource;
    }

    std::pair<int, void *> serializeAttribute(std::string dotSource) const {
        return std::make_pair(dotSource.size() + 1, strdup(dotSource.c_str()));
    }

    std::string deserializeAttribute(std::pair<int, void *> attribute) const {
        return std::string((const char *) attribute.second);
    }

    void deleteSerializedAttribute(std::pair<int, void *> attribute) const {
        std::free(attribute.second);
    }
};




int main(int argc, char **argv) {
    /* read the AST */
    SgProject *project = frontend(argc, argv);

    /* the user must call this function before performing any distributed
     * memory analyses */
    initializeDistributedMemoryProcessing(&argc, &argv);

    /* pre-traversal that computes context information for the distributed
     * analysis -- in this case the black part of the AST outside of function
     * definitions */
    ColoredDotGraphTraversal preTraversal("black");

    /* post-traversal that collects the dot graphs computed in the distributed
     * analysis */
    DotGraphPostTraversal postTraversal;

    /* actual analysis that computes colored graphs of function definitions */
    DistributedDotGraph distributedDotGraph;

    /* the initial inherited attribute passed to the pre-traversal */
    SgNode *rootInheritedAttribute = NULL;

    /* run the analysis: this will automagically invoke the pre-traversal and
     * call the distributed analyzer's analyzeSubtree() method for each
     * function definition with the attribute computed by the pre-traversal */
    distributedDotGraph.performAnalysis(project, rootInheritedAttribute,
                                        &preTraversal, &postTraversal);

    /* on exactly one of the processes, this method will return true; the
     * distributed analyzer's state can then be examined for further
     * processing */
    if (distributedDotGraph.isRootProcess()) {
        DistributedDotGraph::dumpDot("coloredAST.dot",
                                     preTraversal.get_dotSource(),
                                     distributedDotGraph.getFinalResults());
        std::cout << "wrote DOT graph to file 'coloredAST.dot'" << std::endl;
    }

    /* the user must call this function after all distributed memory analyses
     * have finished */
    finalizeDistributedMemoryProcessing();
}
