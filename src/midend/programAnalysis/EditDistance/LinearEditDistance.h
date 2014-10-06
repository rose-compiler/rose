#ifndef ROSE_EditDistance_LinearEditDistance_H
#define ROSE_EditDistance_LinearEditDistance_H

#include <EditDistance/Levenshtein.h>

namespace rose {
namespace EditDistance {

/** Edit distance for ASTs.
 *
 *  Computes the edit distance of two abstract syntax trees, a souce and target, by converting each tree to a list of nodes,
 *  then computing Levenshtein edit distance on the two lists.  The list nodes are constructed from AST nodes using a
 *  user-defined type (template parameter) so that the user has full control over when two list nodes are considered equal.
 *
 *  Example usage:
 *
 * @code
 *  using namespace rose::EditDistance;
 *  SgNode *ast1=..., *ast2=...;
 *
 *  // Compare AST nodes only by their class
 *  struct ListNode: LinearEditDistance::Node {
 *      VariantT variant;
 *      explicit ListNode(SgNode *treeNode) {
 *          variant = treeNode->variantT();
 *      }
 *      bool operator==(const ListNode &other) const {
 *          return variant == other.variant;
 *      }
 *  };
 *
 *  // Create an object that will perform the analysis
 *  LinearEditDistance::Analysis<ListNode> led;
 *
 *  // Compute the distance between two trees
 *  size_t editDistance = led.compute(ast1, ast2);
 * @endcode */
namespace LinearEditDistance {

/** Type for comparing two AST nodes.
 *
 *  This type defines the API used by @ref Analysis and also serves as the default parameter for that class template.  It must
 *  be able to construct an edit distance list node from an AST node and test two list nodes for equality. */
class Node {
    unsigned first_, second_;
public:
    /** Construct a list node from an AST node. */
    explicit Node(SgNode *node) {
        ASSERT_not_null(node);
        first_ = node->variantT();
        if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
            second_ = insn->get_anyKind();
        } else {
            second_ = 0;
        }
    }

    /** Test two list nodes for equality.
     *
     *  This implementation returns true (equal) when both AST nodes are the same type. If the nodes are subclasses of
     *  @ref SgAsmInstruction then they must also be the same kind of instruction (e.g., if they are both @ref
     *  SgAsmX86Instruction then they must also both be @ref x86_pop instructions). */
    bool operator==(const Node &other) const {
        return first_==other.first_ && second_==other.second_;
    }
};

// Used internally to build a list of nodes over which edit distance is computed. The list of nodes is constructed by visiting
// certain nodes of an AST.
template<class NodeType>
class NodeSelector: public SgTopDownBottomUpProcessing<size_t, Sawyer::Nothing> {
    std::vector<NodeType> &nodes_;                      // edit distance nodes constructed from AST nodes
    SgFile *containingFile_;                            // optional constraint: node must belong to this file
    size_t minDepth_, maxDepth_;                        // constraints: limits nodes by their depth under the subtree

public:
    NodeSelector(std::vector<NodeType> &nodes /*out*/, SgFile *containingFile, size_t minDepth, size_t maxDepth)
        : nodes_(nodes), containingFile_(containingFile), minDepth_(minDepth), maxDepth_(maxDepth) {
        ASSERT_require(minDepth <= maxDepth);
    }

    size_t evaluateInheritedAttribute(SgNode *node, size_t depth) ROSE_OVERRIDE {
        Sg_File_Info *nodeInfo = node->get_file_info();
        bool isSelected = (depth >= minDepth_ && depth <= maxDepth_ &&
                           (!containingFile_ || (nodeInfo && nodeInfo->isSameFile(containingFile_))));
        if (isSelected)
            nodes_.push_back(NodeType(node));
        return depth+1;
    }

