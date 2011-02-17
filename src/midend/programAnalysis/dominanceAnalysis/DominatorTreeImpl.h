// #include "rose.h"
#include "filteredCFG.h"
#include "DominatorTree.h"
#include <map>
namespace DominatorTreesAndDominanceFrontiers
{
    template < typename CFGFilterFunction > void TemplatedDominatorTree <
        CFGFilterFunction >::writeDot(char *filename)
    {
        std::ofstream f(filename);

        f << "digraph \"G" << filename << "\" {" << std::endl;
        // output all of the nodes
        int dtSize = getSize();
        for (int i = 0; i < dtSize; i++)
        {
            f << "\"" << i << "\" [label = \"ID " <<i<<"\\n"<< escapeString(getCFGNodeFromID(i).
                toString()) << "\"];" << std::endl;
            if (i > 0)
                f << "\"" << getImDomID(i) << "\" -> \"" << i << "\";" << std::endl;
        }
        f << "}" << std::endl;

        f.close();
    }

        //! initialize cfg0root correctly
  template < typename CFGFilterFunction > TemplatedDominatorTree < CFGFilterFunction >::TemplatedDominatorTree(SgNode * head, Direction d):DominatorForwardBackwardWrapperClass < CFGFilterFunction > (d),
        cfgRoot((d ==
                 PRE_DOMINATOR) ? (head->cfgForBeginning()) : (head->cfgForEnd()))//,treeDirection(d)
    {
//        treeDirection = d;
        init();
        depthFirstSearch();
        calculateImmediateDominators();
    }

                // create the dfs-order for imdom calculations
    template < typename CFGFilterFunction > void TemplatedDominatorTree <
        CFGFilterFunction >::depthFirstSearch()
    {
        std::vector < VirtualCFG::FilteredCFGNode < CFGFilterFunction > >workList;
        workList.push_back(cfgRoot);
                                std::vector<int>parentWorkList;
                                parentWorkList.push_back(0);
        int nodeCount = 0;

        while (workList.size() > 0)
        {
                                                int parent;
            VirtualCFG::FilteredCFGNode < CFGFilterFunction > current = workList.back();
            workList.pop_back();
                                                parent=parentWorkList.back();parentWorkList.pop_back();
#ifdef DEBUG
                                                std::cout <<"DFS: processing node <"<<current.toString()<<">"<<nodeCount<<std::endl;
#endif
            // if the node is not in the map, it has not been processed
            if (nodeToIdMap.count(current) == 0)
            {
#ifdef DEBUG
                                                        std::cout <<"\tThis node has not been process, assigning ID "<<nodeCount<<std::endl;
#endif
                // the first ID is 0. therefore idToNode.size() returns the
                // next id
                nodeToIdMap[current] = nodeCount;
                // put the node in the vector
                idToNode.push_back(current);
                semi.push_back(nodeCount);
                idom.push_back(-1);
                ancestor.push_back(-1);
                dfsParent.push_back(parent);
                buckets.push_back(std::set < int >());

                std::vector < VirtualCFG::FilteredCFGEdge < CFGFilterFunction > >outEdges =
                    getDirectionModifiedOutEdges(current);
                for (unsigned int i = 0; i < outEdges.size(); i++)
                {
                    workList.push_back(target(outEdges[i]));
                                                                                parentWorkList.push_back(nodeCount);
                }
                nodeCount++;
            }
        }
#ifdef DEBUG
        std::cout << "Depth First Search order of CFG-Nodes" << std::endl;
//      VirtualCFG::FilteredCFGNode < CFGFilterFunction > testVar;
//      typename std::map < VirtualCFG::FilteredCFGNode < CFGFilterFunction >, int>::iterator mapTest;
        typename std::map < VirtualCFG::FilteredCFGNode < CFGFilterFunction >, int>::iterator i;
//, std::less < VirtualCFG::FilteredCFGNode < CFGFilterFunction > > >::iterator i;
        for (i = nodeToIdMap.begin(); i != nodeToIdMap.end(); i++)
        {
            std::cout << (*i).first.toString() << "\t" << (*i).second << std::endl;
        }
#endif
    }

                //! the preformance of the DT-algorithm can be improved by implementing a tree-averaging/redistributing algorithm in eval or link, usually it is done in eval
    template < typename CFGFilterFunction > void TemplatedDominatorTree <
        CFGFilterFunction >::link(int ancest, int current)
    {
        ancestor[current] = ancest;
    }

    template < typename CFGFilterFunction > int TemplatedDominatorTree <
        CFGFilterFunction >::eval(int node)
    {
        int cmp = node;
        int tmp = ancestor[cmp];
        while (tmp > -1 && ancestor[tmp] != -1)
        {
            if (semi[cmp] > semi[tmp])
                cmp = tmp;
            tmp = ancestor[tmp];
        }
        return cmp;
    }


