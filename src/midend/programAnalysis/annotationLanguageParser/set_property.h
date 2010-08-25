
#ifndef BDWY_SET_PROPERTY_H
#define BDWY_SET_PROPERTY_H

typedef std::map< std::string, setPropertyAnn *> set_property_map;
typedef set_property_map::iterator set_property_map_p;
typedef set_property_map::const_iterator set_property_map_cp;

// -- NOTE: By choosing 1024 here, we are fixing the maximum size of sets
// and equivalence classes.

typedef std::bitset< 1024 > memoryblock_bitset;

class setPropertyAnn : public propertyAnn
{
public:

  typedef enum { Set, Equivalence } SetPropertyKind;

  typedef enum { Union, Intersect } MeetFunction;

  // -- Manage the mapping from memoryblocks to bit positions

#ifdef __FOO
  typedef std::map< memoryBlock *, int > memoryblock_position_map;
  typedef memoryblock_position_map::iterator memoryblock_position_map_p;

  // -- Store the memoryBlocks in a vector to create the reverse mapping

  typedef vector< memoryBlock * > memoryblock_vector;
  typedef memoryblock_vector::iterator memoryblock_vector_p;

  // -- Set representation: map from program points to sets of blocks

  typedef std::map< memoryAccess *, memoryblock_bitset > set_property_map;
  typedef set_property_map::iterator set_property_map_p;

  // -- Equivalence representation: map from program points to lists of
  // sets of blocks. Each set of blocks in the list represents an
  // equivalence class.

  typedef list< memoryblock_bitset > memoryblock_bitset_list;
  typedef memoryblock_bitset_list::iterator memoryblock_bitset_list_p;
  typedef memoryblock_bitset_list::const_iterator memoryblock_bitset_list_cp;

  typedef std::map< memoryAccess *, memoryblock_bitset_list> equiv_property_map;
  typedef equiv_property_map::iterator equiv_property_map_p;
#endif  
private:

  /** @brief Kind of setProperty */

  SetPropertyKind _kind;

  /** @brief Meet function */

  MeetFunction _meet_function;

  /** @brief Property variable
   *
   * This is a specially generated global variable that is used to hold the
   * binding of the current value of this property. Is it created in the
   * Annotations::add_set_property() method */

  annVariable * _property_variable;

  /** @brief Property variable block
   *
   * When the analyzer is created it creates a memoryBlock for each set
   * property and stores it in this field. */

#ifdef __FOO
  memoryBlock * _block;

  /** @brief Set representation
   *
   * */

  set_property_map _set;

  /** @brief Equivalence representation
   *
   * */

  equiv_property_map _equivalence;

  /** @brief Mapping from memoryBlocks to bit positions */

  memoryblock_position_map _position_of_memoryblock;

  /** @brief Mapping from positions back to memoryBlocks */

  memoryblock_vector _memoryblock_at_position;

  /** @brief Change flag
   *
   * Since we perform all the actual updates in the exprAnn::compute_next()
   * method, we just record whether or not anything changed. Later, we'll
   * pick up this value. */

  bool _change;
#endif
public:

  /** @brief Create a new setProperty property
   *
   * */

  setPropertyAnn(const parserID * name,
                 Direction direction,
		 SetPropertyKind kind,
		 MeetFunction meet);

  /** @brief Clear the analysis information
   *
   * Call this method to re-initialize the analysis information so that the
   * property analyzer can be run again. */

  void clear();

  /** @brief Kind of setProperty */

  inline SetPropertyKind kind() const { return _kind; }

  /** @brief Meet function */

  inline MeetFunction meet_function() const { return _meet_function; }

  /** @brief Set the property variable
   *
   * This method should only be called once by
   * Annotations::add_set_property(). */

  inline void set_property_variable(annVariable * var)
  { _property_variable = var; }

  /** @brief Property variable
   *
   * Return a pointer to the special global variable that represents the
   * state of this property. */

  inline annVariable * property_variable() const { return _property_variable; }

  /** @brief Set the property variable block
   *
   * This method should only be called by the Analyzer constructor. */

#ifdef __FOO
  void set_property_variable_block(memoryBlock * block)
  { _block = block; }

  /** @brief Property variable memory block
   *
   * This block is created by the Analyzer constructor to represent the
   * special property variable. */

  memoryBlock * property_variable_block() const { return _block; }

  /** @name Set operations
   *
   * */

  //{

  /** @brief To top
   *
   * Make the given set into the lattice "TOP" value. For union sets, this is the
   * empty set. For intersection sets, this is the universe set. */

  void to_top(memoryblock_bitset & bits);

  /** @brief To bottom
   *
   * Make the given set into the lattice "BOTTOM" value. For union sets,
   * this is the universe set. For intersection sets, this is the empty
   * set. */

  void to_bottom(memoryblock_bitset & bits);

  /** @brief Meet function
   *
   * Meet the src value with the dest value and store the result in dest. */

  void meet(memoryblock_bitset & dest,
	    memoryblock_bitset & src);

  /** @brief Lookup set value
   *
   * Copy the bitset associated with the given memory access into the input
   * bitset reference. If none is found, provide top. */

  void lookup_set_value(memoryAccess * def_or_use,
			memoryblock_bitset & bits);

  /** @brief Update set value
   *
   * Update the set value associated with the given memory access using the
   * given input bitset. If the def or use is additive, meet the value in
   * with the existing value, otherwise just overwrite it. */

  bool update_set_value(memoryAccess * def_or_use,
			memoryblock_bitset & new_set);

