#ifndef FILTEREDCFGIMPL_H_
#define FILTEREDCFGIMPL_H_

//#include <rose.h>
#include "filteredCFG.h"
#include <sstream>
#include <iomanip>
#include <stdint.h>

// using namespace std;

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

namespace VirtualCFG
{

    template < typename FindSuccessors, typename FindEnd, typename DontAddChildren,
        typename Join, typename FilteredEdge > struct MakeClosure
    {
        std::set < CFGNode > visitedNodes;
        std::vector < CFGPath > visitedPaths;
        const FindSuccessors & findSuccessors;
        const FindEnd & findEnd;
        const DontAddChildren & dontAddChildren;
        const Join & join;

          MakeClosure(const FindSuccessors & findSuccessors, const FindEnd & findEnd,
                      const DontAddChildren & dontAddChildren,
                      const Join & join):findSuccessors(findSuccessors), findEnd(findEnd),
            dontAddChildren(dontAddChildren), join(join)
        {
        }


        void go(const CFGPath & p)
        {
            CFGNode end = findEnd(p);

            if (visitedNodes.find(end) != visitedNodes.end())
                  return;
              visitedNodes.insert(end);
              visitedPaths.push_back(p);
            if (dontAddChildren(end))
                  return;
              std::vector < CFGEdge > edges = findSuccessors(end);
            for (unsigned int i = 0; i < edges.size(); ++i)
            {
                go(join(p, edges[i]));
            }
        }

        std::vector < FilteredEdge > filter() const
        {
            std::vector < FilteredEdge > edges;
            for (std::vector < CFGPath >::const_iterator i = visitedPaths.begin();
                 i != visitedPaths.end(); ++i)
            {
                const CFGPath & p = *i;
                if (dontAddChildren(findEnd(p)))
                      edges.push_back(FilteredEdge(*i));
            }
            return edges;
        }
    };

    template < typename FilteredEdge, typename FindSuccessors, typename FindEnd,
        typename AddChildren,
        typename Join > std::vector < FilteredEdge > makeClosure(const std::vector < CFGPath > &p,
                                                            const FindSuccessors &
                                                            findSuccessors,
                                                            const FindEnd & findEnd,
                                                            const AddChildren & addChildren,
                                                            const Join & join)
    {
        MakeClosure < FindSuccessors, FindEnd, AddChildren, Join,
            FilteredEdge > mc(findSuccessors, findEnd, addChildren, join);
        for (unsigned int i = 0; i < p.size(); ++i)
            mc.go(p[i]);
        return mc.filter();
    }

    template < typename FilteredEdge, typename Filter >
        std::vector < FilteredEdge > makeClosure(const std::vector < CFGEdge > &orig,
                                            std::vector < CFGEdge > (CFGNode::*closure) ()const,
                                            CFGNode(CFGPath::*otherSide) ()const,
                                            CFGPath(*merge) (const CFGPath &, const CFGPath &),
                                            const Filter & filter)
    {
        std::vector < CFGPath > paths(orig.begin(), orig.end());
        return makeClosure < FilteredEdge > (paths, std::mem_fun_ref(closure),
                                             std::mem_fun_ref(otherSide), filter, merge);
    }


    // Class Impl
    template < typename FilterFunction > std::vector < FilteredCFGEdge < FilterFunction >
        >FilteredCFGNode < FilterFunction >::outEdges()const
    {
        return makeClosure < FilteredCFGEdge < FilterFunction > >(n.outEdges(),
                                                                  &CFGNode::outEdges,
                                                                  &CFGPath::target, &mergePaths,
                                                                  filter);
    }
    // Class Impl
    template < typename FilterFunction > std::vector < FilteredCFGEdge < FilterFunction >
        >FilteredCFGNode < FilterFunction >::inEdges() const
    {
        return makeClosure < FilteredCFGEdge < FilterFunction > >(n.inEdges(),
                                                                  &CFGNode::inEdges,
                                                                  &CFGPath::source,
                                                                  &mergePathsReversed, filter);
    }
    // ---------------------------------------------
    // DOT OUT IMPL
    template < typename NodeT, typename EdgeT ,bool Debug> class CfgToDotImpl
    {
        std::multimap < SgNode *, NodeT > exploredNodes;
        std::set < SgNode * >nodesPrinted;
        std::ostream & o;

      public:
      CfgToDotImpl(std::ostream & o):exploredNodes(), nodesPrinted(), o(o)
        {
        }
        void processNodes(NodeT n);
    };

