
#include "broadway.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

// ----------------------------------------------------------------------
//  Set property
// ----------------------------------------------------------------------

setPropertyAnn::setPropertyAnn(const parserID * id,
                               Direction direction,
                               SetPropertyKind kind,
                               MeetFunction meet)
  : propertyAnn(id, direction, SetProperty),
    _kind(kind),
    _meet_function(meet),
    _property_variable(0)
#ifdef __FOO
,
    _block(0),
    _set(),
    _equivalence(),
    _position_of_memoryblock(),
    _memoryblock_at_position(),
    _change(false)
#endif
{
  if (kind == Set)
    _name = id->name(); // string("Set:") + id->name();

  if (kind == Equivalence)
    _name = id->name(); // string("Equiv:") + id->name();
}

#ifdef __FOO

/** @brief Clear the analysis information
 *
 * Call this method to re-initialize the analysis information so that the
 * property analyzer can be run again. */

void setPropertyAnn::clear()
{
  _set.clear();
  _equivalence.clear();
  _position_of_memoryblock.clear();
  _memoryblock_at_position.clear();
}

// ----------------------------------------------------------------------
//  Set operations
// ----------------------------------------------------------------------

/** @brief To top
 *
 * Make the given set into the lattice "TOP" value. For union sets, this is the
 * empty set. For intersection sets, this is the universe set. */

void setPropertyAnn::to_top(memoryblock_bitset & bits)
{
  if (_meet_function == Union)
    bits.reset();

  if (_meet_function == Intersect)
    bits.set();
}

/** @brief To bottom
 *
 * Make the given set into the lattice "BOTTOM" value. For union sets,
 * this is the universe set. For intersection sets, this is the empty
 * set. */

void setPropertyAnn::to_bottom(memoryblock_bitset & bits)
{
  if (_meet_function == Union)
    bits.set();

  if (_meet_function == Intersect)
    bits.reset();
}

/** @brief Meet function
 *
 * Meet the src value with the dest value and store the result in dest. */

void setPropertyAnn::meet(memoryblock_bitset & dest,
                          memoryblock_bitset & src)
{
  if (_meet_function == Union)
    dest |= src;

  if (_meet_function == Intersect)
    dest &= src;
}

/** @brief Lookup set value */

void setPropertyAnn::lookup_set_value(memoryAccess * def_or_use,
                                      memoryblock_bitset & bits)
{
  set_property_map_p p = _set.find(def_or_use);

  if (p == _set.end())
    to_top(bits);
  else
    bits = (*p).second;
}

/** @brief Update current set value */

bool setPropertyAnn::update_set_value(memoryAccess * def_or_use,
                                      memoryblock_bitset & new_set)
{
  if (Annotations::Verbose_properties) {
    cout << "      + Update " << name() << endl;
    print_memoryblock_bitset("        new = ", new_set, cout);
  }

  // -- Get the old value

  memoryblock_bitset old_set;
  lookup_set_value(def_or_use, old_set);

  // -- Handle additive defs
  //if (def_or_use->is_additive())

  meet(new_set, old_set);

  // -- Did anything change?

  bool change = false;

  if (old_set != new_set) {

    change = true;

    // -- Store the new set

    _set[def_or_use] = new_set;
  }

  if (Annotations::Verbose_properties) {
    if (change)
      cout << "       -- changed" << endl;
    else
      cout << "       -- unchanged" << endl;

    print_memoryblock_bitset("        = ", new_set, cout);
    print_memoryblock_bitset("        old = ", old_set, cout);
  }

  return change;
}

