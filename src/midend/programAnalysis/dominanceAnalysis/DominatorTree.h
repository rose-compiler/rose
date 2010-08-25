#ifndef _DOMINATORTREE_H_
#define _DOMINATORTREE_H_

#include <GraphDotOutput.h>
#include <map>
#include "filteredCFG.h"
// #include "rose.h"
namespace DominatorTreesAndDominanceFrontiers
{
        /* ! \class TemplatedDominatorTree
                 This class constructs either a dominator or a post-dominator tree for a control-flow-graph.  */

        typedef enum
        {
                PRE_DOMINATOR,          /* !< This indicates that we are building a
                                                                                                                         dominator tree */
                POST_DOMINATOR          /* !< This indicates that we are building a
                                                                                                                         post-dominator tree */
        } Direction;
        //! This class provides a uniform view of the CFG dissregarding the direction of the CFG-traversal (forward/backward)
        template < typename CFGFilterFunction > class DominatorForwardBackwardWrapperClass
        {
                public:
                        // ! Constructor for the DominatorForwardBackwardWrapperClass
                        DominatorForwardBackwardWrapperClass(Direction dir):treeDirection(dir)
                        {
                        };

                        // ! returns whether this is a dominator tree (PRE) or a
                        // post-dominator tree (POST)
                        Direction getDirection()
                        {
                                return treeDirection;
                        }
                        
                protected:
                        //! helperfunctions to toggle beween pre and post dominator tree
                        std::vector < VirtualCFG::FilteredCFGEdge < CFGFilterFunction > > getDirectionModifiedOutEdges(VirtualCFG::FilteredCFGNode < CFGFilterFunction >
                                                current)
                                {
                                        if (treeDirection == PRE_DOMINATOR)
                                                return current.outEdges();
                                        else
                                                return current.inEdges();
                                }
                        std::vector < VirtualCFG::FilteredCFGEdge < CFGFilterFunction > > getDirectionModifiedInEdges(VirtualCFG::FilteredCFGNode < CFGFilterFunction >
                                                current)
                                {
                                        if (treeDirection == PRE_DOMINATOR)
                                                return current.inEdges();
                                        else
                                                return current.outEdges();
                                }

                        VirtualCFG::FilteredCFGNode < CFGFilterFunction > target(VirtualCFG::FilteredCFGEdge <
                                        CFGFilterFunction > outedge)
                        {
                                if (treeDirection == PRE_DOMINATOR)
                                        return outedge.target();
                                else
                                        return outedge.source();
                        }
                        
                        VirtualCFG::FilteredCFGNode < CFGFilterFunction > source(VirtualCFG::FilteredCFGEdge <
                                        CFGFilterFunction > outedge)
                        {
                                if (treeDirection == PRE_DOMINATOR)
                                        return outedge.source();
                                else
                                        return outedge.target();
                        }
                        
                        // ! treeDirection stores the traversal direction and indicates construction of a dominator or post-dominator tree
                        Direction treeDirection;
        };

