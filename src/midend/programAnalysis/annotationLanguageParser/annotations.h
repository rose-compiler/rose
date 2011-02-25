
#ifndef BDWY_ANNOTATIONS_H
#define BDWY_ANNOTATIONS_H

#include <stdio.h>

// ------------------------------------------------------------
// CLASS: Annotations
// ------------------------------------------------------------

typedef std::map< std::string, procedureAnn * > procedures_map;
typedef procedures_map::iterator procedures_map_p;
typedef procedures_map::const_iterator procedures_map_cp;

//typedef pair< reportAnn *, stmtNode * > error_pair;
//typedef map< error_pair, int > error_report_map;
//typedef error_report_map::iterator error_report_map_p;

typedef std::list <std::string> str_list;
typedef std::set <std::string> str_set;

class Adaptor_Statistics;

class Annotations
{
public:

  /** @brief Current annotation object
   *
   *  This is only used during parsing to allow easy access to the
   *  annotations that are in the process of being built. */
 
  static Annotations * Current;

  /** @brief Public debug flag: turns on all verbosity */

  static bool debug;

  /** @brief Show actions as they are applied */

  static bool Show_actions;

  /** @brief Show reports as they are evaluated */

  static bool Show_reports;

  /** @brief Debug properties */

  static bool Verbose_properties;

  /** @brief Debug pointers only */

  static bool Verbose_pointers;

  /** @brief Flow insensitive properties */

  static bool Flow_insensitive_properties;

  /** @brief Skip traces */

  static bool Skip_traces;

  /** @brief Adaptivity flag */

  static int Adaptivity;

  /** @brief Show adaptivity algorithm */

  static bool Show_adaptivity;

  /** @brief Current precision policy to use in adaptive mode. */
  // TB_unify
  typedef enum { UNIFY, FICI, FSCI, FICS, FSCS } Adapt_Precision;

  static Adapt_Precision Adapt_precision;

  /** @brief Is unification being used? */
  // TB_unify
  static bool Adapt_uses_uba;

  /** @brief Error-oriented adaptivity
   *
   * In this mode, the adaptive algorithm never makes ancestors of a
   * procedure with errors context sensitive. */

  static bool Error_oriented_adaptivity;

  /** @brief Turn on tested object recording
   *
   * For the adaptive algorithm. */

  static bool Record_tested_objects;

  /** @brief Quiet reports
   *
   * When true, don't actually print the reports, but do all the other
   * processing, including adaptivity and accuracy. */

  static bool Quiet_reports;

  /** @brief Prune context sensitive
   *
   * When true, post-process the list of context sensitive procedures
   * proposed by the adative algorithm. Verify that context sensitivity is
   * really warranted. */

  static bool Prune_context_sensitive;

  /** @brief Pruned procedures
   *
   * Keep a list of the pruned procedures. */

  static str_set Pruned_procedures;

  /** @brief Analysis time
   *
   * Use this value to keep an accumulated total for the adaptive modes. */

  static double Analysis_time;

  /** @brief Enabled properties */

  static str_list Enabled_properties;

  /** @brief Disabled properties */

  static str_list Disabled_properties;

  /** @brief Start of heap */

  static unsigned long int Start_brk;

  /** @brief Aggressive adaptive pruning
   *
   * When true, use use-def chains to prune adaptivity. */

  static bool Aggressive_pruning;

  /** @brief Error reports
   *
   * For reports that are designated as "errors" keep a record of the
   * places they occur. */

  //  static error_report_map Error_reports;

  /** @brief Number of enabled properties
   *
   * */

  static int Num_enabled_properties;

  /** @brief Error diagnostic */
  static bool Diagnostic;

  /** @brief Collect statistics on data that affects precision policy built by
   * the adaptor. */

  static Adaptor_Statistics *Adaptor_statistics; // TB
  
private:

  /** @brief Annotation file name */

  std::string _filename;

  /** @brief Any global state variables */

  TREE var_map    _globals;

  /** @brief Global pointer structure
   *
   * This behaves like a global version of the on_entry/on_exit
   * annotations.

  TREE structure_list _global_structures;

  NO LONGER NEEDED: now that we have the init procedure annotation.  */


