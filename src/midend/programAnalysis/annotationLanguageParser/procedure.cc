
#include "broadway.h"
#include <cassert>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

// ------------------------------------------------------------
// CLASS: procedureAnn
// ------------------------------------------------------------

procedureAnn * procedureAnn::Current = 0;

procedureAnn::procedureAnn(parserID * id, parserid_list * params,
                           Annotations * anns, int line)
  : Ann(line),
    _name(id->name()),
    _variables(),
    _formal_params(),
    _uses(),
    _defs(),
    _global_uses(),
    _global_defs(),
    _property_uses(),
    _property_defs(),
    _property_weak_defs(),
    _on_entry(),
    _on_exit(),
    _pointer_rules(),
    _default_pointer_rule(0),
    _analyses(),
    _reports(),
#ifdef __FOO
    _actions(),
    _proc(0),
#endif
    _numeric_identifiers(),
    _annotations(anns),
#ifdef __FOO
    _callsites(),
#endif
    _pure_function(false),
    _allocates_memory(false),
    _deallocates_memory(false),
    _modifies_globals(false),
    _temp_defs(new parserid_list()),
    _temp_uses(new parserid_list()),
    _temp_deletes(new parserid_list())
{
  // annVariable * nl = lookup(string("null"), true);

  // DQ (9/12/2011): Unused variable warning from compiler.
  // annVariable * ret = lookup(string("return"), true);

  if (params) {
    int count = 0;
    for (parserid_list_p p = params->begin();
         p != params->end();
         ++p)
      {
        parserID & id = *p;
        string & formal_param = id.name();
        annVariable * formal = lookup(formal_param, true);
        formal->set_formal();
        _formal_params.push_back(formal);
        count++;
      }
  }

  // Create a procNode for this procedure so that the path-related
  // aglorithms have something to refer to. The body of this synthetic
  // procNode contains one basic block with three statements:
  //
  //   1. The first statement is used to attach all of the
  //   deallocations. This is important because if the annotations also
  //   contain allocations, we need the deallocations to dominate
  //   them. This makes the multiplicity analysis work correctly.
  //
  //   2. The second statement is for all the allocations. We need this to
  //   be after the deallocations, but before any assignments.
  //
  //   3. The third statement is used to attach all the other uses and defs
  //   in the annotations. These include explicit defs annotations, as well
  //   as structure changes in the on_exit annotations, and transfer
  //   functions in the analyze annotations.
#ifdef __FOO
  blockNode * body = new blockNode(0,0);
  basicblockNode * block = new basicblockNode(0, 0);
  body->stmts().push_back(block);

  block->stmts().push_back(new exprstmtNode(0));
  block->stmts().push_back(new exprstmtNode(0));
  block->stmts().push_back(new exprstmtNode(0));

  // Create a declaration for the procedure: VERY IMPORTANT: the
  // decl_location needs to be UNKNOWN so that we can easily tell which
  // procNodes represent library routines (regular application routines
  // should have decl location TOP).

  declNode * decl = new declNode(_name.c_str(), declNode::NONE, 0, 0, 0);

  _proc = new procNode(decl, body);
#endif
}

procedureAnn::~procedureAnn()
{
  // -- Delete all call sites

  clear();

  // -- Delete all variables

  for (var_map_p p = _variables.begin();
       p != _variables.end();
       ++p)
    {
      annVariable * var = (*p).second;
      delete var;
    }

  // -- Delete on_exit and on_entry

  for (structure_list_p p = _on_entry.begin();
       p != _on_entry.end();
       ++p)
    delete (*p);

  for (structure_list_p p = _on_entry.begin();
       p != _on_entry.end();
       ++p)
    delete (*p);

  for (pointerrule_list_p p = _pointer_rules.begin();
       p != _pointer_rules.end();
       ++p)
    delete (*p);

  // -- Delete analysis annotations

  for (analyze_map_p p = _analyses.begin();
       p != _analyses.end();
       ++p)
    delete (*p).second;

  // -- Delete report annotations

  for (report_list_p p = _reports.begin();
       p != _reports.end();
       ++p)
    delete (*p);

  // -- Delete action annotations

#ifdef __ACTIONS
  for (action_list_p p = _actions.begin();
       p != _actions.end();
       ++p)
    delete (*p);
#endif
}