        // CI (01/23/2007): Implemented the DT for the VirtualCFG interface with
        // the Lingauer-Tarjan algorithm
        //! TemplatedDominatorTree constructs a dominator/postdominator tree for a cfg. For the template parameter any cfg following Jeremias interface may be used
        template < typename CFGFilterFunction > class TemplatedDominatorTree:public DominatorForwardBackwardWrapperClass <CFGFilterFunction >
        {
                public:
                        //! writes the DT in DOT-notation to the file given in filename         
                        void writeDot(char *filename);

                        //!constructor for the DT. Head is the start point for the DT construction. DT works for SgFunctionDefintion nodes, unexpected behaviour for non-SgFunctionDefinition!!!
                        //Direction determines Pre/Post-Dominator construction
#ifdef _MSC_VER
                        TemplatedDominatorTree( SgNode * head, Direction d =    PRE_DOMINATOR );
#else
                        TemplatedDominatorTree(SgNode * head, Direction d =     DominatorForwardBackwardWrapperClass <CFGFilterFunction>::PRE_DOMINATOR);
#endif
                        // TemplatedDominatorTree(VirtualCFG::FilteredCFGNode<CFGFilterFunction> 
                        // cfg , Direction d = PRE);

                        // ! get the CFG the dominator tree is built from
                        // ControlFlowGraph * getCFG() {return _cfg;}
                        // ! returns the corresponding direction for the numbering of the CFG.
                        // ControlFlowGraph::ID_dir getCFGDirection() {return _iddir;}
                        
                        // ! returns the number of nodes in the tree
                        int getSize()
                        {
                                return idom.size();
                        }

                        //! returns the set of nodes directly dominated by nodeID
                        std::set<int> getDirectDominatedSet(int nodeID)
                        {
                                std::set<int> dds;
                                for (unsigned int i=0;i<idom.size();i++)
                                {
                                        if (idom[i]==nodeID)    dds.insert(i);
                                }
                                return dds;
                        }
/*
                        //! returns true if b is dominated by a (a is on the path form b to the root)
                        int isDomintated(int a, int b)
                        {
                                // a node dominates itself
                                if (a == b)
                                        return true;
                                int tmp = b;

                                while (tmp > 0 && tmp < a)
                                {
                                        tmp = getImDomID(tmp);
                                        if (tmp == a)
                                                return true;
                                }
                                return false;
                        }
*/
                        // ! for a given nodeID, return the id of its immediate dominator
                        int getImDomID(int i)
                        {
                                return idom[i];
                        }

                        //! get the ImDomID for given SgNode, returns negative for non-cfg-node
                        int getImDomID(VirtualCFG::FilteredCFGNode < CFGFilterFunction > node)
                        {
                                int id = getID(node);

                                if (id < 0)
                                        return -1;
                                else
                                        return getImDomID(id);
                        }

                        //! calculates if a dominates b, i.e. a is on the path from b to the root
                        bool dominates(int a,int b)
                        {
                                if (a==0) return true;
                                int i=b;
                                while(i>0)
                                {
                                        i=getImDomID(i);
                                        if (i==a) return true;                                  
                                }
                                return false;
                        }
                        
                        //! returns true if node a dominates node b, see dominates(int a,int b)         
                        bool dominates(VirtualCFG::FilteredCFGNode < CFGFilterFunction > a,VirtualCFG::FilteredCFGNode < CFGFilterFunction > b)
                        {
                                return dominates(getID(a),getID(b));
                        }

                        // !for an ID return the corresponing CFGNode
                        VirtualCFG::FilteredCFGNode < CFGFilterFunction > getCFGNodeFromID(unsigned int id)
                        {
                                if ( /* id >= 0 && */ id < idToNode.size())
                                        return idToNode[id];
                                ROSE_ASSERT(false);
                                return cfgRoot;
                        }
                        // ! for an CFG Node, return the corresponding id
                        int getID(VirtualCFG::FilteredCFGNode < CFGFilterFunction > node)
                        {
                                if (nodeToIdMap.count(node) == 0)
                                {
                                        return -1;
                                }
                                else
                                {
                                        return nodeToIdMap[node];
                                }
                        }
                private:
                        // ! this is the cfg-root for dominator construction, for pre this is
                        // the source, for post this is the sink of the cfg
                        VirtualCFG::FilteredCFGNode < CFGFilterFunction > cfgRoot;

                        // use a vector for mapping id's to nodes, since all ids are consecutive
                        std::vector < VirtualCFG::FilteredCFGNode < CFGFilterFunction > >idToNode;
                        // a map for mapping nodes to id's,since this might be a sparse set
                        std::map < VirtualCFG::FilteredCFGNode < CFGFilterFunction >, int >nodeToIdMap;

                        //! inits the internal structures for idom calculation
                        void init();
                        //! create the dfs-order of the cfg
                        void depthFirstSearch();
                        //! calculate immediate dominators
                        void calculateImmediateDominators();
                        //! helperfunction according to lingauer&tarjan
                        int eval(int);
                        //! create the forrest of parents
                        void link(int source, int target);
                        //! internal variables
                        std::vector < int >semi, idom, ancestor, dfsParent;
                        std::vector < std::set < int > > buckets;

                        //!debug output
                        void printInfo()
                        {
                                std::cout <<"dfs#\tanc \tsemi\tidom\tbucket"<<std::endl;
                                for(unsigned int i=0;i<idToNode.size();i++)
                                {       
                                        std::cout <<i<<"\t"<<ancestor[i]<<"\t"<<semi[i]<<"\t"<<idom[i]<<"\t"<<buckets[i]<<std::endl;
                                }
                        }
        };

        struct DefaultBasicDominatorTreeIsStatementFilter
        {
                bool operator() (VirtualCFG::CFGNode cfgn) const
                {
                        // get rid of all beginning nodes
                        if (!cfgn.isInteresting())
                                return false;
                        SgNode *n = cfgn.getNode();
                        if (isSgStatement(n))
                                return true;
                        return false;
                }
        };

        //!STD-Dominator-Tree
        typedef TemplatedDominatorTree < DefaultBasicDominatorTreeIsStatementFilter > DominatorTree;

        // end of namespace: DominatorTreesAndDominanceFrontiers
};

#include "DominatorTreeImpl.h"
#endif
