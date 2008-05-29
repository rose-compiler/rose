
// **********************************************************************
//
//     THIS FILE IS OBSOLETE
//
// **********************************************************************

#include "broadway.h"

using namespace std;

// ---------------------------------------------------------------------
//  Property value
//  Element of the lattice
// ---------------------------------------------------------------------

propertyValueAnn::propertyValueAnn(const parserID * id,
				   propertyvalue_list * more_specific)
  : Ann(id->line()),
    _name(id->name()),
    _more_general(0),
    _more_specific(),
    _id(0)
{
  if (more_specific) {
    _more_specific.swap(* more_specific);

    for (propertyvalue_list_p p = _more_specific.begin();
	 p != _more_specific.end();
	 ++p)
      (*p)->_more_general = this;
  }
}

// ---------------------------------------------------------------------
//  Analysis property
//  Abstract superclass
// ---------------------------------------------------------------------

propertyAnn::propertyAnn(const parserID * id,
			 Direction direction,
			 bool enforce_weak_updates,
			 const parserID * default_name,
			 propertyvalue_list * lowest_values)
  : Ann(id->line()),
    _name(id->name()),
    _is_enabled(true),
    _direction(direction),
    _enforce_weak_updates(enforce_weak_updates),
    _values(0),
    _top(0),
    _value_index(),
    _now_values(),
    _always_values(),
    _ever_values()
{
  // -- Create a special "bottom" element from which all others are
  // reachable

  parserID temp("(bottom)", id->line());
  _values = new propertyValueAnn(&temp, lowest_values);

  // -- Create a special "top" element

  parserID temp2("(top)", id->line());
  _top = new propertyValueAnn(&temp2, 0);

  // -- Assign a unique ID and a height in the lattice to each element.

  int cur_id = 0;
  int max_height = 0;
  number_values(_values, cur_id, 0, max_height);

  // -- Give "top" the highest ID and a height greater than all other
  // elements.

  _top->id(cur_id);
  _top->height(max_height+1);
  _value_index.push_back(_top);

  // -- If there is a default value, then look it up. Otherwise, the
  // default is top.

  if (default_name)
    _default = lookup(default_name->name());
  else
    _default = _top;
}

void propertyAnn::number_values(propertyValueAnn * prop,
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

  propertyvalue_list & more_specific = prop->more_specific();

  if (more_specific.empty()) {

    // If there are no more elements above this one, then insert "top"

    more_specific.push_back(_top);
  }
  else {

  // -- Visit the more specific values, increasing the height by one

    for (propertyvalue_list_p p = more_specific.begin();
	 p != more_specific.end();
	 ++p)
      number_values(*p, cur_index, height+1, max_height);
  }
}

void propertyAnn::clear()
{
  _now_values.clear();
  _always_values.clear();
  _ever_values.clear();
}

propertyValueAnn * propertyAnn::lookup(const string & name)
{
  for (propertyvalue_vec_p p = _value_index.begin();
       p != _value_index.end();
       ++p)
    if ((*p)->name() == name)
      return *p;

  return 0;
}

propertyValueAnn * propertyAnn::meet(propertyValueAnn * one,
				     propertyValueAnn * two)
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


bool propertyAnn::at_least(propertyValueAnn * higher,
			   propertyValueAnn * lower)
{
  // X <= Y <==> X ^ Y == X 

  return meet(lower, higher) == lower;
}

bool propertyAnn::test(exprAnn::Operator op, exprAnn::FlowSensitivity flow_sensitivity,
		       pointerValue & lhs, string & lhs_name,
		       pointerValue & rhs, string & rhs_name)
{
  bool result = false;
  propertyValueAnn * rhs_value = 0;

  switch (flow_sensitivity) {

  case exprAnn::Before:
    rhs_value = construct_now_value(rhs, rhs_name);
    result = test(op, flow_sensitivity, lhs, lhs_name, rhs_value);
    break;

  case exprAnn::After:
    rhs_value = construct_after_value(rhs, rhs_name);
    result = test(op, flow_sensitivity, lhs, lhs_name, rhs_value);
    break;

  case exprAnn::Always:
    rhs_value = construct_always_value(rhs, rhs_name);
    result = test(op, flow_sensitivity, lhs, lhs_name, rhs_value);    
    break;

  case exprAnn::Ever:
    {
      propertyvalue_set rhs_vals;
      construct_ever_value(rhs, rhs_name, rhs_vals);

      for (propertyvalue_set_p p = rhs_vals.begin();
	   p != rhs_vals.end();
	   ++p)
	{
	  rhs_value = *p;
	  bool one_result = test(op, flow_sensitivity, lhs, lhs_name, rhs_value);
	  result = result || one_result;
	}
    }
    break;

  default:
    cerr << "ERROR: propertyAnn: invalid flow sensitivity mode passed to test()" << endl;
  }

  return result;
}