void setPropertyAnn::current_set_value(memoryblock_bitset & bits,
                                       bool & found_reaching_value)
{
  if (Annotations::Verbose_properties)
    cout << "        - Lookup value of " << name() << endl;

  // -- Assume that we'll find a reaching value

  found_reaching_value = true;

  // -- Get the property variable memory block

  memoryBlock * block = property_variable_block();

  // -- Forward or backward

  if (direction() == Forward) {

    // -- Forward analysis: the now value is constructed by getting the
    // current use and looking up the reaching definition.

    if (Annotations::Verbose_properties)
      cout << "          use at ";

    memoryUse * use = block->current_use();
    if (use) {
      memoryDef * def = use->reaching_def();

      if (Annotations::Verbose_properties) {
        cout << * (use->where()) << endl;
        cout << "          reaching def at ";
      }

      // -- If there is a reaching def, look up the value associated with
      // it.

      if (def) {

        lookup_set_value(def, bits);

        if (Annotations::Verbose_properties) {
          cout << * (def->where()) << endl;
          print_memoryblock_bitset("        = ", bits, cout);
        }
      }
      else {

        // -- No reaching def; set the flag and set the value to TOP

        found_reaching_value = false;

        to_top(bits);

        if (Annotations::Verbose_properties)
          cout << "(no reaching def)" << endl;
      }
    }
    else {

      // -- This is bad: the variable has no use here (i.e., it is
      // probably missing an access annotation).

      if (Annotations::Verbose_properties)
        cout << " NO USE " << endl;

      to_bottom(bits);
    }
  }
  else {

    // -- Backward analysis: the now value is constructed by getting
    // the current definition, and meeting together the values from all
    // of the uses that it reaches.

    if (Annotations::Verbose_properties)
      cout << "          def at ";

    memoryDef * current_def = block->current_def();
    if (current_def) {

      if (Annotations::Verbose_properties) {
        cout << * (current_def->where()) << endl;
        cout << "          uses reached:" << endl;
      }

      memoryuse_list uses;
      block->def_uses(current_def, uses);

      if (uses.empty()) {

        // -- Special case: if the def has no uses (this can happen with
        // merge points), then return the default value. Also, set the
        // flag.

        found_reaching_value = false;

        to_top(bits);

        if (Annotations::Verbose_properties)
          cout << "              (no uses reached)" << endl;
      }
      else {

        to_top(bits);

        // -- Otherwise, visit all the uses reached by the def

        memoryblock_bitset temp;

        for (memoryuse_list_p u = uses.begin();
             u != uses.end();
             ++u)
          {
            memoryUse * use = *u;

            lookup_set_value(use, temp);
            meet(bits, temp);
            if (Annotations::Verbose_properties)
              cout << "              Use at " << * (use->where()) << endl;
          }

        if (Annotations::Verbose_properties)
          print_memoryblock_bitset("        = ", bits, cout);
      }
    }
    else {

      // -- This is bad: the variable has no def here (i.e., it is
      // probably missing a modifies annotation).

      if (Annotations::Verbose_properties)
        cout << " NO DEF " << endl;

      to_bottom(bits);
    }
  }
}

/** @brief Update current set value
 *
 * Update the "current" set value by getting either the current def or
 * current use depending on the direction of analysis. */

bool setPropertyAnn::update_current_set_value(memoryblock_bitset & new_set)
{
  // -- Get the property variable object

  memoryBlock * block = property_variable_block();

  // -- Assignments are either attached to the current def (in the case of
  // forward analysis) or the current use (in the case of backward
  // analysis).

  memoryAccess * assign_to;

  if (direction() == Forward)
    assign_to = block->current_def();
  else
    assign_to = block->current_use();

  // -- Update the value

  return update_set_value(assign_to, new_set);
}


/** @brief Is element test
 *
 * */

bool setPropertyAnn::is_element_of(pointerValue & var)
{
  bool result = false;

  // -- Build a bitset for the input variables

  memoryblock_bitset var_bits;
  build_memoryblock_bitset(var, var_bits);

  // -- Look up the current value of the set

  memoryblock_bitset current_bits;
  bool found_reaching_value;

  current_set_value(current_bits,
                    found_reaching_value);

  // -- Perform subset test

  if ((var_bits & current_bits) == var_bits)
    result = true;

  return result;
}

/** @brief Is empty set test
 *
 * */

bool setPropertyAnn::is_emptyset()
{
  bool result = false;

  // -- Look up the current value of the set

  memoryblock_bitset current_bits;
  bool found_reaching_value;

  current_set_value(current_bits,
                    found_reaching_value);

  // -- Test for all zeros

  if (current_bits.none())
    result = true;

  return result;
}

/** @brief Add elements to a set
 *
 * */

