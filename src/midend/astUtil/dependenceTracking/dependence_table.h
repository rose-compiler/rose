#ifndef SELECTIVE_TESTING_DEPENDENCE_TABLE
#define SELECTIVE_TESTING_DEPENDENCE_TABLE

#include <fstream>
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <set>
#include "CommandOptions.h"
#include "AstUtilInterface.h"
#include "Rose/FlowGraphInterface.h"

class SgNode;
class AstNodePtr;

namespace AstUtilInterface {

// Representing dependence information between two entities.
class DependenceEntry {
     private:
      std::string first_, second_, deptype_, attr_;
     public:
      DependenceEntry(const std::string& first, const std::string& second, const std::string& deptype, const std::string& attr)
             : first_(first), second_(second), deptype_(deptype), attr_(attr) {}
      std::string first_entry() const { return first_; }
      std::string second_entry() const { return second_; }
      std::string type_entry() const { return deptype_; }
      std::string attr_entry() const { return attr_; }
      bool operator == (const DependenceEntry& e2) const { 
           return first_ == e2.first_ && second_ == e2.second_ && deptype_ == e2.deptype_; 
       }
      bool operator < (const DependenceEntry& e2) const 
        { return first_ < e2.first_ || (first_ == e2.first_ && second_ < e2.second_) ||
                 (first_ == e2.first_ && second_ == e2.second_ && deptype_ < e2.deptype_); }
      std::string to_string() const { return first_ + " : " + " [ " + deptype_ + " ] " + second_ + " = " + attr_; }
      bool first_is_function() const { return true; }
      bool second_is_function() const { return type_entry() == "call"; }
      bool output_data_dependence(std::ostream& output) const {
          if (attr_entry() != "") {
             output << second_entry() << " : [ " << type_entry() << " ] " << attr_entry() << " ;\n";
             return true;
          }
          return false;
      }

};

std::ostream& operator << (std::ostream& output, const DependenceEntry& e);

//! Stores dependence entries in a table.
class DependenceTable : public SaveOperatorSideEffectInterface {
  public:
    enum Direction {CollectForward, CollectBackward};

    DependenceTable(Direction direction = CollectForward) : direction_(direction) {}


    void CollectDependences(const std::function<void(const DependenceEntry&)>& op) {
      for (auto n : nodes_) {
        const auto& p = node_map_.find(n);
        assert(p != node_map_.end());
        for (const auto& e : (*p).second.edges) {
           op(e);
        }
      }
    }

    // Output the new dependences into a new file. 
    void OutputDependences(std::ostream& output) {
         CollectDependences([&output](const DependenceEntry& e) { output << e << std::endl; });
    }
    void OutputDataDependences(std::ostream& output) {
        CollectDependences([&output](const DependenceEntry& e) { e.output_data_dependence(output); });
    }
    void OutputDependencesInGUI(std::ostream& output); 
    void ClearDependence(const std::string& sig) {
        auto p = node_map_.find(sig);
        if (p != node_map_.end()) {
          auto &p1 = (*p).second;
          p1.edges.clear();
        }
    }
    bool InsertNode(const std::string sig) {
       if (node_map_.find(sig) != node_map_.end()) 
         return false;
       nodes_.push_back(sig);
       node_map_.insert({sig, NodeInfo(nodes_.size()-1)});
       return true;
    }
    // Save a dependence entry.
    virtual bool SaveDependence(const DependenceEntry& e) {
       InsertNode(e.first_entry());
       InsertNode(e.second_entry());
       const auto& p1 = node_map_.find(current_start(e));
       assert(p1 != node_map_.end());
       auto& p2 = (*p1).second;
       for (const auto& e1 : p2.edges) {
          if (e1 == e) { // Duplicate entry.
           return false;
          }
       }
       p2.edges.push_back(e);
       return true;
    }

    //! Erase existing annotation for the given operator.
    void ClearOperatorSideEffect(SgNode* op) override; 
    //
    //! The operator op accesses the given memory reference in nature of the given relation.
    bool SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, const AstUtilInterface::OperatorSideEffect& relation) override; 

