
#include "broadway.h"

using namespace std;

callingContext::callingContext(procLocation * where,
			       operandNode * call,
			       propertyAnalyzer * property_analyzer,
			       procedureAnn * procedure_annotations,
			       procedureInfo * caller,
			       Annotations * annotations)
  : _where(where),
    _call(call),
    _property_analyzer(property_analyzer),
    _procedure_annotations(procedure_annotations),
    _caller(caller),
    _annotations(annotations)
{}

stmtLocation * callingContext::def_use_location()
{
  basicblockNode * body = _procedure_annotations->proc()->entry();
  basicblockLocation * block = _where->lookup_block(body);
  stmtLocation * stmt = & block->statements()[2];

  return stmt;
}

stmtLocation * callingContext::dealloc_location()
{
  basicblockNode * body = _procedure_annotations->proc()->entry();
  basicblockLocation * block = _where->lookup_block(body);
  stmtLocation * stmt = & block->statements()[0];

  return stmt;
}

void callingContext::set_current_def_and_use(pointerValue & ptr)
{
  Location * loc = def_use_location();

  for (memoryblock_set_p p = ptr.blocks.begin();
       p != ptr.blocks.end();
       ++p)
    (*p)->set_current_def_use(loc);
}
