
#ifndef BDWY_ENUM_PROPERTY_H
#define BDWY_ENUM_PROPERTY_H

#include <list>
#include <map>
#include <string>
#include <iostream>
#include <bitset>

typedef std::list< enumValueAnn * > enumvalue_list;
typedef enumvalue_list::iterator enumvalue_list_p;
typedef enumvalue_list::const_iterator enumvalue_list_cp;

typedef std::map< std::string, enumPropertyAnn *> enum_property_map;
typedef enum_property_map::iterator enum_property_map_p;
typedef enum_property_map::const_iterator enum_property_map_cp;

/** @brief A dataflow analysis lattice element
 *
 */

class enumValueAnn : public Ann
{
private:

  /** @brief Name of the value */

  std::string _name;

  /** @brief Pointer to the lower lattice element */

  enumValueAnn * _more_general;

  /** @brief List of higher lattice elements  */

  enumvalue_list _more_specific;

  /** @brief Height in the lattice */

  int _height;

  /** @brief Unique id number */

  int _id;

  /** @brief Is leaf
   *
   * True if the only value higher in the lattice is TOP. */

  bool _is_leaf;

public:

  /** @brief Create a new property value */

  enumValueAnn(const parserID * name,
               enumvalue_list * more_specific);

  // Fields

  inline std::string & name() { return _name; }
  inline const std::string & name() const { return _name; }

  inline enumValueAnn * more_general() const { return _more_general; }

  inline enumvalue_list & more_specific() { return _more_specific; }
  inline const enumvalue_list & more_specific() const { return _more_specific; }

  inline int height() const { return _height; }
  inline void height(int new_height) { _height = new_height; }

  inline int id() const { return _id; }
  inline void id(int new_id) { _id = new_id; }

  inline bool is_leaf() const { return _is_leaf; }

  /** @brief Collect atleast values
   *
   * Collect all the values that are at least as specific as this one,
   * excluding top. */

  void collect_atleast_values(enumvalue_set & collect);
};

// ----------------------------------------------------------------------
//  Enum Value Set
// ----------------------------------------------------------------------

#define MAX_ENUMVALUES 32

typedef std::bitset< MAX_ENUMVALUES > bitset32;

class enumvalue_set : public  bitset32
{
public:

  /** @brief Construct an empty set */

  enumvalue_set()
    : bitset32()
  {}

  /** @brief Construct a single element set
   *
   * Contains the single given element */

  enumvalue_set(enumValueAnn * val)
    : bitset32()
  {
    set(val->id());
  }

  /** @brief Insert into the set
   */

  inline void insert(enumValueAnn * val) { set(val->id()); }

  /** @brief Inclusion test
   *
   * Test to see if the input set is wholly included in this set. The test
   * is basically to see if the this intersected with other equals
   * other. */

  inline bool includes(enumvalue_set & other) {
    enumvalue_set temp(*this);
    temp &= other;
    return (temp != other);
  }

  /** @brief Set union */

  inline void set_union(const enumvalue_set & other) { (*this) |= other; }

  /** @brief Set intersection */

  inline void set_intersection(const enumvalue_set & other) { (*this) &= other; }
};

// ----------------------------------------------------------------------
//  Dataflow analysis property
// ----------------------------------------------------------------------

class Diagnostic; // TB new

/** @brief A dataflow analysis property
 *
 */

class enumPropertyAnn : public propertyAnn
{
public:

  typedef std::vector< enumValueAnn *> enumvalue_vec;
  typedef enumvalue_vec::iterator enumvalue_vec_p;
  typedef enumvalue_vec::const_iterator enumvalue_vec_cp;

#ifdef __MEMORYACCESS
  typedef std::map< memoryAccess *, enumvalue_set> defuse_property_map;
  typedef defuse_property_map::iterator defuse_property_map_p;
#endif

#ifdef __MEMORYBLOCK
  typedef std::map< memoryBlock *, enumvalue_set > block_propertyset_map;
  typedef block_propertyset_map::iterator block_propertyset_map_p;
#endif

#ifdef __PROCLOCATION
  typedef std::pair< int, double > count_accuracy_pair;
  typedef map< procLocation *, count_accuracy_pair > accuracy_map;
  typedef accuracy_map::iterator accuracy_map_p;
#endif

#ifdef __MEMORYBLOCK
  typedef std::map< Location *, memoryblock_set > tested_objects_map;
  typedef tested_objects_map::iterator tested_objects_map_p;
#endif

#ifdef __STMTNODE
  typedef std::map< stmtNode *, count_accuracy_pair > stmt_accuracy_map;
  typedef stmt_accuracy_map::iterator stmt_accuracy_map_p;
#endif

