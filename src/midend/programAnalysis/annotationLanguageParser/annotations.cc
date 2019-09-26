
#include "broadway.h"
#ifdef __FOO
#include "semcheck.h"
#include "ref_clone_changer.h"
#endif
#include <unistd.h>
#include <stdlib.h>

// DQ (12/10/2016): Added assert.h to support fix for warning we want to make an error.
#include <assert.h>

using namespace std;

// ------------------------------------------------------------
// CLASS: Annotations
// ------------------------------------------------------------

extern FILE * annin;
extern int annparse();

Annotations * Annotations::Current = 0;

/** @brief Public debug flag: turns on all verbosity */

bool Annotations::debug = false;

/** @brief Show actions as they are applied */

bool Annotations::Show_actions = false;

/** @brief Show reports as they are evaluated */

bool Annotations::Show_reports = false;

/** @brief Debug properties */

bool Annotations::Verbose_properties = false;

/** @brief Debug pointers only */

bool Annotations::Verbose_pointers = false;

/** @brief Flow insensitive properties */

bool Annotations::Flow_insensitive_properties = false;

/** @brief Skip traces */

bool Annotations::Skip_traces = false;

/** @brief Adaptivity flag */

int Annotations::Adaptivity = 0;

/** @brief Show adaptivity algorithm */

bool Annotations::Show_adaptivity = false;

/** @brief Current precision policy to use in adaptive mode. */
// TB_unify
Annotations::Adapt_Precision Annotations::Adapt_precision = Annotations::FICI;

/** @brief Is unification being used? */
// TB_unify
bool Annotations::Adapt_uses_uba = false;

/** @brief Error-oriented adaptivity
 *
 * In this mode, the adaptive algorithm never makes ancestors of a
 * procedure with errors context sensitive. */

bool Annotations::Error_oriented_adaptivity = false;

/** @brief Turn on tested object recording
 *
 * For the adaptive algorithm. */

bool Annotations::Record_tested_objects = false;

  /** @brief Quiet reports
   *
   * When true, don't actually print the reports, but do all the other
   * processing, including adaptivity and accuracy. */

bool Annotations::Quiet_reports = false;

  /** @brief Prune context sensitive
   *
   * When true, post-process the list of context sensitive procedures
   * proposed by the adative algorithm. Verify that context sensitivity is
   * really warranted. */

bool Annotations::Prune_context_sensitive = true;


  /** @brief Pruned procedures
   *
   * Keep a list of the pruned procedures. */

str_set Annotations::Pruned_procedures;

  /** @brief Analysis time
   *
   * Use this value to keep an accumulated total for the adaptive modes. */

double Annotations::Analysis_time = 0.0;
  
  /** @brief Enabled properties */

str_list Annotations::Enabled_properties;

  /** @brief Disabled properties */

str_list Annotations::Disabled_properties;
  
  /** @brief Start of heap */

unsigned long int Annotations::Start_brk = 0;

  /** @brief Aggressive adaptive pruning
   *
   * When true, use use-def chains to prune adaptivity. */

bool Annotations::Aggressive_pruning = false;

  /** @brief Error reports
   *
   * For reports that are designated as "errors" keep a record of the
   * places they occur. */

#ifdef __FOO
error_report_map Annotations::Error_reports;
#endif
  /** @brief Number of enabled properties
   *
   * */

int Annotations::Num_enabled_properties = 0;

  /** @brief Error diagnostic */
bool Annotations::Diagnostic = false;

  /** @brief Collect statistics on data that affects precision policy built by
   * the adaptor. */

Adaptor_Statistics *Annotations::Adaptor_statistics = NULL; // TB

// ----------------------------------------------------------------------

/** @brief Constructor */

Annotations::Annotations(string & filename, str_list * cpp_flags)
  : _filename(filename),
    _globals(),
    _init(0),
    _enum_properties(),
    _constants(0),
    _set_properties(),
    _procedures(),
    _errors(0),
#ifdef __HEADER
    _header(0),
#endif
    _cpp_flags(cpp_flags)
#ifdef __SYMS
    ,_syms(0)
