#include "dependence_table.h"
#include "OperatorDescriptors.h"
#include "OperatorAnnotation.h"
#include "AstUtilInterface.h"
#include "AstInterface_ROSE.h"

namespace {
    std::string  wrap_string (const std::string& s) {
       std::string new_string;
       unsigned wrap = 10, maxwrap = 20, index = 0;
       for (auto c : s) {
         ++index;
         if (index > maxwrap || (index > wrap && c == ':')) {
             new_string.push_back('\\');
             new_string.push_back('n');
             index = 0;
         } 
         new_string.push_back(c);
       }
       return new_string;
    };
};

namespace AstUtilInterface {

std::string CollectDependences::local_read_string(std::istream& input_file) {
    std::string next_string;
    char c ;
    while ((input_file >> c).good()) {
       switch (c) {
       case '\\':
          next_string.push_back(c);
          if (!(input_file >> c).good()) 
              return next_string;  
          break; 
       case '\"': 
          next_string.push_back(c);
          while ((input_file >> c).good() && c != '\"') {
             next_string.push_back(c);
          }
          next_string.push_back(c);
          Log.push("reading quoted string " + next_string);
          return next_string;
       case ' ':
       case '\n':
       case '\r':
            if (next_string != "") {
              // This starts a new token. Return the current one.
              Log.push("Seeing separator. Finished reading token " + next_string);
              return next_string;
            } 
            // Skip empty space.
            break;
       case ':': {
          // Make sure read double "::" as part of a name.
          char c1 = input_file.peek();
          if (c1 == ':') {
             input_file >> c1;
             next_string += "::";
             Log.push("Seeing `::'. continue reading token " + next_string);
             break;
          } 
          // Otherwise, seeing a single ':'
          [[fallthrough]]; // Explicitly indicates intentional fall-through 
         }
       case '[':
       case ']':
       case '{':
       case '}':
       case ';':
       case '=':
            if (next_string != "") {
              // This starts a new token. Return the current one.
              input_file.putback(c);
              Log.push("Seeing separator. Finished reading token " + next_string);
            } else {
             // Found a token. Return it.
             next_string.push_back(c);
             Log.push("reading separator token " + next_string);
            }
            return next_string;
       default: 
             next_string.push_back(c);
             break;
      }
    }
    Log.push("Return Next token  " + next_string);
    return next_string;
}

void CollectDependences::CollectFromFile(std::istream& input_file) {
    Log.push("Constructing DependenceTable");
    while (input_file.good()) {
        // Each line starts with the name of a component in the software.
        std::string dest = local_read_string(input_file);
        if (dest == "")  break;
        if (dest == "}" || dest == ";") continue;
        Log.push("Destination name: " + dest);
        std::string next_string, source, dep_type, attr;
        // Read and process all the components that `dest' depends on immediately.
        while ((next_string = local_read_string(input_file)) != "") {
             if (next_string == ";") {
               if (source != "") {
                  save_dependence(DependenceEntry(dest, source, dep_type, attr));
                  Log.push( "Saving " + source + "->" + dest + "[" + dep_type + "]");
               }
               Log.push("Done reading line\n");
               break;
            } else if (next_string == "->") {
               source = dest;
               Log.push("Setting source = " + source);
               dest = local_read_string(input_file); 
               Log.push("Setting dest = " + dest);
            } else if (next_string == ":") {
               source = local_read_string(input_file); 
               if (source == "[") {
                  while ((next_string = local_read_string(input_file)) != "]") {
                    dep_type += next_string;
                    if (next_string == "") {
                       Log.fatal("Expecting \"]\" but get " + next_string);
                    }
                  }
                  source = local_read_string(input_file); 
               }
               if (source == ";") {
                  Log.push("Warning: Skipping empty dependence for " + dest +"!");
                  next_string = ";"; source = "";
                  break; 
               } 
               Log.push("Successfully setting source = " + source);
            } else if (next_string == "=") {
               attr = "";
               while ((next_string = local_read_string(input_file)) != "") {
                  if (next_string == ";" || next_string == "\n") 
                      break;
                  attr += next_string;
               }
               if (next_string == ";") {
                  input_file.putback(';');
               }
               Log.push("Setting attr:  " + attr);
            } else if (next_string == "{") {
               Log.push("Skipping graph configuration: " + dest + " " + next_string);
               break;
            } else {
               Log.fatal("Unexpected token " + next_string);
            }
        }
        if (next_string != ";" && next_string != "}" && next_string != "{") {
            Log.fatal("Expecting `;' or `}' but getting " + next_string);
        } else if (input_file.peek() == EOF) {
            break;
        }
    }
    Log.push("Done Constructing DependenceTable");
}

void CollectTransitiveDependences:: save_dependence(const DependenceEntry& e) {
  // Here we revert the dependence direction for downstream/backward dependences
   if (dependence_map_.find(current_start(e)) == dependence_map_.end()) {
         saved_sources_.push_back(current_start(e));
   }
   if (already_saved_.find(e) == already_saved_.end()) { 
      already_saved_.insert(e);
      dependence_map_[current_start(e)].push_back(e);
      if (e.attr_entry() != "") {
         dependence_map_[e.attr_entry()].push_back(e);
      }
      if (e.type_entry() != "") {
        dependence_map_[e.type_entry()].push_back(e);
      }
   }
}

void CollectTransitiveDependences:: Compute(
                const std::string& input, 
                std::set<std::string>& result, 
                const std::function<bool(const DependenceEntry&)>* what_to_do) {
    DebugLog Log("-debug-dep-table");
    if (result.find(input) != result.end()) {
      // Transitively collect more results only if it hasn't yet been done.
      Log.push("Skip collecting transitive dependence for " + input);
      return;
    }
    Log.push("Collect transitive dependence for " + input);
    result.insert(input);
    const auto& dependences = dependence_map_[input];
    // Terminates if dependences are empty, with the loop below skipped.
    for (auto dependence : dependences) {
        if (what_to_do == 0 || (*what_to_do)(dependence)) {
           save_dependence(dependence);
           if (current_start(dependence) == input) {
              Compute(next_start(dependence), result, what_to_do);
           }
        }
    }
}

void CollectTransitiveDependences :: Compute(const std::vector<std::string>& input,
                std::set<std::string>* result, 
                const std::function<bool(const DependenceEntry&)>* what_to_do) {
    DebugLog Log("-debug-dep-table");
    Log.push("Output results of transitive dependence analysis");
    auto from = input;
    for (std::string e : from) {
          std::set<std::string> destinations;
          Compute(e, destinations, what_to_do);
          if (result != 0) {
            for (auto d : destinations) {
              result->insert(d);
            }
          }
    } 
}

void DependenceTable :: OutputDependences(std::ostream& output) {
    Log.push("Output results of dependence analysis");
    for (auto op : saved_dependences_sig_) {
      for (auto e : saved_dependences_relation_[op]) {
         output << e << std::endl;
      }
    }
}

void DependenceTable :: OutputDataDependences(std::ostream& output) {
    Log.push("Output data dependences only.");
    for (auto op : saved_dependences_sig_) {
      for (auto e : saved_dependences_relation_[op]) {
        e.output_data_dependence(output);
      }
    }
}

std::ostream& operator << (std::ostream& output, const DependenceEntry& e) { 
        output << e.first_entry() << " : " ;
        if (e.type_entry() != "") {
            output << "[ " << e.type_entry() << " ] ";
        }
        output << e.second_entry();
        if (e.attr_entry() != "") {
             output << " = " << e.attr_entry()  << " ;";
        } else {
             output << " ;";
        }
        return output;
}

/************************************/
/* Class for supporting clustering of dependences in GUI */
/************************************/
class ClusterDependences {
    // map each graph node to the name of its cluster.
    std::map<std::string, std::string> cluster_map; 
    // Maps the cluster name to all member nodes that belong to the  cluster.
    std::map<std::string,std::set<std::string> > clusters;
    // Maps each node to all the outgoing and incoming edges.
    std::map<std::string, std::set<DependenceEntry>> out_edge_map, in_edge_map;
    // The same of all nodes that are functions.
    std::set<std::string> functions;
    // the integer keeping track of new clusters created.
    int cluster_index_ = 0;