  // TB
#ifdef __MEMORYBLOCK
  typedef std::pair< memoryBlock*, Location* > block_loc_pair;
  typedef std::set< block_loc_pair >           block_loc_set;

  typedef std::pair< memoryBlock*, procNode* > block_proc_pair;
  typedef std::set< block_proc_pair >          block_proc_set;
#endif

private:

  /** @brief May or must property
   *
   * When true, the sets of value are combined using union, making the
   * property a "may" property. When false, the sets are combined using
   * intersection, making this a "must" property. */

  bool _is_may_property;

  /** @brief Strong or weak
   *
   * We can have properties that enforce weak updates, or not. It's up to
   * the user to make sure that these are correct. */

  bool _enforce_weak_updates;

  /** @brief Pointer to the lattice elements
   *
   *  This pointer actually refers to the "bottom" element. All other
   *  elements are reachable by following the "more_specific" pointers. */

  TREE enumValueAnn * _values;

  /** @brief Lattice top */

  TREE enumValueAnn * _top;

  /** @brief Default value
   *
   * This is the value we'll use whenever there isn't a reaching definition
   * (or use, depending on the direction). If none is provided, then TOP is
   * used. */

  REF enumValueAnn * _default;

  /** @brief Lattice elements ordered by index */

  REF enumvalue_vec _value_index;

  /** @brief Flow-sensitive "must" values */

#ifdef __MEMORYACCESS
  defuse_property_map _now_values;
#endif

  /** @brief Flow-insensitive "may" values */

#ifdef __MEMORYBLOCK
  block_propertyset_map _ever_values;
#endif

  // TB_unify
  /** @brief Values of property blocks as input_to to a callee. */
#ifdef __MEMORYBLOCK
  typedef std::pair<procedureInfo*,memoryBlock*> Input_to_pair;
  std::map<Input_to_pair,enumvalue_set> _input_to_values;
#endif

  /** @brief Objects that need flow-sensitivity
   *
   * Store a list of the real blocks that need flow sensitivity for this
   * property. */

#ifdef __FLOW_SENSITIVE_SET
  flow_sensitive_set _flow_sensitive_objects;
#endif

  /** @brief Tested objects
   *
   * For trace-back adaptivity, keep track of all objects whose value is
   * tested. */

#ifdef __TESTED_OBJECTS
  tested_objects_map _tested_objects;
#endif

  /** @brief Accuracy measurements
   *
   * Store information about the accuracy of flow values at locations where
   * the property is tested. We can use this information to compute an
   * aggregate accuracy for the property. */

#ifdef __PROCLOCATION
  accuracy_map _accuracy;
#endif

  /** @brief Error statements
   *
   * Keep a list of the statements that have errors that depend on this
   * property. */

#ifdef __FOO
  stmt_set _error_statements;

#endif
  /** @brief Number of flow values
   *
   * The number of possible flow values, exluding top and bottom. */
  double _number_of_flowvalues;

#ifdef __FOO
  /** @brief Error procedures
   *
   * The set of procedures that contain errors of this type. */

  procedureinfo_set _error_procedures;

// begin TB new

  /** @brief Diagnostic
   *
   * When the property declares that error diagnostic on some values are desired, 
     an instance of Diagnostic is created; else it is NULL. */

  Diagnostic *_diagnostic;
#endif
// end TB new

public:

  /** @brief Create a new property */

  enumPropertyAnn(const parserID * name,
		  Direction direction,
		  bool enforce_weak_updates,
		  const parserID * default_name,
		  enumvalue_list * lowest_values,
                  parserid_list * diagnostic_values=NULL);

