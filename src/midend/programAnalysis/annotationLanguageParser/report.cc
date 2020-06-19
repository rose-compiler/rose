
#include "broadway.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

/*
callNode * reportElementAnn::callnode(stmtNode * stmt)
{
  callNode * call = 0;

  // -- Disect it

  if (stmt->typ() == Expr) {
    exprstmtNode * es = (exprstmtNode *) stmt;
    exprNode * e = es->expr();
    if (e->typ() == Call)
      call = (callNode *)e;
    if (e->typ() == Binary) {
      binaryNode * b = (binaryNode *) e;
      if (b->right()->typ() == Call)
        call = (callNode *) b->right();
    }
  }

  return call;
}
*/

// ----------------------------------------------------------------------
//  Literal report element
// ----------------------------------------------------------------------

literalReportElementAnn::literalReportElementAnn(const parserID * literal)
  : reportElementAnn(literal->line()),
    _literal(literal->name())
{}

void literalReportElementAnn::lookup(enumPropertyAnn * default_property,
                                     procedureAnn * procedure,
                                     Annotations * annotations)
{}

#ifdef __PROCLOCATION

void literalReportElementAnn::report(ostream & out,
                                     bool is_error,
                                     Analyzer * analyzer,
                                     procLocation * where)
{
  if (! Annotations::Quiet_reports || is_error)
    out << _literal;
}

#endif

// ----------------------------------------------------------------------
//  Expression report element
// ----------------------------------------------------------------------

expressionReportElementAnn::expressionReportElementAnn(Broadway::FlowSensitivity flow_sensitivity,
                                                       exprAnn * expression, int line)
  : reportElementAnn(line),
    _flow_sensitivity(flow_sensitivity),
    _expr(expression)
{}

void expressionReportElementAnn::lookup(enumPropertyAnn * default_property,
                                        procedureAnn * procedure,
                                        Annotations * annotations)
{
  _expr->lookup(default_property, procedure, annotations);
}

#ifdef __PROCLOCATION

void expressionReportElementAnn::report(ostream & out,
                                        bool is_error,
                                        Analyzer * analyzer,
                                        procLocation * where)
{
  _expr->report(out, is_error, where);
}

#endif

// ----------------------------------------------------------------------
//  Location report element
// ----------------------------------------------------------------------

#ifdef __PATHCOUNTER

class pathCounter
{
public:

  static int count_paths(basicblockNode * block)
  {
    basicblock_set visited;

    return count_paths(block, visited);
  }

  static int count_paths(basicblockNode * bb,
                         basicblock_set & visited)
  {
    int total = 1;

    if (visited.find(bb) != visited.end())
      return 0;

    visited.insert(bb);

    for (basicblock_list_p p = bb->preds().begin();
         p != bb->preds().end();
         ++p)
      total += count_paths(*p, visited);

    return total;
  }
};

#endif

locationReportElementAnn::locationReportElementAnn(const parserID * id)
  : reportElementAnn(id->line()),
    _kind_name(id->name()),
    _kind(Callsite)
{
}

void locationReportElementAnn::lookup(enumPropertyAnn * default_property,
                                      procedureAnn * procedure,
                                      Annotations * annotations)
{
  if (_kind_name == "callsite") _kind = Callsite;
  else
    if (_kind_name == "context") _kind = Context;
    else
      if (_kind_name == "numcontexts") _kind = Num_Contexts;
      else
        annotations->Error(line(), string("Unknown $ location report element: \"") +
                           _kind_name + "\"");
}

#ifdef __PROCLOCATION

void locationReportElementAnn::report(ostream & out,
                                      bool is_error,
                                      Analyzer * analyzer,
                                      procLocation * where)
{
  if (Annotations::Quiet_reports && ! is_error)
    return;

  if (_kind == Callsite) {
    // callNode * call = findCallNodeWalker::find(where->stmt_location()->stmt());
    out << where->stmt_location()->stmt()->coord();
  }

  if (_kind == Context) {

    // -- Location::print_path now includes source location

    cout << * where;

    /*
    procLocation * cur = where;

    while (cur->stmt_location()) {

      callNode * call = findCallNodeWalker::find(callnode(cur->stmt_location()->stmt());

      procedureInfo * info = analyzer->lookup_procedure(cur->proc());

      out << info->qualified_name();
      if (call)
        out << " @ (" << call->coord() << ")";

      cur = cur->stmt_location()->block_location()->proc_location();

      if (cur->stmt_location())
        out << " in ";
    }

    if (cur) {

      // -- Get the top-most procedure

      procedureInfo * info = analyzer->lookup_procedure(cur->proc());
      out << " in " << info->qualified_name();
    }

    // cout << "Number of paths to this location: " << total << endl;
    */
  }

  if (_kind == Num_Contexts) {

    // -- Count the number of ways to reach this location. First, we move
    // up the call chain to skip over any context-sensitive procedures.

    procLocation * cur = where;

    while (cur->stmt_location())
      cur = cur->stmt_location()->block_location()->proc_location();

    // -- Look up the procedure

    procedureInfo * info = analyzer->lookup_procedure(cur->proc());

    // -- Now get the context count

    int count = info->count_calling_contexts();

    out << count;
  }
}