void setPropertyAnn::add_elements(pointerValue & var)
{
  // -- Build a bitset for the input variables

  memoryblock_bitset var_bits;
  build_memoryblock_bitset(var, var_bits);

  // -- Get the current value

  memoryblock_bitset current_bits;
  bool found_reaching_value;

  current_set_value(current_bits,
                    found_reaching_value);

  if (Annotations::Verbose_properties) {
    cout << "  - Add elements to " << name() << endl;
    print_memoryblock_bitset("    + Add ", var_bits, cout);
    print_memoryblock_bitset("      To  ", current_bits, cout);
  }

  // -- Add the bits for the input variables. Special case: if there was no
  // previous reaching value then there is no need to combine with new set
  // with the old set.

  if (found_reaching_value)
    var_bits |= current_bits;

  if (Annotations::Verbose_properties)
    print_memoryblock_bitset("    = ", var_bits, cout);

  // -- Store the current value

  _change = update_current_set_value(var_bits);
}

/** @brief Apply merge
 *
 * */

void setPropertyAnn::apply_set_merge(memoryuse_list & phi_uses,
                                     memoryblock_set & changes)
{
  // -- Some temporaries

  memoryblock_bitset oldval;
  memoryblock_bitset merged_val;
  memoryblock_bitset reaching_val;

  to_top(merged_val);

  // -- Get the property variable block

  memoryBlock * block = property_variable_block();

  // -- Forwards or backwards

  if (direction() == Forward) {

    // -- Forward analysis: collect the values that reach the merge uses,
    // meet them together and propagate that value to the def.

    // -- Get the old value

    memoryDef * def = block->current_def();

    lookup_set_value(def, oldval);

    // -- Merge together the uses that reach this merge point

    for (memoryuse_list_p p = phi_uses.begin();
         p != phi_uses.end();
         ++p)
      {
        memoryUse * phi_use = *p;
        memoryDef * reaching_def = phi_use->reaching_def();

        // -- Find the reaching value, handling the default case as well

        if (reaching_def) {

          // -- There is a reaching def, look it up

          lookup_set_value(reaching_def, reaching_val);

          if (Annotations::Verbose_properties)
            cout << "   at " << * (reaching_def->where()) << endl;
        }
        else {

          // -- No reaching def, use the default value

          to_top(reaching_val);

          if (Annotations::Verbose_properties)
            cout << "   = top " << endl;
        }

        meet(merged_val, reaching_val);
      }

    // -- Did anything change?

    bool changed = update_set_value(def, merged_val);
    if (changed) {

      changes.insert(block);

      if (Annotations::Verbose_properties)
        cout << "   -> merge changed " << name() << endl;
    }
  }
  else {

    // -- Backward analysis: get the value of the definition and just copy
    // it to all the merge uses.

    bool found_reaching_value;

    current_set_value(merged_val,
                      found_reaching_value);

    for (memoryuse_list_p p = phi_uses.begin();
         p != phi_uses.end();
         ++p)
      {
        memoryUse * phi_use = *p;

        // -- Update the value and check for changes

        bool changed = update_set_value(phi_use, merged_val);
        if (changed && phi_use->reaching_def()) {

          changes.insert(block);

          if (Annotations::Verbose_properties)
            cout << "   -> merge changed " << name() << endl;
        }
      }
  }
}

// ----------------------------------------------------------------------
//  Equivalence relation operations
// ----------------------------------------------------------------------

/** @brief To top
 *
 * Make the given equivalence into the lattive "TOP" value. */

void setPropertyAnn::to_top(memoryblock_bitset_list & bits)
{
  bits.clear();
}

/** @brief To bottom
 *
 * Make the given equivalence into the lattice "BOTTOM" value. */

void setPropertyAnn::to_bottom(memoryblock_bitset_list & bits)
{
  bits.clear();
}

/** @brief Meet function
 *
 * Meet the src value with the dest value and store the result in dest. */

void setPropertyAnn::meet(memoryblock_bitset_list & dest,
                          const memoryblock_bitset_list & src)
{
  // -- Visit all the classes in the src and add them to the dest

  for (memoryblock_bitset_list_cp p = src.begin();
       p != src.end();
       ++p)
    {
      add_equivalence_class(dest, *p);
    }
}

/** @brief Lookup equivalence classes
 *
 * Copy the equivalence classes associated with the given memory access
 * into the input bitset_set reference. If none is found, provide top. */

void setPropertyAnn::lookup_equivalence_classes(memoryAccess * def_or_use,
                                                memoryblock_bitset_list & bits)
{
  equiv_property_map_p p = _equivalence.find(def_or_use);

  if (p == _equivalence.end())
    to_top(bits);
  else
    bits = (*p).second;
}

