#ifndef _DOMINANCEFRONTIER_H_
#define _DOMINANCEFRONTIER_H_

#include "DominatorTree.h"
#include <algorithm>
using std::set_difference;

namespace DominatorTreesAndDominanceFrontiers
{

        /* ! \class DominanceFrontier

                 This class constructs the dominance (or post-dominance) frontiers for
                 all nodes in a ControlFlowGraph. A dominance (post-dominance) frontier
                 for node X is simply the set of nodes such that a given node Y from the 
                 set is not dominated (post-dominated) by X, but there is an immediate
                 predecessor of Y that is dominated (post-dominated) by X.

                 The type of frontier we construct is determined by the DominatorTree
                 that DominanceFrontier is initialized with.

         */

        template < typename CFGFilterFunction > class TemplatedDominanceFrontier:public DominatorForwardBackwardWrapperClass <
                                                                                                                                                                                                                                                                                                         CFGFilterFunction
                                                                                                                                                                                                                                                                                                                 >
        {
                public:
                //! retunrs a set of ID's with the nodes dominance-frontier
                std::set<int> getFrontier(int node)
                {
                        if (validID(node))
                        return dominatorFrontier[node];
                        else
                        {
                                std::cerr<<"no such ID ("<<node<<") in dominator-tree\n";
                                ROSE_ASSERT(false);
                        }
                        return std::set<int>();
                }

                //! construct the domnancefrontier
                TemplatedDominanceFrontier(TemplatedDominatorTree < CFGFilterFunction > dt):DominatorForwardBackwardWrapperClass < CFGFilterFunction > (dt.getDirection()),
                                                                                                                                                                                                                                                                                                                                dominatorTree
                                                                                                                                                                                                                                                                                                                                        (dt)
                {
                        dominatorTree = dt;
                        init();
                        buildFrontier();
                }

                        //! debug method to print forntiers
                        void printFrontiers()
                        {
                                std::cout << "x\tDF\n";
                                for (int i = 0; i < dominatorTree.getSize(); i++)
                                {
                                        std::cout <<i<<"\t{";
                                        for (std::set<int>::const_iterator j = dominatorFrontier[i].begin(); j != dominatorFrontier[i].end(); ++j) {
                                          if (j != dominatorFrontier[i].begin()) std::cout << ", ";
                                          std::cout << *j;
                                        }
                                        std::cout <<"}" << std::endl;
                                }
                                return ;

                        }


                        private:
                        //! checks if the id is valid
                        int validID(int id)
                        {
                                if (id>=0 && id<dominatorFrontier.size()) return true;
                                else return false;

                        }

                        // ! The dominator tree the dominance frontiers are based on
                        TemplatedDominatorTree < CFGFilterFunction > dominatorTree;

                        //! a vector of dominance-frontiers, for every possible id one
                        std::vector < std::set < int > >dominatorFrontier;

                        //! allocate the vector and initialize it with sets
                        void init()
                        {
                                for (int i = 0; i < dominatorTree.getSize(); i++)
                                {
                                        dominatorFrontier.push_back(std::set < int >());
                                }
                        }

                        void buildFrontier()
                        {
                                std::vector<std::set<int> > domSets;
                                std::vector<std::set<int> > dfLocal;
                                int nodeCount=dominatorTree.getSize();
                                int i;
#ifdef DEBUG
                                std::cout << "x\tDFl\tDS\tDF\n";
#endif
                                for (i=0;i<nodeCount;i++)
                                {
                                        // make shure that the set acutally exists in the vector
                                        domSets.push_back(dominatorTree.getDirectDominatedSet(i));
                                        dfLocal.push_back(std::set<int>());
                                        // compute the df local
                                        VirtualCFG::FilteredCFGNode < CFGFilterFunction > currentNode=dominatorTree.getCFGNodeFromID(i);
                                        // for each child
                                        std::vector < VirtualCFG::FilteredCFGEdge < CFGFilterFunction > >outEdges =
                                                getDirectionModifiedOutEdges(currentNode);
                                        // for all those children
                                        for (unsigned int edge = 0; edge < outEdges.size(); edge++)
                                        {
                                                VirtualCFG::FilteredCFGNode < CFGFilterFunction > child =
                                                        target(outEdges[edge]);
                                                int imDom=dominatorTree.getImDomID(child);
                                                int childID=dominatorTree.getID(child);
                                                // if the current ID is the immdom of this child, put it in the domSet
                                                if (i!=imDom)
                                                {
                                                        // child is not dominated -> frontier
                                                        dfLocal[i].insert(childID);
                                                }
                                        }
                                }
                        
                                // in the second pass, remove the dominated nodes from the merged dominance-frontier-set
                                for (i=0;i<nodeCount;i++)
                                {
                                        std::set<int> tmp;
                                        tmp.insert(dfLocal[i].begin(),dfLocal[i].end());
                                        for (std::set<int>::iterator j=domSets[i].begin();j!=domSets[i].end();j++)
                                        {
                                                tmp.insert(dfLocal[(*j)].begin(),dfLocal[(*j)].end());                                          
                                        }
                                        set_difference(tmp.begin(),tmp.end(),
                                                        domSets[i].begin(),domSets[i].end(),
                                                        inserter(dominatorFrontier[i],dominatorFrontier[i].begin()));
#ifdef DEBUG
                                        std::cout <<i<<"\t"<<dfLocal[i]<<"\t"<<domSets[i]<<"\t"<<dominatorFrontier[i]<<std::endl;
#endif

                                }
                        }
                };

                // end of namespace: DominatorTreesAndDominanceFrontiers
        }

#endif
