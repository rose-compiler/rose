/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGTERM_H_
#define PROLOGTERM_H_
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <ctype.h>
#include <assert.h>

#ifndef __TERMITE_H__
#  error "Please do not include this file directly. Use termite.h instead."
#endif

bool init_termite(int argc, char **argv, bool interactive=false);

#if !HAVE_SWI_PROLOG

#include <iomanip>
#include <string>
#include <sstream>

/// Representation of a prolog term
class PrologTerm {
 public:
  virtual ~PrologTerm() {};
  /// returns the arity of the term
  virtual int getArity() = 0;
  /// returns whether or not the term is a ground term, i.e. contains
  /// no variables.
  /** Note that this is the case iff all the subterms are ground. */
  virtual bool isGround() = 0;
  /// Gets the name (functor/variable/constant) of the term. 
  /* numbers are represented as strings and therefore returned as such */
  virtual std::string getName() = 0;
  /// the actual prolog term that is represented by this object
  virtual std::string getRepresentation() = 0;

  /// Properly quote and escape an atom if necessary
  static std::string quote(const std::string atom) {
    std::string s = escape(atom);
    if (atom.length() == 0) return "''";
    if (((atom.length() > 0) && (!islower(atom[0])) && (!isdigit(atom[0])))
	|| needs_quotes(atom)) {
      s = "'" + s + "'";
      return s;
    } else if (is_reserved_operator(atom)) {
      s = "(" + s + ")";
      return s;
    }
    return s;
  }

  // true if the pattern can be unified with the term
  bool matches(std::string pattern) { assert(false && "not implemented"); }

protected:

  static bool is_reserved_operator(const std::string s) {
    return s == "volatile";
  }

  static bool needs_quotes(const std::string s) {
    if (s.length() == 0) 
      return true;

    bool alldigits = isdigit(s[0]);
    for (std::string::const_iterator c = s.begin();
	 c != s.end(); ++c) {

      if (alldigits) {
	if (!isdigit(*c))
	  return true;
	else continue;
      }

      if (!islower(*c) && !isupper(*c) && 
	  !(*c == '_'))
	return true;
    }
    return false;
  }

  // Escape non-printable characters
  static std::string escape(std::string s) {
    std::string r;
    for (unsigned int i = 0; i < s.length(); ++i) {
      unsigned char c = s[i];
      switch (c) {
      case '\\': r += "\\\\"; break; // Literal backslash
      case '\"': r += "\\\""; break; // Double quote
      case '\'': r += "\\'"; break;  // Single quote
      case '\n': r += "\\n"; break;  // Newline (line feed)
      case '\r': r += "\\r"; break;  // Carriage return
      case '\b': r += "\\b"; break;  // Backspace
      case '\t': r += "\\t"; break;  // Horizontal tab
      case '\f': r += "\\f"; break;  // Form feed
      case '\a': r += "\\a"; break;  // Alert (bell)
      case '\v': r += "\\v"; break;  // Vertical tab
      default:
	if (c < 32 || c > 127) {
	  std::stringstream strm;
	  strm << '\\' 
	       << std::oct 
	       << std::setfill('0') 
	       << std::setw(3) 
	       << (unsigned int)c // \nnn Character with octal value nnn
	       << '\\'; // Prolog expects this weird syntax with a
			// trailing backslash
	  r += strm.str();
	} else {
	  r += c;
	}
      }
    }
    //cerr<<"escape("<<s<<") = "<< r <<endl;
    return r;
  }


};

#else

# include <SWI-Prolog.h>
# ifdef NORETURN
#   undef NORETURN // incompatability with the PAG headers
# endif
# ifdef TRUE
#   undef TRUE // incompatability with the ROSE headers
# endif
# ifdef FALSE
#   undef FALSE // incompatability with the ROSE headers
# endif

# include <deque>
# include <sstream>