    template < typename CFGFilterFunction > void TemplatedDominatorTree <
        CFGFilterFunction >::init()
    {
        semi.clear();
        idom.clear();
        ancestor.clear();
        dfsParent.clear();
        buckets.clear();

    }

#ifdef DEBUG
        //! in debug-mode this method is very verbose
    template < typename CFGFilterFunction > void TemplatedDominatorTree <
        CFGFilterFunction >::calculateImmediateDominators()
    {
        // do the bottom up part of calculating the semidominators
        // cout << "calculation semidominators"<<endl;;
                                std::cout << "init:"<<std::endl;
                                printInfo();
                                std::cout <<std::endl;
        for (int i = idToNode.size() - 1; i > 0; i--)
        {
                                        std::cout <<"dfs#"<<i<<std::endl;
                                        printInfo();
                                        std::cout<<std::endl;
                                        // get the correct defs parent
            int dfsP = dfsParent[i];

            // get current node from dfs-number
            VirtualCFG::FilteredCFGNode < CFGFilterFunction > current = idToNode[i];
            // cout <<"processing: \""<<current.toString()<<"\""<<endl;
            
                                                // for all predecessors of the current node
            std::vector < VirtualCFG::FilteredCFGEdge < CFGFilterFunction > >inEdges =
                getDirectionModifiedInEdges(current);
            for (unsigned int j = 0; j < inEdges.size(); j++)
            {
                int nodeID = nodeToIdMap[source(inEdges[j])];
                                                                std::cout <<"v="<<nodeID<<std::endl;
                // get the smallest possible semidom from that dfs-subtree
                int possiblySmallestSemiDom = eval(nodeID);
                                                std::cout <<"u=EVAL("<<nodeID<<")="<<possiblySmallestSemiDom<<std::endl;
                                                std::cout <<"semi("<<possiblySmallestSemiDom<<")="<<semi[possiblySmallestSemiDom]<<" < semi("<<i<<")="<<semi[i];
                if (semi[possiblySmallestSemiDom] < semi[i])
                                                                {
                                                        std::cout <<" -> semi("<<i<<")="<<semi[possiblySmallestSemiDom];
                    semi[i] = semi[possiblySmallestSemiDom];
                                                                }
                                                        std::cout <<std::endl;
            }
            // add this node to the list of nodes controlled by its
            // semidomniator
                                        std::cout <<"add "<<i<<" to bucket semi["<<i<<"]"<<semi[i]<<std::endl;
            buckets[semi[i]].insert(i);
                                        std::cout <<"link "<<dfsP<<","<<i<<std::endl;
            link(dfsP, i);
                                                                                
            // for all nodes in the bucket of this nodes parent
            for (std::set < int >::iterator j = buckets[dfsP].begin();
                 j != buckets[dfsP].end(); j++)
            {
                int tmpNodeID = eval(*j);

                // semi[(*j) is dfsParent
                if (semi[tmpNodeID] < dfsP)
                    idom[(*j)] = tmpNodeID;
                else
                    idom[(*j)] = dfsP;
            }
                                                printInfo();

            buckets[dfsP].clear();
        }
        for (unsigned int i = 1; i < idToNode.size(); i++)
        {
            if (idom[i] != semi[i])
            {
                idom[i] = idom[idom[i]];
            }
        }
        idom[0] = 0;
        std::cout << "Dominators:" << std::endl;
        for (unsigned int i = 0; i < idToNode.size(); i++)
        {
            std::cout << ">" << idToNode[i].toString() << "<: " << idom[i] << std::endl;
        }
     }
#else
    template < typename CFGFilterFunction > void TemplatedDominatorTree <
        CFGFilterFunction >::calculateImmediateDominators()
    {
        // do the bottom up part of calculating the semidominators
        for (int i = idToNode.size() - 1; i > 0; i--)
        {
                                        // get the correct defs parent
            int dfsP = dfsParent[i];

            // get current node from dfs-number
            VirtualCFG::FilteredCFGNode < CFGFilterFunction > current = idToNode[i];
            
                                                // for all predecessors of the current node
            std::vector < VirtualCFG::FilteredCFGEdge < CFGFilterFunction > >inEdges =
                getDirectionModifiedInEdges(current);
            for (unsigned int j = 0; j < inEdges.size(); j++)
            {
                int nodeID = nodeToIdMap[source(inEdges[j])];

                // get the smallest possible semidom from that dfs-subtree
                int possiblySmallestSemiDom = eval(nodeID);
                if (semi[possiblySmallestSemiDom] < semi[i])
                                                                {
                    semi[i] = semi[possiblySmallestSemiDom];
                                                                }
            }
            // add this node to the list of nodes controlled by its
            // semidomniator
            buckets[semi[i]].insert(i);
            link(dfsP, i);
                                                                                
            // for all nodes in the bucket of this nodes parent
            for (std::set < int >::iterator j = buckets[dfsP].begin();
                 j != buckets[dfsP].end(); j++)
            {
                int tmpNodeID = eval(*j);

                // semi[(*j) is dfsParent
                if (semi[tmpNodeID] < dfsP)
                    idom[(*j)] = tmpNodeID;
                else
                    idom[(*j)] = dfsP;
            }

            buckets[dfsP].clear();
        }
        for (unsigned int i = 1; i < idToNode.size(); i++)
        {
            if (idom[i] != semi[i])
            {
                idom[i] = idom[idom[i]];
            }
        }
        idom[0] = 0;
     }
#endif

}
