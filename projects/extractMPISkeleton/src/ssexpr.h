//
// simple s-expression parser and object representation
//
// matt@galois.com -- dec. 2011
//

#ifndef __ssexpr_cc__
#define __ssexpr_cc__

#include <string>
#include <stdlib.h>
#include <iostream>

class SExpr {
public:
  typedef enum {
    SX_ATOM, SX_LIST
  } sexpr_elt_t;

  // empty s-expression is by default a list
  SExpr() : atomval("") {
    ty = SX_LIST;
    list = NULL;
    next = NULL;
  }

  // empty s-expression with explicit type
  SExpr(sexpr_elt_t t) : atomval("") {
    ty = t;
    list = NULL;
    next = NULL;
  }

  // constructor with string yields atom
  SExpr(std::string s) : atomval(s) {
    ty = SX_ATOM;
    list = NULL;
    next = NULL;
  }

  // constructor with s-expression yields list
  SExpr(SExpr *sx) : atomval("") {
    ty = SX_LIST;
    list = sx;
    next = NULL;
  }

  // destructor
  ~SExpr() {
    if (list != NULL)
      delete list;
    if (next != NULL)
      delete next;
  }

  // append to the end of the next chain for the current SExpr
  void append(SExpr *sx) {
    if (next == NULL) {
      next = sx;
    } else {
      next->append(sx);
    }
  }

  // set the value.  this by default turns the expression into an atom
  // NOTE: warn if this occurs if it was already a list with a list value
  // set.
  void setValue(const std::string & s) {
    if (ty == SX_LIST && list != NULL) {
      warn("Setting element to atom value when already set to list.");
    }
    ty = SX_ATOM;
    atomval = s;
  }

  // set the next pointer.  warn if this was already set.
  void setNext(SExpr *sx) {
    if (next != NULL) {
      warn("Setting element next pointer when already set to a value.");
    }
    next = sx;
  }

  // set the list pointer, and turn SExpr into a list type.  warn if this
  // was an atom already, or if the list pointer aimed elsewhere to begin
  // with
  void setList(SExpr *sx) {
    if (ty == SX_ATOM && atomval == "") {
      warn("Setting element to list value when already set to atom.");
    }
    ty = SX_LIST;
    list = sx;
  }

  std::string getValue() const {
    return atomval;
  }

  // get the list pointer
  SExpr *getList() const {
    return list;
  }

  // get the next pointer
  SExpr *getNext() const {
    return next;
  }

  // get the SExpr type
  sexpr_elt_t getType() const {
    return ty;
  }

  // pretty print the s-expression to a string
  std::string toString() const {
    std::string s = "";
    if (ty == SX_LIST) {
      if (list == NULL) {
        s = "EMPTYLIST{} ";
      } else {
        s = "LIST{"+list->toString()+"} ";
      }
    } else {
      s = "ATOM{"+atomval+"} ";
    }
    if (next != NULL) {
      s += next->toString();
    }
    return s;
  }

  static SExpr *parse(const std::string & s);

  static SExpr *parse_file(const std::string & fname);

private:

  // SExpr type
  sexpr_elt_t ty;

  // list and next pointers
  SExpr *list, *next;

  // atom value
  std::string atomval;

  class SExprParserState {
  public:
    std::string str;
    std::string::iterator it;

    SExprParserState(std::string s) : str(s) {
      it = str.begin();
    }
  };

  static SExpr *parse_inner(SExprParserState *st);

  void warn(std::string s) {
    std::cerr << "WARNING: " << s << std::endl;
  }

  void abort(std::string s) {
    std::cerr << "FATAL: " << s << std::endl;
    exit(1);
  }
};

// Formats the SExpr so that it can be parsed again.
// We use an ostream so that we can send it to any one of:
//   * cout/cerr
//   * fstream
//   * stringstream
std::ostream& operator<<(std::ostream & os, const SExpr & s);

#endif // __ssexpr_c__