/// Representation of a prolog term
class PrologTerm {
 public:
  PrologTerm() :term(PL_new_term_ref()) {
#   if DEBUG_TERMITE
      std::cerr<<"UNINIT"<<std::endl;
#   endif
  }
  PrologTerm(term_t t) : term(PL_copy_term_ref(t)) { 
#   if DEBUG_TERMITE
      std::cerr<<"new PrologTerm("<<display(t)<<")"<<std::endl;
#   endif
  }

  virtual ~PrologTerm() {
    // Free all children that were generated on-the-fly
    // TODO: replace this with some sorrt of smart pointer
    for (std::deque<PrologTerm*>::iterator p = garbage_bin.begin(); 
	 p != garbage_bin.end(); ++p)
      delete *p;
  }
  /// returns the arity of the term
  virtual int getArity() { 
    int arity = 0;
    if (PL_term_type(term) == PL_TERM) {
      term_t name;
      PL_get_name_arity(term, &name, &arity);
    }
    return arity;
  }
  /// returns whether or not the term is a ground term, i.e. contains
  /// no variables.
  virtual bool isGround() { return PL_is_ground(term); }

  /// Gets the name (functor/variable/constant) of the term. 
  /* numbers are represented as strings and therefore returned as such */
  virtual std::string getName() {
    char *s;
    if (PL_get_atom_chars(term, &s))
      return std::string(s);

    int arity;
    atom_t name;
    PL_get_name_arity(term, &name, &arity);
    return std::string(PL_atom_chars(name));
  }
  /// the actual prolog term that is represented by this object
  virtual std::string getRepresentation() { return display(term); }

  /// Properly quote an atom if necessary
  static std::string quote(const char* s) {
    return std::string(PL_quote('\'', s));
  }

  // true if the pattern can be unified with the term
  bool matches(std::string pattern);

  /// return the SWI-Prolog term
  term_t getTerm() { return term; }

  // Create a new PrologTerm from a real Prolog Atom
  static PrologTerm *wrap_PL_Term(term_t t);

protected:

  term_t term; // The "real" prolog term

  static std::string display(term_t t)
  { 
    // this should be more efficient than 
    // chars_to_term("with_output_to(string(S),write_term(T,[quoted(true)]))",X)
    fid_t fid = PL_open_foreign_frame();
    term_t T		  = PL_copy_term_ref(t);
    term_t a0 = PL_new_term_refs(7);
    term_t S		  = a0 + 0;
    term_t True		  = a0 + 1;
    term_t quoted	  = a0 + 2;
    term_t list           = a0 + 3;
    term_t write_term	  = a0 + 4;
    term_t string	  = a0 + 5;
    term_t with_output_to = a0 + 6;

    PL_put_variable(S);
    PL_cons_functor(string, PL_new_functor(PL_new_atom("string"), 1), S);

    PL_put_atom_chars(True, "true");
    PL_cons_functor(quoted, PL_new_functor(PL_new_atom("quoted"), 1), True);

    PL_put_nil(list);
    PL_cons_list(list, quoted, list);
    
    PL_cons_functor(write_term, 
		    PL_new_functor(PL_new_atom("write_term"), 2), 
		    T, list);
    PL_cons_functor(with_output_to, 
		    PL_new_functor(PL_new_atom("with_output_to"), 2), 
		    string, write_term);
    assert(PL_call(with_output_to, NULL));
    
    char *s;
    size_t len;
    assert(PL_get_string_chars(S, &s, &len));

    std::string r = std::string(s);
    PL_discard_foreign_frame(fid);
    return r;
  }

  std::deque<PrologTerm*> garbage_bin;

  // Create a new PrologTerm from a real Prolog Atom
  // it will automatically be freed at the end of this object's lifetime
  // calls wrap_PL_Term internally
  PrologTerm *newPrologTerm(term_t t);

  // Create a real Prolog term from a PrologTerm
  // it will be garbage-collected by SWI-Prolog
  term_t newTerm_t(PrologTerm* pt);
};

#endif // SWI

#endif
