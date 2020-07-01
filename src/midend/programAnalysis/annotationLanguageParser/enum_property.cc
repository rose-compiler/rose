
#include "broadway.h"
#ifdef __FOO
#include "diagnostic.h"
#endif

#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

// ---------------------------------------------------------------------
//  Property value
//  Element of the lattice
// ---------------------------------------------------------------------

enumValueAnn::enumValueAnn(const parserID * id,
                           enumvalue_list * more_specific)
  : Ann(id->line()),
    _name(id->name()),
    _more_general(0),
    _more_specific(),
    _id(0),
    _is_leaf(false)
{
  if (more_specific) {
    _more_specific.swap(* more_specific);

    for (enumvalue_list_p p = _more_specific.begin();
         p != _more_specific.end();
         ++p)
      (*p)->_more_general = this;
  }
  else
    _is_leaf = true;
}

/** @brief Collect all more specific values
 *
 * Collect all the values that are more specific that this one, excluding
 * top.*/

void enumValueAnn::collect_atleast_values(enumvalue_set & collect)
{
  // -- Skip top

  if (_more_specific.empty())
    return;

  // -- Insert the current value

  collect.insert(this);

  // -- Visit the values above

  for (enumvalue_list_p p = _more_specific.begin();
         p != _more_specific.end();
         ++p)
    (*p)->collect_atleast_values(collect);
}


// ---------------------------------------------------------------------
//  Analysis property
// ---------------------------------------------------------------------

enumPropertyAnn::enumPropertyAnn(const parserID * id,
                                 Direction direction,
                                 bool is_may_property,
                                 const parserID * default_name,
                                 enumvalue_list * lowest_values,
                                 parserid_list * diagnostic_values)
  : propertyAnn(id, direction, EnumProperty),
    _is_may_property(is_may_property),
    _enforce_weak_updates(true),
    _values(0),
    _top(0),
    _value_index()
#ifdef __FOO
  ,
    _now_values(),
    _ever_values(),
    _flow_sensitive_objects(),
    _tested_objects(),
    _accuracy(),
    _error_statements(),
    _number_of_flowvalues(0),
    _error_procedures(),
    _diagnostic(NULL)
#endif
{
  // -- Create a special "bottom" element from which all others are
  // reachable

  parserID temp("(bottom)", id->line());
  _values = new enumValueAnn(&temp, lowest_values);

  // -- Create a special "top" element

  parserID temp2("(top)", id->line());
  _top = new enumValueAnn(&temp2, 0);

  // -- Give "bottom" the lowest ID

  _values->id(0);
  _value_index.push_back(_values);

  // -- Assign a unique ID and a height in the lattice to each element.

  int cur_id = 1;
  int max_height = 0;
  number_values(_values, cur_id, 0, max_height);

  // -- Give "top" the highest ID and a height greater than all other
  // elements.

  _top->id(cur_id);
  _top->height(max_height+1);
  _value_index.push_back(_top);

  _number_of_flowvalues = (double) (_value_index.size() - 3);

  // -- If there is a default value, then look it up. Otherwise, the
  // default is top.

  if (default_name)
    _default = lookup(default_name->name());
  else
    _default = _top;

// begin TB new

#ifdef __DIAGNOSTIC
  // process the declared diagnostic values.
  if(Annotations::Diagnostic && diagnostic_values &&
     ! diagnostic_values->empty()) {
    _diagnostic = new Diagnostic(this);
    for(parserid_list_p d=diagnostic_values->begin(); d!=diagnostic_values->end(); d++) {
      enumValueAnn *v = lookup(d->name());
      if(!v) {
        cout << "Error: unknown property value " << d->name() << " at line " << d->line() << endl;
        exit(1);
      }
      _diagnostic->add_diagnostic_value(v);
    }
  }
#endif
// end TB new
}

void enumPropertyAnn::number_values(enumValueAnn * prop,
                                    int & cur_index, int height, int & max_height)
{
  // -- Set the height and index fields for the current property value

  prop->id(cur_index);
  prop->height(height);

  // -- Keep track of the maximum height

  if (height > max_height)
    max_height = height;

  // -- Put it on the index list in the right place

  _value_index.push_back(prop);

  // -- Increment the index counter

  cur_index++;

  enumvalue_list & more_specific = prop->more_specific();

  if (more_specific.empty()) {

    // If there are no more elements above this one, then insert "top"

    more_specific.push_back(_top);
  }
  else {

  // -- Visit the more specific values, increasing the height by one

    for (enumvalue_list_p p = more_specific.begin();
         p != more_specific.end();
         ++p)
      number_values(*p, cur_index, height+1, max_height);
  }
}

#ifdef __FOO

void enumPropertyAnn::clear()
{
  // -- Clear all of the store values

  _now_values.clear();
  _ever_values.clear();
  _input_to_values.clear();
}

#endif

enumValueAnn * enumPropertyAnn::lookup(const string & name)
{
  for (enumvalue_vec_p p = _value_index.begin();
       p != _value_index.end();
       ++p)
    if ((*p)->name() == name)
      return *p;

  return 0;
}

#ifdef __FOO

enumValueAnn * enumPropertyAnn::meet(enumValueAnn * one,
                                     enumValueAnn * two)
{
  // X ^ BOTTOM == BOTTOM

  if ((one == bottom()) ||
      (two == bottom()))
    return bottom();

  // X ^ TOP == X

  if (one == top())
    return two;

  if (two == top())
    return one;

  // X ^ X == X

  if (one == two)
    return one;

  // Otherwise, move down in the lattice until one of the above cases
  // holds.

  if (one->height() > two->height())
    return meet(one->more_general(), two);

  if (one->height() < two->height())
    return meet(one, two->more_general());

  return meet(one->more_general(), two->more_general());
}

/** @brief Set meet function
 *
 * Depending on whether this is a "may" or "must" property. Side-effects
 * the value of the first parameter. */

void enumPropertyAnn::meet_with(enumvalue_set & first,
                                const enumvalue_set & second)
{
  if (_is_may_property)
    first.set_union(second);
  else
    first.set_intersection(second);
}


bool enumPropertyAnn::at_least(enumValueAnn * left,
                               enumValueAnn * right)
{
  // left >= right  <=> left ^ right == right
  // Also, we'll exclude top because it seems non-intuitive

  return (meet(left, right) == right) && (left != top());
}

bool enumPropertyAnn::at_most(enumValueAnn * left,
                              enumValueAnn * right)
{
  // left <= right  <=> left ^ right == left

  return (meet(left, right) == left);
}

/** @brief Look up a sub-block
 *
 * Given a real memoryBlock, look up the corresponding sub-block for this
 * property. Returns null if none exists. */

memoryBlock * enumPropertyAnn::lookup_property_block(memoryBlock * real_block)
{
  // -- New version: property blocks are stored directly on the real block.

  if (real_block->property_blocks.empty())
    return 0;

  return real_block->property_blocks[get_id()];
}

/** @brief Add a sub-block
 *
 * This method is called by the analyzer during pointer analysis to tell
 * the property about the sub-blocks that it's creating for it. */

void enumPropertyAnn::add_property_block(memoryBlock * real_block, memoryBlock * property_block)
{
  // -- New version: we're storing the property block on the real block
  // itself. We assume that Analyzer::lookup_property_block has already set
  // up the vector.

  real_block->property_blocks[get_id()] = property_block;
}

bool TB_changes = false; // TB

bool enumPropertyAnn::test(Broadway::Operator op, Broadway::FlowSensitivity flow_sensitivity,
                           Location * where,
                           pointerValue & lhs, string & lhs_name,
                           pointerValue & rhs, string & rhs_name)
{
  bool result = false;
  enumvalue_set rhs_value_set;
  bool lost_information = false;
  memoryblock_set complicit_property_blocks;

  switch (flow_sensitivity) {

  case Broadway::Before:

    // -- Construct the value

    rhs_value_set = construct_now_value(where, rhs, rhs_name, lost_information,
                                        complicit_property_blocks);

    // -- Perform the test

    result = test(op, flow_sensitivity, where, lhs, lhs_name, rhs_value_set);

    // -- Record tested objects

    record_tested_objects(where, rhs, rhs_value_set, complicit_property_blocks);
    break;

  case Broadway::After:
    rhs_value_set = construct_after_value(rhs, rhs_name);
    result = test(op, flow_sensitivity, where, lhs, lhs_name, rhs_value_set);
    break;

  case Broadway::Always:
    rhs_value_set = construct_ever_value(rhs, rhs_name);
    result = test(op, flow_sensitivity, where, lhs, lhs_name, rhs_value_set);
    break;

  case Broadway::Ever:
    {
      rhs_value_set = construct_ever_value(rhs, rhs_name);

      for (int i = 0; i < rhs_value_set.size(); i++)
        {
          if (rhs_value_set.test(i)) {

            enumValueAnn * value = _value_index[i];

            enumvalue_set temp(value);

            bool one_result = test(op, flow_sensitivity, where, lhs, lhs_name, temp);
            result = result || one_result;
          }
        }
    }
    break;

  default:
    cerr << "ERROR: enumPropertyAnn: invalid flow sensitivity mode passed to test()" << endl;
  }

  return result;
}

bool enumPropertyAnn::test(Broadway::Operator op, Broadway::FlowSensitivity flow_sensitivity,
                           Location * where,
                           pointerValue & lhs, string & lhs_name,
                           enumvalue_set rhs_value_set)
{
  bool result = false;
  enumvalue_set lhs_value_set;
  bool lost_information = false;
  memoryblock_set complicit_property_blocks;

  switch (flow_sensitivity) {

  case Broadway::Before:

    // -- Construct the value

    lhs_value_set = construct_now_value(where, lhs, lhs_name, lost_information,
                                        complicit_property_blocks);

    // -- Perform the test

    result = test(op, lhs_value_set, rhs_value_set);

    // -- Record tested objects

    record_tested_objects(where, lhs, lhs_value_set, complicit_property_blocks);
    break;

  case Broadway::After:
    lhs_value_set = construct_after_value(lhs, lhs_name);
    result = test(op, lhs_value_set, rhs_value_set);
    break;

  case Broadway::Always:
    lhs_value_set = construct_ever_value(lhs, lhs_name);
    result = test(op, lhs_value_set, rhs_value_set);
    break;

  case Broadway::Ever:
    {
      lhs_value_set = construct_ever_value(lhs, lhs_name);

      for (int i = 0; i < lhs_value_set.size(); i++)
        {
          if (lhs_value_set.test(i)) {

            enumValueAnn * value = _value_index[i];

            enumvalue_set temp(value);

            bool one_result = test(op, temp, rhs_value_set);
            result = result || one_result;
          }
        }
    }
    break;

  default:
    cerr << "ERROR: enumPropertyAnn: invalid flow sensitivity mode passed to test()" << endl;
  }

  return result;
}

bool enumPropertyAnn::test(Broadway::Operator op,
                           enumvalue_set lhs_value_set, enumvalue_set rhs_value_set)
{
  bool result = false;

  // -- First combine the values into a single one

  enumValueAnn * lhs_value = merge_enumvalue_set(lhs_value_set);
  enumValueAnn * rhs_value = merge_enumvalue_set(rhs_value_set);

  // -- Then perform the specified test

  if (Annotations::Verbose_properties)
    cout << "     -> " << name() << " : " << to_string(lhs_value_set);

  switch (op) {

  case Broadway::Is_Exactly:
    result = lhs_value == rhs_value;

    if (Annotations::Verbose_properties)
      cout << " is-exactly " << to_string(rhs_value_set);

    break;

  case Broadway::Is_AtLeast:
    result = at_least(lhs_value, rhs_value);

    if (Annotations::Verbose_properties)
      cout << " is-atleast " << to_string(rhs_value_set);

    break;

  case Broadway::Could_Be:
    {
      enumvalue_set temp = lhs_value_set;
      temp.set_intersection(rhs_value_set);

      result = (temp.count() != 0);
    }
    break;

  case Broadway::Is_AtMost:
    result = at_most(lhs_value, rhs_value);

    if (Annotations::Verbose_properties)
      cout << " is-atmost " << to_string(rhs_value_set);

    break;

  case Broadway::Is_Bottom:
    result = (lhs_value == bottom());

    if (Annotations::Verbose_properties)
      cout << " is-bottom ";

    break;

  default:
    cerr << "ERROR: enumPropertyAnn: invalid operator passed to test()" << endl;
  }

  if (Annotations::Verbose_properties) {
    cout << " == ";
    if (result)
      cout << "true";
    else
      cout << "false";
    cout << endl;
  }

  return result;
}

enumvalue_set enumPropertyAnn::lookup_now_value(memoryBlock * property_block,
                                                memoryAccess * def_or_use)
{
  // -- Look up this def or use

  defuse_property_map_p p = _now_values.find(def_or_use);

  if (p == _now_values.end())
    return enumvalue_set();
  else
    return (*p).second;
}

bool enumPropertyAnn::update_now_value(Location * where,
                                       memoryBlock * property_block,
                                       memoryAccess * def_or_use,
                                       enumvalue_set proposed_newval,
                                       bool & lost_information)
{
  bool changed = false;
  lost_information = false;

  // -- Get the old value currently associated with this def/use

  enumvalue_set oldval = lookup_now_value(property_block, def_or_use);

  // -- Meet the new value with the old value

  enumvalue_set newval = proposed_newval;
  meet_with(newval, oldval);

  // -- Check for change

  changed = (newval != oldval);

  if (newval.test(top()->id()))
    cout << "BAD: update_now_value includes top at " << * where << endl;

  // -- Store the result

  _now_values[def_or_use] = newval;

  if (Annotations::Verbose_properties) {
    cout << "      + Set \"now\" " << property_block->name() << " : "
         << to_string(oldval) << " => " << to_string(newval);

    if (def_or_use->is_weak())
      cout << " (weak)";

    if (changed)
      cout << " -- changed" << endl;
    else
      cout << " -- unchanged" << endl;
  }

  // -- Monitor: information is lost when the combination of the old value
  // and the new proposed value is larger (more conservative) than either
  // one.

  int old_count = oldval.count();

  if ((old_count > 0) &&
      (proposed_newval.count() > 0) &&
      (newval.count() > old_count))
    lost_information = true;

  return changed;
}

bool enumPropertyAnn::update_ever_value(memoryBlock * property_block,
                                        enumvalue_set newval)
{
  enumvalue_set cur_vals = _ever_values[property_block];
  enumvalue_set temp = cur_vals;

  bool result = false;

  // -- Add in the new values

  meet_with(temp, newval);

  // -- Check for change

  if (temp != cur_vals) {
    result = true;

    // -- If changed, store the new value

    _ever_values[property_block] = temp;
  }

  return result;
}

