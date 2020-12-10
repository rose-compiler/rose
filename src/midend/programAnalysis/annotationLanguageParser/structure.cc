
#include "broadway.h"
#include <assert.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

// Multiple targets constructor

structureTreeAnn::structureTreeAnn( const parserID * id,
                                    structuretree_list * targets,
                                    Operator op,
                                    bool target_is_new)
  : Ann(id->line()),
    _name(id->name()),
    _targets(targets),
    _operator(op),
    _target_is_new(target_is_new),
    _is_io(id->is_io())
{}

// Single target constructor

structureTreeAnn::structureTreeAnn( const parserID * id,
                                    structureTreeAnn * target,
                                    Operator op,
                                    bool target_is_new)
  : Ann(id->line()),
    _name(id->name()),
    _targets(new structuretree_list()),
    _operator(op),
    _target_is_new(target_is_new),
    _is_io(id->is_io())
{
  _targets->push_back(target);
}

// Destructor: deletes the whole tree recursively

structureTreeAnn::~structureTreeAnn()
{
  structuretree_list_p p;

  if (_targets) {
    for (p = _targets->begin();
         p != _targets->end();
         ++p)
      delete (*p);
  }

  delete _targets;
}

structureTreeAnn::structureTreeAnn(const structureTreeAnn & X)
   : Ann(X)
   {
  // DQ (9/13/2011): This copy constructor was built because static analysis tools
  // suggested it would avoid a possible double free error.  I agree.
     printf ("Error: it is an error to call this copy constructor. \n");
     assert(false);
   }

void structureTreeAnn::print(int depth) const
{
  for (int i = 0; i < depth; i++)
    cout << " ";

  cout << _name << " ";
  if (_operator == Arrow) cout << "-->";
  if (_operator == Dot) cout << ".";

  if (_target_is_new) cout << " new ";

  if (_targets) {

    int size = _targets->size();

    if (size > 1)
      cout << "{" << endl;

    for (structuretree_list_p p = _targets->begin();
         p != _targets->end();
         ++p)
      {
        structureTreeAnn * child = *p;
        child->print(depth+1);
        if (size > 1)
          cout << endl;
      }

    if (size > 1) {
      for (int i = 0; i < depth; i++)
        cout << " ";
      cout << "}" << endl;
    }
  }

  if (depth == 0)
    cout << endl;
}

/** @brief Pointer structure annotation
 *
 *  This class represents a single structure annotation (either on_entry or
 *  on_exit). It handles both the "dot" operator and the "arrow" operator,
 *  depending on whether the field_name is used. The actual annotation
 *  syntax is decomposed into a list of these objects. For example:
 *
 *  on_entry {  A --> B { width,
 *                        height,
 *                        data --> data1,
 *                        more { stuff,
 *                               things } } }
 *
 *  This introduces the following series of structure annotations:
 *
 *  Source    Operator   Target
 *    A         -->        B
 *    B       .width     B.width
 *    B       .height    B.height
 *    B        .data     B.data
 *  B.data      -->       data1
 *    B        .more     B.more
 *  B.more    .stuff     B.more.stuff
 *  B.more    .things    B.more.things
 *
 *  This naming scheme is convenient because we can bind the names to
 *  actual memory blocks during the processing of the on_entry annotations,
 *  and then we never have to explicity process the "dot" operator because
 *  it's built into the name.  */

structureAnn::structureAnn(annVariable * source, annVariable * target,
                           const string * field_name, int line)
  : Ann(line),
    _source(source),
    _target(target),
    _field_name()
{
  if (field_name)
    _field_name = *field_name;
}

// --- Output

void structureAnn::print(ostream & o) const
{
  o << "\"" << _source->name() << "\"";

  if (! _field_name.empty())
    o << " ." << _field_name << " ";
  else
    o << " --> ";

  o << "\"" << _target->name() << "\"";
}