  /** @brief Clear the analysis information
   *
   * Call this method to re-initialize the analysis information so that the
   * property analyzer can be run again. */

  void clear();

  /** @brief Query whether it enforces weak updates */

  inline bool enforce_weak_updates() const { return _enforce_weak_updates; }

  /** @brief Lattice */

  inline enumValueAnn * values() const { return _values; }

  /** @brief Lookup a value by name */

  enumValueAnn * lookup(const std::string & name);

  /** @brief Return the bottom element */

  inline enumValueAnn * bottom() const { return _values; }

  /** @brief Return the top element */

  inline enumValueAnn * top() const { return _top; }

  /** @brief Return the default element */

  inline enumValueAnn * default_val() const { return _default; }

  /** @brief Meet function
   *
   * This version is for meeting individual elements. */

  enumValueAnn * meet(enumValueAnn * one,
		      enumValueAnn * two);

  /** @brief Set meet function
   *
   * Depending on whether this is a "may" or "must" property. Side-effects
   * the value of the first parameter. */

  void meet_with(enumvalue_set & first,
                 const enumvalue_set & second);

  /** @brief At-least function
   *
   * Return true if left >= right ( left ^ right == right). Includes the
   * special case that if right is top, then return false. */

  bool at_least(enumValueAnn * left,
		enumValueAnn * right);

  /** @brief At-most function
   *
   * Return true if left <= right ( left ^ right == left) */

  bool at_most(enumValueAnn * left,
               enumValueAnn * right);

  /** @brief Look up a sub-block
   *
   * Given a real memoryBlock, look up the corresponding sub-block for this
   * property. Returns null if none exists. */

#ifdef __MEMORYBLOCK
  memoryBlock * lookup_property_block(memoryBlock * real_block);
#endif

  /** @brief Add a sub-block
   *
   * This method is called by the analyzer during pointer analysis to tell
   * the property about the sub-blocks that it's creating for it. */

#ifdef __MEMORYBLOCK
  void add_property_block(memoryBlock * real_block, memoryBlock * property_block);
#endif

  /** @brief Test two variables
   *
   *  Test a particular property condition between two variables. */
#ifdef __TEST
  bool test(Broadway::Operator op, Broadway::FlowSensitivity flow_sensitivity,
	    Location * where,
	    pointerValue & lhs, std::string & lhs_name,
	    pointerValue & rhs, std::string & rhs_name);

  /** @brief Test one variable
   *
   *  Apply the specified test operator to the given variable and property
   *  value. */

  bool test(Broadway::Operator op, Broadway::FlowSensitivity flow_sensitivity,
	    Location * where,
	    pointerValue & lhs, std::string & lhs_name,
	    enumvalue_set rhs_value_set);

  /** @brief Property test
   *
   *  Apply the specified test operator to the two given property values. */

  bool test(Broadway::Operator op,
	    enumvalue_set lhs_value_set, enumvalue_set rhs_value_set);

  /** @brief Lookup flow-sensitive "now" value
   *
   * Defaults to "top" */

#ifdef __MEMORYACCESS
  enumvalue_set lookup_now_value(memoryBlock * property_block,
				  memoryAccess * def_or_use);
#endif

  /** @brief Set flow-sensitive "now" value
   *
   * We don't store "top" values, because the lookup method above defaults
   * to "top". Return true if this changed the value. */

#ifdef __MEMORYACCESS
  bool update_now_value(Location * where,
			memoryBlock * property_block,
			memoryAccess * def_or_use,
			enumvalue_set value,
			bool & lost_information);
#endif

  /** @brief Set flow-insensitive must "ever" value
   *
   * We don't store "top" values, because the lookup method above defaults
   * to "top". Return true if this changed the value. */

#ifdef __MEMORYBLOCK
  bool update_ever_value(memoryBlock * property_block, enumvalue_set values);
#endif

  // TB_unify
  /** Lookup input_to value */
#ifdef __MEMORYBLOCK
  enumvalue_set lookup_input_to_value(procedureInfo *info,
                                      memoryBlock * property_block);
#endif
  // TB_unify
  /** Set input_to value */
#ifdef __MEMORYBLOCK
  void update_input_to_value(procedureInfo *info, memoryBlock * property_block,
                             enumvalue_set values, stmtLocation *callsite);
#endif