    bool setupNamespace(const std::string& s) {
       auto namespace_pos = s.rfind("::");
       if (namespace_pos > 0 && namespace_pos < s.size()) {
           std::string cluster_name = s.substr(0, namespace_pos); 
           clusters[cluster_name].insert(s);
           return true;
       }
       return false;
    }
  public:
    // Call for each node of the graph to set up clustering.
    void setupNode (const std::string& s) {
       assert(s != "");
       // If s has been clustered previously, return.
       if (cluster_map.find(s) != cluster_map.end()) return;
       // set up preliminary clustering.
       if (setupNamespace(s)) return;

       // Try to cluster node s with itself (nobody else). 
       auto cluster_string = std::to_string(cluster_index_);
       cluster_index_ ++;
       clusters[cluster_string].insert(s);
       cluster_map[s] = cluster_string;
    };

    // Call for each edge of the graph to set up clustering.
    void setupEdge (const DependenceEntry& e) {
       assert(e.first_entry() != "");
       assert(e.second_entry() != "");
       // first entry is always a function.
       functions.insert(e.first_entry());
       if (e.type_entry() == "call") {
          // second entry is a function only if this is a call relation..
          functions.insert(e.second_entry());
       }
       // Group the edges incidenting to the node.
       out_edge_map[e.first_entry()].insert(e);
       in_edge_map[e.second_entry()].insert(e);
    }
    