#endif
{
  // -- Open the input file

  if ((filename == "-") ||
      (filename == "(stdin)"))
    annin = stdin;
  else
    annin = _open_input_file();

  // -- Create a special dummy enumPropertyAnn that represents the constant
  // propagation analysis

  parserID consname("constants", 0);
  _constants = new enumPropertyAnn(&consname, Forward, true, (parserID *)0,
			       (enumvalue_list *)0 );

  add_enum_property(_constants);

  // _enum_properties[_constants->name()] = _constants;

  // -- Create the special init procedure

  parserID init_name("__init", 0);
  _init = new procedureAnn(&init_name, (parserid_list *)0,
			   this, 0);

  // -- Create a special "null" global variable. We'll set up the binding
  // in the analyzer.

  // annVariable * null_var = new_global("null", false, false);

  // -- Run the parser

#ifdef __HEADER
  _header = new unitNode(filename, "", "");
  CBZ::current_unit = _header;
#endif

  Current = this;
#ifdef CBZ
  CBZ::set_file(filename.c_str(), 1, true);
#endif
  /*int parse_ok =*/ annparse();

  Current = 0;

  // -- Post-process the init procedure

  _init->postprocess();

  /*

  string nullname("null");
  bool is_new = false;
  bool is_global = true;
  bool is_external = false;
  bool is_formal = false;
  bool is_io = false;
  string procname("(global)");


  annVariable * null_var = new annVariable(nullname, procname,
					   is_new, is_global,
					   is_external, is_formal, is_io);
  null_var->set_global();
  _globals[nullname] = null_var;
  */
}

Annotations::~Annotations()
{
  // -- Delete all global variables

  for (var_map_p p = _globals.begin();
       p != _globals.end();
       ++p)
    {
      annVariable * var = (*p).second;
      delete var;
    }

  // -- Delete all properties

  for (enum_property_map_p p = _enum_properties.begin();
       p != _enum_properties.end();
       ++p)
    delete (*p).second;

  for (set_property_map_p p = _set_properties.begin();
       p != _set_properties.end();
       ++p)
    delete (*p).second;

  // -- Delete all procedure annotations

  for (procedures_map_p p = _procedures.begin();
       p != _procedures.end();
       ++p)
    delete (*p).second;
}

#ifdef _FOO
void Annotations::clear()
{
  // -- Clear all global variables

  for (var_map_p p = _globals.begin();
       p != _globals.end();
       ++p)
    {
      (*p).second->clear_all();
    }

  // -- Clear all properties

  for (enum_property_map_p p = _enum_properties.begin();
       p != _enum_properties.end();
       ++p)
    {
      (*p).second->clear();
    }

  for (set_property_map_p p = _set_properties.begin();
       p != _set_properties.end();
       ++p)
    {
      (*p).second->clear();
    }

  // -- Clear all procedure annotations

  for (procedures_map_p p = _procedures.begin();
       p != _procedures.end();
       ++p)
    {
      (*p).second->clear();
    }
}
#endif  

// --- Procedure operations

void Annotations::add_procedure(procedureAnn * new_procedure)
{
  bool ok_to_add = false;

  new_procedure->postprocess();

  procedureAnn * old = lookup_procedure(new_procedure->name());
  if (old != 0)
    Error(new_procedure->line(), string("Procedure ") +
	  new_procedure->name() + " is already defined");

  // Look up the function declaration

#ifdef __SYMS
  declNode * decl = _syms->externs()->lookup(new_procedure->name());
  if (decl) {
    if (decl->type()->typ() != Func)
      Warning(new_procedure->line(), string("Annotation ") +
	      new_procedure->name() + " is not a function in the header -- annotation ignored.");
    else {

      // -- Check the number of arguments

      funcNode * func = (funcNode *) decl->type();
      if (func->args().size() != new_procedure->formal_params().size()) {

	// -- Different number of args, filter out the weird "void" case.

	if ( ! (func->is_void_args() && (new_procedure->formal_params().size() == 0)))
	  Warning(new_procedure->line(), string("In procedure ") +
		  new_procedure->name() + ", number of arguments does not match header -- annotation ignored.");
	else
	  ok_to_add = true;
      }
      else {
	funcNode * type_copy = (funcNode *) ref_clone_changer::clone(decl->type(), false);
	new_procedure->proc()->decl()->type(type_copy);
	ok_to_add = true;
      }
    }
  }
  else
    Warning(new_procedure->line(), string("Procedure ") +
	    new_procedure->name() + " is not declared in the header -- annotation ignored.");
#else
  ok_to_add = true;
#endif

  if (ok_to_add)
    _procedures[new_procedure->name()] = new_procedure;
}

procedureAnn * Annotations::lookup_procedure(const string & name)
{
  procedures_map_p p = _procedures.find(name);
  if (p == _procedures.end())
    return 0;
  else
    return (*p).second;
}

