
#include "c_breeze.h"

#include <bitset>

#include "semcheck.h"
#include "scope_walker.h"
#include "id_lookup_walker.h"
#include "name_mangle_walker.h"
#include "ref_clone_changer.h"
#include "ssa.h"
#include "pointers.h"
#include "unreachable.h"
#include "dismantle.h"
#include "cfg.h"
#include "callgraph.h"
#include "precision.h"
#include "unification.h"

#include "broadway.h"

//#include <sys/time.h>
#include <unistd.h>

using namespace std;

// ------------------------------------------------------------
//  Remove empty basic blocks
// ------------------------------------------------------------

class CFGSimplifier
{
public:

  static labelNode * get_label(basicblockNode * bb)
  {
    for (stmt_list_p p = bb->stmts().begin();
	 p != bb->stmts().end();
	 ++p)
      {
	if ((*p)->typ() == Label)
	  return (labelNode *) (*p);
      }
    return (labelNode *)0;
  }

  static void fix(unitNode * Unit)
  {
    for (def_list_p dp = Unit->defs().begin();
	 dp != Unit->defs().end();
	 ++dp)
      {
	defNode * dn = (*dp);
	if (dn->typ() == Proc) {
	  procNode * pr = (procNode *) dn;

	  // -- Visit the basic blocks and remove any unused labels

	  stmt_list & stmts = pr->body()->stmts();
	  stmt_list_p q = stmts.begin();
	  stmt_list_p temp2;

	  while (q != stmts.end()) {
	    stmtNode * stmt = *q;
	    temp2 = q;
	    q++;
	    if (stmt->typ() == Block) {
	      basicblockNode * bb = (basicblockNode *) stmt;

	      if (bb->stmts().size() == 1) {
		labelNode * label = get_label(bb);
		if (label) {
		  
		  // -- Found a basic block that consists only of a label.

		  // -- Get the successors label
		  
		  basicblockNode * succ = bb->succs().front();
		  labelNode * succ_label = get_label(succ);

		  // -- Get a *copy* of the reference list:
		  
		  goto_list references = label->references();
		  
		  if (succ_label &&
		      (! references.empty()))
		    {
		      // cout << "Found empty block: " << label->name() << endl;

		      for (goto_list_p p = references.begin();
			   p != references.end();
			   ++p)
			{
			  gotoNode * g = (*p);

			  // cout << "Redirect: goto " << g->name() 
			  //     << " to goto " << succ_label->name() << endl;
			  
			  g->label(succ_label);
			  g->name(succ_label->name());
			}

		      // -- Get rid of the basic block

		      stmts.erase(temp2);
		    }
		}
	      }
	      else {

		// -- Check for blocks with an if-goto, but only one
		// successor

		if (bb->succs().size() == 1) {

		  stmtNode * last = bb->stmts().back();
		  if (last->typ() == If) {
		    bb->stmts().pop_back();
		    ifNode * the_if = (ifNode *) last;
		    blockNode * tr = the_if->get_true_br();
		    bb->stmts().push_back(tr);
		  }
		}
	      }
	    }
	  }
	}
      }
  }
};

// ------------------------------------------------------------
// Make sure its a tree
// ------------------------------------------------------------

typedef set< Node * > node_set;
typedef node_set::iterator node_set_p;

class TreeFixer : public Changer
{
public:

  static void fix(Node * node)
  {
    TreeFixer tf;
    node->change(tf);
  }

private:

  node_set _nodes;

  TreeFixer()
    : Changer(Preorder, Subtree, false),
      _nodes()
  {}

public:

  Node * at_node(Node * the_node, Order ord)
  {
    cout << "At " << the_node << endl;
    node_set_p f = _nodes.find(the_node);
    if (f == _nodes.end()) {
      _nodes.insert(the_node);
      return the_node;
    }
    else {
      Node * replacement = ref_clone_changer::clone(the_node, true);
      cout << "Clone: replace with " << replacement << endl;
      return replacement;
    }
  }
};

class bdwySimplifier :public Changer
{
public:

  static void fix(unitNode * node)
  {
    bdwySimplifier simp;
    node->change(simp);

    if (simp._count > 0)
      cout << "Simplifier: removed " << simp._count << " branches from " << node->input_file() << endl;
  }

  bdwySimplifier()
    : Changer( Preorder, Subtree, true),
      _count(0)
  {}

  Node * at_conditiongoto(conditiongotoNode * C, Order ord)
  {
    bool is_constant = false;
    bool which_branch;

    // -- Get the value of the condition

    exprNode * expr = NULL;
    if(C->left()) {
      if(! C->right()) {
        if(C->op()) // unary
          expr = new unaryNode(C->op()->id(), C->left());
        else
          expr = C->left();
      } else if(C->right())
        expr = new binaryNode(C->op()->id(), C->left(), C->right());
    }

    if(expr) {
      expr->eval();
      if ( ! expr->value().no_val()) {
        is_constant = true;
        which_branch = expr->value().Boolean();
      }
      if(expr != C->left()) delete expr;
    }

    // -- If we can determine the outcome, replace the condition with the
    // appropriate branch.

    if (is_constant) {

      /*
      cout << "FOUND: constant branch at " << the_if->coord() << endl;
      output_context oc(cout);
      the_if->output(oc,0);
      cout << endl;
      */

      _count++;
      if (! which_branch)
        return new exprstmtNode(0);
      // replace with a simple goto.
      return new gotoNode(C->label(), C->coord());
    } else
      return C;
  }

private:

  int _count;

};

class fixupPhase : public Phase
{
public:

  static procNode * Main;
  static unitNode * Unit;
  static bool Fixed;

