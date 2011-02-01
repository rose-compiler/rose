
#ifndef BDWY_PROCEDURE_H
#define BDWY_PROCEDURE_H

// ----------------------------------------------------------------------
//   Procedure Annotation
// ----------------------------------------------------------------------

#ifdef __PROCLOCATION
typedef std::set< procLocation * > proclocation_set;
typedef proclocation_set::iterator proclocation_set_p;
typedef proclocation_set::const_iterator proclocation_set_cp;
#endif

#ifdef _CALLSITEENTRY
class callsiteEntry
{
private:

  /** @brief The stmtNode of the callsite */

  threeAddrNode * _stmt;

  /** @brief The callNode of the callsite */

  operandNode * _call;

  /** @brief Contexts
   *
   * The set of calling contexts that reach this callsite */

#ifdef __PROCLOCATION
  proclocation_set _contexts;
#endif

public:

  /** @brief Create a new call site */

  callsiteEntry(threeAddrNode * stmt, operandNode * call)
    : _stmt(stmt),
      _call(call),
      _contexts()
  {}

  inline threeAddrNode * stmt() const { return _stmt; }
  inline operandNode * call() const { return _call; }

#ifdef __PROCLOCATION
  inline proclocation_set & contexts() { return _contexts; }
#endif
};

typedef std::map< threeAddrNode *, callsiteEntry * > callsite_map;
typedef callsite_map::iterator callsite_map_p;
typedef callsite_map::const_iterator callsite_map_cp;

#endif /* _CALLSITEENTRY */

typedef std::set< enumPropertyAnn * > enum_property_set;
typedef enum_property_set::iterator enum_property_set_p;
typedef enum_property_set::const_iterator enum_property_set_cp;

typedef std::map< annVariable *, enum_property_set > variable_property_map;
typedef variable_property_map::iterator variable_property_map_p;
typedef variable_property_map::const_iterator variable_property_map_cp;


/** @brief Procedure annotation
 *
 *  Holds the annotations for one library procedure. */

class procedureAnn : public Ann
{
public:

  static procedureAnn * Current;

private:

  /** @brief the name of the procedure */

  std::string _name;

  /** @brief local variables
   *
   *  This map contains an entry for each local variable (formal
   *  parameters, as well as names introduced by on_entry and on_exit
   *  annotations). */

  TREE var_map _variables;

  /** @brief the list of formal parameters
   *
   *  Each entry is a reference into the _variables map. */

  REF var_list _formal_params;

  /** @brief Used variables
   *
   * The set of variables whose value is read. */

  REF var_set _uses;

  /** @brief Def'd variables
   *
   * The set of variables whose value is written. */

  REF var_set _defs;

  /** @brief Global variable uses
   *
   * These are also included in the _uses set. */

  REF var_set _global_uses;

  /** @brief Global variable defs
   *
   * These are also included in the _defs set. */

  REF var_set _global_defs;

  /** @brief Property uses
   *
   * For each variable in the annotations, record which properties of that
   * variable are tested. */

  REF variable_property_map _property_uses;

  /** @brief Property defs
   *
   * For each variable in the annotations, record which properties of that
   * variable are modified. */

  REF variable_property_map _property_defs;

  /** @brief Property weak defs
   *
   * This is a subset of the property defs. It holds the variables that
   * have been forced by the user's annotations to be weakly updated. */

  REF variable_property_map _property_weak_defs;

  /** @brief on_entry and on_exit annotations
   *
   */
  //@{

  TREE structure_list _on_entry;
  TREE structure_list _on_exit;

  //@}

  /** @brief New implementation of on_exit
   *
   * This contains a set of pointer rules: now we can have different
   * pointer behavior based on the states of the inputs. NOTE: this doesn't
   * change which objects get used/defd, so we need to be careful. */

  TREE pointerrule_list _pointer_rules;

  /** @brief Default pointer rule
   *
   * Keep a default (this is used in most cases). */

  REF pointerRuleAnn * _default_pointer_rule;

