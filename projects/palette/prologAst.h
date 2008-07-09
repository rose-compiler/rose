#ifndef PROLOGAST_H
#define PROLOGAST_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include "string_functions.h"
#include "escape.h"

namespace PrologAst {

  class Node {
    public:
    virtual void unparse(std::ostream& o) const = 0;
    virtual ~Node() {}
  };

  class Term: public Node {
    std::string functor;
    std::vector<Node*> args;

    public:
    Term(const std::string& functor, const std::vector<Node*>& args)
      : functor(functor), args(args) {}

    void unparse(std::ostream& o) const {
      o << functor << "/" << args.size();
      if (args.size() != 0) {
        o << "(";
        for (unsigned int i = 0; i < args.size(); ++i) {
          if (i != 0) o << ", ";
          args[i]->unparse(o);
        }
        o << ")";
      }
    }

    std::string getFunctor() const {return functor;}
    unsigned int getArity() const {return args.size();}
    const std::vector<Node*>& getArgs() const {return args;}
  };

  class Variable: public Node {
    std::string name;

    public:
    Variable(const std::string& name): name(name) {}
    void unparse(std::ostream& o) const {
      o << name;
    }

    std::string getName() const {return name;}
  };

  class Wildcard: public Node {
    public:
    Wildcard() {}
    void unparse(std::ostream& o) const {
      o << "_";
    }
  };

  class Integer: public Node {
    int value;
    public:
    Integer(int value): value(value) {}
    void unparse(std::ostream& o) const {
      o << value;
    }

    int getValue() const {return value;}
  };

  class FloatingPoint: public Node {
    double value;
    public:
    FloatingPoint(double value): value(value) {}
    void unparse(std::ostream& o) const {
      o << std::showpoint << value;
    }

    double getValue() const {return value;}
  };

  class String: public Node {
    std::string value;
    public:
    String(const std::string& value): value(value) {}
    void unparse(std::ostream& o) const {
      o << '"' << escapeString(value) << '"';
    }

    std::string getValue() const {return value;}
  };

  inline std::ostream& operator<<(std::ostream& os, Node* n) {
    n->unparse(os);
    return os;
  }

}

#endif // PROLOGAST_H