  static void cleanup_unit(unitNode * Unit)
  {
    // -- First establish all the variable declarations, and goto-labels

    // Unit->fixup();

    // -- Then remove the unused ones

    for (def_list_p dp = Unit->defs().begin();
	 dp != Unit->defs().end();
	 ++dp)
      {
	defNode * dn = (*dp);
	if (dn->typ() == Proc) {
	  procNode * pr = (procNode *) dn;

	  // -- Visit all block-level declarations and removed the unused
	  // ones

	  decl_list & decls = pr->body()->decls();
	  decl_list_p p = decls.begin();
	  decl_list_p temp;

	  while (p != decls.end()) {
	    declNode * decl = *p;
	    temp = p;
	    p++;
	    if (decl->references() == 0) // TB: wrong
	    if (decl->ref_list().empty())
	      decls.erase(temp);
	  }

	  // -- Visit the basic blocks and remove any unused labels

	  stmt_list & stmts = pr->body()->stmts();
	  stmt_list_p q = stmts.begin();
	  stmt_list_p temp2;

	  while (q != stmts.end()) {
	    stmtNode * stmt = *q;
	    temp2 = q;
	    q++;
	    if (stmt->typ() == Label) {
	      labelNode * label = (labelNode *) stmt;
	      if (label->references().empty())
		stmts.erase(temp2);
	    }
	  }
	}
      }
  }

  static void semi_fix_unit(unitNode * u)
  {
    bdwySimplifier::fix(u);

    // TB: id_lookup_walker::fixup(u, false);

    cleanup_unit(u);
  }

  static void fix_unit(unitNode * u)
  {
    // u->fixup();

    bdwySimplifier::fix(u);

    // Dismantle

    cout << "    + Dismantle " << u->input_file() << endl;

    // TB: cfg_changer::generate_cfg (u, DEFAULT_DISMANTLER_FLAGS | INDEX_IS_PRIMITIVE);
    cfg_changer::generate_cfg (u);

    // Fixup indentifiers, removed unused variables and labels

    // TB: id_lookup_walker::fixup(u, false);

    // TB: semcheck_walker::check(u, false);
    cleanup_unit(u);
    // TreeFixer::fix(u);

    // Build CFG


    // cfg_changer cfgc;
    // u->change (cfgc);

    // Remove unreachable code

    // Unreachable::remove(u);
  }

  void run()
  {
    if (Main != 0)
      return;

    for (unit_list_p up = CBZ::Program.begin();
	 up != CBZ::Program.end();
	 ++up) {
      unitNode * u = (*up);
      fix_unit(u);
    }

    // -- Find the main function

    bool found = false;
    for (unit_list_p up = CBZ::Program.begin();
	 up != CBZ::Program.end();
	 ++up)
      {
	Unit = (*up);

	for (def_list_p dp = Unit->defs().begin();
	     dp != Unit->defs().end();
	     ++dp)
	  {
	    defNode * dn = (*dp);
	    if (dn->typ() == Proc) {
	      procNode * pr = (procNode *) dn;
	      // cout << "Fixup at " << pr->decl()->name() << endl;
	      if (pr->decl()->name() == "main") {
		if (Main) {
		  cout << "Warning: Found another main() at " << pr->coord() << endl;
		  cout << "         Using main() at " << Main->coord() << endl;
		}
		else
		  Main = pr;
	      }
	    }
	  }
      }

    Fixed = true;
  }
};

procNode * fixupPhase::Main = 0;
unitNode * fixupPhase::Unit = 0;
bool fixupPhase::Fixed = false;

Phases FixupPhase("fixup", new fixupPhase());

// ------------------------------------------------------------
//  Code cleanup utility
// ------------------------------------------------------------

class codeCleanup
{
public:

  static void cleanup(unitNode * u)
  {
    u->fixup();

    return;

    // -- Visit all statements and declarations, removing the ones not
    // needed.

    for (def_list_p dp = u->defs().begin();
	 dp != u->defs().end();
	 ++dp)
      {
	defNode * dn = (*dp);
	if (dn->typ() == Proc) {
	  procNode * pr = (procNode *) dn;
	  blockNode * body = pr->body();

	  // -- Removed unused variables

	  decl_list_p p = body->decls().begin();
	  while (p != body->decls().end()) {
	    declNode * var = *p;
	    typeNode * type = var->type();
	    assert(type);

	    decl_list_p cur = p;
	    ++p;

	    if ((var->references() == 0) &&
		(type->typ() != Func) &&
		(var->decl_location() == declNode::BLOCK) &&
		(var->storage_class() != declNode::EXTERN))
	      {
		body->decls().erase(cur);
	      }
	  }

	  // -- Remove empty statements

	  for (stmt_list_p q = body->stmts().begin();
	       q != body->stmts().end();
	       ++q)
	    {
	      if ((*q)->typ() == Block) {
		blockNode * block = (blockNode *)(*q);
		stmt_list_p w = block->stmts().begin();

		while (w != block->stmts().end()) {
		  stmtNode * s = *w;

		  stmt_list_p cur = w;
		  ++w;

		  if (s->typ() == Expr) {
		    exprstmtNode * ex = (exprstmtNode *)s;
		    if ( ! ex->expr())
		      block->stmts().erase(cur);
		  }
		}
	      }
	    }
	}
      }
  }
};
	    

// ------------------------------------------------------------
//  Main broadway phase
// ------------------------------------------------------------

//int debug_pass_count; // debug
// extern bool show_bb_count;

class palPhase : public Phase
{
private:

  string _ann_filename;
  bool _defs_uses;
  bool _stats;
  bool _precision;
  bool _adaptor_statistics;

  typedef enum { Reports, Actions, Constants, DeadCode, PointersOnly } PassKind;

  typedef list< PassKind > pass_list;
  typedef pass_list::iterator pass_list_p;