// --- Global variable operations. We have a special case here. If the name
// of the global variable matches the name of an actual global variable in
// the program, then we force a binding to that variable.

void Annotations::add_globals(structuretree_list * structures)
{
  if (structures) {
    for (structuretree_list_p p = structures->begin();
	 p != structures->end();
	 ++p)
      {
	structureTreeAnn * top_node = *p;

	// -- Start with the first variable...

	annVariable * top_var = add_one_global(top_node->name(), 
					       top_node->is_io());

	// -- Build the structure underneath it

	if (top_node->targets())
	  _init->add_global_structures(this, top_var, top_node);

	delete top_node;
      }
  }
}

/** @brief Add one global
 *
 * Look up the given name, and create a new global variable if
 * necessary. If one already exists, just return it. This method also
 * handles looking up actual global variables declared in the header
 * file. */

annVariable * Annotations::add_one_global(const string & varname, bool is_io)
{
  annVariable * global = lookup_global(varname);
  if (global == 0) {

    bool is_external = false;

    // -- Check to see if this global represents an actual global variable
    // in the header files.

#ifdef __HEADER
    declNode * ext = lookup_header_decl(varname);
#else
    char *ext = NULL;
#endif
    if (ext)
      is_external = true;

    // Create a new global

    global = new_global(varname, is_external, is_io);
  }
  else {

    // This option allows us to mark existing globals as I/O

    if (global->is_external()) {
      if (is_io)
	global->set_io();
    }
  }

  return global;
}

annVariable * Annotations::new_global(const string & varname,
				      bool is_external,
				      bool is_io)
{
  bool is_new = false;
  bool is_global = true;
  bool is_formal = false;
  string procname("(global)");

  annVariable * g = new annVariable(varname,
				    procname,
				    is_new,
				    is_global,
				    is_external,
				    is_formal,
				    is_io);

  _globals[varname] = g;

  return g;
}

annVariable * Annotations::lookup_global(const string & name)
{
  // Search the list of globals, by name

  var_map_p p = _globals.find(name);
  if (p == _globals.end()) {

    // Not found, see if it's an application-level global variable

#ifdef __HEADER
    declNode * ext = lookup_header_decl(name);
#else
    char *ext = NULL;
#endif
    if (ext) {

      // If it is, create a variable for it.

      bool is_external = true;
      bool is_io = false;
      /*annVariable * g =*/ new_global(name, is_external, is_io);
    }
    else
      return 0;
  }
  else
    return (*p).second;

// DQ (12/10/2016): Added return statement, but because this was not specified, we want to make it an error to preserve the previous semantics.
   printf ("Exiting at a previously undefined function return location. \n");
   assert(false);

// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=return-type.
   return NULL;
}

// --- Access information from the header file

#ifdef __HEADER
declNode * Annotations::lookup_header_decl(const string & name)
{
  declNode * d = 0;

  d = _syms->ids()->lookup(name);
  if (!d)
    d = _syms->externs()->lookup(name);

  return d;
}
#endif

// --- Add an enum property

void Annotations::add_enum_property(enumPropertyAnn * property)
{
  // -- Make sure it doesn't already exist

  propertyAnn * existing = lookup_property(property->name());
  if ( ! existing ) {

    // -- Add it

    int id = _enum_properties.size();
    _enum_properties[property->name()] = property;
    property->set_id(id);
  }
  else
    Error(property->line(), string(" property \"") + property->name() +
	  "\" already defined.");
}

// --- Lookup a property

enumPropertyAnn * Annotations::lookup_enum_property(const string & name)
{
  enum_property_map_p p = _enum_properties.find(name);
  if (p == _enum_properties.end())
    return 0;
  else
    return (*p).second;
}

// --- Add a setProperty

void Annotations::add_set_property(setPropertyAnn * setProperty)
{
  // -- Make sure it doesn't already exist

  propertyAnn * existing = lookup_property(setProperty->name());
  if ( ! existing ) {

    // -- Store the new property

    int id = _set_properties.size();
    _set_properties[setProperty->name()] = setProperty;
    setProperty->set_id(id);

    // -- Generate a special global variable for it's value

    annVariable * property_var = new_global(setProperty->name(), false, false);

    // -- Pass it back to the property

    setProperty->set_property_variable(property_var);

    // -- Record that fact that this is a special property variable. We'll
    // need to test this later in the analyzer to determine how to merge
    // it.

    property_var->set_property_variable();
  }
  else
    Error(setProperty->line(), string(" set property \"") + setProperty->name() +
	  "\" already defined.");
}