  /** @brief Current set value
   *
   * Construct the "current" value of the set using the current_use() and
   * current_def() fields of the property variable memory block. For
   * forward analysis, look up the value associated with the reaching def
   * of the current use. For backward analysis, meet together all the uses
   * that are reached by the current def.
   *
   * The boolean found_reaching_value is set to false if there is no
   * previous reaching value (i.e., no reaching def in the forward case, or
   * no uses of the def in the backward case. We need this in order to
   * implement the add_elements() method properly. */

  void current_set_value(memoryblock_bitset & bits,
                         bool & found_reaching_value);

  /** @brief Update current set value
   *
   * Update the "current" set value by getting either the current def or
   * current use depending on the direction of analysis. */

  bool update_current_set_value(memoryblock_bitset & new_set);

  /** @brief Is element test
   *
   * Test to see if the given blocks are in the current set value. In order
   * for this to be true all the blocks must be in the set. */

  bool is_element_of(pointerValue & var);

  /** @brief Is empty set test
   *
   * Test to see if the current set value is empty. */

  bool is_emptyset();

  /** @brief Add elements to a set
   *
   * Add the given memory blocks to the set. */

  void add_elements(pointerValue & var);

  /** @brief Merge sets
   *
   * Handle a control-flow merge. */

  /** @brief Merge sets at a phi function
   *
   * */

  void apply_set_merge(memoryuse_list & phi_uses,
                       memoryblock_set & changes);  
  //}

  /** @name Equivalence operations
   *
   * */

  //{

  /** @brief To top
   *
   * Make the given equivalence into the lattive "TOP" value. */

  void to_top(memoryblock_bitset_list & bits);

  /** @brief To bottom
   *
   * Make the given equivalence into the lattice "BOTTOM" value. */

  void to_bottom(memoryblock_bitset_list & bits);

  /** @brief Meet function
   *
   * Meet the src value with the dest value and store the result in dest. */

  void meet(memoryblock_bitset_list & dest,
	    const memoryblock_bitset_list & src);

  /** @brief Lookup equivalence classes
   *
   * Copy the equivalence classes associated with the given memory access
   * into the input bitset_set reference. If none is found, provide top. */

  void lookup_equivalence_classes(memoryAccess * def_or_use,
				  memoryblock_bitset_list & bits);

  /** @brief Update equivalence classes
   *
   * Update the equivalence classes associated with the given memory access using the
   * given input bitset. If the def or use is additive, meet the value in
   * with the existing value, otherwise just overwrite it. */

  bool update_equivalence_classes(memoryAccess * def_or_use,
				  memoryblock_bitset_list & classes);
  
  /** @brief Current equivalence classes
   *
   * Construct the "current" value of the equivalence classes using the
   * current_use() and current_def() fields of the property variable memory
   * block. For forward analysis, look up the value associated with the
   * reaching def of the current use. For backward analysis, meet together
   * all the uses that are reached by the current def. */

  void current_equivalence_classes(memoryblock_bitset_list & bits,
                                   bool & found_reaching_value);

  /** @brief Update current set value
   *
   * Update the "current" value of the equivalence classes by getting
   * either the current def or current use depending on the direction of
   * analysis. */

  bool update_current_equivalence_classes(memoryblock_bitset_list & new_set);

  /** @brief Equivalence test
   *
   * */

  bool is_equivalent(pointerValue & left,
                     pointerValue & right);

  /** @brief Add an equivalence
   *
   * */

  void add_equivalence(pointerValue & left,
                       pointerValue & right);

  /** @brief Merge equivalence classes
   *
   * */

  void apply_equivalence_merge(memoryuse_list & phi_uses,
                               memoryblock_set & changes);

  //}

  /** @brief Apply next
   *
   * Check to see if a call from setPropertyExprAnn::compute_next() caused
   * any changes in this property. If so, put the property variable block
   * into the changes set. */

  void apply_next(memoryblock_set & changes);

  /** @brief Apply merge
   *
   * Just call the appropriate merge operation depending on whether this is
   * a set or an equivalence relation. */

  void apply_merge(memoryuse_list & phi_uses,
                   memoryblock_set & changes);

  /** @brief Self assignment
   *
   * Assignment of set properties only occurs because context insensitive
   * analysis performs assignments of the external inputs and outputs,
   * which are normally not explicit in the program. */

  void self_assignment(memoryblock_set & changes);

  /** @brief Report
   *
   * Just print out the contents of the set or relation. */

  void report(std::ostream & out);
#endif /* __FOO */
  /** @brief Output operator */

  friend std::ostream& operator<<(std::ostream & o, const setPropertyAnn & anns) {
    anns.print(o);
    return o;
  }

  /** @brief Output method */

  void print(std::ostream & o) const;

private:

  /** @brief Build a bit set
   * */

#ifdef __FOO
  void build_memoryblock_bitset(pointerValue & variables,
				memoryblock_bitset & bits);

  /** @brief Add an equivalence class
   *
   * Add a class to an existing equivalence relation, fixing the existing
   * classes to accomodate the new information. */

  void add_equivalence_class(memoryblock_bitset_list & equiv,
                             const memoryblock_bitset & new_class);

  /** @brief Compare equivalences
   *
   * */

  bool is_same_equivalence(memoryblock_bitset_list & equiv1,
                           memoryblock_bitset_list & equiv2);

  /** @brief Print out a set or class */

  void print_memoryblock_bitset(const std::string & label,
				memoryblock_bitset & bits,
				std::ostream & out);
  /** @brief Print out equivalence classes */

  void print_memoryblock_bitset_list(const std::string & label,
                                     memoryblock_bitset_list & bits,
				     std::ostream & out);
#endif /* __FOO */
};

#endif