  protected:
    const std::vector<DependenceEntry>& get_dependences(const std::string& sig) {
       auto p = node_map_.find(sig);
       assert(p != node_map_.end());
       return (*p).second.edges; 
    } 

  protected:
    // Saves information about each node.
   struct NodeInfo {
       // Length of the longest chain.
       int node_index = -1;
       std::vector<DependenceEntry> edges;
       NodeInfo(int _index) : node_index(_index) { assert(node_index >= 0); }
   };
   std::string next_start(const DependenceEntry& e) const {
     return (direction_ == Direction::CollectBackward)? e.first_entry() : e.second_entry();
   }
   std::string current_start(const DependenceEntry& e) const {
     return (direction_ == Direction::CollectBackward)? e.second_entry() : e.first_entry();
   }
   std::string current_attr(const DependenceEntry& e) const {
     return e.attr_entry();
   }
   std::string current_type(const DependenceEntry& e) const {
     return e.type_entry();
   }
 private:
   std::map<std::string, NodeInfo> node_map_;
   std::vector<std::string> nodes_;
   Direction direction_;
    
};

// Read and collect all the AST components that are tagged by a dependence name
class SelectDependences : public DependenceTable{
  private:
    std::set<std::string> tag_first_, tag_second_, tag_type_;
  protected:
    bool SaveDependence(const DependenceEntry& e) override {
       if ((tag_first_ .empty() || tag_first_.find(e.first_entry()) != tag_first_.end())  && (tag_second_ .empty() || tag_second_.find(e.second_entry()) != tag_second_.end())
   && (tag_type_.empty() || tag_type_.find(e.type_entry())!=tag_type_.end())) {
          DependenceTable::SaveDependence(e);
          return true;
       }
       return false;
    }
  public:
    void add_selection(const std::string& _tag_first, const std::string& _tag_second = "", const std::string& _tag_type = "") { 
      if (!_tag_first.empty()) {
        tag_first_.insert(_tag_first); 
      } 
      if (!_tag_second.empty()) {
        tag_second_.insert(_tag_second);
      }
      if (!_tag_type.empty()) {
         tag_type_.insert(_tag_type);
      }
    }
};

// Store the forward dependences of all software components internally as a map.
class CollectTransitiveDependences : public DependenceTable{
  public:
    CollectTransitiveDependences(Direction direction) : DependenceTable(direction) {}
    // Collect all the components that depends on input downstream, by transitively
    // traversing the dependence table. The result should be initialized to empty
    // before invocation. The what_to_do parameter, if given, can skip computing some
    // entries by returning false.
    void Compute(const std::vector<std::string>& input, 
                    const std::function<bool(const DependenceEntry&)>* what_to_do=0,
                    std::set<std::string>* result = 0); 
    void Compute(const std::string& input, std::set<std::string>& result, 
                    const std::function<bool(const DependenceEntry&)>* what_to_do=0);

    // Collect transitive dependences from input, invoke what to do for each entry.
};



/* Here implement the portable ROSE analysis interface */
template <class Node, class Edge>
class FlowGraphViaDependenceTable : 
    public Rose::FlowGraphInterface::FlowGraphCreateInterface<Node,Edge, std::string, DependenceEntry>, 
    public DependenceTable {
 public:
   FlowGraphViaDependenceTable() {}
   virtual std::string addNode(const Node& node) {
       auto sig = AstUtilInterface::GetVariableSignature(node);
       DependenceTable::InsertNode(sig);
       return sig;
    }
   virtual DependenceEntry addEdge(const std::string& p1, const std::string& p2, const Edge& t) {
      const auto& rel_sig = t.relation_name();
      const auto& attr = t.attr_name();
      DependenceEntry e(p1, p2, rel_sig, attr);
      DependenceTable::SaveDependence(e);
      return e;
    }
};

}; /* name space SelectiveTesting*/

#endif