enumvalue_set enumPropertyAnn::construct_now_value(Location * where,
                                                   pointerValue & variable,
                                                   string & variable_name,
                                                   bool & lost_information,
                                                   memoryblock_set & complicit_property_blocks)
{
  enumvalue_set result;

  lost_information = false;

  if (Annotations::Verbose_properties)
    cout << "      + Construct \"now\" value for " << variable_name << endl;

  if ( ! variable.blocks.empty()) {

    // -- For each memory block ...

    int min_local_count = 9999;
    enumValueAnn * highest_value = 0;

    for (memoryblock_set_p p = variable.blocks.begin();
         p != variable.blocks.end();
         ++p)
      {
        memoryBlock * real_block = *p;
        memoryBlock * property_block = lookup_property_block(real_block);

        if (property_block) {
          enumvalue_set local_val = construct_now_value(where, property_block, false);

          // -- Monitor: the best value is the lowest count

          int count = local_val.count();
          if (// (count != 0) &&
              (count < min_local_count))
            min_local_count = count;

          if (count > 1)
            complicit_property_blocks.insert(property_block);

          // -- Add into the result

          meet_with(result, local_val);
        } else if (default_val() != top())
          meet_with(result, default_val()); // TB
      }

    if (result.count() > min_local_count)
      lost_information = true;

    if (Annotations::Verbose_properties) {
      cout << "       = " << to_string(result) << endl;
      cout << "         [ min count = " << min_local_count << ", result count = " << result.count() << "]" << endl;
    }
  }
  else {

    // -- No objects -- what should we do?
  }

  return result;
}

enumvalue_set enumPropertyAnn::construct_now_value(Location * where,
                                                   memoryBlock * property_block,
                                                   bool default_to_top)
{
  enumvalue_set result;

  if (Annotations::Verbose_properties)
    cout << "        - Lookup \"Now\" value of " << property_block->name()
         << " real block = " << property_block->container()
         << " real decl = " << property_block->container()->decl() << endl;

  // -- Forward or backward

  if (direction() == Forward) {

    // -- Forward analysis: the now value is constructed by getting the
    // current use and looking up the reaching definition.

    if (Annotations::Verbose_properties)
      cout << "          use at ";

    memoryUse * use = property_block->current_use();
    if (use) {
      memoryDef * def = use->reaching_def();

      if (Annotations::Verbose_properties) {
        cout << * (use->where()) << endl;
        cout << "          reaching def at ";
      }

      // -- If the def has a property value, look it up. If there is no
      // reaching def, use the default value.

      if (def) {
        result = lookup_now_value(property_block, def);

        if (Annotations::Verbose_properties) {
          cout << * (def->where()) << endl;
          cout << "         = " << to_string(result) << endl;
        }
      }
      else {

        if (Annotations::Verbose_properties) {
          cout << "(no reaching def)" << endl;
          cout << "         = " << to_string(result) << endl;
        }

        // -- Weird special case: when a write-protected object has no
        // reaching def (for example, the null object), then we'll return
        // top.

        // if (property_block->container()->write_protected())
        //  result = top();
      }
    }
    else {

      // -- This is bad: the variable has no use here (i.e., it is
      // probably missing an access annotation).

      if (Annotations::Verbose_properties)
        cout << " NO USE " << endl;

      // result = 0;
    }
  }
  else {

    // -- Backward analysis: the now value is constructed by getting
    // the current definition, and meeting together the values from all
    // of the uses that it reaches.

    if (Annotations::Verbose_properties)
      cout << "          def at ";

    memoryDef * current_def = property_block->current_def();
    if (current_def) {

      if (Annotations::Verbose_properties) {
        cout << * (current_def->where()) << endl;
        cout << "          uses reached:" << endl;
      }

      memoryuse_list uses;
      property_block->def_uses(current_def, uses);

      if (uses.empty()) {

        // -- Special case: if the def has no uses (this can happen with
        // merge points), then return the default value.

        // result = 0;

        if (Annotations::Verbose_properties)
          cout << "              (no uses reached)" << endl;

        // -- Weird special case: when a write-protected object has no
        // reaching def (for example, the null object), then we'll return
        // top.

        // if (property_block->container()->write_protected())
        //  result = top();
      }
      else {

        // -- Otherwise, visit all the uses reached by the def

        for (memoryuse_list_p u = uses.begin();
             u != uses.end();
             ++u)
          {
            memoryUse * use = *u;

            enumvalue_set local_val = lookup_now_value(property_block, use);
            meet_with(result, local_val);
            if (Annotations::Verbose_properties)
              cout << "              Use at " << * (use->where()) << " value = " << to_string(local_val) << endl;
          }
      }
    }
    else {

      // -- This is bad: the variable has no def here (i.e., it is
      // probably missing a modifies annotation).

      if (Annotations::Verbose_properties)
        cout << " NO DEF " << endl;
      // result = 0;
    }
  }

  // -- Did we find anything?

  // if (no_result) {

  if (result.none()) {

    // -- Decide on the default behavior

    if (default_val() != top())
      result = enumvalue_set(default_val());
  }

  if (result.test(top()->id()))
    cout << "BAD: construct_now_value includes top at " << * where << endl;

  return result;
}

enumvalue_set enumPropertyAnn::construct_after_value(pointerValue & variable,
                                                     string & name)
{
  enumvalue_set result;

  if (Annotations::Verbose_properties)
    cout << "      + Construct \"after\" value for " << name << endl;

  // -- For each memory block ...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * real_block = *p;
      memoryBlock * property_block = lookup_property_block(real_block);

      if (property_block) {
        enumvalue_set local_val = construct_after_value(property_block);
        meet_with(result, local_val);
      }
    }

  if (Annotations::Verbose_properties)
    cout << "       = " << to_string(result) << endl;

  return result;
}

enumvalue_set enumPropertyAnn::construct_after_value(memoryBlock * property_block)
{
  enumvalue_set result;

  if (Annotations::Verbose_properties)
    cout << "        - Lookup \"After\" value of " << property_block->name();

   // -- Forward or backward

  if (direction() == Forward) {

    // -- Forward analysis: the new property value is associated with
    // the current definition.

    if (Annotations::Verbose_properties)
      cout << " def at ";

    memoryDef * def = property_block->current_def();

    // -- If the def has a property value, collect it into the return
    // value using the meet function.

    if (def) {
      if (Annotations::Verbose_properties)
        cout << * (def->where()) << endl;

      enumvalue_set val = lookup_now_value(property_block, def);
      meet_with(result, val);
    }
    else {

      if (Annotations::Verbose_properties)
        cout << "NO CURRENT DEF" << endl;

      // -- This is bad: the variable has no def here (i.e., it is
      // probably missing a modify annotation).
    }
  }
  else {

    // -- Backward analysis: the new property value is associated with
    // the current use.

    if (Annotations::Verbose_properties)
      cout << " use at ";

    memoryUse * use = property_block->current_use();

    // -- If the use has a property value, collect it into the return
    // value using the meet function.

    if (use) {
      if (Annotations::Verbose_properties)
        cout << * (use->where()) << endl;

      enumvalue_set temp = lookup_now_value(property_block, use);
      meet_with(result, temp);
    }
    else {

      if (Annotations::Verbose_properties)
        cout << "NO CURRENT USE" << endl;

      // -- This is bad: the variable has no use here (i.e., it is
      // probably missing an access annotation).
    }
  }

  return result;
}

enumvalue_set enumPropertyAnn::construct_ever_value(pointerValue & variable,
                                                    string & name)
{
  enumvalue_set result;

  if (Annotations::Verbose_properties)
    cout << "      + Construct \"ever\" values for " << name << endl;

  // -- For each memory block...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * real_block = *p;
      memoryBlock * property_block = lookup_property_block(real_block);

      if (property_block) {

        if (Annotations::Verbose_properties)
          cout << "        - Lookup \"Ever\" value of " << property_block->name()
               << " = { ";

        // -- Look up the set of values

        block_propertyset_map_p q = _ever_values.find(property_block);
        if (q != _ever_values.end()) {
          enumvalue_set & vals = (*q).second;

          // -- Collect those values into the return set

          meet_with(result, vals);

          if (Annotations::Verbose_properties) {
            cout << to_string(vals);
          }
        }

        if (Annotations::Verbose_properties)
          cout << "}" << endl;
      }
    }

  if (Annotations::Verbose_properties) {
    cout << "          = { " << to_string(result) << "}" << endl;
  }

  return result;
}

enumvalue_set enumPropertyAnn::construct_weak_now_value(Location * where,
                                                        pointerValue & variable, string & name)
{
  enumvalue_set result;

  if (Annotations::Verbose_properties)
    cout << "      + Construct \"weak now\" value..." << endl;

  // -- For each memory block ...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * real_block = *p;
      memoryBlock * property_block = lookup_property_block(real_block);

      if (property_block) {

        enumvalue_set local_val;

        // -- Get the now value

        local_val = construct_now_value(where, property_block, false);

        // -- Add it in the collected value

        meet_with(result, local_val);

        // -- If the block was weakly updated, then add in the after value

        if (property_block->current_def() &&
            property_block->current_def()->is_weak()) {

          local_val = construct_after_value(property_block);

          meet_with(result, local_val);
        }
      }
    }

  if (Annotations::Verbose_properties)
    cout << "       = " << to_string(result) << endl;

  return result;
}


/** @brief Compute next state
 *
 * This is called in enumPropertyExprAnn::compute_next(), but it's
 * basically just a wrapper around contruct_now_value(), with the
 * addition of the assignment management. */

enumvalue_set enumPropertyAnn::compute_next(Location * where,
                                            ruleAnn * rule,
                                            exprAnn * expr,
                                            pointerValue & right,
                                            string & right_name,
                                            bool & rhs_lost_information,
                                            memoryblock_set & complicit_property_blocks,
                                            enumvalue_set & ever_values)
{
  // -- The new property value is just the value of the right-hand side

  enumvalue_set result = construct_now_value(where, right, right_name,
                                             rhs_lost_information,
                                             complicit_property_blocks);

  // -- Also, collect the ever values

  ever_values = construct_ever_value(right, right_name);

  return result;
}

/** @brief Change state
 *
 *  Update the state of the given variable to have the given property
 *  value. This method updates all three kinds of flow information. Put any
 *  changes into the changes set (NOTE: these will be the property blocks,
 *  not the real blocks). */

//extern cbzTimer r_timer2;

void enumPropertyAnn::apply_next(Location * where,
                                 stmtLocation * parameter_callsite,
                                 ruleAnn * rule, enumPropertyExprAnn * expr,
                                 pointerValue & left,
                                 string & left_name,
                                 pointerValue & right,
                                 enumvalue_set proposed_new_value,
                                 bool rhs_lost_information,
                                 bool rhs_changed,
                                 memoryblock_set & complicit_property_blocks,
                                 enumvalue_set ever_values,
                                 memoryblock_set & changes)
{
  memoryblock_set right_property_blocks;

  // -- For each block in the pointer

  for (memoryblock_set_p p = left.blocks.begin();
       p != left.blocks.end();
       ++p)
    {
      memoryBlock * real_block = *p;

      memoryDef * change_def = 0;
      memoryAccess * attach_value_to = 0;
      enumvalue_set new_value = proposed_new_value;

      bool lost_information = false;

      // -- Skip write-protected objects

      if ( ! real_block->write_protected()) {

        memoryBlock * property_block = lookup_property_block(real_block);

        if (property_block) {

          if (Annotations::Verbose_properties) {
            cout << "    + Apply: " << property_block->name()
                 << " <- " << to_string(new_value) << endl;
            cout << "       Real block: " << real_block << endl;
          }

          // -- Record this right-hand side

          right_property_blocks.insert(property_block);

          // -- Set up forward or backward update

          if (direction() == Forward) {

            // -- Forward analysis: propagate the new states to the current
            // definition

            memoryDef * def = property_block->current_def();

            change_def = def;
            attach_value_to = def;
          }
          else {

            // -- Backward analysis: propagate the new states to the
            // current use.

            memoryUse * use = property_block->current_use();

            // -- We record the reaching def as the changed definition

            change_def = use->reaching_def();

            // -- Attach the value to the use

            attach_value_to = use;
          }

          bool weak_update_lost_information = false;

// begin TB new
          bool any_rhs_diagnostic_value = false; // verify
// end TB new

          if (enforce_weak_updates()) {

            // -- Handle weak updates. Special case: never apply a weak
            // update when there is no "now" value (in other words, don't
            // just mindlessly merge in the default value). If there are no
            // reaching defs/uses, then just leave the value alone.

            if (attach_value_to->is_weak()) {

              // -- Get the value that reaches this location (the previous value)

              enumvalue_set previous_value = construct_now_value(where, property_block, true);

              // -- Meet it in with the new value

              meet_with(new_value, previous_value);

              // -- Check to see if the weak update lost information

              if (new_value.count() > proposed_new_value.count())
                weak_update_lost_information = true;

// begin TB new
              if(_diagnostic &&
                 _diagnostic->contain_diagnostic_value(previous_value))
                any_rhs_diagnostic_value = true;
// end TB new
            }
          }

          // -- Set the "now" value

          bool change_now = update_now_value(where, property_block, attach_value_to, new_value, lost_information);
if (Annotations::Verbose_properties) { // debug
  cout << "  inside apply_next, attach_value_to= " << attach_value_to
       << " def=" << property_block->current_def() << " use="
       << property_block->current_use() << endl;
}

// begin TB new

          if(_diagnostic && ! property_block->is_flow_sensitive())
            assert(! real_block->is_flow_sensitive());
          if(_diagnostic && property_block->is_flow_sensitive()) {
            // as suggested by Sam, use attach_value_to's location which is a
            // stmtLocation instead of `where' which is a procLocation.
            Location *loc = attach_value_to->where();
if(loc->kind() != Location::Statement &&
   loc->kind() != Location::Procedure) { // hack
  cout << "wrong assumption: apply_next @ "<< *loc << " kind=" << loc->kind() << endl;
  cout << "property_block " << property_block->name() << " real " << real_block->name() << endl;
  cout << "where @" << *where << endl;
  if(parameter_callsite) cout << "callsite? @" << *parameter_callsite << endl;
  // exit(1);
}

            if(_diagnostic->contain_diagnostic_value(new_value)) {
              // new_value contains diagnostic value; remember the def @lhs and
              // use @rhs.
              memoryDef *def = property_block->current_def();
if(def->where() != loc) {
  Location *loc1 = def->where();
  cout << "apply_next loc " << loc << " " << *loc
       << " loc1 " << loc1 << *loc1 << endl;
  exit(1);
}
              for (memoryblock_set_p r=right.blocks.begin();
                   r!=right.blocks.end(); ++r) {
                if(*r == real_block) continue; // TB
                memoryBlock * rhs_block = lookup_property_block(*r);
                if (rhs_block) {
                  enumvalue_set local_val =
                    construct_now_value(where, rhs_block, false);
                  // add_defuse only if property block contains diagnostic value
                  if(_diagnostic->contain_diagnostic_value(local_val)) {
                    _diagnostic->add_defuse(loc, def, rhs_block->current_use());
                    if (Annotations::Verbose_properties)
                      cout << "  diagnostic add_defuse (apply_next) " <<" @"
                           << *loc << " def=" << def << " use="
                           << rhs_block->current_use()
                           << " l_property " << property_block
                           << " r_property " << rhs_block << endl;
                    any_rhs_diagnostic_value = true;
                  }
                }
              }
              if(! any_rhs_diagnostic_value && !right.blocks.empty()) { // hack
                cout << "apply_next !any_rhs_diagnostic @" << *where << endl;
                cout << "  proposed " << to_string(proposed_new_value)
                     << "\n  new_value " << to_string(new_value) << endl;
                for (memoryblock_set_p r=right.blocks.begin();
                     r!=right.blocks.end(); ++r) {
                  memoryBlock * rhs_block = lookup_property_block(*r);
                  if (rhs_block) {
                    enumvalue_set local_val =
                      construct_now_value(where, rhs_block, false);
                    cout << "  local " << to_string(local_val) << endl;
                  }
                }
              }
            }
          }

// end TB new

          // -- Update the flow-insensitive "ever" value

          bool change_ever = update_ever_value(property_block, new_value);

          // -- Pass on any "ever" values

          // bool pass_evers = false;
          bool pass_evers = update_ever_value(property_block, ever_values);

          // -- If anything changed, record where it happened

          // TB_unify: enforce rhs_changed? No need if lhs is not unified.
          if(rhs_changed && !change_now && !real_block->unifyType())
            rhs_changed = false;

          // TB_unify: add rhs_changed below
          if ((change_now || rhs_changed) && change_def) { // || change_always || change_ever || pass_evers) && change_def)
            if (Annotations::Verbose_properties)
              if(rhs_changed)
                cout << "      - but rhs value changed, forced changed.\n";
            changes.insert(property_block);
          }

          // -- Monitor: Diagnose the information loss

          if (pointerOptions::Monitor_precision) {

            // -- If this is a parameter being passed, then record the
            // reaching defs.

            if ((parameter_callsite) &&
                (where->kind() == Location::Procedure))
              {
                procLocation * procloc = (procLocation *) where;

                memorydef_set reaching_defs;

                property_block->add_parameter_assignment(procloc->proc(),
                                                         parameter_callsite,
                                                         right_property_blocks);
              }

                  // -- If the right-hand-side pointer dereference lost
            // information, then add the complicit assignment information.

            if (rhs_lost_information)
              property_block->add_complicit_assignment(where, right.dereferenced);

            if (proposed_new_value.count() > 1) {

              // -- Add complicit objects from the RHS

              property_block->add_complicit_assignment(where, complicit_property_blocks);
            }

            // -- Override when weak-update is forced

            bool forced_weak = false;

            if ( expr &&
                 expr->is_weak())
              forced_weak = true;

            // -- Check if a weak update lost precision

            if (weak_update_lost_information && ! forced_weak) {

              // -- If a weak update occured, check to see if it was caused by
              // multiple left-hand-sides or by multiplicity.

              Multiplicity multiplicity = Unallocated;

              // -- Estimate multiplicity:

              if (real_block->is_indexed())
                multiplicity = Bounded;
              else {
                if (real_block->is_heap_object())
                  multiplicity = Unbounded;
                else
                  multiplicity = Single;
              }

              if ((multiplicity == Single) &&
                  (left.blocks.size() > 1))
                {

                  // -- Multiple left-hand-sides: the problem is the
                  // left-hand dereferenced pointers.

                  property_block->add_complicit_assignment(where, left.dereferenced);
                }
              else {

                // -- High multiplicity: if it wasn't a pointer-induced
                // weak update, then it must have been caused by
                // multiplicity. If it's a heap block, then blame the
                // alloc object.

                memoryBlock * alloc_object = property_block->allocation_object();
                if (alloc_object)
                  property_block->add_complicit_assignment(where, alloc_object);

                // -- NOTE: We always add the destructive assignment to
                // force this object to become flow
                // sensitive. Otherwise we'll never track the
                // multiplcity accurately.

                property_block->add_destructive_assignment(where, memoryBlock::Weak_update);
              }

              // -- See if the update itself lost information

              if (lost_information) {

                // -- Strong update

                if ((where->kind() == Location::Statement) ||
                    (rule != 0)) {

                  // -- Regular statement: this can only happend with an
                  // additive assignment caused by flow-insensitivity.

                  property_block->add_destructive_assignment(where, memoryBlock::Additive);
                }
                else {

                  // -- Parameter pass: this happens when the procedure
                  // is context-insensitive and it gets different values
                  // in the different contexts.

                  property_block->add_destructive_assignment(where, memoryBlock::Parameter_pass);
                }
              }
            }
          } // -- END monitor
        } // -- END if property_block
      } // -- END not write-protected
    } // -- END for all objects
}