// --- Lookup a setProperty

setPropertyAnn * Annotations::lookup_set_property(const string & name)
{
  set_property_map_p p = _set_properties.find(name);
  if (p == _set_properties.end())
    return 0;
  else
    return (*p).second;
}

/** @brief Lookup either kind of property */

propertyAnn * Annotations::lookup_property(const string & name)
{
  propertyAnn * result = 0;

  result = lookup_enum_property(name);

  if ( ! result)
    result = lookup_set_property(name);

  return result;
}

// --- Error handling

void Annotations::Error(int where, const string & msg)
{
  _errors++;

  cout << "Error in \"" << _filename << "\" line " << where << ": " << msg << endl;

  /*
    if (procedureAnn::Current()) {
    cout << "  in procedure " << procedureAnn::Current()->name() << endl;
  }
  */
}

void Annotations::Warning(int where, const string & msg)
{
  cout << "Warning: in \"" << _filename << "\" line " << where << ": " << msg << endl;

  /*
    if (procedureAnn::Current()) {
    cout << "  in procedure " << procedureAnn::Current()->name() << endl;
  }
  */
}

// --- Output

void Annotations::print(ostream & o) const
{
  o << "// File: " << _filename << endl;
  o << "// Errors: " << _errors << endl;

  o << "global {";
  for (var_map_cp p = _globals.begin();
       p != _globals.end();
       ++p)
    {
      annVariable * glob = p->second;

      if (p != _globals.begin())
	o << ", ";

      o << glob->name();
    }
  o << "}" << endl;

  o << "// Properties" << endl;
  for (enum_property_map_cp p = _enum_properties.begin();
       p != _enum_properties.end();
       ++p)
    {
      enumPropertyAnn * prop = (*p).second;
      prop->print(o);
    }

  for (set_property_map_cp p = _set_properties.begin();
       p != _set_properties.end();
       ++p)
    {
      setPropertyAnn * rel = (*p).second;
      rel->print(o);
    }

  o << endl;

  for (procedures_map_cp p = _procedures.begin();
       p != _procedures.end();
       ++p)
    {
      procedureAnn * proc = p->second;
      o << *proc << endl;
    }
}

#ifdef __HEADER
void Annotations::store_header(decl_list * decls)
{
  // -- Store all the definitions on the unitNode

  def_list defs;

  for (decl_list_p p = decls->begin();
       p != decls->end();
       ++p)
    _header->defs().push_back(*p);

  // -- Run the fixer-upper

  _header->fixup();

  // -- Semcheck the whole thing

  semcheck_walker::check(_header, true);

  // -- Build the symbol table

  _syms = id_lookup_walker::fixup_and_return(_header, false);

  delete decls;
}
#endif

// --- Open the given annotation file, first passing it through the
// preprocessor to expan the header file inclusions and resolve any
// #define's used in the annotations. This method is pretty much
// identical to the method in unitNode.

FILE * Annotations::_open_input_file()
{
  FILE * in_file;
  bool std_in;

  std_in = (_filename == "(stdin)");
  if (! std_in &&
      (access(_filename.c_str(), F_OK) != 0) &&
      (access(_filename.c_str(), R_OK) != 0)) {
    cerr << "Unable to open input file " << _filename << endl;
    exit(1);
  }

#ifdef __CBZ
  if (CBZ::Preprocess && ! std_in) {
    string command = CBZ::cc_cmd + " -E " + CBZ::preproc_flags;
    for (str_list_p p = _cpp_flags->begin();
	 p != _cpp_flags->end();
	 ++p)
      command += " " + (*p);

    command += " " + _filename;

    CBZ::ShowPreprocess = true;
    if (CBZ::ShowPreprocess)
      cout << "Preprocess: " << command << endl;

    in_file = popen(command.c_str(), "r");

    if (! in_file) {
      cerr << "Unable to preprocess input file " << _filename << endl;
      cerr << "Command: " << command << endl;
      exit(1);
    }
  }
  else {
#endif /* __CBZ */
    if (! std_in) {
      in_file = fopen(_filename.c_str(), "r");

      if (! in_file) {
	cerr << "Unable to open input file " << _filename << endl;
	exit(1);
      }
    }
    else
      in_file = stdin;
#ifdef __CBZ
  }
#endif /* __CBZ */
  return in_file;
}