bool propertyAnn::test(exprAnn::Operator op, exprAnn::FlowSensitivity flow_sensitivity,
		       pointerValue & lhs, string & lhs_name,
		       propertyValueAnn * rhs_value)
{
  bool result = false;
  propertyValueAnn * lhs_value = 0;

  switch (flow_sensitivity) {

  case exprAnn::Before:
    lhs_value = construct_now_value(lhs, lhs_name);
    result = test(op, lhs_value, rhs_value);
    break;

  case exprAnn::After:
    lhs_value = construct_after_value(lhs, lhs_name);
    result = test(op, lhs_value, rhs_value);
    break;

  case exprAnn::Always:
    lhs_value = construct_always_value(lhs, lhs_name);
    result = test(op, lhs_value, rhs_value);    
    break;

  case exprAnn::Ever:
    {
      propertyvalue_set lhs_vals;
      construct_ever_value(lhs, lhs_name, lhs_vals);

      for (propertyvalue_set_p p = lhs_vals.begin();
	   p != lhs_vals.end();
	   ++p)
	{
	  lhs_value = *p;
	  bool one_result = test(op, lhs_value, rhs_value);
	  result = result || one_result;
	}
    }
    break;

  default:
    cerr << "ERROR: propertyAnn: invalid flow sensitivity mode passed to test()" << endl;
  }

  return result;
}

bool propertyAnn::test(exprAnn::Operator op,
		       propertyValueAnn * lhs_value, propertyValueAnn * rhs_value)
{
  bool result = false;

  if (Annotations::debug)
    cout << "     -> " << name() << " : " << lhs_value->name();

  switch (op) {

  case exprAnn::Is_Exactly:
    result = lhs_value == rhs_value;

    if (Annotations::debug)
      cout << " is-exactly " << rhs_value->name();

    break;

  case exprAnn::Is_AtLeast:
    result = at_least(lhs_value, rhs_value);

    if (Annotations::debug)
      cout << " is-atleast " << rhs_value->name();

    break;

  case exprAnn::Is_Bottom:
    result = lhs_value == bottom();

    if (Annotations::debug)
      cout << " is-bottom ";

    break;

  default:
    cerr << "ERROR: propertyAnn: invalid operator passed to test()" << endl;
  }
  
  if (Annotations::debug) {
    cout << " == ";
    if (result)
      cout << "true";
    else
      cout << "false";
    cout << endl;
  }

  return result;
}

propertyValueAnn * propertyAnn::lookup_now_value(memoryBlock * block,
						 memoryAccess * def_or_use)
{
  // -- Look up this def or use

  defuse_property_map_p p = _now_values.find(def_or_use);

  if (p == _now_values.end())
    return top();
  else
    return (*p).second;
}

bool propertyAnn::update_now_value(memoryBlock * block,
				   memoryAccess * def_or_use,
				   propertyValueAnn * newval)
{
  propertyValueAnn * oldval = lookup_now_value(block, def_or_use);

  if (def_or_use->is_additive())
    newval = meet(newval, oldval);

  if (newval != top()) {
    _now_values[def_or_use] = newval;
  }

  if (Annotations::debug) {
    cout << "      + Set \"now\" " << block->decl()->name() << " : "
	 << oldval->name() << " => " << newval->name();
    if (oldval != newval)
      cout << " -- changed" << endl;
    else
      cout << " -- unchanged" << endl;
  }

  return oldval != newval;
}

propertyValueAnn * propertyAnn::lookup_always_value(memoryBlock * block)
{
  block_property_map_p p = _always_values.find(block);
  if (p == _always_values.end())
    return default_val();
  else
    return (*p).second;
}