  /** @name Value construction methods */
  //@{

  /** @brief Construct flow-sensitive "now" value
   *
   * For each memoryblock in the pointer value, get the property value
   * associated with the most recent dominating def. Meet all of these
   * values together. If there is no reaching def/use we return the default
   * property value. */

#ifdef __MEMORYBLOCK
  enumvalue_set construct_now_value(Location * where,
				    pointerValue & variable,
				    std::string & variable_name,
				    bool & lost_information,
				    memoryblock_set & complicit_property_blocks);
#endif

  /** @brief Construct flow-sensitive "now" value
   *
   * 
   * Same method as above, but for a single memoryBlock. Also, if there is
   * no reaching def/use, return null. */

#ifdef __MEMORYBLOCK
  enumvalue_set construct_now_value(Location * where,
				    memoryBlock * property_block,
				    bool default_to_top);
#endif

  /** @brief Construct flow-sensitive "after now" value
   *
   * For each memoryblock in the pointer value, get the property value
   * associated with modifications in the transfer functions (defs at this
   * location). Meet all these values together. */

  enumvalue_set construct_after_value(pointerValue & variable, std::string & name);

  /** @brief Construct flow-sensitive "after now" value
   *
   * Same method as above, but for a single memoryBlock. Also, if there is
   * no reaching def/use, return null. */

#ifdef __MEMORYBLOCK
  enumvalue_set construct_after_value(memoryBlock * real_block);
#endif

  /** @brief Construct flow-insensitive may "ever" value
   *
   * For each memoryblock in the pointer value, union the sets of property
   * values together. */

  enumvalue_set construct_ever_value(pointerValue & variable, std::string & name);

  /** @brief Construct weak value
   *
   * During backwards analysis, when we construct the value for the LHS, we
   * need to take into account the previous value of objects that were
   * weakly updated. Therefore, this is a special version of
   * construct_now_value() that meets the now value with the after value
   * for weakly updated blocks. */

  enumvalue_set construct_weak_now_value(Location * where,
                                         pointerValue & variable, std::string & name);

  //@}

  /** @brief Compute next state
   *
   * This is called in enumPropertyExprAnn::compute_next(), but it's
   * basically just a wrapper around contruct_now_value(), with the
   * addition of the assignment management. */

#ifdef __MEMORYBLOCK
  enumvalue_set compute_next(Location * where,
                             ruleAnn * rule,
			     exprAnn * expr,
			     pointerValue & right,
			     std::string & right_name,
			     bool & rhs_lost_information,
			     memoryblock_set & complicit_property_blocks,
			     enumvalue_set & save_ever_values);
#endif

  /** @brief Change state
   *
   *  Update the state of the given variable to have the given property
   *  value. This method updates all three kinds of flow information. Put
   *  any changes into the changes set (NOTE: these will be the property
   *  blocks, not the real blocks). */

#ifdef __MEMORYBLOCK
  void apply_next(Location * where,
		  stmtLocation * parameter_callsite,
		  ruleAnn * rule,
		  enumPropertyExprAnn * expr,
		  pointerValue & left,
		  std::string & left_name,
		  pointerValue & right,
		  enumvalue_set new_value,
		  bool rhs_lost_information,
		  bool rhs_changed, // TB_unify
		  memoryblock_set & complicit_property_blocks,
		  enumvalue_set ever_values,
		  memoryblock_set & changes);
#endif
  /** @brief Apply merge
   *
   * At a control-flow merge, get the reaching "now" values for the given
   * object and merge them together. Return true if that results in a
   * change. */

#ifdef __MEMORYBLOCK
  void apply_merge(Location * where,
                   memoryBlock * property_block, memoryuse_list & phi_uses,
		   memoryblock_set & changes);
#endif