  /** @brief Special init procedure
   *
   * This procedure represents the initial state of the system before main
   * is called. It holds initial values for the analyses, as well as
   * setting up initial pointer relationships. */

  TREE procedureAnn * _init;

  /** @brief Properties for analysis */

  TREE enum_property_map _enum_properties;

  /** @brief Special constant propagation property */

  REF enumPropertyAnn * _constants;

  /** @brief Set-based properties for analysis*/

  TREE set_property_map _set_properties;

  /** @brief Annotations for each procedure */

  TREE procedures_map _procedures;

  /** @brief Error count (during parsing) */

  int _errors;

  /** @brief header file
   *
   *  These fields contains the AST for the C header (usually, the header
   *  file for the library). */
  //@{

#ifdef __FOO
  unitNode * _header;
#endif
  str_list * _cpp_flags;
#ifdef __FOO
  id_lookup_walker * _syms;
#endif

  //@}

public:

  /** @brief Create new annotations
   *
   * Read in the annotations contained in the given filename, first
   * preprocessing the file with the given cpp flags. */

  Annotations(std::string & filename, str_list * cpp_flags);

  /** @brief Delete annotations */

  ~Annotations();

  /** @brief Clear
   *
   * Call clear on all sub-annotations. */

  void clear();

  /** @brief The special init() procedure annotations */

  inline procedureAnn * init() const { return _init; }

  /** @brief Add a new procedure */

  void add_procedure(procedureAnn * new_procedure);


  /** @brief Add globals
   *
   * This is a new version that allows global variables to have structure,
   * like the on_entry and on_exit annotations. */

  void add_globals(structuretree_list * structures);

  /** @brief Add one global
   *
   * Look up the given name, and create a new global variable if
   * necessary. If one already exists, just return it. This method also
   * handles looking up actual global variables declared in the header
   * file. */

  annVariable * add_one_global(const std::string & varname, bool is_io);

  /** @brief Add an enum property */

  void add_enum_property(enumPropertyAnn * property);

  /** @brief Add a setProperty property */

  void add_set_property(setPropertyAnn * set_property);

  /** @brief Lookup procedure annotation by name */

  procedureAnn * lookup_procedure(const std::string & name);

  /** @brief Lookup global variable by name */

  annVariable * lookup_global(const std::string & name);

  /** @brief Lookup header declaration by name */

#ifdef __HEADER
  declNode * lookup_header_decl(const std::string & name);
#endif

  /** @brief Lookup an enum property by name */

  enumPropertyAnn * lookup_enum_property(const std::string & name);

  /** @brief Lookup a set property by name */

  setPropertyAnn * lookup_set_property(const std::string & name);

  /** @brief Lookup either kind of property */

  propertyAnn * lookup_property(const std::string & name);

  /** @brief Get the special constants property */

  enumPropertyAnn * constants_property() const { return _constants; }

  /** @brief Reset the properties (including set properties) */

  void clear_properties();

  /** @brief Get the procedures list */

  const procedures_map & procedures() const { return _procedures; }

  /** @brief Get the globals list */

  const var_map & globals() const { return _globals; }

  /** @brief Get the enum properties list */

  const enum_property_map & enum_properties() const { return _enum_properties; }

  /** @brief Get the set properties list */

  const set_property_map & set_properties() const { return _set_properties; }

  /** @brief Store the C header (used only in the parser) */

#ifdef __HEADER
  void store_header(decl_list * decls);
#endif

  // --- Error handling

  void Error(int where, const std::string & msg);
  void Warning(int where, const std::string & msg);
  int errors() const { return _errors; }

  // --- Output

  friend std::ostream& operator<<(std::ostream & o, const Annotations & anns) {
    anns.print(o);
    return o;
  }

  void print(std::ostream & o) const;

  std::string & filename() { return _filename; }

private:

  FILE * _open_input_file(); 

  /** @brief Create a new global
   *
   * This method is used by add_one_global to create a new object if one
   * isn't found. */

  annVariable * new_global(const std::string & varname,
                           bool is_external,
                           bool is_io);

  /** @brief OLD add globals method
   *
   * This version works, but it doesn't handle the structured globals.*/

  void old_add_globals(parserid_list * vars);
};

#endif /* BDWY_ANNOTATIONS_H */