void enumPropertyAnn::apply_merge(Location * where,
                                  memoryBlock * property_block,
                                  memoryuse_list & phi_uses,
                                  memoryblock_set & changes)
{
  if (Annotations::Verbose_properties)
    cout << "  + Merge " << property_block->name() << endl;

  enumvalue_set oldval;
  enumvalue_set merged_val;

  // -- Record if we lose information

  bool lost_information = false;
  bool throw_away;

  // -- Forwards or backwards

  if (direction() == Forward) {

    // -- Forward analysis: collect the values that reach the merge uses,
    // meet them together and propagate that value to the def.

    // -- Get the old value

    memoryDef * def = property_block->current_def();

    if (def)
      oldval = lookup_now_value(property_block, def);

    // -- Merge together the uses that reach this merge point

    int min_reaching_count = 9999;

    for (memoryuse_list_p p = phi_uses.begin();
         p != phi_uses.end();
         ++p)
      {
        memoryUse * phi_use = *p;
        memoryDef * reaching_def = phi_use->reaching_def();

        // -- Find the reaching value, handling the default case as well

        enumvalue_set reaching_val;

        if (reaching_def) {

          // -- There is a reaching def, look it up

          reaching_val = lookup_now_value(property_block, reaching_def);

          if (Annotations::Verbose_properties)
            cout << "   = " << to_string(reaching_val) << " at " << * (reaching_def->where()) << endl;
        }
        else {

          // -- No reaching def, use the default value

          if (default_val() != top())
            reaching_val = enumvalue_set(default_val());

          if (Annotations::Verbose_properties)
            cout << "   = default value " << to_string(reaching_val) << endl;
        }

        int count = reaching_val.count();
        if (count < min_reaching_count)
          min_reaching_count = count;

        meet_with(merged_val, reaching_val);

// begin TB new
        if(_diagnostic) { // note: check with reaching_val, not merged_val
          Location *loc = def->where();
if(loc->kind() != Location::BasicBlock) { // hack
  cout << "apply_merge @ "<< *loc << " kind=" << loc->kind() << endl;
  exit(1);
}
          if(_diagnostic->contain_diagnostic_value(reaching_val)) {
            // new_value contains diagnostic value; remember the def @lhs and use @rhs.
            _diagnostic->add_defuse( loc, def, phi_use );
if(def->where() != loc) {
  Location *loc1 = def->where();
  cout << "apply_merge loc " << loc << " " << *loc
       << " loc1 " << loc1 << *loc1 << endl;
  exit(1);
}
            if (Annotations::Verbose_properties)
              cout << "  diagnostic add_defuse (apply_merge) " << " @" << *loc
                   << " def=" << def << " use=" << phi_use << endl;
          }
        }
// end TB new

      }

    // -- Check for information loss

    if ((min_reaching_count == 1) &&
        (merged_val.count() > 1))
      lost_information = true;

    // -- Did anything change?

    bool changed = update_now_value(where, property_block, def, merged_val, throw_away);
    if (changed) {

      changes.insert(property_block);

      if (Annotations::Verbose_properties)
        cout << "   -> merge changed " << property_block->name() << endl;
    }

  }
  else {

    // -- Backward analysis: get the value of the definition and just copy
    // it to all the merge uses.

    merged_val = construct_now_value(where, property_block, false);

// begin TB new
    bool any_diagnostic_value = false;
    memoryDef * def = property_block->current_def();
    Location *loc = def->where();
    if(_diagnostic) {
if(loc->kind() != Location::BasicBlock) { // hack
  cout << "apply_merge @ "<< *loc << " kind=" << loc->kind() << endl;
  exit(1);
}

      if(_diagnostic->contain_diagnostic_value(merged_val))
        // new_value contains diagnostic value
        any_diagnostic_value = true;
    }
// end TB new

    for (memoryuse_list_p p = phi_uses.begin();
         p != phi_uses.end();
         ++p)
      {
        memoryUse * phi_use = *p;

        // oldval = lookup_now_value(block, phi_use);

        // -- Did anything change?

        bool changed = update_now_value(where, property_block, phi_use, merged_val, lost_information);
        if (changed && phi_use->reaching_def()) {

          changes.insert(property_block);

          if (Annotations::Verbose_properties)
            cout << "   -> merge changed " << property_block->name() << endl;
        }

// begin TB new
        if(any_diagnostic_value) {
          _diagnostic->add_defuse( loc, def, phi_use );
          if (Annotations::Verbose_properties)
            cout << "  diagnostic add_defuse (apply_merge) " << " @" << *loc
                 << " def=" << def << " use=" << phi_use << endl;
if(def->where() != loc) {
  Location *loc1 = def->where();
  cout << "apply_merge (backward) loc " << loc << " " << *loc
       << " loc1 " << loc1 << *loc1 << endl;
  exit(1);
}
        }
// end TB new
      }
  }

  // -- Monitor: see if the merge caused a loss of information

  if (pointerOptions::Monitor_precision) {

    if (lost_information)
      property_block->add_destructive_assignment(where, memoryBlock::Control_flow);
  }

  // -- NOTE that a merge will never change the "always" or "ever" values,
  // so there is no point touching them.
}

/** @brief Apply assignment
 *
 * At a normal assignment, build the "now" value for the right-hand side,
 * and assign it to the left-hand side. Weak updates are handled by the
 * apply_next() method. */

void enumPropertyAnn::apply_assignment(Location * where,
                                       stmtLocation * parameter_callsite,
                                       pointerValue & left,
                                       pointerValue & right,
                                       bool is_parameter,
                                       memoryblock_set & changes)
{
/*static bool called = false; // hack
if(!called) {
  cout << "enumP apply_assignment\n";
  called = true;
}*/
//r_timer2.start();
  enumvalue_set new_value;
  enumvalue_set ever_values;
  pointerValue * to_update = 0;
  bool rhs_lost_information = false;
  memoryblock_set complicit_property_blocks;

  // -- Skip expressions where the right-hand-side is an address:
  // (e.g., "p = &x" does not propagate the value to or from x).

  if ( ! right.is_address) {

    string rhs_name("right-hand side");
    string lhs_name("left-hand side");

    // -- Forward or backward: figure out what the new value is, and which
    // blocks to update.

    if (direction() == Forward) {

      // -- Forward: new value comes the right, and updates the left.

      new_value = construct_now_value(where, right, rhs_name,
                                      rhs_lost_information,
                                      complicit_property_blocks);
      ever_values = construct_ever_value(right, rhs_name);

      // -- Perform the actual update

      apply_next(where, parameter_callsite,
                 (ruleAnn *)0, (enumPropertyExprAnn *)0,
                 left, lhs_name,
                 right, new_value,
                 rhs_lost_information, false,
                 complicit_property_blocks,
                 ever_values, changes);
    }
    else {

      // -- Backward: new value comes from the left and updates the
      // right. Note that we make sure to get the value *after* any weak
      // updates.

      new_value = construct_weak_now_value(where, left, lhs_name);
      ever_values = construct_ever_value(left, lhs_name);

      // -- Perform the actual update

      apply_next(where, parameter_callsite,
                 (ruleAnn *)0, (enumPropertyExprAnn *)0,
                 right, rhs_name,
                 left, new_value,
                 rhs_lost_information, false, complicit_property_blocks,
                 ever_values, changes);
    }
  }
//r_timer2.stop();
}

/** @brief Self assignment
 *
 * Now that properties are associated with special property blocks, we need
 * to explicitly handle self-assignment caused by passing parameters and
 * external inputs and outputs. */

void enumPropertyAnn::self_assignment(Location * source,
                                      Location * target,
                                      memoryBlock * property_block,
                                      memoryblock_set & changes,
              bool is_input)
{
  if (Annotations::Verbose_properties)
    cout << "  + Pass " << name() << endl;

  // -- Get the current reaching value

  enumvalue_set current_value = construct_now_value(source, property_block, true);

  // -- Figure out the direction

  memoryAccess * attach_value_to = 0;

  if (direction() == Forward) {

    // -- Forward analysis: propagate the new states to the current
    // definition

    attach_value_to = property_block->current_def();
  }
  else {

    // -- Backward analysis: propagate the new states to the current
    // use.

    attach_value_to = property_block->current_use();
  }

  // -- Assign it to itself

  bool lost_information = false;
  bool change = update_now_value(target, property_block, attach_value_to, current_value, lost_information);
if (Annotations::Verbose_properties) { // debug
  cout << "  inside self_assignment, attach_value_to= " << attach_value_to
       << " def=" << property_block->current_def() << " use="
       << property_block->current_use() << endl;
}

  // -- Record any change

  if (change)
    changes.insert(property_block);

// begin TB new

  Location *loc;
  if(is_input) {
    assert(property_block->use_at(source)->where() == source); // external_input
    loc = target; // source;
  } else {
    assert(property_block->current_def()->where() == target); // external_output
    loc = target;
  }

// cout << "self-assign " << *source << " " << *target << endl;
  if(_diagnostic && ! property_block->is_flow_sensitive())
    assert(! property_block->container()->is_flow_sensitive());
  if(/*change_now &&*/ _diagnostic && property_block->is_flow_sensitive()) {
    // as suggested by Sam, use attach_value_to's location which is a stmtLocation
    // instead of `where' which is a procLocation.
    // Location *loc = attach_value_to->where();
if(loc->kind() != Location::Statement &&
   loc->kind() != Location::Procedure) { // hack
  cout << "wrong assumption: self_assign @ "<< *loc << " kind=" << loc->kind() << endl;
  cout << "property_block " << property_block->name() << endl;
  cout << "source @" << *source << " target @" << *target << endl;
  memoryUse * use = property_block->use_at(source);
  memoryDef * def = property_block->current_def();
  cout << "    use " << use << " @" << *use->where() << " def " << def << " @" << *def->where() << endl;
  return;
  exit(1);
}
    if(_diagnostic->contain_diagnostic_value(current_value)) {
/* cout << "  diagnostic (self_assignment) " << " source@" << *source << " target@" << *target
<< " attach_value_to @" << *loc << " " << loc
<< " property_block=" << property_block
<< " " << property_block->name()
<< " def=" << property_block->current_def() << endl; */
      // current_value contains diagnostic value; remember the def @lhs and use @rhs.
// cout << "    property_block " << property_block->name() << " " << property_block->current_use() << endl;
      memoryUse * use = property_block->use_at(source);
      memoryDef * def = property_block->current_def();
// cout << "    use " << use << " @" << *use->where() << " def " << def << " @" << *def->where() << endl;
      _diagnostic->add_defuse( loc, def, use );
      if (Annotations::Verbose_properties)
        cout << "  diagnostic add_defuse (self_assignment) " << " @" << *loc
             << " def=" << def << " use=" << use << endl;
if(def->where() != loc) {
  Location *loc1 = def->where();
  cout << "self-assignment loc " << loc << " " << *loc
       << " loc1 " << loc1 << " " << *loc1 << endl;
  cout << "is_input=" << is_input << endl;
  cout << "source " << source << " " << *source << " target " << target
       << " " << *target << endl;
  cout << "def's loc " << def->where() << " " << *def->where() << endl;
  cout << "use's loc " << use->where() << " " << *use->where() << endl;
  exit(1);
}
    }
  }

// end TB new
}