bool propertyAnn::update_always_value(memoryBlock * block,
				      propertyValueAnn * newval)
{
  propertyValueAnn * oldval = lookup_always_value(block);

    /* --- TBD: Does this make sense? It seems like we should always
       update this value using the meet function
      
       if (def->is_strong()) {
       newval = rhs_value;
       }
       else {
       newval = meet(oldval, rhs_value);
       }
    */

  newval = meet(oldval, newval);

  if (newval != default_val())
    _always_values[block] = newval;

  if (Annotations::debug) {
    cout << "      + Set \"always\" " << block->decl()->name() << " : "
	 << oldval->name() << " => " << newval->name();
    if (oldval != newval)
      cout << " -- changed" << endl;
    else
      cout << " -- unchanged" << endl;
  }

  return oldval != newval;
}

bool propertyAnn::update_ever_value(memoryBlock * block,
				    propertyValueAnn * newval)
{
  propertyvalue_set & cur_vals = _ever_values[block];

  propertyvalue_set_p q = cur_vals.find(newval);
  if (q == cur_vals.end()) {
    cur_vals.insert(newval);
    
    return true;
  }

  return false;
} 

propertyValueAnn * propertyAnn::construct_now_value(pointerValue & variable,
						    string & name)
{
  propertyValueAnn * result = top();

  if (Annotations::debug)
    cout << "      + Construct \"now\" value for " << name << endl;

  // -- For each memory block ...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * block = *p;
      propertyValueAnn * local_val = construct_now_value(block);
      if (local_val)
	result = meet(result, local_val);
      else
	result = meet(result, default_val());
    }

  if (Annotations::debug)
    cout << "       = " << result->name() << endl;

  return result;
}

propertyValueAnn * propertyAnn::construct_now_value(memoryBlock * block)
{
  propertyValueAnn * result = top();

  if (Annotations::debug)
    cout << "        - Lookup \"Now\" value of " << block->decl()->name() << endl;

  // -- Forward or backward

  if (direction() == Forward) {

    // -- Forward analysis: the now value is constructed by getting the
    // current use and looking up the reaching definition.

    if (Annotations::debug)
      cout << "          use at ";

    memoryUse * use = block->current_use();
    if (use) {
      memoryDef * def = use->reaching_def();

      if (Annotations::debug) {
	cout << * (use->where()) << endl;
	cout << "          reaching def at ";
      }

      // -- If the def has a property value, look it up. If there is no
      // reaching def, use the default value.

      if (def) {
	result = lookup_now_value(block, def);

	if (Annotations::debug) {
	  cout << * (def->where()) << endl;
	  cout << "         = " << result->name() << endl;
	}
      }
      else {
	result = 0;

	if (Annotations::debug)
	  cout << "(no reaching def)" << endl;
      }

      // -- Collect this value into the result using the "meet" function
    }
    else {

      // -- This is bad: the variable has no use here (i.e., it is
      // probably missing an access annotation).

      if (Annotations::debug)
	cout << " NO USE " << endl;

      result = 0;
    }
  }
  else {

    // -- Backward analysis: the now value is constructed by getting
    // the current definition, and meeting together the values from all
    // of the uses that it reaches.

    if (Annotations::debug)
      cout << "          def at ";

    memoryDef * current_def = block->current_def();
    if (current_def) {

      if (Annotations::debug) {
	cout << * (current_def->where()) << endl;
	cout << "          uses reached:" << endl;
      }

      if (current_def->uses().empty()) {

	// -- Special case: if the def has no uses (this can happen with
	// merge points), then return the default value.

	result = 0;

	if (Annotations::debug)
	  cout << "              (no uses reached)" << endl;
      }
      else {

	// -- Otherwise, visit all the uses reached by the def

	for (memoryuse_list_p u = current_def->uses().begin();
	     u != current_def->uses().end();
	     ++u)
	  {
	    memoryUse * use = *u;

	    propertyValueAnn * local_val = lookup_now_value(block, use);
	    result = meet(result, local_val);
	    if (Annotations::debug)
	      cout << "              Use at " << * (use->where()) << " value = " << local_val->name() << endl;
	  }
      }
    }
    else {

      // -- This is bad: the variable has no def here (i.e., it is
      // probably missing a modifies annotation).

      if (Annotations::debug)
	cout << " NO DEF " << endl;

      result = 0;
    }
  }

  return result;
}

