
#ifndef BDWY_LOCALVARIABLE_H
#define BDWY_LOCALVARIABLE_H

#include <map>
#include <list>
#include <set>
#include <string>
#include <iostream>

typedef std::map< std::string , annVariable * > var_map;
typedef var_map::iterator var_map_p;
typedef var_map::const_iterator var_map_cp;

typedef std::list< annVariable * > var_list;
typedef var_list::iterator var_list_p;
typedef var_list::const_iterator var_list_cp;

typedef std::set< annVariable * > var_set;
typedef var_set::iterator var_set_p;
typedef var_set::const_iterator var_set_cp;

#ifdef PROCLOCATION
typedef std::map< procLocation *, pointerValue > variable_binding_map;
typedef variable_binding_map::iterator variable_binding_map_p;
#endif

/** @brief Local variable
 *
 *  This class represents a local variable in the procedure annotations. A
 *  local variable has a name and a binding that is used to associate the
 *  actual memory objects assigned to it. */

class annVariable
{
private:

  /** @brief the variable name */

  std::string _name;

  /** @brief bindings
   *
   *  This field maps each context of use of this variable to it's actual
   *  pointer value in that context. We store all the contexts (and not
   *  just the current bindings) for two reasons. First, we use this data
   *  structure to store the memoryBlocks that represent heap allocations
   *  (uses of "new" in the on_exit annotations). Second, we use these
   *  contexts during other phases of the compiler, such as reports and
   *  actions. */

  // TB pointerValue _bindings;

  /** @brief Test
   *
   */

#ifdef PROCLOCATION
  variable_binding_map _bindings /* TB _all_bindings */ ;
#endif

  /** @brief Synthetic declaration node */

#ifdef DECLNODE 
  declNode * _decl;
#endif
  /** @name Flags
   */

  //@{

  bool _is_new;
  bool _is_used;
  bool _is_defined;
  bool _is_global;
  bool _is_external;
  bool _is_formal;
  bool _is_io;
  bool _is_deleted;
  bool _is_property_variable;

  //@}

public:

  /** @brief Create a new annotation variable
   *
   * The is_external boolean determines whether or not to create a
   * synthetic declNode for this variable. For external variables, we'll
   * get the real declNode from the header files. */

  annVariable(std::string name, 
              std::string procedure_name,
              bool is_new,
              bool is_global,
              bool is_external,
              bool is_formal,
              bool is_io);
  
  /** @brief Destructor
   *
   * Note: we have to be careful here because we want to delete the
   * synthetic declarations, but not the real ones (see the constructor
   * above). */

  ~annVariable();

  /** @brief Clear
   *
   * Clear all the bindings. */

#ifdef PROCLOCATION
  void clear(procLocation *where);
  void clear_all() { _bindings.clear(); }
#endif

  // Fields access

  std::string & name() { return _name; }

#ifdef DECLNODE 
  inline declNode * decl() const { return _decl; }
#endif
  inline bool is_new() const { return _is_new; }
  inline bool is_used() const { return _is_used; }
  inline bool is_defined() const { return _is_defined; }
  inline bool is_global() const { return _is_global; }
  inline bool is_external() const { return _is_external; }
  inline bool is_formal() const { return _is_formal; }
  inline bool is_io() const { return _is_io; }
  inline bool is_deleted() const { return _is_deleted; }
  inline bool is_property_variable() const { return _is_property_variable; }

  inline void set_used() { _is_used = true; }
  inline void set_defined() { _is_defined = true; }
  inline void set_global()
  {
    _is_global = true;
#ifdef DECLNODE 
    _decl->decl_location(declNode::TOP);
#endif
  }

  inline void set_external() { _is_external = true; }

  inline void set_formal()
  {
    _is_formal = true;
#ifdef DECLNODE 
    _decl->decl_location(declNode::FORMAL);
#endif
  }

  inline void set_io() { _is_io = true; }
  inline void set_deleted() { _is_deleted = true; }
  inline void set_property_variable() { _is_property_variable = true; }

  /** @brief Get the bindings
   *
   * Return the set of memoryBlocks bound to the variable in the given
   * calling context. For globals, there is only one binding (this is taken
   * care of internally). */

  // TB add argument <where>
#ifdef PROCLOCATION
  pointerValue & get_binding(procLocation *where);
#endif

  /** @brief Get a fresh binding
   *
   * This is used by the Analyzer when it is setting up the new bindings at
   * a call site. It first clears the existing pointerValue then returns
   * it. */

#ifdef PROCLOCATION
  pointerValue & fresh_binding(procLocation *where)
  {
    clear(where);
    return get_binding(where);
  }

  /** @brief Set binding
   *
   * Set the binding to be a specific memoryBlock. This overwrites any
   * existing bindings. */

  void set_binding(memoryBlock * block, procLocation *where)
  {
    // clear();
    _bindings[where].blocks.clear();
    _bindings[where].blocks.insert(block);
  }

  /** @brief Save bindings */

  //TB void save_bindings(procLocation * where) { _all_bindings[where] = _bindings; }
#endif
  /** @brief Create bindings
   *
   * Set up a binding for the given calling context and initialize it with
   * the given memoryBlock. */

  // void create_binding(memoryBlock * mb) { 

  /** @brief Check for a binding
   *
   * See if the variable has a binding in the given calling context. */

    // bool has_binding(procLocation * loc = 0);

  // --- Output

  friend std::ostream& operator<<(std::ostream & o, const annVariable & var) {
    var.print(o);
    return o;
  }

  void print(std::ostream & o) const;
};

#endif /*  */
