
#ifndef BDWY_STRUCTURE_H
#define BDWY_STRUCTURE_H

// ----------------------------------------------------------------------
//   Structure Annotation
// ----------------------------------------------------------------------

class structureTreeAnn;
class structureAnn;

typedef std::list<structureAnn * > structure_list;
typedef structure_list::iterator structure_list_p;
typedef structure_list::const_iterator structure_list_cp;

typedef std::list< structureTreeAnn * > structuretree_list;
typedef structuretree_list::iterator structuretree_list_p;

/** @brief Structure Tree annotation
 *
 *  This class is used to construct an in-memory representation of on-entry
 *  and on-exit annotations. We post-process this tree structure into the
 *  list of pointer operations that is actually used during analysis.
 *
 *  The representation works like this: for A --> B, we create an object
 *  for A, with a single target for B; for B { ... }, we create an object
 *  for B with a list of the components. The operator indicates which
 *  situation we have (although we can never have an arrow with multiple
 *  targets. */

class structureTreeAnn : public Ann
{
public:

  typedef enum { None, Arrow, Dot } Operator;

private:

  std::string _name;
  structuretree_list * _targets;
  Operator _operator;
  bool _target_is_new;
  bool _is_io;

public:

  /** Multiple targets constructor */

  structureTreeAnn( const parserID * name,
		    structuretree_list * targets,
		    Operator op,
		    bool target_is_new);

  /** Single target constructor */

  structureTreeAnn( const parserID * name,
		    structureTreeAnn * target,
		    Operator op,
		    bool target_is_new);

  /** Destructor
   *
   * Recursively destroys the data structure */

  ~structureTreeAnn();

  inline std::string & name() { return _name; }
  inline structuretree_list * targets() { return _targets; }
  inline Operator op() const { return _operator; }
  inline bool is_target_new() const { return _target_is_new; }
  inline bool is_io() const { return _is_io; }

  /** @brief Print out
   */

  void print(int depth = 0) const;
};


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

class structureAnn : public Ann
{
private:

  REF annVariable * _source;
  REF annVariable * _target;
  std::string _field_name;

public:

  structureAnn(annVariable * source, annVariable * target,
               const std::string * field_name,
	       const int line);

  // --- Fields

  inline annVariable * source() const { return _source; }
  inline annVariable * target() const { return _target; }
  inline const std::string & field_name() const { return _field_name; }

  // --- Output

  friend std::ostream& operator<<(std::ostream & o, const structureAnn & sa) {
    sa.print(o);
    return o;
  }

  void print(std::ostream & o) const;
};

#endif /*  */