  /** @brief Apply assignment
   *
   * At a normal assignment, build the "now" value for the right-hand side,
   * and assign it to the left-hand side. We'll also handle weak updates by
   * calling the weak_update() method below. */

#ifdef __MEMORYBLOCK
  void apply_assignment(Location * where,
			stmtLocation * parameter_callsite,
			pointerValue & left, pointerValue & right,
			bool is_parameter,
			memoryblock_set & changes);
#endif
  /** @brief Self assignment
   *
   * Now that properties are associated with special property blocks, we
   * need to explicitly handle self-assignment caused by passing parameters
   * and external inputs and outputs. */

#ifdef __MEMORYBLOCK
  void self_assignment(Location * source,
                       Location * target,
		       memoryBlock * property_block, 
		       memoryblock_set & changes,
           bool is_input);
#endif

  /** @brief Conservative procedure call
   *
   * Record a conservative procedure call, primarily for back-trace
   * purposes. */

#ifdef __MEMORYBLOCK
  void conservative_procedure_call(stmtLocation * current,
                                   pointerValue & reachable,
				   memoryblock_set & changes);
#endif

  /** @brief Report
   *
   * Retrieve the value for the given variable and flow-sensitivity mode,
   * print out the results. */

  void report(std::ostream & out,
	      bool is_error, 
	      procLocation * where,
	      Broadway::FlowSensitivity flow_sensitivity,
	      pointerValue & lhs, std::string & lhs_name);

#endif /* __TEST */
  /** @brief Output operator */

  friend std::ostream& operator<<(std::ostream & o, const enumPropertyAnn & anns) {
    anns.print(o);
    return o;
  }

  /** @brief Precision analysis
   *
   * Post-process the results of analysis to determine a list of variables
   * to make flow sensitive. */

#ifdef __MEMORYBLOCK
  void precision_analysis(memoryModel * memory_model,
			  memoryblock_set & flow_sensitive);
#endif

  /** @brief NEW Precision analysis
   */

  void precision_analysis(Analyzer * analyzer, int analyzed_properties);

  /** @brief Compute accuracy
   *
   * Print out accuracy information and return overall value. */

  double compute_accuracy(Analyzer * analyzer);

  /** @brief Add flow-sensitive object
   *
   * Record that in the future, this property block should be flow
   * sensitive. */

#ifdef __MEMORYBLOCK
  void add_flow_sensitive_object(memoryBlock * property_block);
#endif

  /** @brief Set flow sensitivity
   *
   * For a given real blocks, see if it's property block for this
   * property. Called by Analyzer::lookup_property_block() */

#ifdef __MEMORYBLOCK
  void set_flow_sensitivity(memoryBlock * real_block);
#endif

  /** @brief Count flow sensitive objects
   */

#ifdef __FLOW_SENSITIVE_SET
  inline int count_flow_sensitive_objects() { return _flow_sensitive_objects.size(); }
#endif
  /** @brief Output method */

  void print(std::ostream & o) const;  

  /** @brief Merge enumvalue_set
   *
   * Visit the values in an enumvalue_set and merge them into a single
   * value using the meet function. */

  enumValueAnn * merge_enumvalue_set(enumvalue_set value_set);

  /** @brief Convert enum values to string */

  std::string to_string(enumvalue_set value_set);

// begin TB new
  /** @brief Reset error diagnostic.
   */
  void reset_diagnostic();

  /** @brief Performs error diagnostic.
   *
   * Performs error diagnostic if any diagnostic values declared by the
   * property is found in value_set.
   * The precondition is that a condition involving this property (such
   * as an enumPropertyExprAnn) with the pointer value <i>pv</i>, value set
   * <i>value_set</i>, and location <i>where</i>is already evaluated to be
   * true; we now want to trace back from this location how <i>pv</i> gets
   * this value. */

#ifdef __PROCLOCATION
  void diagnostic(std::ostream &out, procLocation *where, pointerValue &pv,
                  enumvalue_set values) const;
#endif
// end TB new

  /** @brief Compare the value at a location to a given value. */
  // TB_unify
#ifdef __MEMORYBLOCK
  bool compare_now_value(Location *where, memoryBlock *property_block,
                         enumvalue_set compare_to);
#endif
private:

  /** @brief Print out this property */

  void print(std::ostream & o, enumValueAnn * prop, int depth) const;

  /** @brief Number values */

  void number_values(enumValueAnn * prop,
                     int & cur_index, int height, int & max_height);


