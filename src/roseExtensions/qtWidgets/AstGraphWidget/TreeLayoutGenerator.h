#ifndef TREELAYOUTGENERATOR_H
#define TREELAYOUTGENERATOR_H_

#include <QHash>
#include "DisplayNode.h"

/**
 * \brief Layout of Tree according to Reingold and Tilford
 *
 *   - bottom up approach: two subtrees are merged
 *   - for merging subtrees, the right outline of the left subtree \n
 *     is compared to left outline of right subtree, layerwise, i.e two \n
 *     subtrees can interleave \n
 *   - for faster scanning of tree outlines "threads" are added
 *     threads are needed if trees of different height are merged
 *     if for example left subtree is taller, a thread from the rightmost node of the
 *     lowest level of right subtree to next lower level of left subtree is added
 *   - scanning of a tree-outline is simply following right or leftmost children and,
 *     if node is a leaf, follow the thread (if one exists)
 */
class TreeLayoutGenerator
{

        public:
                TreeLayoutGenerator();
                void layoutTree(DisplayTreeNode * root);

        protected:
                struct AdditionalNodeInfo;
                struct ExtremeInfo;


                // First traversal computes relative position for each node (bottom up)
                // here the main work is done
                typedef QPair<ExtremeInfo,ExtremeInfo> ExtInfoPair;
                ExtInfoPair posCalcTraversal(DisplayTreeNode * node, int layer);

                // Second traversal, positions the node according to their stored rel. positions
                void layoutTraversal(DisplayTreeNode * node);

                // Displays the threads for debugging
                void paintThreadTraversal(DisplayTreeNode * node);


                // --------  Helper Functions for posCalcTraversal ----------------

                /** Returns the next node on right outline
                        @param right current node on right outline
                        @param rightPos in-out parameter , updates the relative offset
                        @param threadUsed is set to true when a thread was followed     */
                DisplayTreeNode * advanceRight(DisplayTreeNode * right, double & rightPos, bool & threadUsed);

                /// see advanceRight
                DisplayTreeNode * advanceLeft(DisplayTreeNode * left, double & leftPos, bool & threadUsed);


                /// Calculates the Distance between two subtrees (which have already been processed,
                /// and therefore relPosition is already set, and computes the distance between them
                /// arguments: extremeInfoR means: on right subtree the rightmost node
                double calcSubtreeDistance(DisplayTreeNode * left, DisplayTreeNode * right,
                                   const ExtremeInfo & extInfoL,
                                   const ExtremeInfo & extInfoR);


                /// For each node additional layout-information is stored
                /// by using a hash index with nodePointer
                QHash<DisplayTreeNode *, AdditionalNodeInfo *> nodeInfo;


                //TODO Write getter and setter for this attributes:

                /// distance between nodes, from end of boundingBox1
                /// to begin boundingBox2, not the midpoint distance
                double minNodeDistance;

                /// if subtrees are interleaved this distance is added to minNodeDistance
                double extraDistSubtrees;

                /// basis y-Distance between nodes
                double layerHeight;

                /// if node has many children, the layerHeight is increased, so that
                /// there is more space for the edges
                /// total layerHeight is computed as layerHeight+layerChildFactor*childCount
                double layerChildFactor;
};



#endif