/** @brief Update equivalence classes
 *
 * Update the equivalence classes associated with the given memory access
 * using the given input bitset. If the def or use is additive, meet the
 * value in with the existing value, otherwise just overwrite it. */

bool setPropertyAnn::update_equivalence_classes(memoryAccess * def_or_use,
                                                memoryblock_bitset_list & new_set)
{
  if (Annotations::Verbose_properties) {
    cout << "      + Update " << name() << endl;
    print_memoryblock_bitset_list("        new = ", new_set, cout);
  }

  // -- Get the old value

  memoryblock_bitset_list old_set;
  lookup_equivalence_classes(def_or_use, old_set);

  // -- Handle additive defs

  //  if (def_or_use->is_additive())
  meet(new_set, old_set);

  // -- Did anything change?

  bool change = false;

  if ( ! is_same_equivalence(old_set, new_set)) {

    change = true;

    // -- Store the new set

    _equivalence[def_or_use] = new_set;
  }

  if (Annotations::Verbose_properties) {
    if (change)
      cout << "       -- changed" << endl;
    else
      cout << "       -- unchanged" << endl;

    print_memoryblock_bitset_list("        = ", new_set, cout);
    print_memoryblock_bitset_list("        old = ", old_set, cout);
  }

  return change;
}

/** @brief Current equivalence classes
 *
 * Construct the "current" value of the equivalence classes using the
 * current_use() and current_def() fields of the property variable memory
 * block. For forward analysis, look up the value associated with the
 * reaching def of the current use. For backward analysis, meet together
 * all the uses that are reached by the current def. */

void setPropertyAnn::current_equivalence_classes(memoryblock_bitset_list & bits,
                                                 bool & found_reaching_value)
{
  if (Annotations::Verbose_properties)
    cout << "        - Lookup value of " << name() << endl;

  // -- Assume that we'll find a reaching value

  found_reaching_value = true;

  // -- Get the property variable memory block

  memoryBlock * block = property_variable_block();

  // -- Forward or backward

  if (direction() == Forward) {

    // -- Forward analysis: the now value is constructed by getting the
    // current use and looking up the reaching definition.

    if (Annotations::Verbose_properties)
      cout << "          use at ";

    memoryUse * use = block->current_use();
    if (use) {
      memoryDef * def = use->reaching_def();

      if (Annotations::Verbose_properties) {
        cout << * (use->where()) << endl;
        cout << "          reaching def at ";
      }

      // -- If there is a reaching def, look up the value associated with
      // it.

      if (def) {

        lookup_equivalence_classes(def, bits);

        if (Annotations::Verbose_properties) {
          cout << * (def->where()) << endl;
          print_memoryblock_bitset_list("        = ", bits, cout);
        }
      }
      else {

        // -- No reaching def; set the flag and set the value to TOP

        found_reaching_value = false;

        to_top(bits);

        if (Annotations::Verbose_properties)
          cout << "(no reaching def)" << endl;
      }
    }
    else {

      // -- This is bad: the variable has no use here (i.e., it is
      // probably missing an access annotation).

      if (Annotations::Verbose_properties)
        cout << " NO USE " << endl;

      to_bottom(bits);
    }
  }
  else {

    // -- Backward analysis: the now value is constructed by getting
    // the current definition, and meeting together the values from all
    // of the uses that it reaches.

    if (Annotations::Verbose_properties)
      cout << "          def at ";

    memoryDef * current_def = block->current_def();
    if (current_def) {

      if (Annotations::Verbose_properties) {
        cout << * (current_def->where()) << endl;
        cout << "          uses reached:" << endl;
      }

      memoryuse_list uses;
      block->def_uses(current_def, uses);

      if (uses.empty()) {

        // -- Special case: if the def has no uses (this can happen with
        // merge points), then return the default value. Also, set the
        // flag.

        found_reaching_value = false;

        to_top(bits);

        if (Annotations::Verbose_properties)
          cout << "              (no uses reached)" << endl;
      }
      else {

        to_top(bits);

        // -- Otherwise, visit all the uses reached by the def

        memoryblock_bitset_list temp;

        for (memoryuse_list_p u = uses.begin();
             u != uses.end();
             ++u)
          {
            memoryUse * use = *u;

            lookup_equivalence_classes(use, temp);
            meet(bits, temp);
            if (Annotations::Verbose_properties)
              cout << "              Use at " << * (use->where()) << endl;
          }

        if (Annotations::Verbose_properties)
          print_memoryblock_bitset_list("        = ", bits, cout);
      }
    }
    else {

      // -- This is bad: the variable has no def here (i.e., it is
      // probably missing a modifies annotation).

      if (Annotations::Verbose_properties)
        cout << " NO DEF " << endl;

      to_bottom(bits);
    }
  }
}