  /** @brief Track destructive assignments
   */

#ifdef __MEMORYBLOCK
  void track_destructive_assignments(Analyzer * analyzer,
                                     Location * where,
				     memoryBlock * block,
				     bool disallow_context_sensitivity,
				     block_loc_set & already_seen,
				     block_loc_set seen_stack,
				     memoryblock_set & made_flow_sensitive,
				     memoryblock_set & made_fs_destructive,
				     block_loc_set & made_context_sensitive,
				     block_proc_set & eval_cs_seen_destructive,
				     block_proc_set & eval_cs_seen_complicit,
				     block_proc_set & eval_made_cs,
				     bool & make_chain_flow_sensitive,
				     bool & make_chain_context_sensitive,
				     memoryblock_vector & chain,
				     std::string & indent);
#endif
  /** @brief Record tested objects
   *
   * This method is called by the test functions to record which object are
   * tested. It also sets up any complicit assignments that start the
   * adaptation process. */

#ifdef __MEMORYBLOCK
  void record_tested_objects(Location * where,
                             pointerValue & ptr,
			     enumvalue_set & value_set,
			     memoryblock_set & complicit_property_blocks);
#endif
  /** @brief Trace object
   *
   * Trace the assignments to a property, generate a report. */

#ifdef __MEMORYBLOCK
  void trace_object(std::ostream & out,
		    memoryBlock * property_block,
		    memoryblock_set & already_seen,
		    std::string & indent);
#endif
  /** @brief Evaluate a procedure for context sensitivity
   *
   * Includes all the code to prune out unnecessary cases. Returns true if
   * it actually made the procedure context sensitive.*/

#ifdef __MEMORYBLOCK
  bool evaluate_context_sensitivity(Analyzer * analyzer,
				    Location * target,
				    memoryBlock * block,
				    procLocation * procloc,
                                    bool destructive,
				    block_proc_set & eval_cs_seen_destructive,
				    block_proc_set & eval_cs_seen_complicit,
                                    block_proc_set & evaled_cs,
				    std::string & indent);
#endif
  /** @brief Validate context sensitivity for property
   *
   * */

#ifdef __MEMORYBLOCK
  bool validate_property_cs(memoryBlock * block,
			    const callsite_objects_map & assignments,
                            procedureInfo *procedure, // TB
			    std::string & indent);
#endif
    /** @brief Validate context sensitivity for multiplicity
   *
   * */

#ifdef __MEMORYBLOCK
  bool validate_multiplicity_cs(memoryBlock * block,
				const callsite_objects_map & assignments,
                                procedureInfo *procedure, // TB
				std::string & indent);
#endif
  /** @brief Validate context sensitivity for regular pointer variable
   *
   * */

#ifdef __MEMORYBLOCK
  bool validate_pointer_cs(memoryBlock * block,
                           const callsite_objects_map & assignments,
                           procedureInfo *procedure, // TB
			   std::string & indent);
#endif
  /** @brief Validate pointer flow sensitivity
   *
   * Make sure that distinguishing the various pointer targets would
   * actually help. */

#ifdef __MEMORYBLOCK
  bool validate_pointer_fs(memoryBlock * block,
                           stmtLocation * where,
			   std::string & indent);
#endif
  /** @brief Is location reachable?
   *
   * Check to see if a def of the block at the source could reach the
   * target. */

#ifdef __MEMORYBLOCK
  bool is_location_reachable(Location * source, Location * target, memoryBlock * block);
#endif

  /** @brief Reachable values
   *
   * Find all the reachable property blocks and gather their values. */
  
#ifdef __MEMORYBLOCK
  enumvalue_set reachable_values(stmtLocation * where,
                                 const memoryblock_set & blocks,
                                 procedureInfo *callee=NULL);
#endif
  /** @brief Make context sensitive
   *
   * Make the given procedure context sensitive. */

#ifdef __PROCEDUREINFO
  bool add_context_sensitive_proc(procedureInfo * info,
				  procedureinfo_set & already_seen,
				  procedureinfo_set & make_cs);
#endif
};

#endif /* BDWY_ENUM_PROPERTY_H */