    // Call to cluster the registered nodes and edges. 
    // Group nodes into single clusters if connected more than #threshold edges.
    void setupClusters (int threshold = 1) { 
       std::list<std::string> work; 
       for (auto m : cluster_map) {
          auto node = m.first;
          assert(m.second != "");
          assert(m.first != "");
          // do nothing if it's already includes other nodes.
          if (clusters[m.second].size() > 1)  continue;
          work.push_back(node);
       }
       for (std::string node : work) {
          std::map <std::string, int> connectivity;
          // Fuse into the edge sink if this is 
          for (auto e : in_edge_map[node]) {
            auto c = cluster_map[e.first_entry()];
            if (c == "") continue; // TODO: Not supposed to happen.
            if (connectivity.find(c) == connectivity.end()) 
               connectivity[c] == 0;
            else 
               connectivity[c] ++;
          }  
          for (auto e : out_edge_map[node]) {
            auto c = cluster_map[e.second_entry()];
            if (c == "") continue; // TODO: not supposed to happen.
            if (connectivity.find(c) == connectivity.end()) 
               connectivity[c] == 0;
            else 
               connectivity[c] ++;
          }  
          std::string cluster = "";
          // If node has only one connection, cluster it.
          if (connectivity.size() == 1) {
            cluster = (*connectivity.begin()).first;
            cluster_map[node] = cluster; 
            clusters[cluster].insert(node);
          } else {
            for (auto con : connectivity) {
              if (con.second >= threshold) {
                 if (cluster == "") {
                    cluster = con.first;
                    assert(cluster != "");
                    cluster_map[node] = cluster; 
                    clusters[cluster].insert(node);
                 } else {
                    for (auto n : clusters[con.first]) {
                       cluster_map[n] = cluster;
                       clusters[cluster].insert(n);
                    }
           } } } } // else
      } // while
    };
 public:
    std::string edge_to_string (const std::string& s, std::ostream& output) {
       std::string color = "black";
       if (s == "modify") color = "red";
       else if (s == "read") color = "green";
       if (s != ""){
          return "[ label=\"" + s + "\", color=" + color + "]";
       }
       return "[ color=" + color + "]";
    };