  /** @brief Analyze annotations
   */

  TREE analyze_map   _analyses;

  /** @brief Report annotations
   */

  TREE report_list _reports;

  /** @brief Substitution annotations
   */

#ifdef __ACTIONS
  TREE action_list _actions;
#endif

  /** @brief synthetic procNode
   *
   * Build a synthetic procNode that can be used in various path-related
   * functions. We should look up the actual function declaration in the
   * library header. TBD. */
#ifdef __PROCNODE 
  TREE procNode * _proc;
#endif
  /** @brief Numeric identifiers
   *
   * This is a list of idNodeAnn objects which are used in numeric
   * expressions. We keep this list so that when we enter a library
   * procedure during analysis, we can assign the numeric values to these
   * nodes. We can then use the regular exprNode::eval() function to get
   * their values. This list is created during the lookup() phase. */

  REF idnodeann_list _numeric_identifiers;

  /** @brief Annotations file
   *
   *  This is a reference to the Annotations object that contains this
   *  procedure. We need it to look up global variables */

  REF Annotations * _annotations;

  /** @brief Callsites
   *
   * Record all the places where this library routine is called. This will
   * help us merge contexts for optimizations. They are stored according to
   * the callsite that generated them. */

#ifdef __CALLSITE_MAP
  TREE callsite_map _callsites;
#endif

  /** @name Special properties
   *
   * This set of booleans records the special properties discovered about
   * this procedure. */
  //@{

  bool _pure_function;
  bool _allocates_memory;
  bool _deallocates_memory;
  bool _modifies_globals;
  bool _accesses_io;

  //@}

public:

  /** @brief Build a new procedure annotation
   *
   *  The new procedure annotation has the given name and list of input
   *  parameters. We also need a reference to the global Annotations object
   *  to allow proper variable lookup.
   *
   *  @param name the name of the procedure
   *  @param params the list of formal parameters
   *  @param anns the global Annotations object
   *  @param line the line number of the source annotation in the file */

  procedureAnn(parserID * name, parserid_list * params, Annotations * anns,
               const int line);

  /** @brief Delete a procedure
   *
   * Delete the procedure all all objects controlled by it. These include
   * all of the procedure-specific annotations, as well as all the local
   * variables. */

  ~procedureAnn();

  /** @name Accessors
   *
   *  Methods to get and set fields in the class. */
  //@{

  inline const std::string & name() const { return _name; }

  inline const var_map & variables() const { return _variables; }

  inline const var_set & uses() const { return _uses; }
  inline const var_set & defs() const { return _defs; }

  inline const var_list & formal_params() const { return _formal_params; }

  inline const structure_list & on_entry() const { return _on_entry; }
  inline const structure_list & on_exit()  const { return _on_exit; }

  inline const pointerrule_list & pointer_rules() const { return _pointer_rules; }
  inline pointerRuleAnn * default_pointer_rule() const { return _default_pointer_rule; }

  inline const analyze_map & analyses() const { return _analyses; }

  inline const report_list & reports() const { return _reports; }

#ifdef __ACTIONS
  inline const action_list & actions() const { return _actions; }
#endif

#ifdef __PROCNODE 
  inline procNode * proc() const { return _proc; }
#endif
  inline idnodeann_list & numeric_identifiers() { return _numeric_identifiers; }

#ifdef __CALLSITE_MAP
  inline const callsite_map & callsites() const { return _callsites; }
#endif

  inline bool pure_function() const { return _pure_function; }
  inline bool allocates_memory() const { return _allocates_memory; }
  inline bool deallocates_memory() const { return _deallocates_memory; }
  inline bool modifies_globals() const { return _modifies_globals; }
  inline bool accesses_io() const { return _accesses_io; }

  //@}

  /** @brief Lookup a variable
   *
   *  This method looks up a local variable by name. The "create" flag
   *  indicates whether or not to create a new one if it doesn't already
   *  exist. The optional "is_new" flag should be set to true for local
   *  variables that represent newly allocated memory (i.e., are introduced
   *  with the "new" keyword in the on_exit annotations.
   *
   *  @param variable the name of the variable to look up
   *  @param create pass true to create a new variable if one doesn't already exist
   *  @param is_new pass true if the object is allocated in the annoation
   */