/** @brief Update current set value
 *
 * Update the "current" value of the equivalence classes by getting
 * either the current def or current use depending on the direction of
 * analysis. */

bool setPropertyAnn::update_current_equivalence_classes(memoryblock_bitset_list & new_classes)
{
  // -- Get the property variable object

  memoryBlock * block = property_variable_block();

  // -- Assignments are either attached to the current def (in the case of
  // forward analysis) or the current use (in the case of backward
  // analysis).

  memoryAccess * assign_to;

  if (direction() == Forward)
    assign_to = block->current_def();
  else
    assign_to = block->current_use();

  // -- Update the value

  return update_equivalence_classes(assign_to, new_classes);
}

/** @brief Equivalence test
 *
 * */

bool setPropertyAnn::is_equivalent(pointerValue & left,
                                   pointerValue & right)
{
  // -- Get the current list of equivalence classes

  memoryblock_bitset_list equiv;
  bool found_reaching_value;

  current_equivalence_classes(equiv, found_reaching_value);

  // -- Build a bitset for the input variables. This seems like a
  // reasonable optimization: just build a single bitset with both the left
  // and right variables in it.

  memoryblock_bitset var_bits;
  build_memoryblock_bitset(left, var_bits);
  build_memoryblock_bitset(right, var_bits);

  // -- Visit each class to see if there is one that contains all of the variables

  for (memoryblock_bitset_list_p p = equiv.begin();
       p != equiv.end();
       ++p)
    {
      const memoryblock_bitset & current_bits = (*p);

      if ((var_bits & current_bits) == var_bits)
        return true;
    }

  return false;
}

/** @brief Add an equivalence
 *
 * */

void setPropertyAnn::add_equivalence(pointerValue & left,
                                     pointerValue & right)
{
  // -- Get the current list of equivalence classes

  memoryblock_bitset_list equiv;
  bool found_reaching_value;

  current_equivalence_classes(equiv, found_reaching_value);

  // -- If there is no reaching value, start fresh

  if (! found_reaching_value)
    equiv.clear();

  // -- Build a bitset for the input variables. The way to do this is to
  // put all the variables into a single set.

  memoryblock_bitset var_bits;
  build_memoryblock_bitset(left, var_bits);
  build_memoryblock_bitset(right, var_bits);

  if (Annotations::Verbose_properties) {
    cout << "  - Add equivalences to " << name() << endl;
    print_memoryblock_bitset("    + Add ", var_bits, cout);
    print_memoryblock_bitset_list("      To  ", equiv, cout);
  }

  // -- Add the new class into the equivalence

  add_equivalence_class(equiv, var_bits);

  // -- Record the update

  if (Annotations::Verbose_properties)
    print_memoryblock_bitset_list("    = ", equiv, cout);

  _change = update_current_equivalence_classes(equiv);
}

/** @brief Merge equivalence classes
 *
 * */