propertyValueAnn * propertyAnn::construct_after_value(pointerValue & variable,
						      string & name)
{
  propertyValueAnn * result = top();

  if (Annotations::debug)
    cout << "      + Construct \"after\" value for " << name << endl;

  // -- For each memory block ...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * block = *p;
      propertyValueAnn * local_val = construct_after_value(block);
      if (local_val)
	result = meet(result, local_val);
      else
	result = meet(result, default_val());
    }

  if (Annotations::debug)
    cout << "       = " << result->name() << endl;

  return result;
}

propertyValueAnn * propertyAnn::construct_after_value(memoryBlock * block)
{
  propertyValueAnn * result = top();

  if (Annotations::debug)
    cout << "        - Lookup \"After\" value of " << block->decl()->name();

   // -- Forward or backward

  if (direction() == Forward) {

    // -- Forward analysis: the new property value is associated with
    // the current definition.

    if (Annotations::debug)
      cout << " def at ";

    memoryDef * def = block->current_def();

    // -- If the def has a property value, collect it into the return
    // value using the meet function.

    if (def) {
      if (Annotations::debug)
	cout << * (def->where()) << endl;
      
      result = meet(result, lookup_now_value(block, def));
    }
    else {

      if (Annotations::debug)
	cout << "NO CURRENT DEF" << endl;
 
      // -- This is bad: the variable has no def here (i.e., it is
      // probably missing a modify annotation).
	  
      result = 0;
    }
  }
  else {

    // -- Backward analysis: the new property value is associated with
    // the current use.

    if (Annotations::debug)
      cout << " use at ";

    memoryUse * use = block->current_use();

    // -- If the use has a property value, collect it into the return
    // value using the meet function.

    if (use) {
      if (Annotations::debug)
	cout << * (use->where()) << endl;

      result = meet(result, lookup_now_value(block, use));
    }
    else {

      if (Annotations::debug)
	cout << "NO CURRENT USE" << endl;
 
      // -- This is bad: the variable has no use here (i.e., it is
      // probably missing an access annotation).
      
      result = 0;
    }
  }

  return result;
}

propertyValueAnn * propertyAnn::construct_always_value(pointerValue & variable,
						       string & name)
{
  propertyValueAnn * result = top();

  if (Annotations::debug)
    cout << "      + Construct \"always\" value for " << name << endl;

  // -- For each memory block...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * block = *p;

      propertyValueAnn * val = lookup_always_value(block);

      if (Annotations::debug)
	cout << "        - Lookup \"Always\" value of " << block->decl()->name()
	     << " = " << val->name() << endl;

      // -- If it has a property value, collect it into the return value
      // using the meet function.

      result = meet(result, lookup_always_value(block));
    }
  
  if (Annotations::debug)
    cout << "       = " << result->name() << endl;

  return result;
}

void propertyAnn::construct_ever_value(pointerValue & variable,
				       string & name,
				       propertyvalue_set & values)
{
  if (Annotations::debug)
    cout << "      + Construct \"ever\" values for " << name << endl;

  // -- For each memory block...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * block = *p;

      if (Annotations::debug)
	cout << "        - Lookup \"Ever\" value of " << block->decl()->name()
	     << " = { ";

      // -- Look up the set of values

      block_propertyset_map_p q = _ever_values.find(block);
      if (q != _ever_values.end()) {
	propertyvalue_set & vals = (*q).second;

	// -- Collect those values into the return set

	for (propertyvalue_set_p w = vals.begin();
	     w != vals.end();
	     ++w)
	  {
	    propertyValueAnn * val = *w;
	    if (Annotations::debug)
	      cout << val->name() << " ";
	    values.insert(val);
	  }
      }

      if (Annotations::debug)
	cout << "}" << endl;
    }

  if (Annotations::debug) {
    cout << "          = { ";
    for (propertyvalue_set_p p = values.begin();
	 p != values.end();
	 ++p)
      cout << (*p)->name() << " ";
    cout << "}" << endl;
  }
}

