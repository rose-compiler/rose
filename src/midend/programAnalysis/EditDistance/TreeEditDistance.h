#ifndef ROSE_TreeEditDistance_H
#define ROSE_TreeEditDistance_H

#include "Diagnostics.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <map>
#include <string>
#include <vector>

namespace rose {

/** Analysis to determine how to make one AST look like another.
 *
 *  Actually editing one tree have the same shape as another is, in many cases, nonsensical but the edit distance metric is
 *  nonetheless useful to determine the similarity of two trees.  See @ref compute for details about how this analysis is
 *  implemented.
 *
 *  The way the analysis is used is like this:
 *
 * @code
 *  SgNode *ast1 = ...;                                 // The first subtree
 *  SgNode *ast2 = ...;                                 // Second subtree
 *  TreeEditDistance analysis;                          // Object for performing the analysis
 *  analysis.substitutionCost(0.0);                     // Adjust some parameters
 *  analysis.compute(ast1, ast2);                       // Run the analysis
 *  double cost = analysis.cost();                      // Query some results
 * @endcode
 *
 *  The analysis object can be reused as many times as one likes by calling its @ref compute method with different trees. The
 *  query methods always return the same results until the next call to @ref compute. */
class TreeEditDistance {
public:
    typedef boost::property<boost::edge_weight_t, double> EdgeProperty;

    /** Graph used for computing Dijkstra's shortest path (DSP). */
    typedef boost::adjacency_list<boost::listS,         // edge representation
                                  boost::vecS,          // vertex representation
                                  boost::directedS,     // edges are directed
                                  boost::no_property,   // vertex values
                                  EdgeProperty          // edge values
                                 > Graph;

    /** Graph vertex ID type. */
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

    /** Graph edge type. */
    typedef std::pair<size_t, size_t> Edge;             // source and target vertex IDs

    /** Base class for substitution prediates.
     *
     *  @sa substitutionPredicate */
    class SubstitutionPredicate {
    public:
        virtual ~SubstitutionPredicate() {}
        virtual bool operator()(SgNode *source, SgNode *target) = 0;
    };

private:
    double insertionCost_;
    double deletionCost_;
    double substitutionCost_;

    SgNode *ast1_, *ast2_;                              // trees being compared
    std::vector<SgNode*> nodes1_, nodes2_;              // list of nodes from parts of trees being compared
    std::vector<size_t> depths1_, depths2_;             // subtree depths for nodes1_ and nodes2_
    Graph graph_;                                       // graph connectivity and edge weights
    std::vector<double> totalCost_;                     // total cost of minimal-cost path from origin to each vertex
    std::vector<Vertex> predecessors_;                  // predecessor vertex for each node in minimal-cost path from origin
    SubstitutionPredicate *substitutionPredicate_;      // determines whether one node can be substituted for another
    static Sawyer::Message::Facility mlog;              // message facility for debugging

public:
    /** Construct an analysis with default values. */
    TreeEditDistance()
        : insertionCost_(1.0), deletionCost_(1.0), substitutionCost_(1.0), ast1_(NULL), ast2_(NULL),
          substitutionPredicate_(NULL)  {}

    /** Forget calculated results. */
    void clear() {
        ast1_ = ast2_ = NULL;
        nodes1_.clear(), nodes2_.clear();
        depths1_.clear(), depths2_.clear();
        graph_ = Graph();
        totalCost_.clear(), predecessors_.clear();
    }
    
    /** Property: insertion cost.
     *
     *  The non-negative cost of performing an insertion when editing one tree to make it look like another.
     *
     * @{ */
    double insertionCost() const {
        return insertionCost_;
    }
    void insertionCost(double weight) {
        ASSERT_require(weight >= 0.0);
        insertionCost_ = weight;
    }
    /** @} */

    /** Property: deletion cost.
     *
     *  The non-negative cost of performing a deletion when editing one tree to make it look like another.
     *
     * @{ */
    double deletionCost() const {
        return deletionCost_;
    }
    void deletionCost(double weight) {
        ASSERT_require(weight >= 0.0);
        deletionCost_ = weight;
    }
    /** @} */

    /** Property: substitution cost.
     *
     *  The non-negative cost of performing a substitution when editing one tree to make it look like another.
     *
     * @{ */
    double substitutionCost() const {
        return substitutionCost_;
    }
    void substitutionCost(double weight) {
        ASSERT_require(weight >= 0.0);
        substitutionCost_ = weight;
    }
    /** @} */

    /** Property: substitution predicate.
     *
     *  A substitution can only occur when the source and destination AST nodes are at the same depth in their respective
     *  subtrees, and when the substitution predicate returns true.  The default predicate (when the pointer is null) always
     *  returns true, thus taking only tree shape into account when making one tree look like another.
     *
     * @{ */
    SubstitutionPredicate* substitutionPredicate() const {
        return substitutionPredicate_;
    }
    void substitutionPredicate(SubstitutionPredicate *predicate) {
        substitutionPredicate_ = predicate;
    }
    /** @} */