// ----------------------------------------------------------------------
//  Lookup variables
// ----------------------------------------------------------------------

annVariable * procedureAnn::lookup(const string & variable, bool create,
                                     bool is_new)
{
  // --- Lookup the variable name

  var_map_p p = _variables.find(variable);
  if (p == _variables.end()) {

    // --- If its not there, either create it (if create == true) or return
    // null. Note: if we are creating a new variable, then don't bother
    // searching the global variables because the new variable will shadow
    // any global with the same name.

    if (create) {
      bool is_global = false;
      bool is_external = false;
      bool is_formal = false;
      bool is_io = false;

      annVariable * new_var = new annVariable(variable, name(), is_new, is_global,
                                              is_external, is_formal, is_io);

      _variables[variable] = new_var;
      return new_var;
    }
    else {

      // --- Not local, try globals

      annVariable * glob = _annotations->lookup_global(variable);
      if (glob)
        return glob;
      else
        return 0;
    }
  }
  else
    return (*p).second;
}

// ----------------------------------------------------------------------
//   Defs and Uses
// ----------------------------------------------------------------------

void procedureAnn::add_defs(parserid_list * variables)
{
  // At this point, just store the defs. We'll postprocess them after we've
  // generated all the necessary local variables during the construction of
  // the on_entry and on_exit annotations.

  for (parserid_list_p p = variables->begin();
       p != variables->end();
       ++p)
    _temp_defs->push_back(*p);

  // BROKEN? _defs->splice(_defs->end(), * variables);
}

void procedureAnn::add_uses(parserid_list * variables)
{
  // At this point, just store the defs. We'll postprocess them after we've
  // generated all the necessary local variables during the construction of
  // the on_entry and on_exit annotations.

  for (parserid_list_p p = variables->begin();
       p != variables->end();
       ++p)
    _temp_uses->push_back(*p);

  // BROKEN? _uses->splice(_uses->end(), * variables);
}

// ------------------------------------------------------------
//  Manage property uses and defs
// ------------------------------------------------------------

/** @brief Add property use
 *
 * Add this variable to the list of variables whose property value is
 * tested. This does not actually mark the variable itself as used. */

void procedureAnn::add_property_use(annVariable * var, enumPropertyAnn * enum_property)
{
  _property_uses[var].insert(enum_property);
}

/** @brief Add property def
 *
 * Add this variable to the list of variables whose property value is
 * modified. This does not actually mark the variable itself as def'd. */

void procedureAnn::add_property_def(annVariable * var, enumPropertyAnn * enum_property)
{
  _property_defs[var].insert(enum_property);
  // _property_uses[var].insert(enum_property);
}

/** @brief Add property weak def
 *
 * Add this variable to the list of variables whose property value is
 * weakly updated. This should be a subset of the property defs. */