#endif

// ----------------------------------------------------------------------
//  Binding report element
// ----------------------------------------------------------------------

bindingReportElementAnn::bindingReportElementAnn(const parserID * varname,
                                                 bool size_only)
  : reportElementAnn(varname->line()),
    _variable_name(varname->name()),
    _size_only(size_only),
    _variable(0)
{}

void bindingReportElementAnn::lookup(enumPropertyAnn * default_property,
                                     procedureAnn * procedure,
                                     Annotations * annotations)
{
  _variable = procedure->lookup(_variable_name, false);
  if ( ! _variable )
    annotations->Error(line(),
                       string("Undeclared identifier \"") + _variable_name + "\"");
}

#ifdef __PROCLOCATION

void bindingReportElementAnn::report(ostream & out,
                                     bool is_error,
                                     Analyzer * analyzer,
                                     procLocation * where)
{
  if (Annotations::Quiet_reports && ! is_error)
    return;

  // -- Get the binding

  pointerValue & var_binding = _variable->get_binding(where);

  if (_size_only) {

    // -- Just print the number of objects in the binding

    out << var_binding.blocks.size();
  }
  else {

    // -- Print out the names of the memoryBlocks (these could be weird
    // generated names).

    bool first = true;
    for (memoryblock_set_p p = var_binding.blocks.begin();
         p != var_binding.blocks.end();
         ++p)
      {
        memoryBlock * block = (*p);

        if ( ! first ) out << ", ";
        out << block->name();
        first = false;

        /*
        cout << "BLOCK: " << block->name() << endl;

        const memorydef_list defs = block->defs();
        for (memorydef_list_cp mp = defs.begin();
             mp != defs.end();
             ++mp)
          {
            memorydef_key mlp = (*mp);
            Location * where = mlp.location;
            memoryDef * def = mlp.def;

            cout << "  + Def at " << * where << " : ";

            const memoryblock_set & points_to = def->points_to();
            for (memoryblock_set_cp q = points_to.begin();
                 q != points_to.end();
                 ++q)
              {
                cout << (*q)->name() << " ";
              }

            cout << endl;
          }
        */
      }
  }
}

#endif

// ----------------------------------------------------------------------
//  Report annotation
// ----------------------------------------------------------------------

reportAnn::reportAnn(exprAnn * condition,
                     bool is_error,
                     report_element_list * elements, int line)
  : Ann(line),
    _condition(condition),
    _elements(),
    _is_error(is_error)
{
  if (elements)
    _elements.swap( * elements );
}

void reportAnn::lookup(procedureAnn * procedure,
                       Annotations * annotations)
{
  // -- Invoke lookup on the condition, if there is one

  if (_condition)
    _condition->lookup((enumPropertyAnn *)0, procedure, annotations);

  // -- Visit each report element and call lookup

  for (report_element_list_p p = _elements.begin();
       p != _elements.end();
       ++p)
    (*p)->lookup((enumPropertyAnn *)0, procedure, annotations);
}

#ifdef __PROCLOCATION

void reportAnn::report(ostream & out,
                       Analyzer * analyzer,
                       procLocation * where,
                       propertyAnalyzer * property_analyzer)
{
  // -- First, check the condition

  bool old_verbose_val = Annotations::Verbose_properties;
  if (Annotations::Show_reports) {
    Annotations::Verbose_properties = true;
    cout << " -- Report at " << * (where) << " ------" << endl;
  }

  bool trigger = true;
  if (_condition) {

    if (Annotations::Show_reports) {
      cout << "  + Test condition: ";
      _condition->print(cout);
      cout << endl;
    }

    trigger = _condition->test(where, property_analyzer);
  }

  if (Annotations::Show_reports) {
    if (trigger)
      cout << " => true" << endl << endl;
    else
      cout << " => false" << endl << endl;
  }

  // -- If true, print out the report elements

  if (trigger) {
    for (report_element_list_p p = _elements.begin();
         p != _elements.end();
         ++p)
      (*p)->report(out, _is_error, analyzer, where);

    if (_is_error) {
      stmtLocation * stmt_loc = where->stmt_location();
      stmtNode * stmt = stmt_loc->stmt();

      error_pair key(this, stmt);

      error_report_map_p p = Annotations::Error_reports.find(key);
      if (p == Annotations::Error_reports.end())
        Annotations::Error_reports[key] = 1;
      else
        ((*p).second)++;
// begin TB new
      // perform error diagnostic
      if(_condition) _condition->diagnostic(out, where, property_analyzer);
// end TB new
    }

  }

  Annotations::Verbose_properties = old_verbose_val;
}

#endif
