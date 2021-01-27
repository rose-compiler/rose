//
// ROSE/SARIF wrapper.  This provides an abstraction layer between
// ROSE tools and the SARIF library that generates SARIF-formatted
// JSON files.  This is intended to achieve two goals:
//
//   1. Insulate ROSE tools from the detail of SARIF.  Many
//      seemingly simple things require layers of nested
//      SARIF objects where the nesting structure requires
//      some knowledge of the SARIF format.  Encapsulating
//      that detail in the wrapper reduces the exposure of
//      ROSE developers to the SARIF details.
//
//   2. Provide a stable ROSE-facing API if/when the SARIF
//      specification changes.  As the SARIF specification
//      evolves the nesting structure of SARIF objects may
//      change and new SARIF objects may be introduced.
//      In many simple use cases, this change will not affect
//      how ROSE tools report their results - just the
//      manner by which the results are represented in the
//      SARIF file.  Hiding those details in this wrapper
//      should minimize ROSE-tool breakage when the SARIF
//      format changes as most necessary changes will be
//      concentrated in this file.
//
// This API is intended to be relatively general, so there
// will likely exist special cases in which a tool has objects
// representing analysis results to serialize that are not
// included in this API.  The recommendation is to either add
// appropriate methods to the SARIFOutput class here, or use
// methods here as a model for writing specialized serializers
// to include in specific analyzers.
//
// SARIF schema -> C++ generated via https://github.com/quicktype/quicktype
//
#ifndef __ROSESARIF_HPP__
#define __ROSESARIF_HPP__

#include "sarif.hpp"
#include "rose.h"

using json = nlohmann::json;

/**
 * This class manages SARIF output information.  A SARIF file contains
 * a description of the tool being run, the rules that correspond to
 * the analyses, and the results of rule application.  A result associates
 * a rule with a physical location within a file.
 *
 * This wrapper class does not attempt to provide a full mapping of the
 * SARIF schema - instead, the goal is to hide much of the detail in setting
 * up various objects within a SARIF file to insulate ROSE tool developers
 * from the complexity.
 */
class SARIFOutput {
private:
  SARIF::Run run;

  // this pass to remove null-valued keys is due to the
  // way the automatically generated SARIF C++ objects
  // implement to_json.  instead of skipping keys that
  // have no value, it emits them with null values.
  // there is no clean way according to the author of the
  // JSON library to remove them other than traversing
  // the JSON structure and removing them by hand...
  void strip_nulls(nlohmann::json &j) {
    std::vector<std::string> removals;

    // accumulate up things to delete after iteration
    for (auto iter = j.begin();
         iter != j.end();
         ++iter) {
      if (iter.value().is_null()) {
        removals.push_back(iter.key());
      } else {
        if (iter.value().is_structured()) {
          strip_nulls(iter.value());
        }
      }
    }
    
    for (std::string s: removals) {
      j.erase(s);
    }
  }

  //  typedef std::pair<std::string, std::string> mapkey_t;
  //  std::map<mapkey_t, std::shared_ptr<SARIF::Result>> results;
  std::vector<std::shared_ptr<SARIF::Result>> results;
  std::map<std::string, SARIF::ReportingDescriptor> rules;
  std::map<std::string, int> rule_indices;
  SARIF::Tool *t;
  SARIF::ToolComponent *tc;
  int cur_index;
  
public:
  // create a SARIF output object for a given tool component name.
  // this creates a fresh SARIF file that represents results
  // from a single tool component.  If we have a tool with more than
  // one component we will need to generalize this interface (or split
  // out a secondary object) that allows one tool to have multiple
  // components.
  SARIFOutput(const std::string &toolname) {
    tc = new SARIF::ToolComponent();
    tc->set_name(toolname);
    t = new SARIF::Tool();
    
    std::vector<SARIF::Invocation> invocations;
    run.set_invocations(std::make_shared<std::vector<SARIF::Invocation>>(invocations));

    cur_index = 0;
  }

  // register a tool invocation with a command line specified as a single string
  void register_invocation(const std::string &cmdline) {
    SARIF::Invocation invoke;

    invoke.set_command_line(std::make_shared<std::string>(cmdline));
    auto invocations = this->run.get_invocations();
    invocations->push_back(invoke);
  }

  // register a tool invocation with a vector of arguments
  void register_invocation(const std::vector<std::string> &args) {
    SARIF::Invocation invoke;

    invoke.set_arguments(std::make_shared<std::vector<std::string>>(args));
    auto invocations = this->run.get_invocations();
    invocations->push_back(invoke);
  }