    Sawyer::Nothing evaluateSynthesizedAttribute(SgNode*, size_t depth, SubTreeSynthesizedAttributes) ROSE_OVERRIDE {
        return Sawyer::Nothing();
    }
};
    
/** Edit distance analysis.
 *
 *  Analyzes two ASTs to determine the edit distance between them using the following algorithm:
 *
 * @li Traverse the trees, selecting certain AST nodes.
 * @li For each selected AST node, create an instance of @p NodeType and append it to a list.
 * @li Compute and return the Levenshtein edit distance of the two lists. */
template<typename NodeType = Node>
class Analysis {
    SgNode *ast1_, *ast2_;
    std::vector<NodeType> nodes1_, nodes2_;
    size_t cost_;
public:
    /** Constructs an analysis object with no associated trees. */
    Analysis(): ast1_(NULL), ast2_(NULL), cost_(0) {}

    /** Associate an AST with this analysis.
     *
     *  The @ref setTree1 sets the source AST and @ref setTree2 sets the target AST.  It does so by forgetting any previous
     *  source or target list nodes, traversing the specified tree to select new source or target nodes, creating a list of
     *  @ref NodeType nodes, and storing the list in this analysis.
     *
     *  During traversal, AST nodes whose tree depth is between @p minDepth and @p maxDepth, inclusive are added to the
     *  list. However, if a @p file is specified then the node must also belong to the specified file.
     *
     * @{ */
    Analysis& setTree1(SgNode *ast, SgFile *file=NULL, size_t minDepth=0, size_t maxDepth=size_t(-1)) {
        return setTree(ast1_=ast, file, minDepth, maxDepth, nodes1_/*out*/);
    }
    Analysis& setTree2(SgNode *ast, SgFile *file=NULL, size_t minDepth=0, size_t maxDepth=size_t(-1)) {
        return setTree(ast2_=ast, file, minDepth, maxDepth, nodes2_/*out*/);
    }
    /** @} */

    /** Compute edit distance.
     *
     *  The no-argument version computes edit distance over the source and target ASTs that have been previously added to this
     *  analysis object.  The other versions call @ref setTree2 and/or @ref setTree1 before doing the computation.
     *
     *  Returns this analysis object so that a query method can be chained:
     *
     * @code
     *  size_t distance = LinearEditDistance::Analysis<>::compute(ast1,ast2).cost();
     * @endcode
     *
     * @{ */
    Analysis& compute(SgNode *source, SgNode *target, SgFile *sourceFile=NULL, SgFile *targetFile=NULL) {
        setTree1(source, sourceFile);
        return compute(target, targetFile);
    }
    Analysis& compute(SgNode *target, SgFile *targetFile=NULL) {
        setTree2(target, targetFile);
        return compute();
    }
    Analysis& compute() {
        cost_ = levenshteinDistance(nodes1_, nodes2_);
        return *this;
    }
    /** @} */

    /** Edit distance.
     *
     *  Returns the edit distance previously computed.  The @ref compute method must be called first, otherwise zero is
     *  returned. */
    size_t cost() const {
        return cost_;
    }

    /** Relative edit distance.
     *
     *  Returns an edit distance relative to the size of the ASTs.  A relative edit distance is the absolute edit distance
     *  divided by the size of the larger tree.  Only nodes actually selected from the tree (those satisfying the constraints
     *  used by @ref setTree1 and @ref setTree2) are counted in the size. */
    double relativeCost() const {
        size_t n = std::max(nodes1_.size(), nodes2_.size());
        return n ? 1.0 * cost_ / n : 1.0 * cost_;
    }

private:
    // Does the actual work for setTree1 and setTree2
    Analysis& setTree(SgNode *ast, SgFile *file, size_t minDepth, size_t maxDepth, std::vector<NodeType> &nodes /*out*/) {
        nodes.clear();
        NodeSelector<NodeType> nodeSelector(nodes /*out*/, file, minDepth, maxDepth);
        nodeSelector.traverse(ast, 0);
        return *this;
    }
};

} // namespace
} // namespace
} // namespace

#endif