void procedureAnn::add_property_weak_def(annVariable * var, enumPropertyAnn * enum_property)
{
  _property_weak_defs[var].insert(enum_property);
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------

void procedureAnn::add_delete(parserID * id)
{
  _temp_deletes->push_back(*id);
}

void procedureAnn::postprocess()
{
  postprocess_variables( * _temp_defs, true);
  postprocess_variables( * _temp_uses, false);

  delete _temp_defs;
  delete _temp_uses;

  _temp_defs = 0;
  _temp_uses = 0;

  // --- Post-process the deletes

  for (parserid_list_p i = _temp_deletes->begin();
       i != _temp_deletes->end();
       ++i)
    {
      parserID & id = *i;
      int line = id.line();
      string & varname = id.name();

      annVariable * var = lookup(varname, false);
      if (var)
        var->set_deleted();
      else
        _annotations->Error(line, string("Undefined variable ") + varname);
    }

  delete _temp_deletes;
  _temp_deletes = 0;

  // --- Post-process the analysis rules

  for (analyze_map_p p = _analyses.begin();
       p != _analyses.end();
       ++p)
    (*p).second->lookup(this, _annotations);

  // --- Post-process the report list

  for (report_list_p q = _reports.begin();
       q != _reports.end();
       ++q)
    (*q)->lookup(this, _annotations);

  // --- Post-process the action list

#ifdef __ACTIONS
  for (action_list_p q = _actions.begin();
       q != _actions.end();
       ++q)
    (*q)->lookup(this, _annotations);
#endif
  // --- Determine any special properties

  //     Look for memory allocation and deallocation

  for (var_map_p p = _variables.begin();
       p != _variables.end();
       ++p)
    {
      annVariable * var = (*p).second;

      if (var->is_new())
        _allocates_memory = true;

      if (var->is_deleted())
        _deallocates_memory = true;
    }

  //     Look for modified globals

  if ( ! _global_defs.empty())
    _modifies_globals = true;

  //     Look for I/O variables (global only)

  for (var_set_p p = _global_defs.begin();
       p != _global_defs.end();
       ++p)
    if ((*p)->is_io())
        _accesses_io = true;

  for (var_set_p p = _global_uses.begin();
       p != _global_uses.end();
       ++p)
    if ((*p)->is_io())
        _accesses_io = true;

  if ( ! _allocates_memory &&
       ! _deallocates_memory &&
       ! _modifies_globals &&
       ! _accesses_io)
    _pure_function = true;

  // -- Warn about routines with no apparent effects at all

  if (_pure_function && _defs.empty() && _property_defs.empty())
    cout << "WARNING: procedure \"" << name() << "\" appears to have no effects at all." << endl;
}

void procedureAnn::postprocess_variables(parserid_list & uses_or_defs,
                                         bool is_defs)
{
  // Visit all uses and defs and mark the corresponding variables.

  for (parserid_list_p p = uses_or_defs.begin();
       p != uses_or_defs.end();
       ++p)
    {
      parserID & id = *p;
      int line = id.line();
      string & varname = id.name();

      annVariable * var = lookup(varname, false);
      if (var) {
        if (is_defs)
          add_one_def(var);
        else
          add_one_use(var);
      }
      else
        _annotations->Error(line, string("Undefined variable ") + varname);
    }
}

void procedureAnn::add_one_use(annVariable * var)
{
  var->set_used();

  _uses.insert(var);

  if (var->is_global())
    _global_uses.insert(var);
}

void procedureAnn::add_one_def(annVariable * var)
{
  var->set_defined();

  _defs.insert(var);

  if (var->is_global())
    _global_defs.insert(var);
}

#ifdef __FOO

/** @brief Def/use location
 *
 * Given a procLocation corresponding to a call to this procedure, return
 * the special synthetic stmtLocation that holds all of the defs and
 * uses. */

stmtLocation * procedureAnn::def_use_location(procLocation * procloc)
{
  if (this == _annotations->init()) {

    // -- This is the special "init" procedure

    return dealloc_location(procloc);
  }

  basicblockNode * body = procloc->proc()->entry();
  basicblockLocation * block = procloc->lookup_block(body);
  stmtLocation * stmt = & block->statements()[2];

  return stmt;
}

/** @brief Deallocation location
 *
 * Given a procLocation corresponding to a call to this procedure, return
 * the special synthetic stmtLocation to which we attach deallocation of
 * heap objects. It needs to dominate the allocation location and the
 * def/use location so that changes to the multiplicity of an object are
 * properly reflected in assignments. */

stmtLocation * procedureAnn::dealloc_location(procLocation * procloc)
{
  basicblockNode * body = procloc->proc()->entry();
  basicblockLocation * block = procloc->lookup_block(body);
  stmtLocation * stmt = & block->statements()[0];

  return stmt;
}

/** @brief Allocation location
 *
 * Given a procLocation corresponding to a call to this procedure, return
 * the special synthetic stmtLocation to which we attach allocation of heap
 * objects. It needs to dominate the def/use location, but be dominated by
 * the deallocation location so that changes to the multiplicity of an
 * object are properly reflected in assignments. */

stmtLocation * procedureAnn::alloc_location(procLocation * procloc)
{
  if (this == _annotations->init()) {

    // -- This is the special "init" procedure

    return dealloc_location(procloc);
  }

  basicblockNode * body = procloc->proc()->entry();
  basicblockLocation * block = procloc->lookup_block(body);
  stmtLocation * stmt = & block->statements()[1];

  return stmt;
}

#endif /* __FOO */

// ----------------------------------------------------------------------
//  on_entry and on_exit annotations
// ----------------------------------------------------------------------

// Process the structure tree into a list of structure operations, creating
// the local variable entries along the way. Here is an example of how this
// is processed:
//
//  on_entry {  A --> B { width,
//                        height,
//                        data --> data1,
//                        more { stuff,
//                               things } } }
//
//  This introduces the following series of structure annotations:
//
//  Source    Operator   Target
//    A         -->        B
//    B       .width     B.width
//    B       .height    B.height
//    B        .data     B.data
//  B.data      -->       data1
//    B        .more     B.more
//  B.more    .stuff     B.more.stuff
//  B.more    .things    B.more.things
//
//  This naming scheme is convenient because we can bind the names to
//  actual memory blocks during the processing of the on_entry annotations,
//  and then we never have to explicity process the "dot" operator because
//  it's built into the name.
//
//  These two public methods handle the top variables, which are required
//  to be previously declared (either as variables from previous
//  structures, formal paramters to the procedure, or global variables).
//
//  These two routines also handle deletion of the structure trees.

void procedureAnn::add_on_entry(structuretree_list * structures)
{
  if (structures) {
    for (structuretree_list_p p = structures->begin();
         p != structures->end();
         ++p)
      {
        structureTreeAnn * top_node = *p;

        annVariable * top_var = lookup(top_node->name(), false, false);
        if (top_var)
          add_structures(true, _on_entry, top_var, top_node);
        else
          _annotations->Error(top_node->line(),
                              string("Could not find target \"") + top_node->name() +
                              string("\""));

        delete top_node;
      }
  }
}

void procedureAnn::add_on_exit(structuretree_list * structures)
{
  if (structures) {
    for (structuretree_list_p p = structures->begin();
         p != structures->end();
         ++p)
      {
        structureTreeAnn * top_node = *p;

        annVariable * top_var = lookup(top_node->name(), false, false);
        if (top_var)
          add_structures(false, _on_exit, top_var, top_node);
        else
          _annotations->Error(top_node->line(),
                              string("Could not find target \"") + top_node->name() +
                              string("\""));

        delete top_node;
      }
  }
}

void procedureAnn::add_on_exit(pointerRuleAnn * pointer_rule)
{
  // -- Get the tree form of the pointers

  structuretree_list * structures = pointer_rule->effects_tree();

  // -- Process the structures just like we used to, except put the results
  // back into the pointerRule:

  structure_list & the_list = pointer_rule->effects();

  if (structures) {
    for (structuretree_list_p p = structures->begin();
         p != structures->end();
         ++p)
      {
        structureTreeAnn * top_node = *p;

        annVariable * top_var = lookup(top_node->name(), false, false);
        if (top_var)
          add_structures(false, the_list, top_var, top_node);
        else
          _annotations->Error(top_node->line(),
                              string("Could not find target \"") + top_node->name() +
                              string("\""));
      }
  }

  // -- Check for default

  if ( ! pointer_rule->has_condition()) {

    if (_default_pointer_rule)
      _annotations->Error(pointer_rule->line(),
                          "Each on_exit annotation may only have one default rule.");
    else
      _default_pointer_rule = pointer_rule;
  }

  // -- Run the lookup method

  pointer_rule->lookup(this, _annotations);

  // -- Add it to the list

  _pointer_rules.push_back(pointer_rule);
}

void procedureAnn::add_structures(bool is_on_entry,
                                  structure_list & the_list,
                                  annVariable * parent_var,
                                  structureTreeAnn * parent_node)
{
  structureAnn * new_struc = 0;
  annVariable * child_var = 0;

  // -- Determine if the introduction of the children of this node is
  // allowed to create new local variables. This is true if we are in an
  // on_entry annotation, or if the children are qualified by the "new"
  // operator. There is a special case for the dot operator: if the parent
  // was new, then the children (the fields) are also new.

  bool is_target_new = parent_node->is_target_new();
  bool create = is_on_entry || is_target_new;

  // -- Visit each child of the current node, looking at the operator to
  // determine what kind of entry to create in the structure list.

  structuretree_list * structures = parent_node->targets();

  for (structuretree_list_p p = structures->begin();
       p != structures->end();
       ++p)
    {
      structureTreeAnn * child_node = *p;

      new_struc = 0;

      // -- CASE 1: This is the target of an arrow operator. In this case,
      // we look up the target and then create a new structureAnn.

      if (parent_node->op() == structureTreeAnn::Arrow) {
        child_var = lookup(child_node->name(), create, is_target_new);
        if (child_var)
          new_struc = new structureAnn(parent_var, child_var, 0, child_node->line());
      }

      // -- CASE 2: This is the target of a dot operator. The target name
      // is a concatenation of the parents name with the dot, and the
      // current node's name (which is basically the field name).

      // Special case: new structure implies new fields...

      create = create || parent_var->is_new();

      if (parent_node->op() == structureTreeAnn::Dot) {
        string child_name = parent_var->name() + "." + child_node->name();
        child_var = lookup(child_name, create, parent_var->is_new());
        if (child_var)
          new_struc = new structureAnn(parent_var, child_var,
                                       & child_node->name(), child_node->line());
      }

      // -- If we successfully created an entry, then add it to the
      // appropriate list, and then work recursively on the tree below.

      if (new_struc) {

        // -- Error check: cannot use "new" inside an on_entry annotation

        if (is_on_entry && child_var->is_new())
          _annotations->Error(child_node->line(),
                              string("Use of new operator on \"") + child_node->name() +
                              string("\" is not allowed inside on_entry."));

        // -- Error check: "null" cannot be a source

        if ( parent_var->name() == "null")
          _annotations->Error(child_node->line(),
                              string("the \"null\" object cannot point to anything or be decomposed."));

        // -- Error check: "return" cannot be pointed to

        if ( child_var->name() == "return")
          _annotations->Error(child_node->line(),
                              string("the \"return\" object cannot be a target."));

        // -- All check pass, add the new structure. Also record the fact
        // that the source will be a use or a def, depending on whether we
        // are processing on_entry or on_exit, respectively.

        the_list.push_back(new_struc);

        if (is_on_entry)
          add_one_use(parent_var);
        else
          add_one_def(parent_var);

        if (child_node->targets())
          add_structures(is_on_entry, the_list, child_var, child_node);
      }
      else {

        // -- ERROR: Couldn't find the target child

        _annotations->Error(child_node->line(),
                            string("Could not find target \"") + child_node->name() +
                            string("\""));

        return;
      }
    }
}

/** @brief Add structured globals
 *
 * This method is only used to process the global structure annotations. It
 * is similar to add structures with the following exceptions: (1) all the
 * new variables that it discovers are added as globals, (2) all "dot"
 * operators go into the on_entry annotations and (3) all "arrow" operators
 * go into the on_exit annotations. This way we can just call
 * Analyzer::process_on_entry() and Analyzer::process_on_exit() to set them
 * up. */

void procedureAnn::add_global_structures(Annotations * annotations,
                                         annVariable * parent_var,
                                         structureTreeAnn * parent_node)
{
  structureAnn * new_struc = NULL;
  annVariable * child_var = NULL;

  // -- Make sure we have a pointer rule

  pointerRuleAnn * pointer_rule = default_pointer_rule();

  if ( ! pointer_rule) {

    // -- This is just to hold the effects

    _default_pointer_rule = new pointerRuleAnn((exprAnn *)0,
                                               (structuretree_list *)0, parent_node->line());

    pointer_rule = _default_pointer_rule;

    _pointer_rules.push_back(pointer_rule);
  }

  // -- Now compile the structures

  structuretree_list * structures = parent_node->targets();

  for (structuretree_list_p p = structures->begin();
       p != structures->end();
       ++p)
    {
      structureTreeAnn * child_node = *p;

      new_struc = 0;

      // -- CASE 1: This is the target of an arrow operator. In this case,
      // we look up the target and then create a new structureAnn.

      if (parent_node->op() == structureTreeAnn::Arrow) {
        child_var = annotations->add_one_global(child_node->name(),
                                                child_node->is_io());
        new_struc = new structureAnn(parent_var, child_var, 0, child_node->line());

        // -- This is automatically a def

        add_one_def(parent_var);

        // -- All arrow operators go into the on_exit annotations

        pointer_rule->effects().push_back(new_struc);
      }

      // -- CASE 2: This is the target of a dot operator. The target name
      // is a concatenation of the parents name with the dot, and the
      // current node's name (which is basically the field name).

      if (parent_node->op() == structureTreeAnn::Dot) {
        assert (parent_var != NULL);
        string child_name = parent_var->name() + "." + child_node->name();
        child_var = annotations->add_one_global(child_name,
                                                child_node->is_io());
        new_struc = new structureAnn(parent_var, child_var,
                                     & child_node->name(), child_node->line());

        // -- This is automatically a use

        add_one_use(parent_var);

        // -- All dot operators go into the on_entry annotations

        _on_entry.push_back(new_struc);
      }

   // DQ (9/12/2011): Static analysis reports that this could be NULL, check for it explicitly.
      assert(child_node != NULL);
      if (child_node->targets())
         {
        // DQ (9/13/2011): Static analysis reports that this could be NULL, check for it explicitly.
           assert(child_var  != NULL);
           add_global_structures(annotations, child_var, child_node);
         }
    }
}

void procedureAnn::add_analysis(analyzeAnn * new_analysis)
{
  // NOTE: for global analysis, this implementation means that we can't
  // have more than one global analyze annotation for each property.

  propertyAnn * property = new_analysis->property();
  _analyses[property] = new_analysis;
}

void procedureAnn::add_report(reportAnn * report)
{
  _reports.push_back(report);
}

#ifdef __FOO

void procedureAnn::add_action(actionAnn * action)
{
  _actions.push_back(action);
}

void procedureAnn::add_calling_context(operandNode * call, procLocation * where)
{
  threeAddrNode * stmt = (threeAddrNode *) where->stmt_location()->stmt();

  assert(stmt->typ() == ThreeAddr);

  callsiteEntry * callsite = 0;

  // -- Lookup the entry, creating one if necessary

  callsite_map_p p = _callsites.find(stmt);
  if (p != _callsites.end())
    callsite = (*p).second;
  else {
    callsite = new callsiteEntry(stmt, call);
    _callsites[stmt] = callsite;
  }

  // -- Add the given calling context

  callsite->contexts().insert(where);
}

callsiteEntry * procedureAnn::find_callsite(threeAddrNode * stmt)
{
  callsite_map_p p = _callsites.find(stmt);
  if (p != _callsites.end())
    return (*p).second;
  else
    return 0;
}

#endif

void procedureAnn::clear()
{
#ifdef __CALLSITE_MAP
  // -- Delete all the callsite entries

  for (callsite_map_p p = _callsites.begin();
       p != _callsites.end();
       ++p)
    delete (*p).second;

  _callsites.clear();
#endif
  // -- Clear all the variable bindings

#ifdef __PROCLOCATION

  for (var_map_p p = _variables.begin();
       p != _variables.end();
       ++p)
    {
      (*p).second->clear_all();
    }

#endif
}

#ifdef __FOO

// ----------------------------------------------------------------------
//  Action annotations (reports, actions)
// ----------------------------------------------------------------------

void procedureAnn::process_reports(Analyzer * analyzer)
{
  propertyAnalyzer * property_analyzer = analyzer->property_analyzer();

  // -- For each callsite (location in the text of the program)...

  for (callsite_map_cp ccp = callsites().begin();
       ccp != callsites().end();
       ++ccp)
    {
      callsiteEntry * callsite = (*ccp).second;

      const proclocation_set & proclocations = callsite->contexts();
      operandNode * call = callsite->call();
      operand_list call_args = callsite->stmt()->arg_list();

      // -- And for each context that reaches this callsite...

      for (proclocation_set_cp pp = proclocations.begin();
           pp != proclocations.end();
           ++pp)
        {
          procLocation * libproc_location = *pp;

          // -- Make sure all the defs and uses are set

          analyzer->setup_all_bindings(this, call, call_args, libproc_location);

          // -- Now it's safe to call compute_all_constants

          property_analyzer->compute_all_constants(libproc_location, this);

          // -- Invoke each report

          for (report_list_cp p = reports().begin();
               p != reports().end();
               ++p)
            (*p)->report(cout, analyzer, libproc_location, property_analyzer);
        }
    }
}

void procedureAnn::test_actions(Analyzer * analyzer)
{
  memoryblock_set unused_defs;
  memoryuse_set unused_uses;
  memoryblock_set unused_changes;

  propertyAnalyzer * property_analyzer = analyzer->property_analyzer();

  // -- For each callsite (location in the text of the program)...

  for (callsite_map_cp ccp = callsites().begin();
       ccp != callsites().end();
       ++ccp)
    {
      callsiteEntry * callsite = (*ccp).second;
      const proclocation_set & proclocations = callsite->contexts();
      operandNode * call = callsite->call();
      operand_list call_args = callsite->stmt()->arg_list();

      // -- And for each context that reaches this callsite...

      for (proclocation_set_cp pp = proclocations.begin();
           pp != proclocations.end();
           ++pp)
        {
          procLocation * libproc_location = *pp;

          // -- Make sure all the defs and uses are set

          analyzer->setup_all_bindings(this, call, call_args, libproc_location);

          // -- Now it's safe to call compute_all_constants

          property_analyzer->compute_all_constants(libproc_location, this);

          // -- Test each action annotation

          for (action_list_cp p = actions().begin();
               p != actions().end();
               ++p)
            (*p)->test(callsite, libproc_location, property_analyzer);
        }
    }
}

actionAnn * procedureAnn::find_applicable_action(threeAddrNode * stmt)
{
  // -- See if this procedure is actually called here

  callsiteEntry * callsite = find_callsite(stmt);
  if (callsite) {

    // -- Look for an action that applies here

    for (action_list_cp p = actions().begin();
         p != actions().end();
         ++p)
      {
        actionAnn * action = *p;

        if (action->is_applicable(callsite))
          return action;
      }
  }

  return 0;
}

#endif

// ----------------------------------------------------------------------
//  Print the annotations
// ----------------------------------------------------------------------

// --- Output

typedef structure_list::const_iterator structure_list_cp;

void procedureAnn::print(ostream & o) const
{
  o << "procedure " << _name << " (";

  for (var_list_cp p = _formal_params.begin();
       p != _formal_params.end();
       ++p)
    {
      annVariable * decl = *p;
      if (p != _formal_params.begin())
        o << ", ";
      o << decl->name();
    }
  o << ")" << endl;
  o << "{" << endl;

  o << "  on_entry {" << endl;
  for (structure_list_cp p = _on_entry.begin();
       p != _on_entry.end();
       ++p)
    {
      structureAnn * sa = *p;
      o << "    " << *sa << ";" << endl;
    }
  o << "  }" << endl;

  o << "  access { ";
  for (var_set_cp p = _uses.begin();
       p != _uses.end();
       ++p)
    {
      annVariable * decl = *p;
      if (p != _uses.begin())
        o << ", ";
      o << decl->name();
    }
  o << "  }" << endl;

  o << "  modify { ";
  for (var_set_cp p = _defs.begin();
       p != _defs.end();
       ++p)
    {
      annVariable * decl = *p;
      if (p != _defs.begin())
        o << ", ";
      o << decl->name();
    }
  o << "  }" << endl;

  o << "  on_exit {" << endl;
  for (structure_list_cp p = _on_exit.begin();
       p != _on_exit.end();
       ++p)
    {
      structureAnn * sa = *p;
      o << "    " << *sa << ";" << endl;
    }
  for (pointerrule_list_cp r = _pointer_rules.begin(); r!= _pointer_rules.end();
       r++) { // TB
    if((*r)->condition()) {
      cout << "    if ( ";
      (*r)->condition()->print(cout);
      cout << ") {\n";
    } else
      cout << "    default {\n";
    for (structure_list_cp p = (*r)->effects().begin();
         p != (*r)->effects().end();
         ++p)
      {
        structureAnn * sa = *p;
        o << "    " << *sa << ";" << endl;
      }
    cout << "    }\n";
  }
  o << "  }" << endl;

  o << "  // variables: " << endl;
  for (var_map_cp p = _variables.begin();
       p != _variables.end();
       ++p)
    {
      annVariable * decl = p->second;
      o << "  //    " << * decl << endl;
    }
  o << endl;

  o << "  // global variable defs:" << endl;
  for (var_set_cp p = _global_defs.begin();
       p  != _global_defs.end();
       ++p)
    {
      annVariable * glob = *p;
      o << "  //    " << * glob << endl;
    }
  o << endl;

  o << "  // global variable uses:" << endl;
  for (var_set_cp p = _global_uses.begin();
       p  != _global_uses.end();
       ++p)
    {
      annVariable * glob = *p;
      o << "  //    " << * glob << endl;
    }
  o << endl;

  o << "}" << endl;
}