  annVariable * lookup(const std::string & variable, bool create, bool is_new = false);

  // --- Manage uses and defs -----------------------

  /** @brief Add defs
   *
   *  This method is used by the parser to process the "modify"
   *  annotations. At this point, the variable names are just temporarily
   *  stored.
   *
   *  @param variables the list of variables from the "modify" annotation */

  void add_defs(parserid_list * variables);

  /** @brief Add one def
   *
   * Mark the variable as defined, and if it's global, add it to the list
   * of global defs. */

  void add_one_def(annVariable * var);

  /** @brief Add uses
   *
   *  This method is used by the parser to process the "access"
   *  annotations. At this point, the variable names are just temporarily
   *  stored.
   *
   *  @param variables the list of variables from the "access" annotation */

  void add_uses(parserid_list * variables);

  /** @brief Add one use
   *
   * Mark the variable as used, and if it's global, add it to the list of
   * global uses. */

  void add_one_use(annVariable * var);

  /** @brief Def/use location
   *
   * Given a procLocation corresponding to a call to this procedure, return
   * the special synthetic stmtLocation that holds all of the defs and
   * uses. */

#ifdef __PROCLOCATION
  stmtLocation * def_use_location(procLocation * procloc);
#endif

  /** @brief Allocation/deallocation location
   *
   * Given a procLocation corresponding to a call to this procedure, return
   * the special synthetic stmtLocation to which we attach deallocation of
   * heap objects. It needs to dominate the def/use location so that
   * changes to the multiplicity of an object are properly reflected in
   * assignments. */

#ifdef __PROCLOCATION
  stmtLocation * dealloc_location(procLocation * procloc);
#endif

  /** @brief Allocation location
 *
 * Given a procLocation corresponding to a call to this procedure, return
 * the special synthetic stmtLocation to which we attach allocation of heap
 * objects. It needs to dominate the def/use location, but be dominated by
 * the deallocation location so that changes to the multiplicity of an
 * object are properly reflected in assignments. */

#ifdef __PROCLOCATION
  stmtLocation * alloc_location(procLocation * procloc);
#endif


  // --- Manage property uses and defs -----------------------

  /** @brief Add property use
   *
   * Add this variable to the list of variables whose property value is
   * tested. This does not actually mark the variable itself as used. */

  void add_property_use(annVariable * var, enumPropertyAnn * enum_property);

  /** @brief Get the property uses
   *
   * */

  inline const variable_property_map & property_uses() const { return _property_uses; }

  /** @brief Add property def
   *
   * Add this variable to the list of variables whose property value is
   * modified. This does not actually mark the variable itself as def'd. */

  void add_property_def(annVariable * var, enumPropertyAnn * enum_property);

  /** @brief Get the property defs
   *
   * */

  inline const variable_property_map & property_defs() const { return _property_defs; }

  /** @brief Add property weak def
   *
   * Add this variable to the list of variables whose property value is
   * weakly updated. This should be a subset of the property defs. */

  void add_property_weak_def(annVariable * var, enumPropertyAnn * enum_property);

  /** @brief Get the property weak defs
   *
   * */

  inline const variable_property_map & property_weak_defs() const { return _property_weak_defs; }

  // ---------------------------------------

  /** @brief Add a deleted variable
   *
   * Record the given variable as deleted. We store it temporarily in the
   * _deletes list, and then perform the actual lookup during the
   * postprocess method. */

  void add_delete(parserID * id);

  /** @brief Postprocess procedure annotation
   *
   *  This method is called by Annotations::add_procedure() once parsing of
   *  the procedure annotation is complete. Take the temporary uses and
   *  defs sets and look up each annVariable. Set the 'used' or 'defined'
   *  flag appropriately. */

  void postprocess();

