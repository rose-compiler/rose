
#include "broadway.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

annVariable::annVariable(string name,
                         string procedure_name,
                         bool is_new,
                         bool is_global,
                         bool is_external,
                         bool is_formal,
                         bool is_io)
    : _name(name),
#ifdef __BINDINGS
      _bindings(),
#endif
#ifdef __DECL
      _decl(0),
#endif
      _is_new(is_new),
      _is_used(false),
      _is_defined(false),
      _is_global(is_global),
      _is_external(is_external),
      _is_formal(is_formal),
      _is_io(is_io),
      _is_deleted(false),
      _is_property_variable(false)
{
  // -- For external variables, we'll get the declNode from the actual
  // header files. For all other variables, create a synthetic declNode.

  if ( ! is_external) {

    // -- The name of the declNode will include the name of the procedure:

    string variable_name = procedure_name + "::" + name;
#ifdef __DECL
    _decl = new declNode(variable_name.c_str(),
                         declNode::NONE,
                         (typeNode *)0,
                         (exprNode *)0,
                         (exprNode *)0);

    // -- Set the decl location to BLOCK. This is really important,
    // otherwise memoryModel::lookup_variable won't treat annotation
    // variables as local variables.

    _decl->decl_location(declNode::BLOCK);
#endif
  }
}

annVariable::~annVariable()
{
#ifdef __DECL
  delete _decl;
#endif
}

#ifdef __BINDINGS
void annVariable::clear(procLocation *where)
{
  _bindings[where].blocks.clear();
}

pointerValue & annVariable::get_binding(procLocation *where) {
  if(_bindings.find(where) == _bindings.end() && is_global())
    return _bindings[NULL];
  return _bindings[where];
}

// --- Manage the bindings
/*
pointerValue & annVariable::binding(procLocation * loc)
{
  if ((loc == 0) &&
      ! _is_global)
    cerr << "annVariable: ERROR: Looking for global binding of a local variable!" << endl;

  if (_is_global)
    return _bindings[(procLocation *)0];
  else
    return _bindings[loc];
}

void annVariable::create_binding(memoryBlock * mb,
                                 procLocation * loc)
{
  if ((loc == 0) &&
      ! _is_global)
    cerr << "annVariable: ERROR: Looking for global binding of a local variable!" << endl;

  if (_is_global) {
    pointerValue & glob = _bindings[(procLocation *)0];
    glob.blocks.clear();
    glob.blocks.insert(mb);
  }
  else {
    pointerValue & local = _bindings[loc];
    local.blocks.clear();
    local.blocks.insert(mb);
  }
}

// --- Check to see if a binding exists

bool annVariable::has_binding(procLocation * loc)
{
  procLocation * look_for;

  if ((loc == 0) &&
      ! _is_global) {
    cerr << "annVariable: ERROR: Looking for global binding of a local variable!" << endl;
    return false;
  }

  if (_is_global)
    look_for = 0;
  else
    look_for = loc;

  variable_binding_map_p p = _bindings.find(look_for);

  return p != _bindings.end();
}
*/
#endif

void annVariable::print(ostream & o) const
{
  bool need_comma = false;

  o << _name << "(";

  if (_is_new) {
    if (need_comma) o << ", ";
    o << "new";
    need_comma = true;
  }

  if (_is_used) {
    if (need_comma) o << ", ";
    o << "used";
    need_comma = true;
  }

  if (_is_defined) {
    if (need_comma) o << ", ";
    o << "defd";
    need_comma = true;
  }

  if (_is_global) {
    if (need_comma) o << ", ";
    o << "global";
    need_comma = true;
  }

  o << ")";
}
