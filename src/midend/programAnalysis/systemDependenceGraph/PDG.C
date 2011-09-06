/* 
 * File:   PDG.C
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#include "PDG.h"
#include "util.h"
#include <VariableRenaming.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/graph/graphviz.hpp>


#define foreach BOOST_FOREACH


namespace SDG
{


void ProgramDependenceGraph::build(const CFG& cfg)
{
    std::map<CFGVertex, Vertex> cfgVerticesToPdgVertices;

    //// Remove all nodes and edges.
    //this->clear();

    // Add the entry.
    entry_ = boost::add_vertex(*this);

    // Add all CFG vertices to PDG.
    foreach (CFGVertex cfgVertex, boost::vertices(cfg))
    {
        if (cfgVertex == cfg.getEntry() || cfgVertex == cfg.getExit())
            continue;

        Vertex pdgVertex = boost::add_vertex(*this);
        (*this)[pdgVertex] = cfg[cfgVertex];
        cfgVerticesToPdgVertices[cfgVertex] = pdgVertex;
    }



    // Add control dependence edges.
    addControlDependenceEdges(cfgVerticesToPdgVertices, cfg);

    // Add data dependence edges.
    addDataDependenceEdges(cfgVerticesToPdgVertices, cfg);

}


void ProgramDependenceGraph::addControlDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToPdgVertices,
        const CFG& cfg)
{
    // Build the dominance frontiers of the reverse CFG, which represents the CDG
    // of the original CFG.
    CFG rvsCfg = cfg.makeReverseCopy();
    DominanceFrontiersT domFrontiers = buildDominanceFrontiers(rvsCfg);

    foreach (const DominanceFrontiersT::value_type& vertices, domFrontiers)
    {
        Vertex src, tar;

        CFGVertex from = vertices.first;

        if (from == cfg.getEntry() || from == cfg.getExit())
            continue;

        ROSE_ASSERT(cfgVerticesToPdgVertices.count(from));
        src = cfgVerticesToPdgVertices.find(from)->second;

        typedef std::pair<CFGVertex, std::vector<CFGEdge> > VertexEdges;
        foreach (const VertexEdges& vertexEdges, vertices.second)
        {            
            CFGVertex to = vertexEdges.first;
            const std::vector<CFGEdge>& cdEdges = vertexEdges.second;

            ROSE_ASSERT(cfgVerticesToPdgVertices.count(to));
            tar = cfgVerticesToPdgVertices.find(to)->second;

            foreach (const CFGEdge& cdEdge, cdEdges)
            {
                // Add the edge.
                Edge edge = boost::add_edge(tar, src, *this).first;
                //(*this)[edge].cfgEdge   = edgeTable[rvsCfg[cdEdge]];
                (*this)[edge].type = PDGEdge::ControlDependence;
                (*this)[edge].condition = rvsCfg[cdEdge]->condition();
                (*this)[edge].caseLabel = rvsCfg[cdEdge]->caseLabel();
            }
        }
    }

    // Connect an edge from the entry to every node which does not have a control dependence.
    foreach (Vertex pdgVertex, boost::vertices(*this))
    {
        if (pdgVertex == entry_) continue;
        //if ((*this)[pdgVertex] == cfg[cfg.getExit()])
        //    continue;

        if (boost::in_degree(pdgVertex, *this) == 0)
        {
            Edge edge = boost::add_edge(entry_, pdgVertex, *this).first;
            //(*this)[edge].cfgEdge   = edgeTable[rvsCfg[cdEdge]];
            (*this)[edge].type = PDGEdge::ControlDependence;
            (*this)[edge].condition = VirtualCFG::eckTrue;
        }
    }
}


void ProgramDependenceGraph::addDataDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToPdgVertices,
        const CFG& cfg)
{

}

void ProgramDependenceGraph::toDot(const std::string& filename) const
{
    std::ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, *this,
            boost::bind(&ProgramDependenceGraph::writeGraphNode, this, ::_1, ::_2),
            boost::bind(&ProgramDependenceGraph::writeGraphEdge, this, ::_1, ::_2));
}

void ProgramDependenceGraph::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
    std::string str, style;
    const PDGEdge& pdgEdge = (*this)[edge];
    if (pdgEdge.type == PDGEdge::ControlDependence)
    {
        std::string label;
        switch (pdgEdge.condition)
        {
            case VirtualCFG::eckTrue:
                label = "T";
                break;
            case VirtualCFG::eckFalse:
                label = "F";
                break;
            case VirtualCFG::eckCaseLabel:
                label = "case " + pdgEdge.caseLabel->unparseToString();
                break;
            case VirtualCFG::eckDefault:
                label = "default";
                break;
            default:
                break;
        }
        out << "[label=\"" << label << "\", style=\"" << "solid" << "\"]";
    }
    else
    {
        foreach (const VarName& varName, (*this)[edge].varNames)
            str += VariableRenaming::keyToString(varName) + " ";
        style = "dotted";
        out << "[label=\"" << str << "\", style=\"" << style << "\"]";
    }
}


}
