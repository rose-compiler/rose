#include "dependence_table.h"
#include "OperatorDescriptors.h"
#include "OperatorAnnotation.h"
#include "AstUtilInterface.h"
#include "AstInterface_ROSE.h"

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
               Log.push("Setting source = " + source);
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


void DependenceTable :: OutputDependencesInGUI(std::ostream& output) {
    Log.push("Output dependence analysis GUI");

    auto wrap_string = [](const std::string& s) {
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
    // Save all the nodes into different clusters based on their namespaces.
    std::map <std::string, std::set<std::string> > clusters;
    std::map<std::string, std::string> cluster_map;
    std::map<std::string, std::set<DependenceEntry>> edge_map;
    auto setup_namespace = [&clusters, this, &edge_map, &cluster_map](const std::string& s, const DependenceEntry& e) {
       auto namespace_pos = s.find("::");
       std::string cluster_name;
       if (namespace_pos > 0 && namespace_pos < s.size()) {
           cluster_name = s.substr(0, namespace_pos); 
       } else {
          if (node_map_[s].out_no > 1 && node_map_[s].in_no > 1) {
             // save all the edges incidenting the node.
               edge_map[s].insert(e);
           } 
           // Here we try to cluster nodes based on call relations.
           else if (s == e.first_entry() && node_map_[s].out_no == 1) {
                cluster_map[s] = e.second_entry();
                if (cluster_map.find(e.second_entry()) == cluster_map.end()) {
                   cluster_map[e.second_entry()] = e.second_entry();
                } 
           } else if (s == e.second_entry() && node_map_[s].in_no == 1 && node_map_[s].out_no != 1)  {
                cluster_map[s] = e.first_entry();
                if (cluster_map.find(e.first_entry()) == cluster_map.end()) {
                   cluster_map[e.first_entry()] = e.first_entry();
                } 
           } 
       }
       
       if (!cluster_name.empty()) {
         clusters[cluster_name].insert(s);
       }
    };
    std::set<std::string> functions;
    auto setup_node = [&functions, &output, &wrap_string, setup_namespace](const std::string& s, const DependenceEntry& e) {
       if (e.type_entry() == "call" || s == e.first_entry()) {
          functions.insert(s);
       }
       setup_namespace(s, e);
    };
    auto edge_to_string = [&output](const std::string& s) {
       std::string color = "black";
       if (s == "modify") color = "red";
       else if (s == "read") color = "green";
       if (s != ""){
          return "[ label=\"" + s + "\", color=" + color + "]";
       }
       return "[ color=" + color + "]";
    };
    // Output clustering of nodes in different namespaces.
    std::function<void(const std::string&)> output_cluster = [&wrap_string, &functions, &clusters,&output_cluster, &output, this]
           (const std::string& cluster_name) {
      Log.push("output cluster:" + cluster_name);
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

    output << "digraph {\n";
    for (auto op : saved_dependences_sig_) {
      for (auto e : saved_dependences_relation_[op]) {
       setup_node(e.first_entry(), e); 
       setup_node(e.second_entry(), e); 
       output << "\"" << wrap_string(e.first_entry()) << "\" -> \"" << wrap_string(e.second_entry()) << "\"" <<  edge_to_string(e.type_entry()) << " ;\n";
      }
    }
    // Make new clusters based on call relations recorded in cluster_map.
    if (!cluster_map.empty()) {
       auto is_cluster = [](const std::string& s) {
              return s != "" && isdigit(s[0]);
       };
       int cluster_index = 0;
       for (auto& e : node_map_) {
           auto node = e.first;
           auto cluster_string = std::to_string(cluster_index);
           if (cluster_map.find(node) == cluster_map.end()) {
              clusters[cluster_string].insert(node);
              cluster_index++;
           } else {
              std::vector <std::string> mapped_nodes;
              while (node != "" && !is_cluster(node)) {
                auto next_node = cluster_map[node];
                cluster_map[node] = cluster_string;
                mapped_nodes.push_back(node);
                if (next_node == node) 
                    break;
                node = next_node;
              } 
              if (node != "" && is_cluster(node)) {
                 cluster_string = node;
              } else { 
                 cluster_index ++;
              }
              for (auto n : mapped_nodes) {
                  cluster_map[n] = cluster_string;
                  clusters[cluster_string].insert(n);
              }
          }
       }
       bool change = true;
       while (change) {
         change = false;
         for (auto& p : edge_map) {
            auto node = p.first;
            if (is_cluster(cluster_map[node])) {
                continue;
            }
            std::string cluster_up, cluster_down;
            for (auto& e : p.second) {
               if (node == e.first_entry()) {
                 if (cluster_down == "") {
                   cluster_down = cluster_map[e.second_entry()];
                 } else if (cluster_down != cluster_map[e.second_entry()]) {
                    cluster_down = "bottom";
                 }
               } else if (node == e.second_entry()) {
                 if (cluster_up == "") {
                   cluster_up = cluster_map[e.first_entry()];
                 } else if (cluster_up != cluster_map[e.first_entry()]) {
                    cluster_up = "bottom";
                 }
               }
            }
            if (is_cluster(cluster_up)) {
              cluster_map[node] = cluster_up;
              clusters[cluster_up].insert(node);
              change = true;
            } else if (is_cluster(cluster_down)) {
              cluster_map[node] = cluster_down;
              clusters[cluster_down].insert(node);
              change = true;
            }
          }
       }
    }
    // This must be done after all the dependences have been output, which sets up the clusters.
    for (auto cluster_entry : clusters) {
      output_cluster(cluster_entry.first);
    };
    output << "}";
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
  std::string prefix, attr;
  bool save_annot = false;
  switch (relation) {
    case AstUtilInterface::OperatorSideEffect::Modify: save_annot=true; break;
    case AstUtilInterface::OperatorSideEffect::Read:   save_annot=true; break;
    case AstUtilInterface::OperatorSideEffect::Call:   break;
    case AstUtilInterface::OperatorSideEffect::Parameter: break;
    case AstUtilInterface::OperatorSideEffect::Return:   break;
    case AstUtilInterface::OperatorSideEffect::Kill:  return false; 
    case AstUtilInterface::OperatorSideEffect::Decl:  break;
    case AstUtilInterface::OperatorSideEffect::Allocate:  break;
    case AstUtilInterface::OperatorSideEffect::Free:  break;
    default:
     std::cerr << "Unexpected case:" << relation << "\n";
     assert(0);
  }


  if (save_annot) {
     AstUtilInterface::AddOperatorSideEffectAnnotation(op, varref, relation);
  }
  if (details != 0) {
       attr = AstUtilInterface::GetVariableSignature(details);
  }
  DependenceEntry e(AstUtilInterface::GetVariableSignature(op), AstUtilInterface::GetVariableSignature(varref), 
                                    AstUtilInterface::OperatorSideEffectName(relation), attr); 
  Log.push("saving dependence: " + e.to_string());
  SaveDependence(e);
  return true;
}

};