    template < typename NodeT > inline void printNode(std::ostream & o, const NodeT & n)
    {
        std::string id = n.id();
        std::string nodeColor = "black";

        if (isSgStatement(n.getNode()))
            nodeColor = "blue";
        else if (isSgExpression(n.getNode()))
            nodeColor = "green";
        else if (isSgInitializedName(n.getNode()))
            nodeColor = "red";

        o << id << " [label=\""  << escapeString(n.
                                                toString()) << "\", color=\"" << nodeColor <<
            "\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\", " 
            << "group=\"" << SageInterface::getEnclosingFunctionDeclaration(n.getNode())->get_name().getString()
            << "\"];\n";
    }

    template < typename EdgeT >
        inline void printEdge(std::ostream & o, const EdgeT & e, bool isInEdge)
    {
        o << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(e.
                                                                                          toString
                                                                                          ()) <<
            "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
    }

    template < typename NodeT, typename EdgeT > void printNodePlusEdges(std::ostream & o,
                                                                        NodeT n);

    template < typename NodeT, typename EdgeT ,bool Debug>
        void CfgToDotImpl < NodeT, EdgeT, Debug >::processNodes(NodeT n)
    {
        ROSE_ASSERT(n.getNode());
        std::pair < typename std::multimap < SgNode *, NodeT >::const_iterator,
            typename std::multimap < SgNode *, NodeT >::const_iterator > ip =
            exploredNodes.equal_range(n.getNode());
        for (typename std::multimap < SgNode *, NodeT >::const_iterator i = ip.first;
             i != ip.second; ++i)
        {
            if (i->second == n)
                return;
        }
        exploredNodes.insert(make_pair(n.getNode(), n));
        printNodePlusEdges<NodeT, EdgeT>(o, n);
        std::vector < EdgeT > outEdges = n.outEdges();
        for (unsigned int i = 0; i < outEdges.size(); ++i)
        {
            ROSE_ASSERT(outEdges[i].source() == n);
            processNodes(outEdges[i].target());
        }
        std::vector < EdgeT > inEdges = n.inEdges();
        for (unsigned int i = 0; i < inEdges.size(); ++i)
        {
            ROSE_ASSERT(inEdges[i].target() == n);
            processNodes(inEdges[i].source());
        }
    }

    template < typename NodeT, typename EdgeT > void printNodePlusEdges(std::ostream & o,
                                                                        NodeT n)
    {
        printNode(o, n);
        std::vector < EdgeT > outEdges = n.outEdges();
        for (unsigned int i = 0; i < outEdges.size(); ++i)
        {
            printEdge(o, outEdges[i], false);
        }
				#ifdef DEBUG
        std::vector < EdgeT > inEdges = n.inEdges();
        for (unsigned int i = 0; i < inEdges.size(); ++i)
        {
            printEdge(o, inEdges[i], true);
        }
				#endif
    }
#if 0
    template < typename NodeT, typename EdgeT >
        void CfgToDotImpl < NodeT, EdgeT >::processNodes(SgNode *)
    {
        for (typename std::multimap < SgNode *, NodeT >::const_iterator it =
             exploredNodes.begin(); it != exploredNodes.end(); ++it)
        {
            printNodePlusEdges < NodeT, EdgeT > (o, it->second);
        }
    }
#endif


    template < typename FilterFunction > std::ostream & cfgToDot(std::ostream & o,
                                                                 std::string graphName,
                                                                 FilteredCFGNode <
                                                                 FilterFunction > start)
    {
        o << "digraph " << graphName << " {\n";
        CfgToDotImpl < FilteredCFGNode < FilterFunction >,
            FilteredCFGEdge < FilterFunction > ,false>impl(o);
        impl.processNodes(start);
        o << "}\n";
        return o;
    }

}

#endif //! define FILTEREDCFGIMPL_H_