void setPropertyAnn::apply_equivalence_merge(memoryuse_list & phi_uses,
                                             memoryblock_set & changes)
{
  // -- Some temporaries

  memoryblock_bitset_list oldval;
  memoryblock_bitset_list merged_val;
  memoryblock_bitset_list reaching_val;

  to_top(merged_val);

  // -- Get the property variable block

  memoryBlock * block = property_variable_block();

  // -- Forwards or backwards

  if (direction() == Forward) {

    // -- Forward analysis: collect the values that reach the merge uses,
    // meet them together and propagate that value to the def.

    // -- Get the old value

    memoryDef * def = block->current_def();

    lookup_equivalence_classes(def, oldval);

    // -- Merge together the uses that reach this merge point

    for (memoryuse_list_p p = phi_uses.begin();
         p != phi_uses.end();
         ++p)
      {
        memoryUse * phi_use = *p;
        memoryDef * reaching_def = phi_use->reaching_def();

        // -- Find the reaching value, handling the default case as well

        if (reaching_def) {

          // -- There is a reaching def, look it up

          lookup_equivalence_classes(reaching_def, reaching_val);

          if (Annotations::Verbose_properties)
            cout << "   at " << * (reaching_def->where()) << endl;
        }
        else {

          // -- No reaching def, use the default value

          to_top(reaching_val);

          if (Annotations::Verbose_properties)
            cout << "   = top " << endl;
        }

        meet(merged_val, reaching_val);
      }

    // -- Did anything change?

    bool changed = update_equivalence_classes(def, merged_val);
    if (changed) {

      changes.insert(block);

      if (Annotations::Verbose_properties)
        cout << "   -> merge changed " << name() << endl;
    }
  }
  else {

    // -- Backward analysis: get the value of the definition and just copy
    // it to all the merge uses.

    bool found_reaching_value;

    current_equivalence_classes(merged_val,
                                found_reaching_value);

    for (memoryuse_list_p p = phi_uses.begin();
         p != phi_uses.end();
         ++p)
      {
        memoryUse * phi_use = *p;

        // -- Update the value and check for changes

        bool changed = update_equivalence_classes(phi_use, merged_val);
        if (changed && phi_use->reaching_def()) {

          changes.insert(block);

          if (Annotations::Verbose_properties)
            cout << "   -> merge changed " << name() << endl;
        }
      }
  }
}


// ----------------------------------------------------------------------
//  Other operations
// ----------------------------------------------------------------------

/** @brief Apply next
 *
 * Check to see if a call from setPropertyExprAnn::compute_next() caused
 * any changes in this property. If so, put the property variable block
 * into the changes set. */

void setPropertyAnn::apply_next(memoryblock_set & changes)
{
  // -- If anything changed...

  if (_change) {

    // -- Reset the flag

    _change = false;

    // -- Put the property variable block in the change set

    changes.insert(property_variable_block());
  }
}

/** @brief Apply merge
 *
 * */

void setPropertyAnn::apply_merge(memoryuse_list & phi_uses,
                                 memoryblock_set & changes)
{
  if (Annotations::Verbose_properties)
    cout << "  + Merge " << name() << endl;

  if (kind() == Set)
    apply_set_merge(phi_uses, changes);

  if (kind() == Equivalence)
    apply_equivalence_merge(phi_uses, changes);
}

/** @brief Self assignment
 *
 * Assignment of set properties only occurs because context insensitive
 * analysis performs assignments of the external inputs and outputs,
 * which are normally not explicit in the program. */

void setPropertyAnn::self_assignment(memoryblock_set & changes)
{
  bool change = false;

  if (Annotations::Verbose_properties)
    cout << "  + Pass " << name() << endl;

  if (kind() == Set) {

    // -- Get the current reaching value

    memoryblock_bitset value;
    bool found_reaching_value;

    current_set_value(value,
                      found_reaching_value);

    // -- Assign it to itself

    change = update_current_set_value(value);
  }

  if (kind() == Equivalence) {

    // -- Get the current reaching value

    memoryblock_bitset_list value;
    bool found_reaching_value;

    current_equivalence_classes(value,
                                found_reaching_value);

    // -- Assign it to itself

    change = update_current_equivalence_classes(value);
  }

  // -- Record any change

  if (change)
    changes.insert(property_variable_block());
}

/** @brief Report
 *
 * Just print out the contents of the set or relation. */

void setPropertyAnn::report(ostream & out)
{

}

/** @brief Build a bit set
 * */

