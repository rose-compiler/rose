//#include <rose.h>
#include "filteredCFG.h"
#include <sstream>
#include <iomanip>
#include <stdint.h>
#include <set>

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

#define DEBUG_CFG_HEADER 0

namespace VirtualCFG
{
  /* Documented by Liao, May not be entirely accurate. 2/14/2012
   *
   * Filtered CFG is handled internally when FilteredCFGNode <T>::inEdges() and ::outEdges() are requested
   * Take FilteredCFGNode < FilterFunction >::outEdges() as an example, here is how it works:
   * Each raw outgoing virtual CFG edge is a candidate, and converted to a CFGPath each. 
   * For each candidate CFGPath, check the target Node (the node pointed to by the last edge of the path) 
   *  - if the target node is an interesting node according to the filter function, then the path is good, the edge can be returned as it is mostly. 
   *  - if the target node is a CFG node to be filtered out, then the original path is extended to include all successors 
   *    By following raw outEdges of the target node, we now have N candidate CFGPaths if there are N raw outEdges of target 
   *    - Recursively handle all the new N CFGPaths until a path has a target node which is not being filtered out (interesting)
   */

    template < typename FindSuccessors, // a function to obtain successor edges of a node
               typename FindEnd,  // obtain the final node of a CFG path
               typename DontAddChildren,  // filter function (functor)
               typename Join, // merge two paths into one path
               typename FilteredEdge > 
    struct MakeClosure // a template struct to make raw input edges closure (filter out unnecessary edges) 
    {
        std::set < CFGNode > visitedNodes;
        std::vector < CFGPath > visitedPaths;
        const FindSuccessors & findSuccessors;
        const FindEnd & findEnd;
        const DontAddChildren & dontAddChildren;
        const Join & join;

        MakeClosure(const FindSuccessors & findSuccessors, 
                    const FindEnd & findEnd,
                    const DontAddChildren & dontAddChildren,
                    const Join & join) :
            findSuccessors(findSuccessors), findEnd(findEnd),
            dontAddChildren(dontAddChildren), join(join)
        {
        }

        //! Process one CFGPath at a time: make sure the end edge is an interesting one (should not be filtered out)
        void go(const CFGPath & p)
        {
            CFGNode end = findEnd(p);// obtain the final node of the path

            // skip visited end CFGNode, the corresponding successor paths of the end node are processed already
            if (visitedNodes.find(end) != visitedNodes.end()) 
               return;
            visitedNodes.insert(end); //bookkeeping
            visitedPaths.push_back(p);
            // Reach an end CFG node which should be not filtered out. The path is valid already (ending with an interesting node).
            if (dontAddChildren(end)) 
                  return;
            // The end Node is one which will be filtered out, look further down the successors:outEdges() or inEdges()
            // to find an interesting node as the new end
            std::vector < CFGEdge > edges = findSuccessors(end); 
            for (unsigned int i = 0; i < edges.size(); ++i)
            {
                go(join(p, edges[i])); // connect the current path to successors, for each connected path, 
                                       //do the same processing (make sure end node is interesting)
            }
        }
        //! Process visited CFGPaths: convert them into edges
        std::vector < FilteredEdge > filter() const
        {
            std::vector < FilteredEdge > edges;
            // for each valid paths
            for (std::vector < CFGPath >::const_iterator i = visitedPaths.begin(); i != visitedPaths.end(); ++i)
            { 
                const CFGPath & p = *i; 
                if (dontAddChildren(findEnd(p))) // if the end edge of a current path should not filtered out (interesting path)
                   edges.push_back(FilteredEdge(*i));
            }
            return edges;
        }
    };
    // a template function to instantiate MakeClosure
    template < typename FilteredEdge, 
               typename FindSuccessors, 
               typename FindEnd,
               typename AddChildren, // CFG Filter function
               typename Join > 
    std::vector < FilteredEdge > makeClosure(const std::vector < CFGPath > &p, //  a vector of CFGPath be be made closure, in of out edges
                                             const FindSuccessors & findSuccessors,
                                             const FindEnd & findEnd,
                                             const AddChildren & addChildren, // filter functor
                                             const Join & join)
    {
        MakeClosure < FindSuccessors, FindEnd, AddChildren, Join, FilteredEdge > 
           mc(findSuccessors, findEnd, addChildren, join);
        for (unsigned int i = 0; i < p.size(); ++i)
            mc.go(p[i]);
        return mc.filter();
    }

   // internal function: make a set of raw edges closure
    template < typename FilteredEdge, typename Filter >
    std::vector < FilteredEdge > makeClosure(const std::vector < CFGEdge > &orig, // input raw edges
                                                   std::vector < CFGEdge > (CFGNode::*closure) ()const, // FindSuccessors operator: CFGNode::outEdges() in fact
                                                   CFGNode(CFGPath::*otherSide) ()const, //FindEnd operator: CFGPath::target() 
                                                   CFGPath(*merge) (const CFGPath &, const CFGPath &), // Join operator: VirtualCFG::mergePaths()
                                             const Filter & filter) // the filter function
    {
        std::vector < CFGPath > paths(orig.begin(), orig.end()); // convert each raw edges into a path: 1-to-1 conversion for now
        return makeClosure < FilteredEdge > (paths, 
                                             std::mem_fn(closure),
                                             std::mem_fn(otherSide),
                                             filter,
                                             merge);
    }