/** @brief Conservative procedure call
 *
 * Record a conservative procedure call, primarily for back-trace
 * purposes. */

void enumPropertyAnn::conservative_procedure_call(stmtLocation * current,
                                                  pointerValue & reachable,
                                                  memoryblock_set & changes)
{
  // -- Conservative call: set everything to bottom
  /*
  for (memoryblock_set_p p = reachable.blocks.begin();
       p != reachable.blocks.end();
       ++p)
    {
      memoryBlock * real_block = *p;

      // -- Use the apply_next() method because it does the right thing

      apply_next(real_block, bottom(), changes);
    }
  */
}

void enumPropertyAnn::report(ostream & out,
                             bool is_error,
                             procLocation * where,
                             Broadway::FlowSensitivity flow_sensitivity,
                             pointerValue & lhs, string & lhs_name)
{
  enumvalue_set lhs_value_set;
  enumValueAnn * lhs_value = 0;
  bool lost_information = false;
  memoryblock_set complicit_property_blocks;

  switch (flow_sensitivity) {

  case Broadway::Before:
    lhs_value_set = construct_now_value(where, lhs, lhs_name, lost_information,
                                        complicit_property_blocks);
    lhs_value = merge_enumvalue_set(lhs_value_set);

    if (! Annotations::Quiet_reports || is_error) {
      out << lhs_value->name();
      out << " (" << to_string(lhs_value_set) << ")";
    }

    // -- Record tested objects

    record_tested_objects(where, lhs, lhs_value_set, complicit_property_blocks);
    break;

  case Broadway::After:
    lhs_value_set = construct_after_value(lhs, lhs_name);
    lhs_value = merge_enumvalue_set(lhs_value_set);

    if (! Annotations::Quiet_reports || is_error) {
      out << lhs_value->name();
    }

    break;

  case Broadway::Always:
    lhs_value_set = construct_ever_value(lhs, lhs_name);
    lhs_value = merge_enumvalue_set(lhs_value_set);

    if (! Annotations::Quiet_reports || is_error) {
      out << lhs_value->name();
    }

    break;

  case Broadway::Ever:
    {
      lhs_value_set = construct_ever_value(lhs, lhs_name);

      if (! Annotations::Quiet_reports || is_error) {
        out << to_string(lhs_value_set);
      }
    }
    break;

  case Broadway::Trace:
    {
      if (Annotations::Skip_traces)
        out << "(skipping trace)";
      else {

        for (memoryblock_set_p p = lhs.blocks.begin();
             p != lhs.blocks.end();
             ++p)
          {
            memoryBlock * real_block = *p;
            memoryBlock * property_block = lookup_property_block(real_block);

            if (! Annotations::Quiet_reports || is_error) {
              out << endl << "TRACE: " << property_block->name() << endl;

              memoryblock_set already_seen;
              string indent = "  ";

              trace_object(out, property_block, already_seen, indent);
            }
          }
      }
    }
    break;

  case Broadway::Confidence:
    {
      lhs_value_set =construct_now_value(where, lhs, lhs_name, lost_information,
                                         complicit_property_blocks);

      // -- Count the number of possible values.

      double dsize = (double) lhs_value_set.count();

      // -- Check for top

      double confidence;

      if (dsize == 0)
        confidence = 0.0;
      else {
        confidence = (_number_of_flowvalues - dsize) / (_number_of_flowvalues - 1.0);
        // OLD value confidence = (1.0 / dsize );
      }

      if (! Annotations::Quiet_reports || is_error) {
        out << confidence;
      }

      if (dsize != 0) {

        // -- Also, store the confidence values in order to compute accuracy

        accuracy_map_p p = _accuracy.find(where);
        if (p == _accuracy.end()) {

          _accuracy[where] = count_accuracy_pair(1, confidence);
        }
        else {
          count_accuracy_pair & cap = (*p).second;
          cap.first++;
          cap.second += confidence;
        }

        // -- If this is an error report, record the statement where it
        // occured

        if (is_error)
          _error_statements.insert(where->stmt_location()->stmt());
      }
    }
    break;

  default:
    cerr << "ERROR: enumPropertyAnn: invalid flow sensitivity mode passed to report()" << endl;
  }
}

#endif /* __FOO */

void enumPropertyAnn::print(ostream & o) const
{
  o << "property " << name();

  o << "{" << endl;
  print(o, values(), 2);
  o << "}" << endl;
}

void enumPropertyAnn::print(ostream & o, enumValueAnn * prop, int depth) const
{
  for (int i = 0; i < depth; i++)
    o << " ";

  o << prop->name() << ", id = " << prop->id() << ", height = " << prop->height() << endl;

  const enumvalue_list & vals = prop->more_specific();
  for (enumvalue_list_cp p = vals.begin();
       p != vals.end();
       ++p)
    print(o, *p, depth+2);
}

#ifdef __FOO

/** @brief Merge enumvalue_set
 *
 * Visit the values in an enumvalue_set and merge them into a single value
 * using the meet function. */

enumValueAnn * enumPropertyAnn::merge_enumvalue_set(enumvalue_set value_set)
{
  enumValueAnn * result = top();

  // -- Visit all the values in the set and meet them together

  for (int i = 0; i < value_set.size(); i++) {

    if (value_set.test(i)) {

      enumValueAnn * value = _value_index[i];

      result = meet(result, value);
    }
  }

  return result;
}

/** @brief Convert enum values to string */

string enumPropertyAnn::to_string(enumvalue_set value_set)
{
  bool first = true;
  string s;

  if (value_set.count() == 0)
    s = "(empty)";
  else {

    for (int i = 0; i < value_set.size(); i++) {

      if (value_set.test(i)) {

        enumValueAnn * value = _value_index[i];
        if ( ! first ) s += ", ";
        s += value->name();
        first = false;
      }
    }
  }

  return s;
}

// ----------------------------------------------------------------------
//  Adaptive algorithm
// ----------------------------------------------------------------------

/** @brief NEW Precision analysis
 */

void enumPropertyAnn::precision_analysis(Analyzer * analyzer,
                                         int analyzed_properties)
{
  if (Annotations::Adaptivity == 3) {
    if (Annotations::Show_adaptivity)
      cout << "Track tested objects...\n";

    if(Annotations::Adapt_precision == Annotations::UNIFY) { // TB_unify
      if (Annotations::Show_adaptivity)
        cout << "Non-unify " << name() << ":" << endl;
      // next precision policy is FICI

      memoryblock_set un_unified; // objects marked as un-unified

      for (tested_objects_map_p p = _tested_objects.begin();
           p != _tested_objects.end(); ++p) {
        Location * target = (*p).first;
        memoryblock_set objects = (*p).second;

        while(! objects.empty()) {
          memoryblock_set more;
          for(memoryblock_set_p o=objects.begin(); o!=objects.end(); o++) {
            memoryBlock * real = *o;
            if(un_unified.find(real) != un_unified.end()) continue;
            un_unified.insert(real);

            real->add_to_non_unify_list(pointerOptions::Non_unify_types);
            memoryBlock * property = lookup_property_block(real);
            property->add_to_non_unify_list(pointerOptions::Non_unify_types);
            if (Annotations::Show_adaptivity)
              cout << "AT " << *target << " non-unify " << real->name()
                   << endl;

            // TBD? ignore real->complicit_assignments() ??
            const memoryBlock::complicit_assignment_map & complicit
              = property->complicit_assignments();
            for(memoryBlock::complicit_assignment_map_cp p = complicit.begin();
                p != complicit.end(); ++p) {
              for(memoryblock_set_cp q = p->second.begin();
                  q != p->second.end(); q++) {
                if((*q)->property == this)
                  more.insert((*q)->container()); // q is a property
              }
            }
          }
          objects = more;
        }
      }

      Annotations::Adapt_precision = Annotations::FICI;
    } else {

      // -- Adaptivity Option 3: Use the pointer monitor to track the tested
      // objects back to destructive assignments.

      // -- First populate the list of error procedures

int locs = 0; // debug
memoryblock_set tested_objs;

      for (tested_objects_map_p p = _tested_objects.begin();
           p != _tested_objects.end();
           ++p)
        {
          Location * target = (*p).first;
          procLocation * target_proc_loc = Location::procedure(Location::procedure(target)->stmt_location());
          procNode * target_proc = target_proc_loc->proc();
          procedureInfo * target_info = analyzer->lookup_procedure(target_proc);

          _error_procedures.insert(target_info);
if(! p->second.empty()) {
  locs++;
  tested_objs.insert(p->second.begin(), p->second.end());
}
        }

if (Annotations::Show_adaptivity)
cout << "#test_objects=" << tested_objs.size() << " #locs=" << locs << endl;

      memoryblock_set made_flow_sensitive;
      block_loc_set made_context_sensitive /* TB */;
      memoryblock_set made_fs_destructive;
      block_proc_set eval_cs_seen_destructive, eval_cs_seen_complicit;
      block_proc_set eval_made_cs;

      block_loc_set already_seen, seen_stack;

      memoryblock_vector chain;

      string indent(" ");
      for (tested_objects_map_p p = _tested_objects.begin();
           p != _tested_objects.end();
           ++p)
        {
          Location * target = (*p).first;
          memoryblock_set & objects = (*p).second;

          if(pointerOptions::Slice_analysis) {
            procNode *proc = Location::procedure(target)->proc();
            if(analyzer->annotations()->lookup_procedure(proc->decl()->name()))
              proc = Location::procedure(Location::procedure(target)->parent())
                     ->proc();
            procedureInfo *info = analyzer->lookup_procedure(proc);
            pointerOptions::No_Skip_procs.insert(info);
          }

          if (Annotations::Show_adaptivity)
            cout << "AT " << *target << ": num objects = " << objects.size() << endl;

if (Annotations::Show_adaptivity)
cout << "target " << *target << " #objects " << objects.size() << " #seen="
     << already_seen.size() << endl; // debug
//if(already_seen.size() > 178*952) exit(1);
          for (memoryblock_set_p q = objects.begin();
               q != objects.end();
               ++q)
            {
              memoryBlock * real_block = *q;
              memoryBlock * property_block = lookup_property_block(real_block);
if (Annotations::Show_adaptivity)
cout << " property " << property_block << " " << property_block->name()//debug
     << " fs="
     << (made_flow_sensitive.find(property_block) != made_flow_sensitive.end())
     << endl;

              if (Annotations::Show_adaptivity)
                cout << "TRACK " << real_block->name() << " for property " << name()
                     << " at " << * target << endl;

              bool toss1;
              bool toss2;

              if (property_block &&
                  ! real_block->write_protected()) {
                track_destructive_assignments(analyzer, target, property_block, false,
                                              already_seen, seen_stack, made_flow_sensitive,
                                              made_fs_destructive,
                                              made_context_sensitive,
                                              eval_cs_seen_destructive,
                                              eval_cs_seen_complicit,
                                              eval_made_cs,
                                              toss1, toss2, chain, indent);
              }
            }
        }

      if (Annotations::Show_adaptivity) {
        cout << "FS-Property " << name() << ":" << endl;
        for (memoryblock_set_p p = made_flow_sensitive.begin();
             p != made_flow_sensitive.end();
             ++p)
          {
            memoryBlock * block = (*p);

            if ( ! block->is_flow_sensitive()) {
              memoryDef * def = block->find_def_at(procLocation::main());
              cout << "  + FI: " << (*p)->name();
              if (def) {

                const memoryblock_set & pt = def->points_to();
                if (pt.size() > 0) {

                  if (pt.size() == 1)
                    cout << " -- single-pointer-target: ";
                  else
                    cout << " -- multi-pointer-target: ";

                  for (memoryblock_set_cp i = pt.begin();
                       i != pt.end();
                       ++i)
                    {
                      cout << (*i)->name() << " ";
                    }
                  cout << endl;
                }
                else
                  cout << " -- not-pointer" << endl;
              }
              else
                cout << " -- no-def" << endl;
            }
            else {
              cout << "  + FS: " << (*p)->name() << endl;
            }

            /*
            if ((*p)->is_single_assignment())
              cout << " -- single assign" << endl;
            else
              cout << " -- multiple assign" << endl;
            */
            // (*p)->print_def_use(cout);
          }
      }
      Annotations::Adapt_precision = Annotations::FSCS;

      if(pointerOptions::Slice_analysis) {
        set<procNode*> previous_skip_set; // 20030917
        if(analyzed_properties>0) {
          previous_skip_set = pointerOptions::Skip_procs;
          pointerOptions::Skip_procs.clear();
        }

        memoryblock_set sliced_blocks;

        proc_info_map all_procs = analyzer->procedures();
        procedureinfo_set do_not_skip;
        for(proc_info_map_p p=all_procs.begin(); p!=all_procs.end(); p++) {
          procNode *proc = p->first;
          procedureInfo *P = p->second;
          if(analyzed_properties>0 &&
             previous_skip_set.find(proc) == previous_skip_set.end())
            continue; // not in previous set, will be excluded in current set.

          if(analyzer->annotations()->lookup_procedure(P->name())) continue;
          if(pointerOptions::No_Skip_procs.find(P) ==
             pointerOptions::No_Skip_procs.end() &&
             do_not_skip.find(P) == do_not_skip.end()) {
            bool ancestor = false;
            for(set<procedureInfo*>::iterator
                t=pointerOptions::No_Skip_procs.begin();
                t!=pointerOptions::No_Skip_procs.end(); t++) {
              procedureinfo_set ancestors = (*t)->ancestors();
              if(ancestors.find(P) != ancestors.end())
              { ancestor=true; break; }
            }

            bool interesting_ext_output = false; // 20030917
            if(! ancestor &&
               (!P->external_outputs().empty() || P->return_block())) {
              // get sliced_blocks if not already.
              if(sliced_blocks.empty() && !already_seen.empty())
                for(block_loc_set::iterator a=already_seen.begin();
                    a!=already_seen.end(); a++) {
                  sliced_blocks.insert(a->first);
                  if(a->first->property)
                    sliced_blocks.insert(a->first->container());
                }

              memoryblock_set ext_outputs = P->external_outputs();
              if(P->return_block()) {
                // need to get return values in all calling contexts
                // 20030918: this is necessary because we do not yet have
                // variable slicing; it leads to fewer slipped procs. If we have
                // variable slicing, we may be able to improve on this.
                const procedureInfo::callsite_map & callsites = P->callsites();
                for(procedureInfo::callsite_map_cp c=callsites.begin();
                    c!=callsites.end(); c++) {
                  procLocation *call = P->procedure_location(c->first);
                  memoryblock_list wl; // worklist
                  wl.push_back(P->return_block());
                  memoryblock_set reachables;
                  analyzer->reachable_blocks(call->last(), wl, reachables);
                  ext_outputs.insert(reachables.begin(), reachables.end());
                }
              }
              for(memoryblock_set_cp e=ext_outputs.begin();
                  e!=ext_outputs.end(); e++) {
                // check if *e is in sliced_blocks
                if(sliced_blocks.find(*e) != sliced_blocks.end())
                { interesting_ext_output = true; break; }
                // check if (*e)->property is in sliced_blocks
                memoryBlock *property = lookup_property_block(*e);
                if(property && sliced_blocks.find(property) !=
                   sliced_blocks.end())
                { interesting_ext_output = true; break; }
                // check if *e ever points to something in sliced_blocks
                /*if(! (*e)->property) {
                  const memorydef_list defs = (*e)->defs();
                  for(memorydef_list_cp d=defs.begin(); d!=defs.end(); d++) {
                    const memoryblock_set points_to = d->def->points_to();
                    for(memoryblock_set_cp p=points_to.begin();
                        p!=points_to.end(); p++)
                      if(sliced_blocks.find(*e) != sliced_blocks.end())
                      { interesting_ext_output = true; break; }
                    if(interesting_ext_output) break;
                  }
                  if(interesting_ext_output) break;
                }*/
              }
            }

            if(! ancestor && ! interesting_ext_output) {
              if(analyzed_properties==0 ||
                 previous_skip_set.find(proc) != previous_skip_set.end()) {
                pointerOptions::Skip_procs.insert(proc);
              }
            } else {
              if(analyzed_properties>0 &&
                 previous_skip_set.find(proc) != previous_skip_set.end())
                cout << "Unmark skip-proc: " << P->name() << " ("
                     << proc->coord() << ")\n";
              // all P's ancestors cannot be skipped.
              procedureinfo_set ancestors = P->ancestors();
              for(procedureinfo_set_p anc=ancestors.begin();
                  anc!=ancestors.end(); anc++)
                pointerOptions::Skip_procs.erase((*anc)->proc());
              do_not_skip.insert(P);
              do_not_skip.insert(ancestors.begin(), ancestors.end());
            }
          }
        }
        cout << "Mark skip-proc:\n";
        for(set<procNode*>::iterator p=pointerOptions::Skip_procs.begin();
            p!=pointerOptions::Skip_procs.end(); p++)
          cout << " + " << (*p)->decl()->name() << " (" << (*p)->coord()
               << ")\n";
      } // Slice_analysis

    }
  }

  // -- Clear the tested objects set

  _tested_objects.clear();

  _error_procedures.clear();
}

