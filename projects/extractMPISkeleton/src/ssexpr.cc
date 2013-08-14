//
// really simple s-expression parser
//
// supports basic atoms and double quoted atoms, with simple
// character escaping.  parsing considered failed if an incomplete
// expression is passed to the parser.  only one expression per string
// allowed.
//
// matt@galois.com /// dec. 2011
//
#include <string>
#include <iostream>
#include <fstream>
#include "ssexpr.h"

using namespace std;

SExpr *SExpr::parse_inner(SExprParserState *st) {
  string curatom = "";
  SExpr *s;
  bool quoted = false;

  for (; st->it != st->str.end(); st->it++) {
    char c = (char)*(st->it);

    if (quoted) {
      // quoted
      switch(c) {
      case '"':
	quoted = false;
	st->it++;
	s = new SExpr(curatom);
	s->setNext(parse_inner(st));
	return s;
	break;
      default:
	curatom += c;
	break;
      }
      // quoted
    } else {
      // !quoted

      switch (c) {
      case ';':
	// comment, so spin to end of line
	while (st->it != st->str.end() && 
	       c != '\n' && 
	       c != '\r') {
	  st->it++;
	  c = (char)*(st->it);
	}

	break;	  
      case '"':
	quoted = true;
	break;
      case '(':
	st->it++;
	s = new SExpr(SX_LIST);
	s->setList(parse_inner(st));
	s->setNext(parse_inner(st));
	return s;
	break;
      case ')':
	st->it++;
	if (curatom != "") {
	  return (new SExpr(curatom));
	} else {
	  return NULL;
	}
	break;
      case ' ':
      case '\n':
      case '\r':
	if (curatom != "") {
	  s = new SExpr(curatom);
	  s->setNext(parse_inner(st));
	  return s;
	}
	
	break;
      default:
	curatom += c;
	break;
      }
      // !quoted
    }

  }

  if (quoted == true) {
    cerr << "Malformed s-expression.  Quote not closed." << endl;
    return NULL;
  }

  if (curatom != "") {
    s = new SExpr(curatom);
    s->setNext(parse_inner(st));
    return s;
  }

  return NULL;
}

// todo: refactor this to parse off the ifstream directly instead
//       of reading it into a string and then parsing off that.
SExpr *SExpr::parse_file(const string & fname) {
  ifstream file;

  file.open(fname.c_str(), ifstream::in);

  string accum = "";
  string line;

  while (file.good()) {
    getline(file,line);
    accum += line + "\n";
  }

  file.close();

  SExpr *sx = parse(accum);

  return sx;
}

SExpr *SExpr::parse(const string & s) {
  SExprParserState *st = new SExprParserState(s);
  SExpr *sx = parse_inner(st);
  delete(st);

  return sx;
}

std::ostream& operator<<(std::ostream & os, const SExpr & s){
  if(s.getType() == SExpr::SX_LIST){
    if (s.getList() == NULL) {
      os << "()";
    } else {
      os << "(" << *s.getList() << ")";
    }
  } else {
    // We don't have to use quotes here, but this way
    // there won't be any ambiguity when parsing
    // this output later.
    os << "\"" << s.getValue() << "\"";
  }
  const SExpr * const next = s.getNext();
  if( next != NULL ){
    os << " " << *next;
  }
  return os;
}
