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
      bool output_data_dependence(std::ostream& output) {
          if (attr_entry() != "") {
             output << second_entry() << " : [ " << type_entry() << " ] " << attr_entry() << " ;\n";
             return true;
          }
          return false;
      }

};

std::ostream& operator << (std::ostream& output, const DependenceEntry& e);

// Read a table that records the dependences among all the components of a software.
// The syntax for specifying a dependence is x1 : y1 y2 ... ym; which
// specifies that x1 is dependent on y1, y2, ..., ym, where x1, y1, ..., ym are
// strings that uniquely identifes each component of the whole program.
class CollectDependences{
  protected:
    virtual void save_dependence(const DependenceEntry& e) = 0;
    std::string local_read_string(std::istream& input_file);
  public:
    DebugLog Log = DebugLog("-debug-dep-table");
    CollectDependences() {}
    // Read the system dependence table from an input file.
    void CollectFromFile(std::istream& input_file);
};

// Read and collect all the AST components that are tagged by a dependence name
class SelectDependences : public CollectDependences{
  private:
    std::vector<DependenceEntry> result;
    std::string tag_first_, tag_second_, tag_type_;
  protected:
    virtual void save_dependence(const DependenceEntry& e) {
       if ((tag_first_ == "" || tag_first_ == e.first_entry()) && (tag_second_ == "" || tag_second_ == e.second_entry()) && 
           (tag_type_ == "" || tag_type_ == e.type_entry())) {
          result.push_back(e);
       }
    }
  public:
    SelectDependences(const std::string& _tag_first, const std::string& _tag_second = "", const std::string& _tag_type = "") : tag_first_(_tag_first), tag_second_(_tag_second), tag_type_(_tag_type) {}
    // Read the system dependence table from an input file.
    using CollectDependences::CollectFromFile;
    const std::vector<DependenceEntry>& get_result() { return result; }
};

// Store the forward dependences of all software components internally as a map.
class CollectTransitiveDependences : public CollectDependences {
  protected:
    // Read the system dependence table from an input file.
    virtual void save_dependence(const DependenceEntry& e) override;
  public:
    enum Direction {CollectForward, CollectBackward};
    CollectTransitiveDependences(Direction direction) : direction_(direction) {}
    // Collect all the components that depends on input downstream, by transitively
    // traversing the dependence table. The result should be initialized to empty
    // before invocation. The what_to_do parameter, if given, can skip computing some
    // entries by returning false.
    void Compute(const std::vector<std::string>& input, std::set<std::string>* result = 0, 
                    const std::function<bool(const DependenceEntry&)>* what_to_do = 0);
    void Compute(const std::string& input, std::set<std::string>& result,
                    const std::function<bool(const DependenceEntry&)>* what_to_do = 0);

    void Output(std::ostream& output, std::set<std::string>* select = 0, 
                     const std::function<bool(const DependenceEntry&)>* what_to_do = 0) {
       for (const auto& from : saved_sources_) {
          if (select == 0  || select->find(from) != select->end()) {
          for (const auto& e2 : dependence_map_[from]) {
             if (what_to_do == 0 || (*what_to_do)(e2)) {
                output << e2 << "\n";
             }
           }
          }
       }
    }
    // Collect transitive dependences from input, invoke what to do for each entry.
 private:
   // Store downstream dependences from a component x to all the other components depending on x.
   std::map<std::string, std::vector<DependenceEntry>> dependence_map_;
   std::vector<std::string> saved_sources_;
   std::set<DependenceEntry> already_saved_;
   Direction direction_;

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
};

//! Stores dependence entries in a table.
class DependenceTable : public CollectDependences, public SaveOperatorSideEffectInterface {
  public:
    DependenceTable(bool update_annotations) 
      : update_annotations_(update_annotations) {}

    // Output the new dependences into a new file. 
    void OutputDependences(std::ostream& output);
    void OutputDependencesInGUI(std::ostream& output);
    void OutputDataDependences(std::ostream& output);
    using CollectDependences::CollectFromFile;
    void ClearDependence(const std::string& sig) {
        auto p = saved_dependences_relation_.find(sig);
        if (p != saved_dependences_relation_.end()) {
          saved_dependences_relation_[sig].clear();
        }
    }
    void SaveDependence(const DependenceEntry& e) {
       auto p = saved_dependences_relation_.find(e.first_entry());
       if (p == saved_dependences_relation_.end()) {
          saved_dependences_sig_.push_back(e.first_entry());
       }
       for (const auto& e1 : saved_dependences_relation_[e.first_entry()]) {
          if (e1 == e) { // Duplicate entry.
           return;
          }
       }
       saved_dependences_relation_[e.first_entry()].push_back(e);
       node_map_[e.first_entry()].out_no ++;
       node_map_[e.second_entry()].in_no ++;
       int lower_rank = 0;
       if (node_map_.find(e.second_entry()) == node_map_.end()) {
           node_map_[e.second_entry()].rank = 0; // no incoming dependence yet.
       } else {
            lower_rank = node_map_[e.second_entry()].rank;
      }
      int higher_rank = lower_rank + 1;
      if (node_map_.find(e.first_entry()) == node_map_.end()) {
         node_map_[e.first_entry()].rank = higher_rank; // one outgoing dependence so far .
      } else {
         int current_rank = node_map_[e.first_entry()].rank;
         if (current_rank < higher_rank) {
              node_map_[e.first_entry()].rank = higher_rank; // increase existing rank.
         }
      }
    }

    //! Erase existing annotation for the given operator.
    void ClearOperatorSideEffect(SgNode* op) override; 
    //
    //! The operator op accesses the given memory reference in nature of the given relation.
    bool SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, AstUtilInterface::OperatorSideEffect relation, SgNode* sig = 0) override; 


  private:
    // Whether the internal entries need to be reflected in annotations.
    bool update_annotations_;
    // The signatures of all dependence entities.
    std::vector<std::string> saved_dependences_sig_;
    std::map<std::string, std::vector<DependenceEntry> > saved_dependences_relation_;
    // Saves information about each node.
    struct NodeInfo {
       // Number of outgoing and incoming edges.
       int out_no = 0, in_no = 0;
       // Length of the longest chain.
       int rank = 0; 
       NodeInfo() : out_no(0), in_no(0), rank(0) {}
    };
    std::map<std::string, NodeInfo> node_map_;
 protected:
    virtual void save_dependence(const DependenceEntry& e) override; 
};

}; /* name space SelectiveTesting*/

#endif
