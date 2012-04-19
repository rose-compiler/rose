
#ifndef __PARSE_TREE_HPP__
#  define __PARSE_TREE_HPP__

#include "rose-kernel/PolyhedralKernel.hpp"

#include <iostream>
#include <string>

struct isl_map;

class SgExpression;

class PolyGraph;

class ParseTree {
  protected:
    PolyGraph * polygraph;

  public:
    ParseTree(PolyGraph * polygraph_);
    virtual ~ParseTree();

    virtual void print(std::ostream & out) const = 0;
    std::string toString() const;
    virtual void toDot(std::ostream & out) const = 0;

  static ParseTree * extractParseTree(SgExpression * exp, const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program, PolyGraph * polygraph, std::string isl_domain, unsigned stmt_id);
};

class Operator : public ParseTree {
  private:
    unsigned id;
    ParseTree * child1;
    ParseTree * child2;
    ParseTree * child3;

  public:
    Operator(PolyGraph * polygraph_, unsigned id_, ParseTree * c1, ParseTree * c2 = NULL, ParseTree * c3 = NULL);
    virtual ~Operator();

    virtual void print(std::ostream & out) const;
    virtual void toDot(std::ostream & out) const;
};

class Data : public ParseTree {
  private:
    unsigned id;
    isl_map * access;

  public:
    Data(PolyGraph * polygraph_, SgExpression * exp, const PolyhedricAnnotation::FunctionPolyhedralProgram & function_program, std::string isl_domain);
    virtual ~Data();

    unsigned getID() const;

    virtual void print(std::ostream & out) const;
    virtual void toDot(std::ostream & out) const;
};

#endif /* __PARSE_TREE_HPP__ */