  /** @brief Add an on_entry annation
   *
   *  The given on_entry structure information is added to the procedure by
   *  processing it into a series of simpler operations. See structure.h
   *  for a discussion of this conversion.
   *
   *  @param structures the on_entry structure information to add
   *  @see structureTreeAnn
   *  @see structureAnn
   */

  void add_on_entry(structuretree_list * structures); 

  /** @brief Add an on_exit annation
   *
   *  The given on_exit structure information is added to the procedure by
   *  processing it into a series of simpler operations. See structure.h
   *  for a discussion of this conversion.
   *
   *  @param structures the on_exit structure information to add
   *  @see structureTreeAnn
   *  @see structureAnn
   */

  void add_on_exit(structuretree_list * structures);

  /** @brief New add on_exit
   *
   * This uses the new conditional form of pointer annotations. This method
   * handles compiling the structuretree_list into the form used by the
   * analyzer. */

  void add_on_exit(pointerRuleAnn * pointer_rule);

  /** @brief Add structured globals
   *
   * This method is only used to process the global structure
   * annotations. It is similar to add structures with the following
   * exceptions: (1) all the new variables that it discovers are added as
   * globals, (2) all "dot" operators go into the on_entry annotations and
   * (3) all "arrow" operators go into the on_exit annotations. This way we
   * can just call Analyzer::process_on_entry() and
   * Analyzer::process_on_exit() to set them up. */

  void add_global_structures(Annotations * annotations,
                             annVariable * parent_var,
                             structureTreeAnn * parent_node);

  /** @brief Add an analysis annotation
   *
   *  @param new_analysis the analysis object to add */

  void add_analysis(analyzeAnn * new_analysis);

  /** @brief Add a calling context entry
   *
   * Create a new callsite entry if one doesn't yet exist */

#ifdef __PROCLOCATION
  void add_calling_context(operandNode * call, procLocation * where);
#endif

  /** @brief Add a report annotation */

  void add_report(reportAnn * report);

  /** @brief Add a action annotation */
  void add_action(actionAnn * action);

  /** @brief Find a callsite
   *
   * Return null if the procedure is not called at this statement. */

#ifdef __CALLSITE_MAP
  callsiteEntry * find_callsite(threeAddrNode * stmt);
#endif

  /** @brief Process report annotations
   *
   * */

  void process_reports(Analyzer * analyzer);

  /** @brief Test actions
   *
   * Evaluate the condition of each action annotation in each calling
   * context of this procedure. */

  void test_actions(Analyzer * analyzer);

  /** @brief Find applicable action
   *
   * Given a particular statement, see if there is an applicable action for
   * this procedure. It is not necessary to check if the statement is
   * actually a call to the procedure. Only the first applicable aciton is
   * returned, even if there is more than one. */

#ifdef __CALLSITE_MAP
  actionAnn * find_applicable_action(threeAddrNode * stmt);
#endif
  /** @brief Clear
   *
   * Re-initialize the callsite information in preparation for analysis. */

  void clear();

  // --- Output

  friend std::ostream& operator<<(std::ostream & o, const procedureAnn & pa) {
    pa.print(o);
    return o;
  }

  void print(std::ostream & o) const;

private:

  void add_structures(bool is_on_entry,
                      structure_list & the_list,
                      annVariable * parent_var,
                      structureTreeAnn * parent_node);

  /** @brief Temporary variable lists
   *
   *  Besides the obvious access and modifies annotations, we also augment
   *  these lists with any objects that are implicitly accessed or modified
   *  as a result of on_entry, on_exit or analyze annoations. After the
   *  structure annotations are processed, we go through this list and
   *  update the corresponding annVariable flags a reference into the
   *  _variables map. ALSO: store the list of deleted objects here. */
  //@{

  parserid_list * _temp_defs;
  parserid_list * _temp_uses;
  parserid_list * _temp_deletes;

  //@}

  void postprocess_variables(parserid_list & uses_or_defs,
                             bool is_defs);
};
  
#endif /*  */