void setPropertyAnn::build_memoryblock_bitset(pointerValue & variables,
                                              memoryblock_bitset & bits)
{
  // -- For each variable

  for (memoryblock_set_p p = variables.blocks.begin();
       p != variables.blocks.end();
       ++p)
    {
      memoryBlock * block = (*p);

      // -- Find it's bit position, or allocate a new position if necessary

      int position = 0;

      memoryblock_position_map_p q = _position_of_memoryblock.find(block);

      if (q == _position_of_memoryblock.end()) {

        // -- Use the current size of the vector to tell us the next
        // available bit position.

        position = _memoryblock_at_position.size();

        // -- This sucks, but it has to be done...

        if (position == 1024) {

          // -- Representation is full, so we're in serious trouble

          cerr << "INTERNAL ERROR: Too many objects in the bitset representation of " << name() << endl;
        }
        else {

          // -- Add the block to the end vector

          _memoryblock_at_position.push_back(block);

          // -- Store the information in the map

          _position_of_memoryblock[block] = position;

          // cout << "Property " << name() << ": position of " << block->name() << " = " << position << endl;
        }
      }
      else
        position = (*q).second;

      // -- Set the bit

      bits.set(position);

    }
}

/** @brief Add an equivalence class
 *
 * Add a class to an existing equivalence relation, fixing the existing
 * classes to accomodate the new information. */

void setPropertyAnn::add_equivalence_class(memoryblock_bitset_list & equiv,
                                           const memoryblock_bitset & new_class)
{
  // -- Algorithm:
  //
  // (0) If the class is already there, just return
  //
  // (1) For each exisiting class, check to see if it has a non-empty
  // intersection with the new class.
  //
  // (2) If it does, remove the existing class and save it.
  //
  // (3) When we're done, union together all the resulting saved classes,
  // along with the new class.
  //
  // (4) Insert the result back into the equivalence

  // -- (0) Existing class, just return

  /*
  memoryblock_bitset_list_cp test = equiv.find(new_class);
  if (test != equiv.end())
    return;
  */

  // -- (1) Collect the intersecting classes

  memoryblock_bitset merged_classes;
  memoryblock_bitset intersection;

  memoryblock_bitset_list_p p = equiv.begin();
  memoryblock_bitset_list_p temp;

  while (p != equiv.end()) {

    const memoryblock_bitset & current = *p;

    // -- Test the intersection

    intersection = current & new_class;

    if (intersection.any()) {

      // -- (2) Intersection is non-empty

      merged_classes |= current;

      temp = p;
      ++p;

      equiv.erase(temp);
    }
    else
      ++p;
  }

  // -- (3) Add in the new class

  merged_classes |= new_class;

  // -- Add this back in to the equivalence relation

  equiv.push_back(merged_classes);
}

/** @brief Compare equivalences
 *
 * */

bool setPropertyAnn::is_same_equivalence(memoryblock_bitset_list & equiv1,
                                         memoryblock_bitset_list & equiv2)
{
  // -- Quick out

  if (equiv1.size() != equiv2.size())
    return false;

  // -- Pair-wise compare. Ug.

  for (memoryblock_bitset_list_p p = equiv1.begin();
       p != equiv1.end();
       ++p)
    {
      memoryblock_bitset & look_for = *p;

      bool found = false;

      for (memoryblock_bitset_list_p q = equiv2.begin();
           q != equiv2.end();
           ++q)
        {
          memoryblock_bitset & current = *q;

          if (current == look_for) {
            found = true;
            break;
          }
        }

      if ( ! found )
        return false;
    }

  return true;
}

#endif /* __FOO */

/** @brief Output method */

void setPropertyAnn::print(ostream & o) const
{
  o << name() << endl;
}

#ifdef __FOO

/** @brief Print out a set or class */

void setPropertyAnn::print_memoryblock_bitset(const string & label,
                                              memoryblock_bitset & bits,
                                              ostream & out)
{
  int size = _memoryblock_at_position.size();

  out << label << '{';

  for (int i = 0; i < size; i++)
    if (bits[i]) {
      memoryBlock * block = _memoryblock_at_position[i];

      out << block->name() << " ";
    }

  out << '}' << endl;
}

/** @brief Print out equivalence classes */

void setPropertyAnn::print_memoryblock_bitset_list(const string & label,
                                                  memoryblock_bitset_list & bits,
                                                  ostream & out)
{
  int size = _memoryblock_at_position.size();

  out << label << "{ ";

  for (memoryblock_bitset_list_p p = bits.begin();
       p != bits.end();
       ++p)
    {
      const memoryblock_bitset & one = *p;

      out << "{";

      for (int i = 0; i < size; i++)
        if (one[i]) {
          memoryBlock * block = _memoryblock_at_position[i];

          out << block->name() << " ";
        }
      out << "} ";
    }

  out << '}' << endl;
}

#endif /* __FOO */