double enumPropertyAnn::compute_accuracy(Analyzer * analyzer)
{
  // -- Collect the information by statement

  stmt_accuracy_map by_statement;

  for (accuracy_map_p p = _accuracy.begin();
       p != _accuracy.end();
       ++p)
    {
      procLocation * where = (*p).first;
      count_accuracy_pair & cap = (*p).second;

      // -- Count the number of ways to reach this location. First, we move
      // up the call chain to skip over any context-sensitive procedures.

      procLocation * cur = where;

      while (cur->stmt_location())
        cur = cur->stmt_location()->block_location()->proc_location();

      // -- Look up the procedure

      procedureInfo * info = analyzer->lookup_procedure(cur->proc());

      // -- Now get the context count

      int count = info->count_calling_contexts();

      // -- Construct accuracy information that takes into account the
      // number of contexts.

      count_accuracy_pair new_cap;

      new_cap.first  = cap.first * count;
      new_cap.second = cap.second * ((double)count);

      // -- Get the statement itself

      stmtNode * stmt = where->stmt_location()->stmt();

      // -- Add the values in to the total

      stmt_accuracy_map_p w = by_statement.find(stmt);
      if (w == by_statement.end())
        by_statement[stmt] = new_cap;
      else {
        count_accuracy_pair & existing_cap = (*w).second;

        existing_cap.first  += new_cap.first;
        existing_cap.second += new_cap.second;
      }
    }

  // -- Print out the results, according to the call site and compute
  // aggregate accuracy

  double total = 0.0;
  int count = 0;

  double error_total = 0.0;
  int error_count = 0;

  for (stmt_accuracy_map_p p = by_statement.begin();
         p != by_statement.end();
       ++p)
    {
      stmtNode * stmt = (*p).first;
      count_accuracy_pair & cap = (*p).second;

      // TB remove: callNode * call = findCallNodeWalker::find(stmt);

      double local_accuracy = (cap.second / ((double) cap.first));

      // cout << "ACCURACY: " << name() <<  " at " << call->coord() << " = " << local_accuracy << endl;
      cout << "ACCURACY: " << name() <<  " at " << stmt->coord() << " = " << local_accuracy << endl;

      count++;
      total += local_accuracy;

      // -- Sum up error accuracy separately

      if (_error_statements.find(stmt) != _error_statements.end()) {
        error_count++;
        error_total += local_accuracy;
      }
    }

  double overall_accuracy = 0.0;

  if (count > 0) {

    overall_accuracy = (total / ((double)count));

    cout << "STAT-accuracy-property-all-" << name() << " " << overall_accuracy << endl;
  }

  if (error_count > 0)
    cout << "STAT-accuracy-property-errors-" << name() << " " << (error_total / ((double)error_count)) << endl;

  // -- Clear information

  _accuracy.clear();
  _error_statements.clear();

  return overall_accuracy;
}

/** @brief Add flow-sensitive object
 *
 * Record that in the future, this property block should be flow
 * sensitive. */

void enumPropertyAnn::add_flow_sensitive_object(memoryBlock * property_block)
{
  // -- Add it to the local list, keying off the real block

  memoryBlock * real_block = property_block->container();

  real_block->add_to_flow_sensitive_list(_flow_sensitive_objects);
}

/** @brief Set flow sensitivity
 *
 * For a given real blocks, see if it's property block for this property
 * needs to be flow sensitive. Called by
 * Analyzer::lookup_property_block(). */

void enumPropertyAnn::set_flow_sensitivity(memoryBlock * real_block)
{
  // -- Set flow sensitivity on the property block

  memoryBlock * property_block = lookup_property_block(real_block);

  // -- Figure out how to set flow sensitivity

  if (Annotations::Adaptivity) {

    // -- Look the in the property-speicif flow-sensitive list for the real
    // block

    if (real_block->is_in_flow_sensitive_list(_flow_sensitive_objects)) {

      // -- Found it: make the property block flow sensitive

      property_block->set_flow_sensitive();
    }
    else {

      // -- Not found: inherit from the real_block

      if (real_block->is_flow_sensitive())
        property_block->set_flow_sensitive();
      else
        property_block->set_flow_insensitive();
    }
  }
  else {

    // -- Other modes...

    if (Annotations::Flow_insensitive_properties)
      property_block->set_flow_insensitive();
    else
      property_block->set_flow_sensitive();
  }
}


/** @brief Track destructive assignments
 */

