// Author: Marc Jasper, 2016.

#include "DotGraphCfgFrontend.h"

using namespace SPRAY;
using namespace std;


Flow DotGraphCfgFrontend::parseDotCfg(string filename) {

  Flow edgeSet;

  ifstream dot_graph(filename.c_str());
  boost::regex transition_expr("[ ]*[[:digit:]]+[ ]*->[ ]*[[:digit:]]+.*");
  boost::regex integer_expr("[[:digit:]]+");
  boost::regex edge_name_expr("label=\"([^\"]*)\"");

  if (dot_graph.is_open()) {
    std::string line;
    while (std::getline(dot_graph, line)){
      pair<size_t, size_t> node_labels;
      if (boost::regex_match(line, transition_expr)) {
	boost::smatch what;
        vector<std::string> labels; 		
        boost::split_regex(labels, line, boost::regex("->"));
        for(vector<string>::iterator i = labels.begin(); i < labels.end(); i++) {
  	  if (boost::regex_search(*i, what, integer_expr)) {
	  } else {
	    cout << "ERROR: could match a transition in the .dot-file but then unable to find any integer labels for either source or target." << endl;
	    ROSE_ASSERT(0);
	  }
	  if (i == labels.begin()) {
	    node_labels.first =  boost::lexical_cast<size_t>(what[0]);
	  } else {
	    node_labels.second =  boost::lexical_cast<size_t>(what[0]);
	  } 
	}
	string edge_label = "";
	if (boost::regex_search(line, what, edge_name_expr)) {
	  edge_label = what[1];
	}
	cout << "DEBUG: parsed transition: (" << node_labels.first << ", "<< edge_label << ", "<< node_labels.second << ")"<<endl;
	//TODO: add Edge to the CFG here
	Label source = Label(node_labels.first);
	Label target = Label(node_labels.second);
	Edge edge = Edge(source, target);
	edge.setAnnotation(edge_label);
	edgeSet.insert(edge);
      }
    }
    dot_graph.close();
  }
  return edgeSet;
} 
