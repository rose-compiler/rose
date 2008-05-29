
#ifndef BDWY_ANN_H
#define BDWY_ANN_H

#ifdef __LOCALSTL_H
#include "localstl.h"
#endif

/** @brief Annotation base class
 *
 * Provides a base class for all other annotations. All it does is hold the
 * Coord information for error messages.
 */

class Ann
{
private:

  int _line;

public:

  Ann(int line)
    : _line(line)
  {}

  inline int line() const { return _line; }
};

/** @brief Parser identifier class
 *
 *  This class holds identifiers during parsing. In addition to the actual
 *  name of the identifier, it also holds the line number on which the
 *  identifier occured. */

class parserID
{
private:

  std::string _name;
  int    _line;
  bool   _is_io;

public:

  parserID(const char * val, int line)
    : _name(val),
      _line(line),
      _is_io(false)
  {}

  inline std::string & name() { return _name; }
  inline const std::string & name() const { return _name; }
  inline int line() const { return _line; }

  inline bool is_io() const { return _is_io; }
  inline void set_io() { _is_io = true; }

  inline void append(const char * str) {
    name() += str;
  }

  inline void append(parserID * other) {
    name() += other->name();
    delete other;
  }
};

typedef std::list< parserID > parserid_list;
typedef parserid_list::iterator parserid_list_p;


#endif /*  */