void enumPropertyAnn::track_destructive_assignments(Analyzer * analyzer,
                                                    Location * target,
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
                                                    string & indent)
{
  if(pointerOptions::Slice_analysis) {
    memoryBlock *B = block;
    do {
      procedureInfo *info;
      if(B->local_to()) {
        info = analyzer->lookup_procedure(B->local_to());
        pointerOptions::No_Skip_procs.insert(info);
      }
      if(B->allocation_site()) {
        procNode *alloc_proc = Location::procedure(B->allocation_site())
                               ->proc();
        info = analyzer->lookup_procedure(alloc_proc);
        pointerOptions::No_Skip_procs.insert(info);
      }
      B = B->container();
    } while(B);
  }

  block_loc_pair blp(block,target);

  bool seen = (already_seen.find(blp) != already_seen.end());
  bool seen_in_stack = (seen_stack.find(blp) != seen_stack.end());

  if(! TB_changes ||
     ! block->is_flow_sensitive() || // 20030915
     Annotations::Adapt_precision == Annotations::FICI ||
     Annotations::Adapt_precision == Annotations::FICS) {
    if(!seen) {
      for(block_loc_set::iterator s=already_seen.begin(); s!=already_seen.end();
          s++)
        if(s->first == block) { seen=true; break; }
      for(block_loc_set::iterator s=seen_stack.begin(); s!=seen_stack.end();
          s++)
        if(s->first == block) { seen_in_stack=true; break; }
    }
  }
  assert(! seen_in_stack);

  if (seen) {

    // -- Already seen this block

    // -- Check to see if it was made flow sensitive, return the bool as
    // needed

    bool fs = (made_flow_sensitive.find(block) != made_flow_sensitive.end());

    bool cs = false;
    if(TB_changes) {
    // TB: the same for context sensitivity
    cs = (made_context_sensitive.find(blp) != made_context_sensitive.end());
    }

    if (Annotations::Show_adaptivity) {
      if (fs)
        cout << indent << "+ (Already made FS: block " << block->name() << ")" << endl;
      if(TB_changes) {
      if (cs)
        cout << indent << "+ (Already made CS: block " << block->name() << ")" << endl;
      if (!fs && !cs)
        cout << indent << "+ (Already seen: block " << block->name() << ")" << endl;
      }
    }
#if 0
{ // begin debug
cout << block->name() << "   destructive_assignments: \n";
const memoryBlock::destructive_assignment_map & destructive = block->destructive_assignments();
for (memoryBlock::destructive_assignment_map_cp p = destructive.begin();
     p != destructive.end(); ++p)
  cout << "      " << * (*p).first << endl;
cout << block->name() << "   complicit_assignments: \n";
const memoryBlock::complicit_assignment_map & complicit = block->complicit_assignments();
for (memoryBlock::complicit_assignment_map_cp p = complicit.begin();
     p != complicit.end(); ++p) {
  cout << "      " << * (*p).first << /*" " << p->second <<" "<< p->second->name()
       << * / endl;
  / * serious: make it CS
  if(! disallow_context_sensitivity) {
    procLocation * procloc = Location::procedure(p->first);
    procNode * proc = procloc->proc();
    procedureInfo * info = analyzer->lookup_procedure(proc);
    evaluate_context_sensitivity(analyzer, target, block, already_seen,
                                 procloc, indent);
  } * /
}
} // end debug
*/
#endif

    make_chain_flow_sensitive = fs;
    if(TB_changes)
    make_chain_context_sensitive = cs;

    return;
  }

  already_seen.insert(blp);
if (Annotations::Show_adaptivity) {
cout << "stack: ";
for(block_loc_set::iterator s=seen_stack.begin(); s!=seen_stack.end(); s++)
  cout << s->first << ",";
cout << " add " << block << endl;
}
  seen_stack.insert(blp);

  if (Annotations::Show_adaptivity)
    cout << indent << "+ Block " << block->name() << ",  target @ " << *target
         << endl;

  // -- Get the destructive assignment list

  const memoryBlock::destructive_assignment_map & destructive = block->destructive_assignments();

  // -- Make recommendations for any destructive assignments

  if (Annotations::Show_adaptivity)
    if (destructive.empty())
      cout << indent << " (No destructive assignments)" << endl;

  bool already_made_FS = // TB
    made_flow_sensitive.find(block) != made_flow_sensitive.end();
  bool make_flow_sensitive = false;
  bool make_context_sensitive = false;
  bool validated = false;

  for (memoryBlock::destructive_assignment_map_cp p = destructive.begin();
       p != destructive.end();
       ++p)
    {
      Location * where = (*p).first;

      if(pointerOptions::Slice_analysis) {
        procNode *proc = Location::procedure(where)->proc();
        if(analyzer->annotations()->lookup_procedure(proc->decl()->name()))
          proc = Location::procedure(Location::procedure(where)->parent())
                 ->proc();
        procedureInfo *info = analyzer->lookup_procedure(proc);
        pointerOptions::No_Skip_procs.insert(info);
      }

      if(TB_changes && Annotations::Aggressive_pruning) {
        if(! is_location_reachable(where, target, block)) continue;
      }

      memoryBlock::DestructiveKind kind = (*p).second;

      switch (kind) {

      case memoryBlock::Control_flow:
        {
          if (Annotations::Show_adaptivity)
            cout << indent << "  --> needs path sensitivity at " << *where << endl;

          // -- Find and record the controlling branch

          basicblockLocation * basicblock_loc = (basicblockLocation *) where;
          basicblockNode * basicblock = basicblock_loc->block();

          pointerOptions::Path_sensitive_branches.insert(basicblock);

          // make_flow_sensitive = true;
        }
        break;

      case memoryBlock::Parameter_pass:
        {
          procLocation * procloc = 0;

          // -- Make sure to handle external outputs correctly

          if (where->kind() == Location::Statement) {

            // -- External outputs are assigned at the call site

            stmtLocation * callsite = (stmtLocation *) where;
            procloc = callsite->calls();
          }
          else {

            // -- Inputs

            procloc = Location::procedure(where);
            if ( ! disallow_context_sensitivity) {
              bool m = evaluate_context_sensitivity(analyzer, target, block,
                                                    /*already_seen,*/ procloc,
                                                    true,
                                                    eval_cs_seen_destructive,
                                                    eval_cs_seen_complicit,
                                                    eval_made_cs, indent);
              make_context_sensitive = make_context_sensitive || m;
if (Annotations::Show_adaptivity) // debug
  cout<<"Parameter_pass(" << block->name() << ") make_context_sensitive("
      << procloc->proc()->decl()->name() <<")="<< make_context_sensitive<<endl;
              if(m && Annotations::Adaptor_statistics)
                Annotations::Adaptor_statistics->make_CS
                                (block,
                                 analyzer->lookup_procedure(procloc->proc()),
                                 where,
                                 memoryBlock::Parameter_pass);
            }
          }
        }
        break;

      case memoryBlock::Weak_update:
        {
          if (! already_made_FS) {
            if (Annotations::Show_adaptivity) {
              cout << indent << "  --> needs lower multiplicity at " << *where << endl;
              cout << indent << "      (allocation site = " << block->allocation_site()
                   << ", alloc_object = " << block->allocation_object() << endl;
            }

            // -- If it's a heap object, make it flow-sensitive

            if (block->is_heap_object())
{
if (Annotations::Show_adaptivity) // debug
cout << "Weak_update " << block << endl;
              make_flow_sensitive = true;
}
          }
          if ( TB_changes && ! disallow_context_sensitivity) {
            procLocation *procloc = Location::procedure(where);
block_proc_pair p(block, procloc->proc()); // debug
bool previously_cs = eval_made_cs.find(p) != eval_made_cs.end();
            bool m = evaluate_context_sensitivity(analyzer, target, block,
                                                  /*already_seen,*/ procloc,
                                                  true,
                                                  eval_cs_seen_destructive,
                                                  eval_cs_seen_complicit,
                                                  eval_made_cs, indent);
            make_context_sensitive = make_context_sensitive || m;
            // is CS due to weak-update possible?
if(! m || previously_cs ||
   analyzer->annotations()->lookup_procedure(procloc->proc()->decl()->name()))
; else {
  cout << *where << endl;
  cout << block << " " << block->name() << " proc="
       << procloc->proc()->decl()->name() << endl;
}
assert(! m || previously_cs ||
    analyzer->annotations()->lookup_procedure(procloc->proc()->decl()->name()));
            if(m && Annotations::Adaptor_statistics)
              Annotations::Adaptor_statistics->make_CS
                                (block,
                                 analyzer->lookup_procedure(procloc->proc()),
                                 where,
                                 memoryBlock::Weak_update);
          }
        }
        break;

      case memoryBlock::Additive:

        if (! already_made_FS) {
          if (Annotations::Show_adaptivity)
            cout << indent << "  --> needs flow sensitivity at " << *where << endl;

if (Annotations::Show_adaptivity) // debug
cout << "Additive " << block << endl;
          make_flow_sensitive = true;
        }


        /*
        if ( ! block->property) {

          // -- It's a pointer, make sure FS helps:

          if (where->kind() == Location::Statement) {

            // -- External outputs are assigned at the call site

            stmtLocation * assignment_loc = (stmtLocation *) where;
            make_flow_sensitive = validate_pointer_fs(block, assignment_loc, indent);
          }
        }
        */

        break;

      default:
        if (Annotations::Show_adaptivity)
          cout << indent << "  --> cause unknown at " << * where << endl;
      }
    }
  if(make_flow_sensitive)
    made_fs_destructive.insert(block); // make FS due to destructive assignments
  // TB
  if(make_context_sensitive)
    made_context_sensitive.insert(blp);

  // -- Special case: for flow-insensitive heap objects, first try making
  // them flow sensitive before going crazy:

  bool throttle_context_sensitivity = disallow_context_sensitivity;

  /*
  if (block->is_heap_object() &&
      ! block->is_flow_sensitive() &&
      make_flow_sensitive) {

    throttle_context_sensitivity = true;
  }
  */

  if (Annotations::Show_adaptivity)
    cout << indent << " CS control: disallow = " << disallow_context_sensitivity
         << ", throttle = " << throttle_context_sensitivity << endl;

  // -- Follow all the complicit assignments...

  const memoryBlock::complicit_assignment_map & complicit = block->complicit_assignments();

  // -- Push myself on the chain

  chain.push_back(block);

  if (!already_made_FS && make_flow_sensitive) {

    // -- Make the whole chain flow-sensitive

    for (memoryblock_vector_p p = chain.begin();
         p != chain.end();
         ++p)
      {
        memoryBlock * make_fs_block = (*p);

        // -- Avoid making blocks flow sensitive if they have only one
        // assignment:

        if ( ! make_fs_block->is_single_assignment()) {

          if (Annotations::Show_adaptivity)
            cout << indent << " --> set flow-sensitive " << make_fs_block->name() << endl;

/*if (make_fs_block->name() == "upper::s")
    make_fs_block->print_def_use(cout);*/

          // -- Make the block flow-sensitive: this depends on whether it is a
          // property block or a real block.

          if (make_fs_block->property) {

            // -- Make a property block flow sensitive. NOTE: it might not be
            // this property!

            make_fs_block->property->add_flow_sensitive_object(make_fs_block);
          }
          else {

            // -- Regular object: make it flow sensitive in the global list

            make_fs_block->add_to_flow_sensitive_list(pointerOptions::Flow_sensitive_objects);
          }

          if (make_fs_block->is_heap_object()) {
            memoryBlock * alloc_object = make_fs_block->allocation_object();
            alloc_object->add_to_flow_sensitive_list(pointerOptions::Flow_sensitive_allocation_objects);
          }

          // -- Keep a list of objects that we make flow sensitive

          made_flow_sensitive.insert(make_fs_block);
        }
        else {

          if (Annotations::Show_adaptivity)
            cout << indent << " --> skip single-assign " << make_fs_block->name() << endl;
        }
      }
  }

  // -- Visit the complicit objects

  if (Annotations::Show_adaptivity)
    if (complicit.empty())
      cout << indent << " (No complicit assignments)" << endl;

  string new_indent = indent + "    ";

  for (memoryBlock::complicit_assignment_map_cp p = complicit.begin();
       p != complicit.end();
       ++p)
    {
      Location * where = (*p).first;
      const memoryblock_set & blocks = (*p).second;

      if(pointerOptions::Slice_analysis) {
        procNode *proc = Location::procedure(where)->proc();
        if(analyzer->annotations()->lookup_procedure(proc->decl()->name()))
          proc = Location::procedure(Location::procedure(where)->parent())
                 ->proc();
        procedureInfo *info = analyzer->lookup_procedure(proc);
        pointerOptions::No_Skip_procs.insert(info);
      }

      for (memoryblock_set_cp q = blocks.begin();
           q != blocks.end();
           ++q)
        {
          memoryBlock * comp = *q;

          // TB_unify
          if(/*Annotations::Adapt_uses_uba &&*/ comp == block) continue;

          if (Annotations::Show_adaptivity)
            cout << indent << "  - Complicit at " << *where
                 << ": assignment \"" << block->name() << " = " << comp->name() << "\"" << endl;

          // TB 20030725 short-circuit
          { block_loc_pair sp(comp,target);
            seen = (already_seen.find(sp) != already_seen.end());
            seen_in_stack = (seen_stack.find(sp) != seen_stack.end());
            if(! TB_changes ||
               ! comp->is_flow_sensitive() || // 20030915
               Annotations::Adapt_precision == Annotations::FICI ||
               Annotations::Adapt_precision == Annotations::FICS) {
              if(!seen) {
                for(block_loc_set::iterator s=already_seen.begin();
                    s!=already_seen.end(); s++)
                  if(s->first == comp) { seen=true; break; }
                for(block_loc_set::iterator s=seen_stack.begin();
                    s!=seen_stack.end(); s++)
                  if(s->first == comp) { seen_in_stack=true; break; }
              }
            }

            if (seen) {
if (Annotations::Show_adaptivity)
cout << block << " seen complicit comp=" << comp << " " << comp->name() << endl;
              // 20030729 need to set some variables before short-circuit
              if(made_flow_sensitive.find(comp) != made_flow_sensitive.end()) {
                // 20030827
                if(seen_in_stack &&
                   made_fs_destructive.find(comp) != made_fs_destructive.end())
                  make_flow_sensitive = true;
              }
              if(made_context_sensitive.find(sp)
                 != made_context_sensitive.end()) {
                if(!seen_in_stack ||
                   pointerOptions::Recursion_Context_sensitive) {
                  bool m = false; // 20030904
                  if ( /*!seen_in_stack &&*/ ! disallow_context_sensitivity) {
                    procLocation * procloc = Location::procedure(where);
                    procNode * proc = procloc->proc();
                    procedureInfo * info = analyzer->lookup_procedure(proc);
                    m = evaluate_context_sensitivity(analyzer, target,
                                                   block, // comp /*TB block*/ ,
                                                   /*already_seen,*/ procloc,
                                                   false,
                                                   eval_cs_seen_destructive,
                                                   eval_cs_seen_complicit,
                                                   eval_made_cs, indent);
                    if(m && Annotations::Adaptor_statistics)
                      Annotations::Adaptor_statistics->make_CS
                                  (block,
                                   analyzer->lookup_procedure(procloc->proc()),
                                   where,
                                   comp);
                  }
                  make_context_sensitive = make_context_sensitive || m;
                }
              }
              continue;
            }
          }

          if (comp == block) {

            if (Annotations::Show_adaptivity) {

              cout << indent << "    + SKIP self-assignment." << endl;
            }
          }
          else {

            // -- Test reachability

            bool reachable = true;

            if (Annotations::Aggressive_pruning)
              reachable = is_location_reachable(where, target, comp);

            /*
            if (Annotations::Show_adaptivity) {
              if (reachable)
                cout << indent << "    [ " << * target << " is reachable from " << * where << " ]" << endl;
              else
                cout << indent << "    [ " << * target << " is NOT reachable from " << * where << " ]" << endl;
            }
            */

            if (reachable) {
              if(make_context_sensitive) // 20030912
                made_context_sensitive.insert(blp);

              bool chain_flow_sensitivity = false;
              bool chain_context_sensitivity = false;

              track_destructive_assignments(analyzer, target /* where */, comp,
                                            throttle_context_sensitivity,
                                            already_seen, seen_stack, made_flow_sensitive,
                                            made_fs_destructive,
                                            made_context_sensitive,
                                            eval_cs_seen_destructive,
                                            eval_cs_seen_complicit,
                                            eval_made_cs,
                                            chain_flow_sensitivity,
                                            chain_context_sensitivity,
                                            chain,
                                            new_indent);
              if (chain_flow_sensitivity) {
if (Annotations::Show_adaptivity)
cout << "chain " << block << " comp=" << comp << endl;
                make_flow_sensitive = true;
                /*
                if ( ! block->property) {

                  // -- It's a pointer, make sure FS helps:

                  if (where->kind() == Location::Statement) {

                    // -- External outputs are assigned at the call site

                    stmtLocation * assignment_loc = (stmtLocation *) where;
                    make_flow_sensitive = validate_pointer_fs(block, assignment_loc, indent);
                  }
                }
                */
              }

              if (chain_context_sensitivity) {

                // -- First, make sure it would help:

                bool m = false;
                procLocation * procloc = Location::procedure(where);
                procNode * proc = procloc->proc();
                if ( ! disallow_context_sensitivity) {
                  procedureInfo * info = analyzer->lookup_procedure(proc);

                  m = evaluate_context_sensitivity(analyzer, target,
                                                 block, // comp /*TB block*/ ,
                                                 /*already_seen,*/ procloc,
                                                 false,
                                                 eval_cs_seen_destructive,
                                                 eval_cs_seen_complicit,
                                                 eval_made_cs, indent);
                  if(m && Annotations::Adaptor_statistics)
                    Annotations::Adaptor_statistics->make_CS
                                (block,
                                 analyzer->lookup_procedure(procloc->proc()),
                                 where,
                                 comp);
                }

                if(TB_changes)
                make_context_sensitive = make_context_sensitive || m; // TB
                else
                make_context_sensitive = m;
                if (m) {

                  // -- Chain context-sensitivity: make the procedure
                  // containing the complicit assignment context-sensitive.

                  if (Annotations::Show_adaptivity)
                    cout << indent << "  --> Chain context-sensitivity of " << proc->decl()->name()
                         << " for assignment \"" << block->name() << " = " << comp->name() << "\"" << endl;

                  /* Handled inside evaluate_context_sensitivity...
                     if ( ! info->is_library_routine())
                     pointerOptions::Context_sensitive_procedures.insert(info->name());
                  */
                }
              }
            }
          }
        }
    }

  if (!already_made_FS && make_flow_sensitive) {

    // -- Avoid making blocks flow sensitive if they have only one
    // assignment:

    if ( ! block->is_single_assignment()) {

      if (Annotations::Show_adaptivity)
        cout << indent << " --> set flow-sensitive " << block->name() << endl;

/*if (block->name() == "upper::s")
  block->print_def_use(cout); */

      // -- Make the block flow-sensitive: this depends on whether it is a
      // property block or a real block.

      if (block->property) {

        // -- Make a property block flow sensitive. NOTE: it might not be
        // this property!

        block->property->add_flow_sensitive_object(block);
      }
      else {

        // -- Regular object: make it flow sensitive in the global list

        block->add_to_flow_sensitive_list(pointerOptions::Flow_sensitive_objects);
      }

      if (block->is_heap_object()) {
        memoryBlock * alloc_object = block->allocation_object();
        alloc_object->add_to_flow_sensitive_list(pointerOptions::Flow_sensitive_allocation_objects);
      }

      // -- Keep a list of objects that we make flow sensitive

      made_flow_sensitive.insert(block);
    }
    else {

      if (Annotations::Show_adaptivity)
        cout << indent << " --> skip single-assign " << block->name() << endl;
    }
  }

  // -- Pop the chain

  chain.pop_back();

  if (destructive.empty() && complicit.empty()) {
    // TB: it is possible that we are currently in some annotated function, and
    // if so, the block is some block created by the function. We want to treat
    // the function as CS.
    procLocation * procloc = Location::procedure(target);
    procNode * proc = procloc->proc();
    if(analyzer->annotations()->lookup_procedure(proc->decl()->name()))
      make_context_sensitive = true;
  }

  // -- Pass the chain values back.

  if(TB_changes)
    make_chain_flow_sensitive = already_made_FS || make_flow_sensitive;
  else
    make_chain_flow_sensitive = make_flow_sensitive;
  make_chain_context_sensitive = make_context_sensitive;
  // TB
  if(make_context_sensitive)
    made_context_sensitive.insert(blp);
if (Annotations::Show_adaptivity) // debug
cout << indent << "--> " << block->name()
     << " make_chain_context_sensitive = make_context_sensitive = "
     << make_context_sensitive << endl; // debug
}

/** @brief Record tested objects
 *
 * This method is called by the test functions to record which object are
 * tested. It also sets up any complicit assignments that start the
 * adaptation process. */

void enumPropertyAnn::record_tested_objects(Location * where,
                                            pointerValue & ptr,
                                            enumvalue_set & value_set,
                                            memoryblock_set & complicit_property_blocks)
{
  if ((Annotations::Adaptivity == 3) &&
      Annotations::Record_tested_objects) {

    // -- Remember that these objects were tested

    for (memoryblock_set_p q = complicit_property_blocks.begin();
         q != complicit_property_blocks.end();
         ++q)
      {
        memoryBlock * property_block = *q;
        memoryBlock * real_block = property_block->container();

        _tested_objects[where].insert(real_block);
      }

    if (value_set.count() > 1) {

      // -- If constructing the value lost information, then record that fact

      for (memoryblock_set_p p = ptr.blocks.begin();
           p != ptr.blocks.end();
           ++p)
        {
          memoryBlock * real_block = *p;
          memoryBlock * property_block = lookup_property_block(real_block);

          property_block->add_complicit_assignment(where, ptr.dereferenced);
        }
    }
  }
}


/** @brief Trace object
 *
 * Trace the assignments to a property, generate a report. */

void enumPropertyAnn::trace_object(ostream & out,
                                   memoryBlock * property_block,
                                   memoryblock_set & already_seen,
                                   string & indent)
{
  // -- Check for cycles

  if (already_seen.find(property_block) != already_seen.end()) {
    out << indent << " (already seen)" << endl;
    return;
  }

  already_seen.insert(property_block);

  // -- Get the destructive assignment list

  const memoryBlock::destructive_assignment_map & destructive = property_block->destructive_assignments();

  // -- Get the complicit assignment list

  const memoryBlock::complicit_assignment_map & complicit = property_block->complicit_assignments();

  // -- Visit all the defs and report

  const memorydef_list defs = property_block->defs();
  for (memorydef_list_cp mp = defs.begin();
       mp != defs.end();
       ++mp)
    {
      memorydef_key mlp = (*mp);
      Location * where = mlp.location;
      memoryDef * def = mlp.def;

      enumvalue_set val = lookup_now_value(property_block, def);

      out << indent << " + ";

      // -- Was it destructive?

      memoryBlock::destructive_assignment_map_cp dp = destructive.find(where);
      if (dp != destructive.end()) {
        memoryBlock::DestructiveKind kind = (*dp).second;
        switch (kind) {
        case memoryBlock::Control_flow:   out << "[D: phi]";
          break;
        case memoryBlock::Parameter_pass: out << "[D: par]";
          break;
        case memoryBlock::Weak_update:    out << "[D: mul]";
          break;
        case memoryBlock::Additive:       out << "[D: fi ]";
          break;
        default:
          break;
        }
      }

      // -- Print out the location and value

      out << " Def at " << * (def->where()) << " = " << to_string(val) << endl;

      // -- Was it complicit?

      memoryBlock::complicit_assignment_map_cp cp = complicit.find(where);
      if (cp != complicit.end()) {

        string new_indent = indent + "    ";

        const memoryblock_set & blocks = (*cp).second;

        for (memoryblock_set_cp q = blocks.begin();
             q != blocks.end();
             ++q)
          {
            memoryBlock * cblock = (*q);

            if (cblock->property == this) {
              out << indent << "    = " << cblock->name() << endl;

              trace_object(out, cblock, already_seen, new_indent);
            }
            else
              out << indent << "    Complicit: " << cblock->name() << endl;
          }
      }
    }
}

/** @brief Make a procedure context sensitive
 *
 * Includes all the code to prune out unnecessary cases. Returns true if
 * it actually made the procedure context sensitive.*/

bool enumPropertyAnn::evaluate_context_sensitivity(Analyzer * analyzer,
                                                   Location * target,
                                                   memoryBlock * block,
                                                   procLocation * procloc,
                                                   bool destructive,
                                                   block_proc_set & eval_cs_seen_destructive,
                                                   block_proc_set & eval_cs_seen_complicit,
                                                   block_proc_set & evaled_cs,
                                                   string & indent)
{
  procNode * proc = procloc->proc();

  block_proc_pair p(block, proc); // TB
  if(evaled_cs.find(p) != evaled_cs.end()) {
    if (Annotations::Show_adaptivity)
      cout << indent << block->name() << " already made "
           << proc->decl()->name() << " CS.\n";
    return true;
  }
  if(destructive) {
    if(eval_cs_seen_destructive.find(p) != eval_cs_seen_destructive.end()) {
      if (Annotations::Show_adaptivity)
        cout << indent << block->name() << " already seen (destructive) "
             << proc->decl()->name() << ", not made CS.\n";
      return false;
    }
    eval_cs_seen_destructive.insert(p);
  } else {
    if(eval_cs_seen_complicit.find(p) != eval_cs_seen_complicit.end()) {
      if (Annotations::Show_adaptivity)
        cout << indent << block->name() << " already seen (complicit) "
             << proc->decl()->name() << ", not made CS.\n";
      return false;
    }
    eval_cs_seen_complicit.insert(p);
  }

  bool make_context_sensitive = false;

  if (Annotations::Show_adaptivity)
    cout << indent << "  --> procedure " << proc->decl()->name() << " needs context-sensitivity;"
         << " parameter passed at " << *procloc << endl;
if (Annotations::Show_adaptivity) // debug
cout << "block is " << block << " " << block->name() << endl;

  // -- Retrieve the reaching defs that reach this call

  if (Annotations::Show_adaptivity && (destructive || !TB_changes))
    cout << indent << "    Callsites:" << endl;

  /*if(block->property && ! block->is_flow_sensitive())
    assert( block->parameter_assignments(proc).empty() ); // ?? debug*/

  const callsite_objects_map & assignments =
    /*(block->property && ! block->is_flow_sensitive()) ?
      block->container()->parameter_assignments(proc) : */
      block->parameter_assignments(proc);

  // -- Get the procedureInfo for this procedure

  procedureInfo * info = analyzer->lookup_procedure(proc);

  // -- Validate the decision to make this procedure context sensitive

  if (Annotations::Prune_context_sensitive) {

    // -- Prune out procedures by re-checking their inputs to
    // make sure what context sensitivity would yield additional
    // accuracy.

    if(analyzer->annotations()->lookup_procedure(proc->decl()->name())) {
      // TB annotated lib function, already context-sensitive, leave it.
      return true;

    } else

    // -- Special case: if there is only one callsite, then
    // context sensitivity cannot possibly help.

    if (assignments.empty()) {
      // TB: block is not even a parameter to this call.
      make_context_sensitive = false;
      if (Annotations::Show_adaptivity)
        cout << indent << "    ==> (No callsite: leave it context-insensitive)" << endl;

    }/* else if (assignments.size() <= 1) {

      // TB new condition
      bool is_recursive_context = info->is_recursive();
      if(! is_recursive_context && assignments.size()==1) {
        // check if the assignment occurs in some recursive function
        procLocation *caller = Location::procedure(assignments.begin()->first);
        is_recursive_context =
          analyzer->lookup_procedure(caller->proc())->is_recursive();
cout << "is_recursive_context = " << is_recursive_context << " caller is "
     << *caller << endl;
      }
      if(! is_recursive_context) {
cout << "#assignments = " << assignments.size() << endl;

        make_context_sensitive = false;

        if (Annotations::Show_adaptivity)
          cout << indent << "    ==> (One callsite: leave it context-insensitive)" << endl;
      } else
        make_context_sensitive = true; // TB new
    } */
    else {

      if (info->is_recursive() &&
          !pointerOptions::Recursion_Context_sensitive /* TB */ ) {

        // Corollary: since we can't make recursive procedures
        // context-sensitive, don't bother testing them.

        make_context_sensitive = false;

        if (Annotations::Show_adaptivity)
          cout << indent << "    ==> (Recursive: leave it context-insensitive)" << endl;
      }
      else {

        make_context_sensitive = true;

        // -- Re-check for information loss depending on the kind of block:

        if(destructive || !TB_changes) { // TB 20030910
          if (block->property) {

            // -- Property block

            make_context_sensitive = validate_property_cs(block, assignments,
                                                          info, indent);
          }
          else
            if (block->is_allocation_object()) {

              // -- Allocation object: check multiplicity

              make_context_sensitive=validate_multiplicity_cs(block,assignments,
                                                              info, indent);
            }
            else {

              // -- Regular pointer object: count points-to sets

              make_context_sensitive = validate_pointer_cs(block, assignments,
                                                           info, indent);
            }
        } else
          if (Annotations::Show_adaptivity)
            cout << indent << "    complicit, no validation done\n";
      }
    }
  }
  else
    make_context_sensitive = true;

  // -- Error-oriented mode: don't make ancestors of the target
  // context-sensitive. (We could, but doing so only improves the precision,
  // but does not reduce number of false positives.) This only works when the
  // error check only involves a single object or property. Otherwise, it can
  // miss cases where particular combinations cause an error.

  if( make_context_sensitive &&
     (Annotations::Error_oriented_adaptivity ||
      (Annotations::Num_enabled_properties == 1))) {

    // -- Get the procedureInfo of the procedure where the error
    // occured. Note that the target is the location within the library
    // call, so we need to get to the caller first.

    bool is_ancestor = false;

    procLocation * target_proc_loc = Location::procedure(Location::procedure(target)->stmt_location());
    procNode * target_proc = target_proc_loc->proc();
    procedureInfo * target_info = analyzer->lookup_procedure(target_proc);

    /* 20030903
    for (procedureinfo_set_p pi = _error_procedures.begin();
         pi != _error_procedures.end();
         ++pi)
      {
        rocedureInfo * target_info = *pi; */

        if (Annotations::Show_adaptivity)
          cout << indent << "      Error-oriented check: is " << info->name() << " an ancestor of "
               << target_info->name() << endl;

        // -- See if the current procedure is an ancestor of the target

        const procedureinfo_set & ancestors = target_info->ancestors();
        if ((info == target_info) ||
            (ancestors.find(info) != ancestors.end()))
          is_ancestor = true;
        /* TB
        if ( make_context_sensitive && ! target_info->is_library_routine() &&
             pointerOptions::Recursion_Context_sensitive) {
          const procedureinfo_set & ancestors1 = info->ancestors();
          if ((info == target_info) ||
              (ancestors.find(target_info) != ancestors.end()))
            // recursive or mutual recursive
            pointerOptions::Context_sensitive_procedures.
              insert(target_info->name());
        } */
      // }

    if (is_ancestor /*&& TB !pointerOptions::Recursion_Context_sensitive */) {

      // -- It is an ancestor, so override the context-sensitivity

      make_context_sensitive = false;

      if (Annotations::Show_adaptivity)
        cout << " ... Yes, override CS" << endl;
    }
    else
      if (Annotations::Show_adaptivity)
        cout << " ... No" << endl;
  }

  // -- Make the procedure context-sensitive

  // make_flow_sensitive = true;

  if (make_context_sensitive) {
    evaled_cs.insert(p); // TB

    // -- Make descendants CS

    // -- Collect the set of descendants in the call graph

    procedureinfo_set already_seen;
    procedureinfo_set descendants;

    if (0) {

      add_context_sensitive_proc(info, already_seen, descendants);

      if (Annotations::Show_adaptivity)
        cout << indent << "      * Procedure " << proc->decl()->name()
             << " has " << descendants.size() << " descendants." << endl;
    }

    if ( ! info->is_library_routine())
      pointerOptions::Context_sensitive_procedures.insert(info->name());


    if (0) {

      if (1) {

        // -- New idea: make context-sensitive any descendant that
        // modifies the value of the given block.

        for (procedureinfo_set_p pp = descendants.begin();
             pp != descendants.end();
             ++pp)
          {
            procedureInfo * des = *pp;
            const string & name = (*pp)->proc()->decl()->name();

            // -- Get the external outputs of the descendant

            const memoryblock_set & outputs = des->external_outputs();

            // -- If the current block is among them, then make the
            // descendant context-sensitive.

            if (outputs.find(block) != outputs.end()) {
              if (Annotations::Show_adaptivity)
                cout << indent << "        + " << name << " modifies " << block->name() << endl;
              pointerOptions::Context_sensitive_procedures.insert(name);
            }
          }
      }
      else {

        // -- Build a set that represents all procedures in this
        // calling sequence: the union of the ancestors and
        // descendants of the procedure.

        procedureinfo_set subtree = info->ancestors();
        subtree.insert(info);
        subtree.insert(descendants.begin(),
                       descendants.end());

        // -- Special test: only make a descendant CS if it can be
        // called outside this subtree.

        typedef list< procedureInfo * > procedureinfo_list;
        typedef procedureinfo_list::iterator procedureinfo_list_p;

        for (procedureinfo_set_p pp = descendants.begin();
             pp != descendants.end();
             ++pp)
          {
            procedureInfo * des = *pp;
            const string & name = (*pp)->proc()->decl()->name();

            // -- Compute the set of procedures that are in the
            // ancestors set of descendant, but not in the subtree.

            const procedureinfo_set & des_ancestors = des->ancestors();

            procedureinfo_list diffs;
            set_difference(des_ancestors.begin(), des_ancestors.end(),
                           subtree.begin(), subtree.end(),
                           back_inserter(diffs));

            if (Annotations::Show_adaptivity) {
              cout << indent << "        + " << name << " has "
                   << diffs.size() << " other contexts." << endl;
              if (diffs.size() == 1)
                cout << indent << "          - Diff is " << diffs.front()->proc()->decl()->name() << endl;
            }

            // -- If the set is non-empty, then make the descendant CS

            if ( ! diffs.empty())
              pointerOptions::Context_sensitive_procedures.insert(name);
          }
      }
    }
  }
  else
    Annotations::Pruned_procedures.insert(proc->decl()->name());

  return make_context_sensitive;
}

// ------------------------------------------------------------

typedef map< stmtNode *, enumvalue_set > stmt_value_map;
typedef stmt_value_map::iterator stmt_value_map_p;

/** @brief Validate context sensitivity for property
 *
 * */

bool enumPropertyAnn::validate_property_cs(memoryBlock * block,
                                           const callsite_objects_map & assignments,
                                           procedureInfo *procedure, // TB
                                           string & indent)
{
  int min = 999999;
  enumValueAnn * highest_value = 0;

  enumvalue_set collected_vals;

  if (Annotations::Show_adaptivity)
    cout << indent << "    Property validation for block " << block->name() << endl;

  // -- Store the values according to non-context-sensitive callsite

  stmt_value_map stmt_values;

  // -- Visit each callsite

  for (callsite_objects_map_cp rdp = assignments.begin();
       rdp != assignments.end();
       ++rdp)
    {
      stmtLocation * callsite = (*rdp).first;
      procLocation * attach_to = procedure->procedure_location(callsite);
      assert( attach_to ); // debug

      const memoryblock_set & reaching_blocks = (*rdp).second;

      // -- Collect all the values that reach this call site. There may be
      // multiple reaching definitions, so we just combine them

      enumvalue_set vals;

      for (memoryblock_set_cp mdp = reaching_blocks.begin();
           mdp != reaching_blocks.end();
           ++mdp)
        {
          memoryBlock * property_block = (*mdp);

          property_block->set_current_def_use(attach_to);
          enumvalue_set local_val = construct_now_value(attach_to, property_block, false);

          meet_with(vals, local_val);

          if (Annotations::Show_adaptivity) {
            cout << indent << "      Callsite " << * callsite
                 << "  block " << property_block->name()
                 << " = " <<  property_block->property->to_string(local_val) << endl;
          }
        }

      // -- Store according to stmt

      meet_with(stmt_values[callsite->stmt()], vals);
    }

  for (stmt_value_map_p p = stmt_values.begin();
       p != stmt_values.end();
       ++p)
    {
      stmtNode * stmt = (*p).first;
      enumvalue_set vals = (*p).second;

      if (Annotations::Show_adaptivity) {
        cout << indent << "      Statement " << stmt->coord() << " reaching values = "
             << to_string(vals) << endl;
      }

      // -- Add the values from this callsite into the collected set

      meet_with(collected_vals, vals);

      // -- Keep track of the callsite with the best information

      int count = vals.count();
      if ((count > 0) &&
          (count < min)) min = count;
    }

  if (Annotations::Show_adaptivity)
    cout << indent << "    ==> Property " << block->property->name()
         << " : Min = " << min << " , Vals = "
         << to_string(collected_vals) << endl;

  bool result;

  if (collected_vals.count() > min)
    result = true;
  else
    result = false;

  if (Annotations::Show_adaptivity) {
    if (result)
      cout << indent << "      -> Validated" << endl;
    else
      cout << indent << "      -> Prune" << endl;
  }

  return result;
}

/** @brief Validate context sensitivity for multiplicity
 *
 * */

bool enumPropertyAnn::validate_multiplicity_cs(memoryBlock * block,
                                               const callsite_objects_map & assignments,
                                               procedureInfo *procedure, // TB
                                               string & indent)
{
  bool any_single = false;
  bool any_unbounded = false;

  if (Annotations::Show_adaptivity)
    cout << indent << "    Multiplicity validation for block " << block->name() << endl;

  for (callsite_objects_map_cp rdp = assignments.begin();
       rdp != assignments.end();
       ++rdp)
    {
      stmtLocation * callsite = (*rdp).first;
      procLocation * attach_to = procedure->procedure_location(callsite);
      assert( attach_to ); // debug
      const memoryblock_set & reaching_blocks = (*rdp).second;

      // -- Collect all the values that reach this call
      // site: for multiplicity, this should be only one.

      Multiplicity reaching_multiplicity = Unallocated;

      for (memoryblock_set_cp mdp = reaching_blocks.begin();
           mdp != reaching_blocks.end();
           ++mdp)
        {
          memoryBlock * reaching_block = (*mdp);

          reaching_block->set_current_def_use(attach_to);

          memoryUse * use = reaching_block->current_use();
          if (use &&
              use->reaching_def()) {

            memoryDef * reaching_def = use->reaching_def();

            if (Annotations::Show_adaptivity) {
              cout << indent << "      Callsite " << * callsite;
              if (reaching_def)
                cout << " reached by " << * (reaching_def->where()) << endl;
              else
                cout << " (no reaching def)" << endl;
            }

            reaching_multiplicity = reaching_def->multiplicity();

            if (reaching_multiplicity == Unbounded)
              any_unbounded = true;

            if (reaching_multiplicity == Single)
              any_single = true;
          }
        }
    }

  if (Annotations::Show_adaptivity)
    cout << indent << "    ==> Any single = " << any_single
         << ", any unbounded = " << any_unbounded << endl;

  bool result;

  if (any_single && any_unbounded)
    result = true;
  else
    result = false;

  if (Annotations::Show_adaptivity) {
    if (result)
      cout << indent << "      -> Validated" << endl;
    else
      cout << indent << "      -> Prune" << endl;
  }

  return result;
}

/** @brief Validate context sensitivity for regular pointer variable
 *
 * */

typedef map< stmtNode *, memoryblock_set > stmt_memoryblock_set_map;
typedef stmt_memoryblock_set_map::iterator stmt_memoryblock_set_map_p;

bool enumPropertyAnn::validate_pointer_cs(memoryBlock * block,
                                          const callsite_objects_map & assignments,
                                          procedureInfo *procedure,
                                          string & indent)
{
#if 0
    memoryblock_set master;
    bool result = false;

    stmt_memoryblock_set_map stmt_values;

    if (Annotations::Show_adaptivity)
      cout << indent << "    Points-to validation for block " << block->name() << endl;

    for (callsite_objects_map_cp rdp = assignments.begin();
         rdp != assignments.end();
         ++rdp)
      {
        stmtLocation * callsite = (*rdp).first;

        const memoryblock_set & reaching_blocks = (*rdp).second;

        // -- Find the points-to sets

        memoryblock_set local_set;

        for (memoryblock_set_cp m = reaching_blocks.begin();
             m != reaching_blocks.end();
             ++m)
          {
            memoryBlock * one = (*m);
            memoryUse * use = one->find_use_at(callsite);
            if (use) {
              memoryDef * def = use->reaching_def();
              if (def) {
                const memoryblock_set & points_to = def->points_to();
                local_set.insert(points_to.begin(),
                                 points_to.end());
              }
            }
          }

        if (Annotations::Show_adaptivity) {
          cout << indent << "      Callsite " << * callsite
               << " : blocks = ";
          for (memoryblock_set_cp mbp = local_set.begin();
               mbp != local_set.end();
               ++mbp)
            cout << (*mbp)->name() << " ";
          cout << endl;
        }

        // -- Store according to stmt

        stmt_values[callsite->stmt()].insert(local_set.begin(),
                                           local_set.end());
      }

    for (stmt_memoryblock_set_map_p p = stmt_values.begin();
         p != stmt_values.end();
         ++p)
      {
        stmtNode * stmt = (*p).first;
        const memoryblock_set & local_set = (*p).second;

        if (Annotations::Show_adaptivity) {
          cout << indent << "      Statement " << stmt->coord() << ": blocks = ";
          for (memoryblock_set_cp mbp = local_set.begin();
               mbp != local_set.end();
               ++mbp)
            cout << (*mbp)->name() << " ";
          cout << endl;
        }

        if (! local_set.empty()) {
          if (master.empty())
            master = local_set;
          else
            if (master != local_set)
              result = true;
        }
      }

    if (Annotations::Show_adaptivity) {
      if (result)
        cout << indent << "      -> Validated" << endl;
      else
        cout << indent << "      -> Prune" << endl;
    }

    return result;

#endif

#if 1
    // memoryblock_set collected_points_to;
    enumvalue_set collected_vals;

    int min_size = 99999;
    int min = 99999;
    enumValueAnn * highest_value = 0;

    if (Annotations::Show_adaptivity)
      cout << indent << "    Points-to validation for block " << block->name() << endl;

    // memoryblock_set flow_insensitive;

    // -- Store the values according to non-context-sensitive callsite

    stmt_value_map stmt_values;

    for (callsite_objects_map_cp rdp = assignments.begin();
         rdp != assignments.end();
         ++rdp)
      {
        stmtLocation * callsite = (*rdp).first;
        procLocation * attach_to = procedure->procedure_location(callsite);
        assert( attach_to ); // debug

        const memoryblock_set & reaching_blocks = (*rdp).second;

        // -- Add the values from this callsite into the collected set

        enumvalue_set vals = reachable_values(callsite, reaching_blocks,
                                              procedure);

        if (Annotations::Show_adaptivity) {
          cout << indent << "      Callsite " << * callsite
               << " : [ Values = " << to_string(vals) << " -- blocks = ";
          for (memoryblock_set_cp mbp = reaching_blocks.begin();
               mbp != reaching_blocks.end();
               ++mbp)
            cout << (*mbp)->name() << " ";
          cout << "]" << endl;
        }

        // -- Store according to stmt

        meet_with(stmt_values[callsite->stmt()], vals);
      }

    for (stmt_value_map_p p = stmt_values.begin();
         p != stmt_values.end();
         ++p)
      {
        stmtNode * stmt = (*p).first;
        enumvalue_set vals = (*p).second;

        if (Annotations::Show_adaptivity) {
          cout << indent << "      Statement " << stmt->coord() << " reaching values = "
               << to_string(vals) << endl;
        }

        meet_with(collected_vals, vals);

        // -- Keep track of the callsite with the best information

        int count = vals.count();
        if ((count > 0) &&
            (count < min)) min = count;
      }

    if (Annotations::Show_adaptivity)
      cout << indent << "    ==> [ Values = " << to_string(collected_vals) << " ]" << endl;

    bool result;

    if (collected_vals.count() > min)
      result = true;
    else
      result = false;

    if (Annotations::Show_adaptivity) {
      if (result)
        cout << indent << "      -> Validated" << endl;
      else
        cout << indent << "      -> Prune" << endl;
    }

    return result;
#endif
}

/** @brief Validate pointer flow sensitivity
 *
 * Make sure that distinguishing the various pointer targets would
 * actually help. */

bool enumPropertyAnn::validate_pointer_fs(memoryBlock * block,
                                          stmtLocation * where,
                                          string & indent)
{
  // memoryblock_set collected_points_to;
  enumvalue_set collected_vals;

  int min_size = 99999;
  int min = 99999;
  enumValueAnn * highest_value = 0;

  if (Annotations::Show_adaptivity)
    cout << indent << "    Points-to validation for block " << block->name() << endl;

  // memoryblock_set flow_insensitive;

  memoryDef * def = block->find_def_at(where);

  if ( ! def )
    return false;

  const memoryblock_set & points_to = def->points_to();

  for (memoryblock_set_cp p = points_to.begin();
       p != points_to.end();
       ++p)
    {
      memoryBlock * target = (*p);

      // -- Don't consider unchangeable values (such as string constants).

      if ( ! target->write_protected()) {

        // -- Get the values of the reachable blocks from this target:

        memoryblock_set target_set;
        target_set.insert(target);
        enumvalue_set vals = reachable_values(where, target_set);

        meet_with(collected_vals, vals);

        if (Annotations::Show_adaptivity) {
          cout << indent << "      Target " << target->name()
               << " : [ Values = " << to_string(vals) << "]" << endl;
        }

        // -- Keep track of the callsite with the best information

        int count = vals.count();
        if ((count > 0) &&
            (count < min)) min = count;
      }
    }

  if (Annotations::Show_adaptivity)
    cout << indent << "    ==> [ Values = " << to_string(collected_vals) << " ]" << endl;

  bool result;

  if (collected_vals.count() > min)
    result = true;
  else
    result = false;

  if (Annotations::Show_adaptivity) {
    if (result)
      cout << indent << "      -> Validated" << endl;
    else
      cout << indent << "      -> Prune" << endl;
  }

  return result;
}

bool enumPropertyAnn::is_location_reachable(Location * source, Location * target, memoryBlock * block)
{
  memoryDef * source_def = block->find_def_at(source);
  memoryDef * target_def = block->nearest_def_at(target);

  if ( ! source_def  || ! target_def )
    return true;

  if (source_def == target_def)
    return true;

  memorydef_set defs;
  memoryuse_set uses;

  memorydef_set already_seen;

  bool not_done = true;

  defs.insert(target_def);

  while (not_done) {

    // -- Assume we're done

    not_done = false;

    // -- Visit all the defs and collect uses at the same location
    // (presumably, the right-hand sides)

    for (memorydef_set_p p = defs.begin();
         p != defs.end();
         p++)
      {
        memoryDef * cur_def = *p;

        // -- Find all the right-hand-side uses

        block->find_uses_at(cur_def->where(), uses);

        memoryUse * use = cur_def->self_assign_use();
        if (use)
          uses.insert(use);
      }

    // -- Clear the set of defs

    defs.clear();

    // -- For each use, get it's reaching definition

    for (memoryuse_set_p w = uses.begin();
         w != uses.end();
         ++w)
      {
        memoryUse * cur_use = *w;
        memoryDef * cur_def = cur_use->reaching_def();

        if (cur_def) {

          // -- If this is the def we're looking for, then exit

          if (cur_def == source_def)
            return true;

          if (already_seen.find(cur_def) == already_seen.end()) {

            // -- Found a new def, keep going

            already_seen.insert(cur_def);

            defs.insert(cur_def);

            not_done = true;
          }
        }
      }

    // -- Clear the uses and continue

    uses.clear();
  }

  // -- None of the defs reaches the targets

  return false;
}

enumvalue_set enumPropertyAnn::reachable_values(stmtLocation * where,
                                                const memoryblock_set & blocks,
                                                procedureInfo *callee)
{
  enumvalue_set result;

  memoryblock_list worklist;
  memoryblock_set found;

  for (memoryblock_set_cp p = blocks.begin();
       p != blocks.end();
       ++p)
    {
      worklist.push_back(*p);
    }

  // -- First, find all the blocks reachable from this one...

  while ( ! worklist.empty()) {

    // -- Remove the first element from the worklist..

    memoryBlock * mb = worklist.front();
    worklist.pop_front();

    // -- Add all the immediately reachable blocks (that we haven't seen yet)

    if(mb->is_flow_sensitive() || !callee || !callee->is_context_insensitive()){
      // TB
      mb->reachable_blocks(where, true, worklist, found, (memoryBlock *)0);
    } else {
      memoryblock_set more = callee->input_to_value(mb, where);
      for(memoryblock_set_p m=more.begin(); m!=more.end(); m++) {
        if(found.find(*m) == found.end()) {
          found.insert(*m);
          (*m)->reachable_blocks(where, true, worklist, found, (memoryBlock *)0,
                               true);
          worklist.push_back(*m);
        }
      }
    }
  }

  // -- Include the blocks themselves

  found.insert(blocks.begin(),
               blocks.end());

  // -- Collect all the property blocks...

  memoryblock_set property_blocks;

  for (memoryblock_set_p p = found.begin();
       p != found.end();
       ++p)
    {
      memoryBlock * real_block = *p;
      memoryBlock * property_block = lookup_property_block(real_block);

      if (property_block) {

        // -- Make sure this is a block we care about

        // memoryblock_set_p found = already_seen.find(property_block);

        //if (found != already_seen.end())

        property_blocks.insert(property_block);
      } /*else if (default_val() != top())
        meet_with(result, default_val()); // TB*/
    }

  // -- Collect all the values

  for (memoryblock_set_p p = property_blocks.begin();
       p != property_blocks.end();
       ++p)
    {
      memoryBlock * property_block = *p;

      property_block->set_current_def_use(where);
      enumvalue_set local_val = construct_now_value(where, property_block, false);

      meet_with(result, local_val);
    }

  return result;
}

/** @brief Make context sensitive
 *
 * Make the given procedure context sensitive. */

bool enumPropertyAnn::add_context_sensitive_proc(procedureInfo * info,
                                                 procedureinfo_set & already_seen,
                                                 procedureinfo_set & make_cs)
{
  // -- Visit all of it's descendants

  const procedureinfo_set & calls = info->calls();

  for (procedureinfo_set_cp p = calls.begin();
       p != calls.end();
       ++p)
    {
      procedureInfo * cur = *p;

      // -- Avoid duplicates

      if (already_seen.find(cur) == already_seen.end()) {

        already_seen.insert(cur);

        // -- Add this procedure, but skip recursive procedures

        bool cur_recursive = add_context_sensitive_proc(cur, already_seen, make_cs);

        if ( ! cur->is_recursive() && ! cur_recursive) {

          const string & name = info->proc()->decl()->name();

          // -- Don't add library routines:

          if (cur->proc()->decl()->decl_location() != declNode::UNKNOWN)
            make_cs.insert(cur);
        }
      }
    }

  return false;
}



// begin TB new
void enumPropertyAnn::reset_diagnostic() {
  if(_diagnostic) _diagnostic->reset();
}

void enumPropertyAnn::diagnostic(ostream &out, procLocation *where,
                                 pointerValue &pv,
                                 enumvalue_set values) const {
  if(!_diagnostic) return;
  if(_diagnostic->contain_diagnostic_value(values))
    _diagnostic->trace(out, where, pv);
}
// end TB new


// TB_unify
/** Lookup input_to value */
enumvalue_set enumPropertyAnn::lookup_input_to_value(procedureInfo *info,
                                                 memoryBlock * property_block){
  return _input_to_values[Input_to_pair(info,property_block)];
} // lookup_input_to_value

// TB_unify
/** Set input_to value */
void enumPropertyAnn::update_input_to_value(procedureInfo *info,
                                            memoryBlock * property_block,
                                            enumvalue_set values,
                                            stmtLocation *callsite) {
  int old_value = _input_to_values[Input_to_pair(info,property_block)].count();
  _input_to_values[Input_to_pair(info,property_block)] = values;

  if (pointerOptions::Monitor_precision) {
    if (values.count() > 1) {
      property_block->add_parameter_assignment(info->proc(), callsite,
                                               property_block);
      if (old_value>0 && values.count() > old_value) { // 20030905
        procLocation *procloc = info->procedure_location(callsite);
        property_block->add_destructive_assignment(procloc,
                                                   memoryBlock::Parameter_pass);
      }
    }
  }
} // update_input_to_value

// TB_unify
bool enumPropertyAnn::compare_now_value(Location *where,
                                        memoryBlock *property_block,
                                        enumvalue_set compare_to) {
  enumvalue_set now = construct_now_value(where, property_block, false);
  return now == compare_to;
} // compare_now_value

#endif /* __FOO */