    // Class Impl: user-level interface function for outEdges for FilteredCFGNode <T>, Only FilterFunction is needed
    // The returned edges already make the filtered CFG closure (internally and transparently)
    template < typename FilterFunction > 
    std::vector < FilteredCFGEdge < FilterFunction > >  FilteredCFGNode < FilterFunction >::outEdges()const
    {
        return makeClosure < FilteredCFGEdge < FilterFunction > >(n.outEdges(), // start with raw CFGNode's outEdges
                                                                  &CFGNode::outEdges, //FindSuccessors operator
                                                                  &CFGPath::target, //FindEnd operator, the target node the path leads to
                                                                  &mergePaths, // merge/join operator: VirtualCFG::mergePaths(), defined in virtualCFG.h
                                                                  filter); // the FilterFunction member of FilteredCFGNode
    }
    // Class Impl: user-level interface function for inEdges() of FilteredCFGNnode <T>
    template < typename FilterFunction > 
    std::vector < FilteredCFGEdge < FilterFunction >  > FilteredCFGNode < FilterFunction >::inEdges() const
    {
        return makeClosure < FilteredCFGEdge < FilterFunction > >(n.inEdges(),
                                                                  &CFGNode::inEdges,
                                                                  &CFGPath::source,
                                                                  &mergePathsReversed, 
                                                                  filter);
    }
    // ---------------------------------------------
    // DOT OUT IMPL
    template < typename NodeT, typename EdgeT ,bool Debug>  /*Filtered Node Type, Filtered CFG Type*/
    class CfgToDotImpl
    {
        std::multimap < SgNode *, NodeT > exploredNodes;
        std::set < SgNode * >nodesPrinted;
        std::ostream & o;

      public:
      CfgToDotImpl(std::ostream & o) :
        exploredNodes(), nodesPrinted(), o(o)
        {
        }
      void processNodes(NodeT n);
    };

//! Helper function to print Node information
template < typename NodeT > 
inline 
void printNode(std::ostream & o, const NodeT & n)
   {
        std::string id = n.id();
        std::string nodeColor = "black";

        if (isSgStatement(n.getNode()))
            nodeColor = "blue";
        else if (isSgExpression(n.getNode()))
            nodeColor = "green";
        else if (isSgInitializedName(n.getNode()))
            nodeColor = "red";

        o << id << " [label=\""  << escapeString(n.toString()) << "\", color=\"" << nodeColor <<
            "\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\"];\n";
    }

//! Edge printer
template < typename EdgeT >
inline void printEdge(std::ostream & o, const EdgeT & e, bool isInEdge)
    {
#if 0
      std::cout << "In printEdge(): e.toString() = " << e.toString() << std::endl;
#endif
        o << e.source().id() << " -> " << e.target().id() << " [label=\"" << 
          escapeString(e.toString()) << "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
    }

//! Print out a node plus all its outgoing edges
template < typename NodeT, typename EdgeT > 
void printNodePlusEdges(std::ostream & o,NodeT n);

/* Internal template function handles the details*/
template < typename NodeT, typename EdgeT ,bool Debug>
void CfgToDotImpl < NodeT, EdgeT, Debug >::processNodes(NodeT n)
   {
     ROSE_ASSERT(n.getNode());
     std::pair < typename std::multimap < SgNode *, NodeT >::const_iterator, typename std::multimap < SgNode *, NodeT >::const_iterator > ip = exploredNodes.equal_range(n.getNode());

     for (typename std::multimap < SgNode *, NodeT >::const_iterator i = ip.first; i != ip.second; ++i)
        {
          if (i->second == n)
             {
               return;
             }
        }

     exploredNodes.insert(std::make_pair(n.getNode(), n));

#if DEBUG_CFG_HEADER
  // printf ("In CfgToDotImpl < NodeT, EdgeT, Debug >::processNodes(): o = %p = %s \n",o.getNode(),o.getNode()->class_name().c_str());
     printf ("In CfgToDotImpl < NodeT, EdgeT, Debug >::processNodes(): n.getNode() = %p = %s \n",n.getNode(),n.getNode()->class_name().c_str());
#endif

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

//! Print out a node plus all its outgoing edges
template < typename NodeT, typename EdgeT > 
void printNodePlusEdges(std::ostream & o, NodeT n)
   {
#if DEBUG_CFG_HEADER
     printf ("In printNodePlusEdges(): n.getNode() = %p = %s \n",n.getNode(),n.getNode()->class_name().c_str());
#endif

     printNode(o, n);

     std::vector < EdgeT > outEdges = n.outEdges();
     for (unsigned int i = 0; i < outEdges.size(); ++i)
        {
#if DEBUG_CFG_HEADER
          printf ("In printNodePlusEdges(): output edges: i = %u \n",i);
#endif
          printEdge(o, outEdges[i], false);
        }

#ifdef DEBUG
  // DQ (1/19/2018): Note that this is a problem for tesst2007_151.f (at least, and perhaps a few other Fortran files).

#if DEBUG_CFG_HEADER
     printf ("In printNodePlusEdges(): output the inEdges for debugging \n");
#endif
     std::vector < EdgeT > inEdges = n.inEdges();
     for (unsigned int i = 0; i < inEdges.size(); ++i)
        {
#if DEBUG_CFG_HEADER
          printf ("In printNodePlusEdges(): input edges: i = %u \n",i);
#endif
          printEdge(o, inEdges[i], true);
        }
#endif

#if DEBUG_CFG_HEADER
     printf ("Leaving printNodePlusEdges(): n.getNode() = %p = %s \n",n.getNode(),n.getNode()->class_name().c_str());
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

/*User-level interface template function */
template < typename FilterFunction > 
std::ostream & cfgToDot(std::ostream & o, /* output stream*/
                            std::string graphName, /* graph name*/
                            FilteredCFGNode < FilterFunction > start) /* start filtered CFG Node */
   {
        o << "digraph " << graphName << " {\n";
        CfgToDotImpl < FilteredCFGNode < FilterFunction >,
                       FilteredCFGEdge < FilterFunction > ,
                       false>  impl(o);
        impl.processNodes(start);
        o << "}\n";
        return o;
   }

}