  // register a tool invocation with argc and argv
  void register_invocation(int argc, char **argv) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
      args.push_back(std::string(argv[i]));
    register_invocation(args);
  }
  
  // this registers a named rule with the object with a rule description string.
  // the rule name is later used to register results generated by the tool component.
  void register_rule(const std::string &rule, const std::string &desc) {
    if (rules.find(rule) == rules.end()) {
      SARIF::ReportingDescriptor r;
      r.set_id(rule);
      auto msg = std::make_shared<SARIF::MultiformatMessageString>(SARIF::MultiformatMessageString());
      msg->set_text(desc);
      r.set_short_description(msg);
      rules[rule] = r;
      rule_indices[rule] = cur_index;
      cur_index += 1;
    }
  }

  SARIF::Location define_source_region(SgNode *n,
                                       std::shared_ptr<SARIF::ArtifactLocation> artifact)
  {
    // extract the file info object from the SgNode to determine the extent of the
    // program that the node represents.
    Sg_File_Info *fi_start = n->get_startOfConstruct();
    Sg_File_Info *fi_end = n->get_endOfConstruct();

    auto r = std::make_shared<SARIF::Region>(SARIF::Region());
    r->set_start_line(std::make_shared<long int>(fi_start->get_line()));
    r->set_start_column(std::make_shared<long int>(fi_start->get_col()));
    r->set_end_line(std::make_shared<long int>(fi_end->get_line()));
    r->set_end_column(std::make_shared<long int>(fi_end->get_col()));

    auto pl = std::make_shared<SARIF::PhysicalLocation>(SARIF::PhysicalLocation());
    pl->set_region(r);
    pl->set_artifact_location(artifact);
    
    SARIF::Location loc;
    loc.set_physical_location(pl);

    return loc;
  }

  // note: see SARIF spec 3.32.12 for expected valid values for address kind.  while any
  // value is legal, a set of strings are specified that producers and consumers of
  // SARIF can agree upon
  SARIF::Location define_binary_location(const rose_addr_t &a,
                                         std::shared_ptr<SARIF::ArtifactLocation> artifact,
                                         int64_t length = 0,
                                         std::string addrkind = "",
                                         std::string name = "")
  {
    SARIF::Location loc;

    auto pl = std::make_shared<SARIF::PhysicalLocation>(SARIF::PhysicalLocation());
    loc.set_physical_location(pl);

    auto addr = std::make_shared<SARIF::Address>(SARIF::Address());
    addr->set_absolute_address(std::make_shared<int64_t>(a));
    if (length != 0) {
      addr->set_length(std::make_shared<int64_t>(length));
    }
    if (addrkind != "") {
      addr->set_kind(std::make_shared<std::string>(addrkind));
    }
    if (name != "") {
      addr->set_name(std::make_shared<std::string>(name));
    }
    pl->set_address(addr);
    pl->set_artifact_location(artifact);

    return loc;
  }

  // define a located artifact given some filename
  std::shared_ptr<SARIF::ArtifactLocation> create_artifact_location(std::shared_ptr<std::string> filename) {
    std::shared_ptr<SARIF::ArtifactLocation> artifact =
      std::make_shared<SARIF::ArtifactLocation>(SARIF::ArtifactLocation());
    artifact->set_uri(filename);
    return artifact;
  }
  
  // register a result for a given named rule.  the rule must have
  // previously been registered with the register_rule method.
  //
  // the location passed in represents either a source or binary
  // location created with the functions above.
  //
  // the artifact location represents the target of the analysis and
  // is created with create_artifact_location.
  //
  // The result kind and level are optional : by default a result is
  // given Kind=PASS and Level=WARNING.  The caller registering the
  // result should pick those appropriately.
  void result(const std::string &rule,
              const std::string &mesg,
              const SARIF::Location &loc,
              const std::shared_ptr<SARIF::ArtifactLocation> &artifact,
              const SARIF::ResultKind &kind = SARIF::ResultKind::PASS,
              const SARIF::Level &lvl = SARIF::Level::WARNING)
  {
    // create the result object with a pointer to the given artifact location,
    // and add it to the result collection.
    std::shared_ptr<SARIF::Result> res;
    res = std::make_shared<SARIF::Result>(SARIF::Result());
    res->set_locations(std::make_shared<std::vector<SARIF::Location>>(std::vector<SARIF::Location>()));
    res->set_analysis_target(artifact);
    results.push_back(res);

    // wrap the message in a Message object
    auto msg = SARIF::Message();
    msg.set_text(std::make_shared<std::string>(mesg));
    res->set_message(msg);

    // update the kind and level of the result
    res->set_kind(std::make_shared<SARIF::ResultKind>(kind));
    res->set_level(std::make_shared<SARIF::Level>(lvl));

    // create the mapping between the result and the rule that
    // it corresponds to
    auto rdr = std::make_shared<SARIF::ReportingDescriptorReference>(SARIF::ReportingDescriptorReference());
    rdr->set_id(std::make_shared<std::string>(rule));
    rdr->set_index(std::make_shared<long int>(rule_indices[rule]));
    res->set_rule(rdr);
    res->set_rule_index(std::make_shared<long int>(rule_indices[rule]));
    res->set_rule_id(std::make_shared<std::string>(rule));
    
    // add the location of the result to the result location vector
    auto locs = res->get_locations();
    locs->push_back(loc);
  }

  // emit the SARIF to the given ofstream
  void emit(std::ofstream &ostr) {
    json j;

    std::vector<SARIF::ReportingDescriptor> rvec;
    for (auto it = rules.begin(); it != rules.end(); ++it) {
      rvec.push_back(it->second);
    }
    tc->set_rules(std::make_shared<std::vector<SARIF::ReportingDescriptor>>(rvec));
    t->set_driver(*tc);
    run.set_tool(*t);

    // SARIF version and schema.
    j["version"] = SARIF::VERSION;
    j["$schema"] = SARIF::SCHEMA;
    std::vector<json> runs;
    
    std::vector<SARIF::Result> res;
    for (auto it = results.begin(); it != results.end(); ++it) {
      res.push_back(*(*it));
    }
    this->run.set_results(std::make_shared<std::vector<SARIF::Result>>(res));

    json jrun;
    to_json(jrun, this->run);
    strip_nulls(jrun);

    runs.push_back(jrun);
    j["runs"] = runs;
    
    // Note: setw() necessary to get the pretty printer to emit well formatted
    // JSON.  Without it a giant flat string is produced that is syntactically
    // correct but not exactly human readable.
    ostr << std::setw(4) << j << std::endl;
  }

  // dump the JSON corresponding to the SARIF object to the named file.
  void emit(const std::string &fname) {
    std::ofstream o(fname);
    emit(o);
  }

};

#endif // __ROSESARIF_HPP__