    /** Compute tree edit distances.
     *
     *  Computes edit distances and stores them in this object.  Most of the other methods simply query the results computed
     *  from this call.
     *
     *  Given two trees, @p source and @p target, compute and store within this object information about the edit distance from
     *  @p source to @p target. That is, the cost for editing @p source to make it the same shape as @p target. Applying edits
     *  to build such a tree is most likely nonsensical, but it gives a measure of similarity between two trees. If files @p
     *  sourceFile and/or @p targetFile are non-null, then the Sg_File_Info of each node under @p source and @p target,
     *  respectively, are compared with the specified files and contribute to the edit distance only when that node belongs to
     *  the specified file.
     *
     *  Returns this analysis object so that queries can be chained, as in
     *
     * @code
     *  double diff = TreeEditDistance().compute(t1,t2).cost();
     * @endcode
     *
     *  There are three versions of this function:
     *
     * @li A version where both trees are specified.
     *
     * @li A version where only the target tree is specified and the source tree is re-used from a previous calculation. This
     *     is useful when comparing one tree against many trees.
     *
     * @li A version that re-uses both trees from a previous calculation.  This is useful when one changes only the edit costs
     *     or other properties that might influence the result.
     *
     *  This analysis uses Dijkstra's shortest path and takes a total of \f$O(V_s V_t)\f$ memory and
     *  \f$O(V_s V_t log(V_s V_t) + E)\f$ where \f$V_s\f$ and \f$V_t\f$ are the number of nodes in the source and target
     *  trees and \f$E\f$ is the number of edges representing possible insertions, deletions, and substitutions.
     *
     * @{ */
    TreeEditDistance& compute(SgNode *source, SgNode *target, SgFile *sourceFile=NULL, SgFile *targetFile=NULL);
    TreeEditDistance& compute(SgNode *target, SgFile *targetFile=NULL);
    TreeEditDistance& compute();
    /** @} */

    /** Total cost for making one tree the same shape as the other.
     *
     *  This is the same value returned by the previous call to @ref compute and also available by querying for the actual list
     *  of edits and summing their costs. */
    double cost() const;

    /** Relative cost.
     *
     *  This function returns an edit distance normalized to the size of the source tree.  It does so by simply dividing the
     *  total edit cost by the number of selected nodes in the source tree. */
    double relativeCost() const;

    /** Type of edit operation. */
    enum EditType {
        INSERT,                                         /**< Insert a node from another tree. */
        DELETE,                                         /**< Delete a node from this tree. */
        SUBSTITUTE,                                     /**< Substitute a node; same as an insert-delete pair. */
    };

    /** A single edit operation. */
    struct Edit {
        EditType editType;                              /**< Type of operation performed. */
        SgNode *sourceNode;                             /**< Node in source tree to be replaced or deleted. */
        SgNode *targetNode;                             /**< Node in target tree for replacement or insertion. */
        double cost;                                    /**< Cost for this operation. */
        Edit(EditType editType, SgNode *sourceNode, SgNode *targetNode, double cost)
            : editType(editType), sourceNode(sourceNode), targetNode(targetNode), cost(cost) {}
        void print(std::ostream&) const;
    };

    /** List of edit operations. */
    typedef std::vector<Edit> Edits;

    /** Edit operations to make one path look like another. */
    Edits edits() const;

    /** The two trees that were compared. */
    std::pair<SgNode*, SgNode*> trees() const {
        return std::make_pair(ast1_, ast2_);
    }

    /** List of nodes in the trees.
     *
     * @{ */
    const std::vector<SgNode*>& sourceTreeNodes() const {
        return nodes1_;
    }
    const std::vector<SgNode*>& targetTreeNodes() const {
        return nodes2_;
    }
    /** @} */

    /** Number of vertices and edges in the graph.
     *
     *  The graph is used to compute Dijkstra's shortest path and minimize the cost of the edits.  This function returns the
     *  number of vertices and edges in the graph. */
    std::pair<size_t, size_t> graphSize() const;

    /** Emit a GraphViz file.
     *
     *  Emits a GraphViz file of the graph used to compute the edit distance and the edit distance path.  The rows of the graph
     *  represent nodes in the source AST and the columns represent nodes in the target AST.  Note that the columns are not in
     *  the same order for each row because of limitations with the GraphViz "dot" program.  Red edges represent deletion of a
     *  node in the source tree; green edges represent insertion of a node from the target tree; blue edges represent
     *  substitution of a node in the source tree with a node from the target tree; and black edges represent the edit path
     *  that was computed by the @ref compute method.
     *
     *  The output file also contains invisible edges in an attempt to coerce "dot" into making a layout that is as close to
     *  being a matrix as possible. */
    void emitGraphViz(std::ostream&) const;

    /** Initialize diagnostics.  Called from rose::Diagnostics::initialize. */
    static void initDiagnostics();

protected:
    void setTree1(SgNode *ast, SgFile*);
    void setTree2(SgNode *ast, SgFile*);
};

std::ostream& operator<<(std::ostream&, const TreeEditDistance::Edit&);

} // namespace

#endif
