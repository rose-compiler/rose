// Author: Markus Schordan
// $Id: DOTRepresentation.C,v 1.3 2006/04/24 00:21:27 dquinlan Exp $

#ifndef DOTREPRESENTATION_C
#define DOTREPRESENTATION_C

#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include "DOTRepresentation.h"

// DQ (4/23/2006): Required for g++ 4.1.0!
#include "assert.h"

template<class NodeType>
DOTRepresentation<NodeType>::DOTRepresentation() {
  dotout = new std::ostringstream(); 
  assert (dotout != NULL);
}

template<class NodeType>
DOTRepresentation<NodeType>::~DOTRepresentation() {
  delete dotout;
  dotout = NULL;
}

template<class NodeType>
void DOTRepresentation<NodeType>::clear() {
  assert(dotout != NULL);
  delete dotout; 
  dotout=new std::ostringstream();
  assert (dotout != NULL);
}

template<class NodeType>
void
DOTRepresentation<NodeType>::writeToFileAsGraph(std::string filename) {
  std::string graphName="\"G"+filename+"\"";
  std::ofstream dotfile(filename.c_str());
   dotfile << graphStart(graphName) << (*dotout).str() << graphEnd();
}

template<class NodeType>
void
DOTRepresentation<NodeType>::writeToFile(std::string filename) {
  std::ofstream dotfile(filename.c_str());
  dotfile << (*dotout).str();
}

template<class NodeType>
std::string
DOTRepresentation<NodeType>::graphStart(std::string graphName) {
  return "digraph " + graphName + " {\n";
}

template<class NodeType>
std::string
DOTRepresentation<NodeType>::graphEnd() {
  return "}\n";
}

template <class T>
T escape_double_quotes(const T & input) {
  std::string result(input);
  size_t cur = result.find("\"");
  while (cur < result.size() && cur != std::string::npos) {
    result.replace(cur, 1, "\\\"");
    cur = result.find("\"", cur+2);
  }
  return result;
}

template<class NodeType>
void 
DOTRepresentation<NodeType>::addNode(NodeType node, std::string nodelabel, std::string option) {
  (*dotout) << nodeName(node) << "[label=\"" << escape_double_quotes(nodelabel) << "\" " << option << "];" << std::endl;
}

template<class NodeType>
void DOTRepresentation<NodeType>::
addEdge(NodeType node1, TraceType downtrace, TraceType uptrace, std::string edgelabel, NodeType node2, std::string option) {
  (*dotout) << nodeName(node1)
	    << " -> "
	    << nodeName(node2)
	    << "[label=\"" << downtrace << ":" << uptrace << ":"<< edgelabel << "\" " << option << " dir=both];" << std::endl;
}
 
// for edges to revisited nodes (there is no uptrace)
template<class NodeType>
void DOTRepresentation<NodeType>::
addEdge(NodeType node1, TraceType downtrace, std::string edgelabel, NodeType node2, std::string option) {
  (*dotout) << nodeName(node1)
	    << " -> "
	    << nodeName(node2)
	    << "[label=\"" << downtrace << ":" << edgelabel << "\" " << option << " arrowhead=odot];" << std::endl;
}
 
// for edges to revisited nodes (there is no uptrace)
template<class NodeType>
void DOTRepresentation<NodeType>::
addEdge(NodeType node1, std::string edgelabel, NodeType node2, std::string option) {
  (*dotout) << nodeName(node1)
	    << " -> "
	    << nodeName(node2)
	   << "[label=\"" << edgelabel << "\" " << option << " ];" << std::endl;
}

// for edges to revisited nodes (there is no uptrace)
template<class NodeType>
void DOTRepresentation<NodeType>::
addEdge(NodeType node1, NodeType node2, std::string option) {
  addEdge(node1,"",node2,option);
}
 
template<class NodeType>
void DOTRepresentation<NodeType>::
addNullValue(NodeType node, std::string nodelabel, std::string edgelabel, std::string option) {
  // a null value is represented by an edge to a diamond node, with the variable name as edge label
  // edge
  (*dotout) << nodeName(node)
	    << " -> "
	    << nullNodeName(node,edgelabel)
	    << "[label=\"" << edgelabel << "\" " << "dir=none "<< option << "];" << std::endl;
  // node
  (*dotout) << nullNodeName(node,edgelabel)
	    << "[label=\"" << nodelabel << "\" shape=diamond "<< option <<"];" << std::endl;
}

template<class NodeType>
void DOTRepresentation<NodeType>::
addNullValue(NodeType node, TraceType trace, std::string varname, std::string option) {
  // a null value is represented by an edge to a diamond node, with the variable name as edge label
  // edge
  (*dotout) << "n_" << node
	    << " -> "
	    << "n_" << node << "__" << varname << "__null"
	    << "[label=\"" << trace << ":" << varname << "\" " << "dir=none "<< option << "];" << std::endl;
  // node
  (*dotout) << "n_" << node << "__" << varname << "__null"
	    << "[label=\""<< trace << ":\" shape=diamond "<< option <<"];" << std::endl;
}
 
template<class NodeType>
void DOTRepresentation<NodeType>::
addEmptyContainer(NodeType node, TraceType trace, std::string varname, std::string /*option*/) {
  (*dotout) << "n_" << node // node: holding null-reference to STL container, using [] to represent container-reference 
	    << " -> "
	    << "n_" << node << "__" << varname << "__null"
	    << "["<< "label=\"" << trace << ":" << varname << "[]\"" << " dir=none ];" << std::endl;
  (*dotout) << "n_" << node << "__" << varname << "__null"
	    << "[label=\"\" shape=diamond ];" << std::endl; // dot-null node
}

template<class NodeType>
std::string DOTRepresentation<NodeType>::
containerEdgeLabel(std::string containerVarName, ContainerIndexType count) {
  std::ostringstream s;
  s << containerVarName << "[" << count << "]";
  return s.str();
}

template<class NodeType>
std::string DOTRepresentation<NodeType>::traceFormat(TraceType td, TraceType bu) {
  std::ostringstream ss;
  ss << td << ":" << bu;
  return ss.str();
}

template<class NodeType>
std::string DOTRepresentation<NodeType>::traceFormat(TraceType tdPos, TraceType buPos, TraceType tdCount, TraceType buCount) {
  std::ostringstream ss;
  ss << tdPos << "(" << tdCount << "): " << buPos << "(" << buCount << ")";
  return ss.str();
}

template<class NodeType>
std::string DOTRepresentation<NodeType>::traceFormat(TraceType tracepos) {
  std::ostringstream ss;
  ss << tracepos;
  return ss.str();
}

template<class NodeType>
std::string DOTRepresentation<NodeType>::nonQuotedNodeName(NodeType node) {
  std::ostringstream ss;
  ss << node;
  return ss.str();
  //return string(typeid(*node).name()); // fuse set of all nodes with same type in one single node
}

template<class NodeType>
std::string DOTRepresentation<NodeType>::nodeName(NodeType node) {
  return "\"" + nonQuotedNodeName(node) + "\"";

}

template<class NodeType>
std::string DOTRepresentation<NodeType>::nullNodeName(NodeType node, std::string extention) {
  return "\""+ nonQuotedNodeName(node) + "__" + extention + "__null\"";
}

#endif