propertyValueAnn * propertyAnn::construct_weak_now_value(pointerValue & variable, string & name)
{
  propertyValueAnn * result = top();

  if (Annotations::debug)
    cout << "      + Construct \"weak now\" value..." << endl;

  // -- For each memory block ...

  for (memoryblock_set_p p = variable.blocks.begin();
       p != variable.blocks.end();
       ++p)
    {
      memoryBlock * block = *p;
      propertyValueAnn * local_val;

      // -- Get the now value

      local_val = construct_now_value(block);
      if (local_val)
	result = meet(result, local_val);
      else
	result = meet(result, default_val());

      // -- If the block was weakly updated, then add in the after value

      if (block->current_def() &&
	  block->current_def()->is_weak()) {

	local_val = construct_after_value(block);
	if (local_val)
	  result = meet(result, local_val);
	else
	  result = meet(result, default_val());
      }
    }

  if (Annotations::debug)
    cout << "       = " << result->name() << endl;

   return result;
}


bool propertyAnn::apply_next(memoryBlock * block, propertyValueAnn * new_value)
{
  bool has_now_value;

  memoryDef * change_def = 0;
  memoryAccess * attach_value_to = 0;

  // -- Skip write-protected objects

  if ( ! block->write_protected()) {

    if (Annotations::debug) {
      if (new_value)
	cout << "    + Apply: " << block->decl()->name()
	     << " <- " << new_value->name() << endl;
      else
	cout << "    + Apply: " << block->decl()->name()
	     << " <- (previous value)" << endl;
    }

    // -- Get the value that reaches this location (the previous value)

    propertyValueAnn * previous_value = construct_now_value(block);

    // -- Special case: if the new value is NULL, then just use the
    // previous value.

    if (! new_value)
      new_value = previous_value;

    // -- Still no new value? Just return....

    if (! new_value)
      return false;

    // -- Set up forward or backward update

    if (direction() == Forward) {

      // -- Forward analysis: propagate the new states to the current
      // definition

      memoryDef * def = block->current_def();

      change_def = def;
      attach_value_to = def;
    }
    else {

      // -- Backward analysis: propagate the new states to the current
      // use. 

      memoryUse * use = block->current_use();

      // -- We record the reaching def as the changed definition

      change_def = use->reaching_def();

      // -- Attach the value to the use

      attach_value_to = use;
    }

    if (enforce_weak_updates()) {

      // -- Handle weak updates. Special case: never apply a weak
      // update when there is no "now" value (in other words, don't
      // just mindlessly merge in the default value). If there are no
      // reaching defs/uses, then just leave the value alone.

      if (attach_value_to->is_weak() && previous_value)
	new_value = meet(new_value, previous_value);
    }

    // -- Set the "now" value

    bool change_now = update_now_value(block, attach_value_to, new_value);

    // -- Update the flow-insensitive "always" value
      
    bool change_always = update_always_value(block, new_value);

    // -- Update the flow-insensitive "ever" value

    bool change_ever = update_ever_value(block, new_value);

    // -- If anything changed, record where it happened

    return (change_now || change_always || change_ever) && change_def;
  }

  return false;
}

void propertyAnn::apply_merge(memoryBlock * block, memoryuse_list & phi_uses,
			      memoryblock_set & changes)
{
  if (Annotations::debug)
    cout << "  + Merge " << block->name() << endl;

  propertyValueAnn * oldval = 0;
  propertyValueAnn * merged_val = top();

  // -- Forwards or backwards

  if (direction() == Forward) {

    // -- Forward analysis: collect the values that reach the merge uses,
    // meet them together and propagate that value to the def.

    // -- Get the old value

    memoryDef * def = block->current_def();

    oldval = lookup_now_value(block, def);

    // -- Merge together the uses that reach this merge point

    for (memoryuse_list_p p = phi_uses.begin();
	 p != phi_uses.end();
	 ++p)
      {
	memoryUse * phi_use = *p;
	memoryDef * reaching_def = phi_use->reaching_def();

	// -- Find the reaching value, handling the default case as well

	propertyValueAnn * reaching_val;

	if (reaching_def) {

	  // -- There is a reaching def, look it up

	  reaching_val = lookup_now_value(block, reaching_def);

	  if (Annotations::debug)
	    cout << "   = " << reaching_val->name() << " at " << * (reaching_def->where()) << endl;
	}
	else {

	  // -- No reaching def, use the default value

	  reaching_val = default_val();

	  if (Annotations::debug)
	    cout << "   = default value " << reaching_val->name() << endl;
	}

	merged_val = meet(merged_val, reaching_val);
      }

    // -- Did anything change?

    bool changed = update_now_value(block, def, merged_val);
    if (changed) {

      changes.insert(block);

      if (Annotations::debug)
	cout << "   -> merge changed " << block->name() << endl;
    }
  }
  else {

    // -- Backward analysis: get the value of the definition and just copy
    // it to all the merge uses.

    merged_val = construct_now_value(block);

    if ( ! merged_val)
      merged_val = default_val();

    for (memoryuse_list_p p = phi_uses.begin();
	 p != phi_uses.end();
	 ++p)
      {
	memoryUse * phi_use = *p;

	oldval = lookup_now_value(block, phi_use);

	// -- Did anything change?

	bool changed = update_now_value(block, phi_use, merged_val);
	if (changed && phi_use->reaching_def()) {

	  changes.insert(block);

	  if (Annotations::debug)
	    cout << "   -> merge changed " << block->name() << endl;
	}
      }
  }

  // -- NOTE that a merge will never change the "always" or "ever" values,
  // so there is no point touching them.
}