  pass_list _passes;

  Annotations * anns;

  unsigned long int initial_memory;

bool wuftp_debug; // debug
int pass_limit_override;
str_list force_cs_proc;

public:

  void get_flags (str_list_p & arg) {
    _ann_filename = (*arg);
    ++arg;

    initial_memory = (unsigned long int) sbrk(0);

    // Defaults:

    _defs_uses = false;
    _stats = false;
    _precision = false;
    _adaptor_statistics = false;

    bool done = false;
wuftp_debug = false; // debug
pass_limit_override = 0;

    do {

      string & flag = (*arg);

      done = true;

      // -- Broadway passes

if (flag == "wuftp") { wuftp_debug = true; done = false; }
if (flag == "tb-changes") {
  extern bool TB_changes;
  TB_changes = true;
  done = false;
}
/*if (flag == "show-bb-count") {
  show_bb_count = true;
  done = false;
}*/
if (flag == "pass-limit") {
  ++arg;
  pass_limit_override = atoi((*arg).c_str());
  done = false;
}
if (flag == "force-cs") {
  ++arg;
  force_cs_proc.push_back(*arg);
  done = false;
}

      if (flag == "actions") {
	_passes.push_back(Actions);
	done = false;
      }

      if (flag == "reports") {
	_passes.push_back(Reports);
	done = false;
      }

      if (flag == "constants") {
	_passes.push_back(Constants);
	done = false;
      }

      if (flag == "deadcode") {
	_passes.push_back(DeadCode);
	done = false;
      }

      if (flag == "pointers") {
	_passes.push_back(PointersOnly);
	done = false;
      }

      if (flag == "unification") {
	pointerOptions::Unification = true;
        Annotations::Adapt_uses_uba = true;
	done = false;
      }

      if (flag == "show-unification") {
	pointerOptions::Show_Unification = true;
	done = false;
      }

      if (flag == "precision") {
	_precision = true;
	done = false;
      }

      if (flag == "disable") {
	++arg;
	Annotations::Disabled_properties.push_back(*arg);
	done = false;
      }

      if (flag == "enable") {
	++arg;
	Annotations::Enabled_properties.push_back(*arg);
	done = false;
      }

      // -- Precision options

      if (flag == "cs") {
	++arg;
	pointerOptions::Context_sensitive_procedures.insert(*arg);
	done = false;
      }

      if (flag == "context-insensitive") {
	pointerOptions::Context_insensitive = true;
	done = false;
      }

      if (flag == "recursion-ci") {
	pointerOptions::Recursion_Context_sensitive = false;
	done = false;
      }

      if (flag == "no-fields") {
	pointerOptions::Ignore_fields = true;
	done = false;
      }

      if (flag == "flow-insensitive-pointers") {
	pointerOptions::Flow_insensitive = true;
	done = false;
      }

      if (flag == "flow-insensitive-properties") {
	Annotations::Flow_insensitive_properties = true;
	done = false;
      }

      if (flag == "equality-based") {
	pointerOptions::Bidirectional_assignment = true;
	done = false;
      }

      if (flag == "conditional") {
	pointerOptions::Conditional_analysis = true;
	done = false;
      }

      if (flag == "aggressive-multiplicity") {
	pointerOptions::Aggressive_multiplicity = true;
	done = false;
      }

      if (flag == "no-multiplicity") {
	pointerOptions::Use_multiplicity = false;
	done = false;
      }

      if (flag == "use-escape-analysis") {
	pointerOptions::Use_escape_analysis = true;
	done = false;
      }

      if (flag == "ci-memory") {
	pointerOptions::Context_insensitive_memory = true;
	done = false;
      }

      if (flag == "adaptive-1") {
	Annotations::Adaptivity = 1;
	pointerOptions::Flow_insensitive = true;
	Annotations::Flow_insensitive_properties = true;
	pointerOptions::Context_insensitive = true;
	done = false;
      }
      
      if (flag == "adaptive-2") {
	Annotations::Adaptivity = 2;
	pointerOptions::Flow_insensitive = true;
	Annotations::Flow_insensitive_properties = true;
	pointerOptions::Context_insensitive = true;
	done = false;
      }
      
      if ((flag == "adaptive") ||
	  (flag == "adaptive-3")) {

	Annotations::Adaptivity = 3;
	pointerOptions::Monitor_precision = true;

	pointerOptions::Flow_insensitive = true;
	Annotations::Flow_insensitive_properties = true;
	pointerOptions::Context_insensitive = true;

	pointerOptions::Aggressive_multiplicity = true;
	pointerOptions::Use_escape_analysis = true;

	done = false;
      }

      if (flag == "error-oriented") {
	Annotations::Error_oriented_adaptivity = true;
	done = false;
      }

      if (flag == "no-prune") {
	Annotations::Prune_context_sensitive = false;
	done = false;
      }

      if (flag == "aggressive-pruning") {
	Annotations::Aggressive_pruning = true;
	done = false;
      }

      if (flag == "minimal") {
	pointerOptions::Flow_insensitive = true;
	Annotations::Flow_insensitive_properties = true;
	pointerOptions::Context_insensitive = true;
	done = false;
      }

      if (flag == "show-memory-leaks") {
	pointerOptions::Show_memory_leaks = true;
	pointerOptions::Aggressive_multiplicity = true;
	pointerOptions::Use_escape_analysis = true;
      }

      // -- Debug and verbose options
      
      if (flag == "debug") {
	pointerOptions::Verbose = true;
	// pointerOptions::Verbose_constants = true;
	Annotations::debug = true;
	Annotations::Show_actions = true;
	Annotations::Show_reports = true;
	Annotations::Verbose_properties = true;
	Annotations::Verbose_pointers = true;

	done = false;
      }

      if (flag == "monitor") {
	++arg;
	pointerOptions::Verbose_procedures.insert(*arg);
	done = false;
      }

      if (flag == "monitor-call") {
	++arg;
        int id = strtol((*arg).c_str(), 0, 10);
        pointerOptions::Verbose_call_id.push_back(id);
	done = false;
      }

      if (flag == "show-actions") {
	Annotations::Show_actions = true;
	done = false;
      }

      if (flag == "show-reports") {
	Annotations::Show_reports = true;
	done = false;
      }

      if (flag == "quiet-reports") {
	Annotations::Quiet_reports = true;
	done = false;
      }

      if (flag == "skip-traces") {
	Annotations::Skip_traces = true;
	done = false;
      }

      if (flag == "debug-properties") {
	Annotations::Verbose_properties = true;
	done = false;
      }

      if (flag == "debug-constants") {
	Annotations::Verbose_properties = true;
	pointerOptions::Verbose_constants = true;
	done = false;
      }

      if (flag == "debug-pointers") {
	pointerOptions::Verbose = true;
	Annotations::Verbose_pointers = true;
	done = false;
      }

      if (flag == "show-stack") {
	pointerOptions::Show_stack = true;
	done = false;
      }

      if (flag == "show-memoryblocks") {
	pointerOptions::Show_memoryblocks = true;
	done = false;
      }

      if (flag == "show-procedures") {
	pointerOptions::Show_procedures = true;
	done = false;
      }

      if (flag == "show-unknown-procedures") {
	pointerOptions::Show_unknown_procedures = true;
	done = false;
      }

      if (flag == "show-defsuses") {
	_defs_uses = true;
	done = false;
      }

      if (flag == "show-stats") {
	_stats = true;
	done = false;
      }

      if (flag == "show-adaptivity") {
	Annotations::Show_adaptivity = true;
	done = false;
      }

      if (flag == "adaptor-statistics") {
	_adaptor_statistics = true;
	done = false;
      }

      if (flag == "slice-analysis") {
	pointerOptions::Slice_analysis = true;
	done = false;
      }

      if (flag == "diagnostic") {
	Annotations::Diagnostic = true;
	done = false;
      }

      if (flag == "path-sensitive") {
	pointerOptions::Path_sensitive = true;
	done = false;
      }

      if ( ! done )
	++arg;
    } while ( ! done );
  }