    // Output clustering of nodes in different namespaces.
    void outputCluster(const std::string& cluster_name, std::ostream& output) {
      bool do_cluster = clusters[cluster_name].size() > 1;
      if (do_cluster) {
        output << "subgraph \"cluster_" << cluster_name << "\" {\n";
        output << "  style=filled;\n";
        output << "  color=lightgrey;\n";
        output << "  label=\"" << cluster_name << "\";\n";
      }
      for (auto cluster_member : clusters[cluster_name]) {
        if (functions.find(cluster_member) != functions.end()) {
          output << "\"" << wrap_string(cluster_member) << "\" [shape=box] ; \n";
        } else {
          output << "\"" << wrap_string(cluster_member) << "\" [shape=diamond] ; \n";
        }
      }
      if (do_cluster) {
         output << "}\n";
      }
    };
    // This must be done after all the dependences have been output, which sets up the clusters.
    void output(std::ostream& output) {
     output << "digraph {\n";
       for (auto cluster_entry : clusters) {
         outputCluster(cluster_entry.first, output);
       };
       for (auto m : out_edge_map) {
         for (auto& e : m.second) {
            output << "\"" << wrap_string(e.first_entry()) << "\" -> \"" << wrap_string(e.second_entry()) << "\"" <<  edge_to_string(e.type_entry(), output) << " ;\n";
         }
       }
     output << "}";
    };
};
/************************************/

void DependenceTable :: OutputDependencesInGUI(std::ostream& output) {
    Log.push("Output dependence analysis GUI");

    // Save all the nodes into different clusters based on their namespaces.
    ClusterDependences clusters;
    for (auto op : saved_dependences_sig_) {
      for (auto e : saved_dependences_relation_[op]) {
       clusters.setupNode(e.first_entry()); 
       clusters.setupNode(e.second_entry()); 
       clusters.setupEdge(e); 
      }
    }
    clusters.setupClusters();
    clusters.output(output);
}

void DependenceTable:: save_dependence(const DependenceEntry& e) {
  // Save inside the dependence table (base class).
  DebugLog DebugSaveDep("-debugdep");
  DebugSaveDep([&e](){ return "processing " + e.to_string(); });

  if (update_annotations_) {
    // Save into annotation  if necessary.
    if (e.type_entry() == "parameter") {
      OperatorSideEffectAnnotation* funcAnnot = OperatorSideEffectAnnotation::get_inst();
      OperatorSideEffectDescriptor* desc1 = funcAnnot->get_modify_descriptor(e.first_entry(), true);
      assert(desc1 != 0);
      desc1->get_param_decl().add_param( /*param type*/ e.attr_entry(),  /* param name*/ e.second_entry());
      OperatorSideEffectDescriptor* desc2 = funcAnnot->get_read_descriptor(e.first_entry(), true);
      assert(desc2 != 0);
      desc2->get_param_decl().add_param( /*param type*/ e.attr_entry(),  /* param name*/ e.second_entry());
      DebugSaveDep([&e](){ return "Saving parameter " + e.second_entry(); });
    }
    else if (e.type_entry() == "modify") {
      OperatorSideEffectAnnotation* funcAnnot = OperatorSideEffectAnnotation::get_inst();
      OperatorSideEffectDescriptor* desc = funcAnnot->get_modify_descriptor(e.first_entry(), true);
      assert(desc != 0);
      DebugSaveDep([&e](){ return "processing " + e.second_entry(); });
      SymbolicVal var = SymbolicValGenerator::GetSymbolicVal(e.second_entry());
      desc->push_back(var);
      DebugSaveDep([&var](){ return "Saving modify " + var.toString(); });
    } else if (e.type_entry() == "read") {
      OperatorSideEffectAnnotation* funcAnnot = OperatorSideEffectAnnotation::get_inst();
      OperatorSideEffectDescriptor* desc = funcAnnot->get_read_descriptor(e.first_entry(), true);
      assert(desc != 0);
      SymbolicVal var = SymbolicValGenerator::GetSymbolicVal(e.second_entry());
      desc->push_back(var);
      DebugSaveDep([&var](){ return "Saving read " + var.toString(); });
    }
  }
  DependenceTable::SaveDependence(DependenceEntry(e));
}

void DependenceTable::
ClearOperatorSideEffect(SgNode* op) {
  auto sig = GetVariableSignature(op);
  ClearDependence(sig);
}

bool DependenceTable::
     SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, AstUtilInterface::OperatorSideEffect relation, SgNode* details) {
  std::string attr;
  if (details != 0) {
       Log.push("Skipping Side effect details: " + AstInterface::AstToString(AstNodePtrImpl(details)));
       // QY: Do not save side effect details in annotation as we currently don't use it.
       //attr = AstUtilInterface::GetVariableSignature(details);
  }
  DependenceEntry e(AstUtilInterface::GetVariableSignature(op), AstUtilInterface::GetVariableSignature(varref), 
                                    AstUtilInterface::OperatorSideEffectName(relation), attr); 
  Log.push("saving dependence: " + e.to_string());
  SaveDependence(e);
  return true;
}

};