/** @brief Apply assignment
 *
 * At a normal assignment, build the "now" value for the right-hand side,
 * and assign it to the left-hand side. Weak updates are handled by the
 * apply_next() method. */

void propertyAnn::apply_assignment(pointerValue & left, pointerValue & right,
				   memoryblock_set & changes)
{
  propertyValueAnn * new_value = 0;
  pointerValue * to_update = 0;

  // -- Skip expressions where the right-hand-side is an address:
  // (e.g., "p = &x" does not propagate the value to or from x).

  if ( ! right.is_address) {

    // -- Forward or backward: figure out what the new value is, and which
    // blocks to update.

    if (direction() == Forward) {

      // -- Forward: new value comes the right, and updates the left.

      string rhs_name("right-hand side");
      new_value = construct_now_value(right, rhs_name);
      to_update = &left;
    }
    else {

      // -- Backward: new value comes from the left and updates the
      // right. Note that we make sure to get the value *after* any weak
      // updates.

      string lhs_name("left-hand side");
      new_value = construct_weak_now_value(left, lhs_name);
      to_update = &right;
    }

    // -- Perform the actual update

    memoryblock_set & blocks = to_update->blocks;

    for (memoryblock_set_p p = blocks.begin();
	 p != blocks.end();
	 ++p)
      {
	memoryBlock * block = *p;
	bool change = apply_next(block, new_value);
	if (change)
	  changes.insert(block);
      }
  }
}


void propertyAnn::report(ostream & out,
			 exprAnn::FlowSensitivity flow_sensitivity,
			 pointerValue & lhs, string & lhs_name)
{
  propertyValueAnn * lhs_value = 0;

  switch (flow_sensitivity) {

  case exprAnn::Before:
    lhs_value = construct_now_value(lhs, lhs_name);
    out << lhs_value->name();
    break;

  case exprAnn::After:
    lhs_value = construct_after_value(lhs, lhs_name);
    out << lhs_value->name();
    break;

  case exprAnn::Always:
    lhs_value = construct_always_value(lhs, lhs_name);
    out << lhs_value->name();
    break;

  case exprAnn::Ever:
    {
      propertyvalue_set lhs_vals;
      construct_ever_value(lhs, lhs_name, lhs_vals);

      bool first = true;
      for (propertyvalue_set_p p = lhs_vals.begin();
	   p != lhs_vals.end();
	   ++p)
	{
	  lhs_value = *p;
	  if ( ! first ) out << ", ";
	  out << lhs_value->name();
	  first = false;
	}
    }
    break;

  default:
    cerr << "ERROR: propertyAnn: invalid flow sensitivity mode passed to report()" << endl;
  }
}

void propertyAnn::print(ostream & o) const
{
  o << "property " << name();

  o << "{" << endl;
  print(o, values(), 2);
  o << "}" << endl;
}

void propertyAnn::print(ostream & o, propertyValueAnn * prop, int depth) const
{
  for (int i = 0; i < depth; i++)
    o << " ";

  o << prop->name() << ", id = " << prop->id() << ", height = " << prop->height() << endl;

  const propertyvalue_list & vals = prop->more_specific();
  for (propertyvalue_list_cp p = vals.begin();
       p != vals.end();
       ++p)
    print(o, *p, depth+2);
}