  void run()
  {
    unsigned long int after_parse_c = (unsigned long int) sbrk(0);

    Annotations * a = new Annotations(_ann_filename, CBZ::cpp_flags);

    unsigned long int after_parse_pal = (unsigned long int) sbrk(0);

    if ( a->errors() == 0) {
      anns = a;

      if ( ! fixupPhase::Fixed) {
	fixupPhase fix;
	fix.run();
      }

      unsigned long int after_fixup = (unsigned long int) sbrk(0);

      if ( ! fixupPhase::Main) {
	cout << "Error: No main() function found." << endl;
	return;
      }

      // -- Stats file

      ofstream * stats_file = 0;
      if (_stats)
	stats_file = new ofstream("pointer_stats");

      // -- Run the analysis

      cout << "--- Broadway Compiler -------------------------------------------" << endl;

      cout << " sizeof(memoryBlock) = " << sizeof(memoryBlock) << endl;
      cout << " sizeof(memoryDef) = " << sizeof(memoryDef) << endl;
      cout << " sizeof(memoryUse) = " << sizeof(memoryUse) << endl;

      cout << "STAT-memory-parsing     " << (after_parse_c - initial_memory) << endl;
      cout << "STAT-memory-annotations " << (after_parse_pal - after_parse_c) << endl;
      cout << "STAT-memory-fixup       " << (after_fixup - after_parse_pal) << endl;

      // -- Figure out which analyses to perform

      const enum_property_map & enum_properties = anns->enum_properties();
      const set_property_map & set_properties = anns->set_properties();

      // -- Process the "enable" options

      if ( ! Annotations::Enabled_properties.empty()) {

	// -- Start with all properties disabled

	for (enum_property_map_cp q = enum_properties.begin();
	     q != enum_properties.end();
	     ++q)
	  {
	    enumPropertyAnn * property = (*q).second;
	    property->disable();
	  }

	for (set_property_map_cp q = set_properties.begin();
	     q != set_properties.end();
	     ++q)
	  {
	    setPropertyAnn * property = (*q).second;
	    property->disable();
	  }

	// -- Enable each specified property

	for (str_list_p p = Annotations::Enabled_properties.begin();
	     p != Annotations::Enabled_properties.end();
	     ++p)
	  {
	    propertyAnn * property = anns->lookup_property(*p);
	    if (property)
	      property->enable();
	    else
	      cout << "Error: unknown property \"" << *p << "\"" << endl;
	  }
      }

      // -- Process the "disable" options

      for (str_list_p p = Annotations::Disabled_properties.begin();
	   p != Annotations::Disabled_properties.end();
	   ++p)
	{
	  propertyAnn * property = anns->lookup_property(*p);
	  if (property)
	    property->disable();
	  else
	    cout << "Error: unknown property \"" << *p << "\"" << endl;
	}

      // -- Print out which analyses will be used

      cout << "Properties to analyze:" << endl;

      for (enum_property_map_cp q = enum_properties.begin();
	   q != enum_properties.end();
	   ++q)
	{
	  enumPropertyAnn * property = (*q).second;
	  cout << "  + property \"" << property->name() << "\"";
	  if (property->is_enabled()) {
	    Annotations::Num_enabled_properties++;
	    cout << " is enabled" << endl;
	  }
	  else
	    cout << " is disabled" << endl;
	}

      for (set_property_map_cp q = set_properties.begin();
	   q != set_properties.end();
	   ++q)
	{
	  setPropertyAnn * property = (*q).second;
	  cout << "  + property \"" << property->name() << "\"";
	  if (property->is_enabled())
	    cout << " is enabled" << endl;
	  else
	    cout << " is disabled" << endl;
	}

      // -- Starting break point

      Annotations::Start_brk = (unsigned long int) sbrk(0);

      // -- Run the passes

      for (pass_list_p p = _passes.begin();
	   p != _passes.end();
	   ++p)
	{
	  PassKind kind = *p;

	  // -- Link

	  Linker linker;
	  linker.link();

	  // -- Reset the timer

	  Annotations::Analysis_time = 0.0;

	  switch (kind) {
	  case Reports:
	    {
	      cout << "--- Reports ------------------------------------------------------" << endl;

	      if (Annotations::Adaptivity == 3) {

		// -- Adaptive mode

		Analyzer * analyzer = 0;
		bool done = false;

		int cs_procedures = 0;
		int fs_objects = 0;
		int fsp_objects = 0;
		int ps_basicblocks = 0;
		double total_accuracy = 0.0;
		int error_count = 0;
		int new_error_count = 0;

		int pass_count = 0;

		const enum_property_map & props = anns->enum_properties();

                UnificationBasedPtr *uba = NULL; // TB_unify
                int pass_limit = 2;
                bool adapt_use_unify = false;
                if(pointerOptions::Unification) {
                  pass_limit = 3;
                  adapt_use_unify = true;
                  // start with this.
                  Annotations::Adapt_precision = Annotations::UNIFY;
                }
                if(pass_limit_override > 0)
                  pass_limit = pass_limit_override;

		cbzTimer analysis_timer;
		cbzTimer adapt_timer;

extern bool TB_changes;
cout << "TB_changes = " << TB_changes << endl; // debug
cout << "recursion-cs = " << pointerOptions::Recursion_Context_sensitive <<endl;
		while ( ! done) {

		  cout << "START " << pass_count << endl;
//debug_pass_count = pass_count;
if(wuftp_debug && pass_count+1==pass_limit) {
pointerOptions::Show_stack = true;
//pointerOptions::Verbose_procedures.insert("acl_getclass");
//pointerOptions::Verbose_procedures.insert("hostmatch");
}

		  // -- Save the current precision configuration

		  cs_procedures = pointerOptions::Context_sensitive_procedures.size();
		  fs_objects = pointerOptions::Flow_sensitive_objects.size();
		  ps_basicblocks = pointerOptions::Path_sensitive_branches.size();

		  fsp_objects = 0;
		  for (enum_property_map_cp p = props.begin();
		       p != props.end();
		       ++p)
		    {
		      enumPropertyAnn * property = (*p).second;
		      if (property->is_enabled() &&
			  (property != anns->constants_property()))
			fsp_objects += property->count_flow_sensitive_objects();
		      if (property->is_enabled())
                        property->reset_diagnostic();
		    }

		  // -- Run the analysis

		  analyzer = new Analyzer(anns, fixupPhase::Main, linker, fixupPhase::Unit);
		  analysis_timer.start();

                  if(pointerOptions::Unification) { // TB_unify
                    analyzer->run_unification();
                    uba = analyzer->unification_analyzer();
                  } else if(uba)
                    analyzer->use_unification_analyzer(uba);

		  analyzer->analyze_properties();

		  analysis_timer.stop();

		  // -- Do the reports: record tested objects

		  Annotations::Record_tested_objects = true;

		  for (procedures_map_cp procs = a->procedures().begin();
		       procs != a->procedures().end();
		       ++procs)
		    {
		      procedureAnn * libproc_anns = (*procs).second;
		      libproc_anns->process_reports(analyzer);
		    }

		  Annotations::Record_tested_objects = false;

		  // -- Show the errors

		  if ( ! Annotations::Error_reports.empty())
		    cout << "Errors at:" << endl;

		  for (error_report_map_p p = Annotations::Error_reports.begin();
		       p != Annotations::Error_reports.end();
		       ++p)
		    {
		      error_pair key = (*p).first;
		      reportAnn * report = key.first;
		      stmtNode * stmt = key.second;
		      /* TB: remove
                      callNode * call = findCallNodeWalker::find(stmt);

		      cout << "  + Error at " << call->coord() << endl; */
		      cout << "  + Error at " << stmt->coord() << endl;
		    }
		  
		  cout << "STAT-errors-total " << Annotations::Error_reports.size() << endl;

// (hack) print now in case stuck in adaptor
double analysis_time1(analysis_timer);
cout << "STAT-time-analysis                 " << analysis_time1 << endl;

		  new_error_count = Annotations::Error_reports.size();

		  // -- Track the precision loss back from the reports

		  /*
		  struct timeval tv_start;
		  struct timeval tv_end;
		  struct timezone tz;

		  gettimeofday(&tv_start, &tz);
		  */

                  if(_adaptor_statistics)
                    Annotations::Adaptor_statistics
                      = new Adaptor_Statistics(a, analyzer);

		  adapt_timer.start();

		  double new_total_accuracy = 0.0;

                  int precision_analyzed = 0;

		  if(! Annotations::Error_reports.empty())
                    for (enum_property_map_cp p = props.begin();
                         p != props.end();
                         ++p)
                      {
                        enumPropertyAnn * property = (*p).second;
                        if (property->is_enabled() &&
                            (property != anns->constants_property()))
                          {
                            property->precision_analysis(analyzer,
                                                         precision_analyzed++);
                            new_total_accuracy += property->compute_accuracy(analyzer);
                          }
                      }

		  cout << "STAT-accuracy-total " << new_total_accuracy << endl;

		  adapt_timer.stop();

		  // analyzer->show_accuracy();

		  /*
		  gettimeofday(&tv_end, &tz);

		  double start_time = (double) tv_start.tv_sec + ((double)tv_start.tv_usec)/1000000.0;
		  double end_time = (double) tv_end.tv_sec + ((double)tv_end.tv_usec)/1000000.0;
		  double total = end_time - start_time;
		  */
		  // -- Report the new precision parameters

		  int new_cs_procedures = pointerOptions::Context_sensitive_procedures.size();
		  int new_fs_objects = pointerOptions::Flow_sensitive_objects.size();
		  int new_ps_basicblocks = pointerOptions::Path_sensitive_branches.size();
		  int new_fsp_objects;

		  int total_procedures;
		  int analyzed_procedures;
		  int actual_ci_procedures;
		  int recursive_procedures;
		  int unanalyzed_procedures;
		  int program_size;

		  analyzer->number_of_procedures(total_procedures, analyzed_procedures,
						 actual_ci_procedures,
						 recursive_procedures,
						 unanalyzed_procedures,
						 program_size);

		  int actual_cs_procedures = analyzed_procedures - actual_ci_procedures;

		  double analysis_time(analysis_timer);
		  double adapt_time(adapt_timer);
		    
		  cout << " -- Precision ------------------------------" << endl;
		  //cout << "STAT-time-analysis                 " << analysis_time << endl;
		  cout << "STAT-time-adapt                    " << adapt_time << endl;
		  cout << "STAT-procedures-total              " << analyzed_procedures << endl;
		  cout << "STAT-procedures-unanalyzed         " << unanalyzed_procedures << endl;
		  cout << "STAT-program-size                  " << program_size << endl;
		  cout << "STAT-procedures-contextinsensitive " << actual_ci_procedures << endl;
		  cout << "STAT-procedures-recursive          " << recursive_procedures << endl;
		  cout << "STAT-procedures-contextsensitive   " << actual_cs_procedures << endl;

		  cout << "  Path-sensitivity:                " << ps_basicblocks
		       << " out of (a lot)" << endl;		  
		  cout << "STAT-flowsensitive-pointers        " << new_fs_objects
		       << " out of " << analyzer->memory_size() << endl;
                  if(adapt_use_unify) // TB_unify
                    cout << "STAT-unify-objects               "
                         << pointerOptions::Unify_objects 
                         << " out of " << analyzer->memory_size() << endl;
/*extern cbzTimer r_timer, r_timer2;
double r_time(r_timer), r_time2(r_timer2);
cout << "r_timer " << r_time << " " << r_timer2 << endl;*/

		  new_fsp_objects = 0;
		  for (enum_property_map_cp p = props.begin();
		       p != props.end();
		       ++p)
		    {
		      enumPropertyAnn * property = (*p).second;
		      if (property->is_enabled() &&
			  (property != anns->constants_property())) {
			  
			int temp = property->count_flow_sensitive_objects();
			new_fsp_objects += temp;

			cout << "STAT-flowsensitive-property-" << property->name() << " " << temp << endl;
		      }
		    }

		  cout << " -------------------------------------------" << endl;

/*if(show_bb_count) {
extern map<procNode*,int> bb_count;
cout << "basic-block counts:\n";
for(map<procNode*,int>::iterator b=bb_count.begin(); b!=bb_count.end(); b++) {
  procedureInfo *info = analyzer->lookup_procedure(b->first);
  cout << "  " << b->first->decl()->name() << " " << b->second << " "
       << (b->second / info->analysis_count()) << endl;
}
cout << "basic-block counts done\n";
bb_count.clear();
}*/


//pointerOptions::Context_sensitive_procedures.insert("show_message"); // hack
/*
pointerOptions::Context_sensitive_procedures.insert("show_readme"); // hack
pointerOptions::Context_sensitive_procedures.insert("expand_id"); // hack
pointerOptions::Context_sensitive_procedures.insert("hostmatch");
pointerOptions::Context_sensitive_procedures.insert("check_reverse_dns");
pointerOptions::Context_sensitive_procedures.insert("check_matching_dns");
pointerOptions::Context_sensitive_procedures.insert("show_banner");
pointerOptions::Context_sensitive_procedures.insert("fixpath");
pointerOptions::Context_sensitive_procedures.insert("execbrc");
pointerOptions::Context_sensitive_procedures.insert("ftpd_popen");
pointerOptions::Context_sensitive_procedures.insert("sizecmd");
pointerOptions::Context_sensitive_procedures.insert("pr_mesg");
pointerOptions::Context_sensitive_procedures.insert("store");
pointerOptions::Context_sensitive_procedures.insert("yyparse");
pointerOptions::Context_sensitive_procedures.insert("show_message");
pointerOptions::Context_sensitive_procedures.insert("makedir");
pointerOptions::Context_sensitive_procedures.insert("delete");
pointerOptions::Context_sensitive_procedures.insert("retrieve");
pointerOptions::Context_sensitive_procedures.insert("pass");
pointerOptions::Context_sensitive_procedures.insert("upper");
pointerOptions::Context_sensitive_procedures.insert("site_exec");*/

for(str_list_p f=force_cs_proc.begin(); f!=force_cs_proc.end(); f++) {
  pointerOptions::Context_sensitive_procedures.insert(*f);
  cout << "force-cs " << *f << endl;
}

		  if (1 || Annotations::Show_adaptivity) {

                    cout << "Set skipped-procs:                 "
                         << pointerOptions::Skip_procs.size() << endl;
		    cout << "Set context-sensitive:             " << new_cs_procedures << endl;
		    for (str_set_p sp = pointerOptions::Context_sensitive_procedures.begin();
			 sp != pointerOptions::Context_sensitive_procedures.end();
			 ++sp)
		      cout << "  + " << (*sp) << endl;

		    cout << "Pruned out:" << endl;
		    for (str_set_p sp = Annotations::Pruned_procedures.begin();
			 sp != Annotations::Pruned_procedures.end();
			 ++sp)
		      {
			if (pointerOptions::Context_sensitive_procedures.find(*sp) ==
			    pointerOptions::Context_sensitive_procedures.end())
			  cout << "  + " << (*sp) << endl;
		      }
		  }

                  if(Annotations::Adaptor_statistics) {
if(TB_changes) cout << "TB's new adaptor algorithm\n";
else           cout << "Sam's adaptor algorithm with (minor) change\n";
                    Annotations::Adaptor_statistics->print_statistics(cout);
cout << "Broadway done\n";
exit(1);
                  }
		  
		  if (_stats) {
		    cout << "(1) After pass..." << endl;
		    // analyzer->stats(*stats_file);
		    memoryBlock::stats();
		    Location::stats();
		    memoryAccess::stats();
		  }

		  cout << "END " << pass_count << endl;

		  pass_count++;

		  // -- We're done when there are no new proposed changes
		  // to the precision or the total accuracy hasn't changed.

		  cout << "OLD accuracy = " << total_accuracy << ", NEW accuracy = " << new_total_accuracy << endl;
		  if (new_total_accuracy == total_accuracy)
		    cout << " ---> same" << endl;
		  else
		    cout << " ---> different" << endl;

                  if(adapt_use_unify) { // TB_unify
                    if(Annotations::Adapt_precision == Annotations::UNIFY) {
                      Annotations::Adapt_precision = Annotations::FICI;
                      pointerOptions::Unification = false;
                    } else
                      Annotations::Adapt_precision = Annotations::FSCS;
                  }

                  if(adapt_use_unify && // TB_unify
		     Annotations::Error_reports.size() > 0 &&
                     pass_count < pass_limit)
                    done = false;
		  else if (((new_cs_procedures == cs_procedures) &&
                            (new_fs_objects == fs_objects) &&
                            // (new_ps_basicblocks == ps_basicblocks) &&
                            (new_fsp_objects == fsp_objects)) ||
                           //((new_total_accuracy == total_accuracy) &&
                           //  (pass_count > 2)) ||
                           // (new_error_count == error_count) ||
                           (pass_count == pass_limit) ||
                           (Annotations::Error_reports.size() == 0))
		    done = true;

		  if (done && _stats)
		    analyzer->stats(*stats_file);

		  delete analyzer;
                  if(Annotations::Adaptor_statistics) {
                    delete Annotations::Adaptor_statistics;
                    Annotations::Adaptor_statistics = NULL;
                  }

		  if (_stats) {
		    cout << "(2) Analyzer deleted: " << endl;
		    // analyzer->stats(*stats_file);
		    memoryBlock::stats();
		    Location::stats();
		    memoryAccess::stats();
		  }

		  // -- TEMPORARY:
		  /*
		  if (analysis_time > 3600) {
		    cout << "NOTE: Hit time limit; exiting" << endl;
		    done = true;
		  }
		  */

		  /*
		  if ((new_cs_procedures * 4) > analyzed_procedures) {
		    cout << "NOTE: Too many procedures CS; exiting" << endl;
		    done = true;
		  }
		  */
		  // -- Also, we're done if there aren't any errors

		  // if (Annotations::Error_reports.size() == 0)
		  //  done = true;

		  total_accuracy = new_total_accuracy;
		  error_count = new_error_count;
		}
	      }
	      else {

		// -- Regular precision modes

		cbzTimer analysis_timer;

		cout << "START 0" << endl;

		Analyzer * analyzer = 0;

		analyzer = new Analyzer(anns, fixupPhase::Main, linker, fixupPhase::Unit);
		analysis_timer.start();

                analyzer->run_unification(); // TB_unify

		analyzer->analyze_properties();

		analysis_timer.stop();

		if (_defs_uses)
		  analyzer->uses_and_defs();

		for (procedures_map_cp procs = a->procedures().begin();
		     procs != a->procedures().end();
		     ++procs)
		  {
		    procedureAnn * libproc_anns = (*procs).second;
		    libproc_anns->process_reports(analyzer);
		  }

		// -- Show the errors

		if ( ! Annotations::Error_reports.empty())
		  cout << "Errors at:" << endl;

		for (error_report_map_p p = Annotations::Error_reports.begin();
		     p != Annotations::Error_reports.end();
		     ++p)
		  {
		    error_pair key = (*p).first;
		    reportAnn * report = key.first;
		    stmtNode * stmt = key.second;
		      /* TB: remove
		    callNode * call = findCallNodeWalker::find(stmt);
		      
		    cout << "  + Error at " << call->coord() << endl; */
		    cout << "  + Error at " << stmt->coord() << endl;
		  }
		  
		cout << "STAT-errors-total " << Annotations::Error_reports.size() << endl;

		// -- Show accuracy

		double new_total_accuracy = 0.0;

		const enum_property_map & props = anns->enum_properties();
		for (enum_property_map_cp p = props.begin();
		     p != props.end();
		     ++p)
		  {
		    enumPropertyAnn * property = (*p).second;
		    if (property->is_enabled() &&
			(property != anns->constants_property()))
		      {
			new_total_accuracy += property->compute_accuracy(analyzer);
		      }
		  }
		cout << "STAT-accuracy-total " << new_total_accuracy << endl;

		analyzer->show_accuracy();

		int total_procedures;
		int analyzed_procedures;
		int actual_ci_procedures;
		int recursive_procedures;
		int unanalyzed_procedures;
		int program_size;

		analyzer->number_of_procedures(total_procedures, analyzed_procedures,
					       actual_ci_procedures,
					       recursive_procedures,
					       unanalyzed_procedures,
					       program_size);

		int actual_cs_procedures = analyzed_procedures - actual_ci_procedures;

		double analysis_time(analysis_timer);

		cout << " -- Precision ------------------------------" << endl;
		cout << "STAT-time-analysis                 " << analysis_time << endl;
		cout << "STAT-program-size                  " << program_size << endl;
		cout << "STAT-procedures-total              " << analyzed_procedures << endl;
		cout << "STAT-procedures-unanalyzed         " << unanalyzed_procedures << endl;
		cout << "STAT-procedures-contextinsensitive " << actual_ci_procedures << endl;
		cout << "STAT-procedures-recursive          " << recursive_procedures << endl;
		cout << "STAT-procedures-contextsensitive   " << actual_cs_procedures << endl;

/*if(show_bb_count) {
extern map<procNode*,int> bb_count;
cout << "basic-block counts:\n";
for(map<procNode*,int>::iterator b=bb_count.begin(); b!=bb_count.end(); b++) {
  procedureInfo *info = analyzer->lookup_procedure(b->first);
  cout << "  " << b->first->decl()->name() << " " << b->second << " "
       << (b->second / info->analysis_count()) << endl;
}
cout << "basic-block counts done\n";
bb_count.clear();
}*/

		if (_stats) {
		  analyzer->stats(*stats_file);
		  Location::stats();
		  memoryAccess::stats();
		}

		cout << "END 0" << endl;

		delete analyzer;
	      }
	    }
	    break;

	  case PointersOnly:
	    {
	      cout << "--- Pointers only ------------------------------------------------" << endl;

	      Analyzer analyzer(anns, fixupPhase::Main, linker, fixupPhase::Unit);
	      analyzer.analyze_pointers();

	      if (_defs_uses)
		analyzer.uses_and_defs();

	      if (_stats) {
		analyzer.stats(*stats_file);
		Location::stats();
		memoryAccess::stats();

		if (_precision) {
		  precisionAnalyzer pre;
		  analyzer.analyze(&pre);
		  pre.report(*stats_file);
		}
	      }
	    }
	    break;

	  case Actions:
	    {
	      cout << "--- Actions ------------------------------------------------------" << endl;

	      Analyzer analyzer(anns, fixupPhase::Main, linker, fixupPhase::Unit);
	      // analyzer.analyze_pointers();

	      analyzer.analyze_properties();

	      if (_defs_uses)
		analyzer.uses_and_defs();

	      for (procedures_map_cp procs = a->procedures().begin();
		   procs != a->procedures().end();
		   ++procs)
		{
		  procedureAnn * libproc_anns = (*procs).second;
		  libproc_anns->test_actions(&analyzer);
		}

	      actionChanger action_changer(&analyzer, a);

	      for (unit_list_p up = CBZ::Program.begin();
		   up != CBZ::Program.end();
		   ++up)
		{
		  unitNode * u = (*up);
		  u->change(action_changer);

		  name_mangle_walker::mangle(u);

		  // TB: id_lookup_walker::fixup(u, false);

		  string temp = u->output_file() + ".ac.c";
		  ofstream outf(temp.c_str());
		  output_context b(outf);
		  u->output(b, 0);
		  outf << endl;

		  fixupPhase::fix_unit(u);
		}

	      if (_stats) {
		analyzer.stats(*stats_file);
		Location::stats();
		memoryAccess::stats();
	      }
	    }
	    break;

	  case Constants:
	    {
	      cout << "--- Constants ----------------------------------------------------" << endl;

	      Analyzer analyzer(anns, fixupPhase::Main, linker, fixupPhase::Unit);
	      // analyzer.analyze_pointers();
	      analyzer.analyze_properties();

	      if (_defs_uses)
		analyzer.uses_and_defs();

	      constantAnalyzer & cons = analyzer.get_constants();

	      for (unit_list_p up = CBZ::Program.begin();
		   up != CBZ::Program.end();
		   ++up)
		{
		  unitNode * u = (*up);

		  constantsChanger::optimize(u, &cons, true);

		  {
		    string temp = u->output_file() + ".co.c";
		    ofstream outf(temp.c_str());
		    output_context b(outf);
		    u->output(b, 0);
		    outf << endl;
		  }

		  // fixupPhase::fix_unit(u);
		  fixupPhase::semi_fix_unit(u);
		}

	    }
	    break;

	  case DeadCode:
	    {
	      cout << "--- Dead code ----------------------------------------------------" << endl;

	      Analyzer analyzer(anns, fixupPhase::Main, linker, fixupPhase::Unit);
	      // analyzer.analyze_pointers();
	      analyzer.analyze_properties();

	      bdwyLivenessAnalyzer liveness;
	      analyzer.analyze_liveness(&liveness);

	      deadcodeChanger::optimize(fixupPhase::Unit, &liveness);

	      for (unit_list_p up = CBZ::Program.begin();
		   up != CBZ::Program.end();
		   ++up)
		{
		  unitNode * u = (*up);

		  string temp = u->output_file() + ".de.c";
		  ofstream outf(temp.c_str());
		  output_context b(outf);
		  u->output(b, 0);
		  outf << endl;

		  //codeCleanup::cleanup(u);
		  // fixupPhase::fix_unit(u);
		  fixupPhase::fix_unit(u);
		  CFGSimplifier::fix(u);
		  fixupPhase::fix_unit(u);
		}
	    }
	  }
	}

      // -- Finished all passes

      if (_stats) {
	delete stats_file;
	stats_file = 0;
      }
    }
    cout << "Broadway done\n";
  }
};

Phases PalPhase("pal", new palPhase());


