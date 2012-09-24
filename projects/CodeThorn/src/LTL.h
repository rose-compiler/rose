// -*- C++ -*-
#ifndef __LTL_H__
#define __LTL_H__

#include <boost/shared_ptr.hpp>

namespace LTL { class Formula; }

// global variables from Bison
extern FILE* ltl_input;
extern int ltl_parse();
extern LTL::Formula* ltl_val;
extern bool ltl_eof;
extern short ltl_label;

/// Linear Temporal Logic (LTL) Abstract syntax tree (AST)
///
/// \authors <pre>
///
/// Copyright (c) 2012 Lawrence Livermore National Security, LLC.
/// Produced at the Lawrence Livermore National Laboratory
/// Written by Adrian Prantl <adrian@llnl.gov>.
/// 
/// UCRL-CODE-155962.
/// All rights reserved.
/// 
/// This file is part of ROSE. For details, see http://www.rosecompiler.org/.
/// Please read the COPYRIGHT file for Our Notice and for the BSD License.
/// 
/// </pre>
///
/// \date 2012
/// \author Adrian Prantl
namespace LTL {
  
  /// Inherited Attribute for visitor pattern
  class InheritedAttribute {};
  typedef boost::shared_ptr<InheritedAttribute> IAttr;
  class Expr;
  class InputSymbol;
  class OutputSymbol;
  class Not;
  class Next;
  class Eventually;
  class Globally;
  class And;
  class Or;
  class Until;
  class WeakUntil;
  class Release;

  /**
   * Abstract visitor pattern Visitor base class for LTL expressions.
   */
  class BottomUpVisitor {
  public:
    virtual void visit(const InputSymbol* e) {}
    virtual void visit(const OutputSymbol* e) {}
    virtual void visit(const Not* e) {}
    virtual void visit(const Next* e) {}
    virtual void visit(const Eventually* e) {}
    virtual void visit(const Globally* e) {}
    virtual void visit(const And* e) {}
    virtual void visit(const Or* e) {}
    virtual void visit(const Until* e) {}
    virtual void visit(const WeakUntil* e) {}
    virtual void visit(const Release* e) {}
  };

  /**
   * Abstract visitor pattern Visitor base class for LTL expressions,
   * which supports the calculation of inherited attributes.
   */
  class TopDownVisitor {
  public:
    virtual IAttr visit(const InputSymbol* e,  IAttr a) { return a; }
    virtual IAttr visit(const OutputSymbol* e, IAttr a) { return a; }
    virtual IAttr visit(const Not* e,          IAttr a) { return a; }
    virtual IAttr visit(const Next* e,         IAttr a) { return a; }
    virtual IAttr visit(const Eventually* e,   IAttr a) { return a; }
    virtual IAttr visit(const Globally* e,     IAttr a) { return a; }
    virtual IAttr visit(const And* e,          IAttr a) { return a; }
    virtual IAttr visit(const Or* e,           IAttr a) { return a; }
    virtual IAttr visit(const Until* e,        IAttr a) { return a; }
    virtual IAttr visit(const WeakUntil* e,    IAttr a) { return a; }
    virtual IAttr visit(const Release* e,      IAttr a) { return a; }
  };

#define LTL_ATOMIC_VISITOR						\
  void accept(BottomUpVisitor& v)         const { v.visit(this); }	\
  void accept(TopDownVisitor& v, IAttr a) const { v.visit(this, a); }

#define LTL_UNARY_VISITOR						\
  void accept(BottomUpVisitor& v)         const { expr1->accept(v); v.visit(this); } \
  void accept(TopDownVisitor& v, IAttr a) const { expr1->accept(v, v.visit(this, a)); }

#define LTL_BINARY_VISITOR					\
  void accept(BottomUpVisitor& v) const					\
  {								\
    expr1->accept(v);						\
    expr2->accept(v);						\
    v.visit(this);						\
  }								\
								\
  void accept(TopDownVisitor& v, IAttr a) const			\
  {								\
    IAttr a1 = v.visit(this, a);				\
    expr1->accept(v, a1);					\
    expr2->accept(v, a1);					\
  }



  /**
   * Base Expression
   */
  class Expr {
  public:
    short label;
    std::string id;
    Expr(std::string _id="not implemented"): id(_id) {}
    virtual operator std::string () const { return id; }
    virtual void accept(BottomUpVisitor& v) const = 0;
    virtual void accept(TopDownVisitor& v, IAttr a) const = 0;
  };

  class UnaryExpr : public Expr {
  public:
    Expr *expr1;
    UnaryExpr(std::string _id, Expr *e): expr1(e), Expr(_id) {
      assert(e);
      label = ltl_label++;
    }
    operator std::string () const {
      std::stringstream s;
      s << id << "(" << std::string(*expr1) << ")" ;
      return s.str();
    }
  };

  class BinaryExpr : public Expr {
  public:
    Expr *expr1, *expr2;
    BinaryExpr(std::string _id, Expr *e1, Expr *e2): expr1(e1), expr2(e2), Expr(_id) {
      assert(e1 && e2);
      label = ltl_label++;
    }
    operator std::string () const {
      std::stringstream s;
      s << id << "(" << std::string(*expr1) << ", " << std::string(*expr2) << ")" ;
      return s.str();
    }
  };

  class InputSymbol : public Expr {
  public:
    char c;
    InputSymbol(int _c) { 
      label = ltl_label++;
      c = (char)_c; 
      id = "input("+std::string(1, c)+")"; 
    }
    LTL_ATOMIC_VISITOR
  };

  class OutputSymbol : public Expr {
  public:
    char c;
    OutputSymbol(int _c) { 
      label = ltl_label++;
      c = (char)_c;
      id = "output("+std::string(1, c)+")"; 
    }
    LTL_ATOMIC_VISITOR
  };
                    
  class Not : public UnaryExpr {
  public:
    Not(Expr *e) : UnaryExpr("not", e) {}
    LTL_UNARY_VISITOR
  };

  /// X / ○ operator
  class Next : public UnaryExpr {
  public:
    Next(Expr *e) : UnaryExpr("next", e) {}
    LTL_UNARY_VISITOR
  };

  /// F / ◇ operator
  class Eventually : public UnaryExpr {
  public:
    Eventually(Expr *e) : UnaryExpr("eventually", e) {}
    LTL_UNARY_VISITOR
  };

  /// G / □ operator
  class Globally : public UnaryExpr {
  public:
    Globally(Expr *e) : UnaryExpr("globally", e) {}
    LTL_UNARY_VISITOR
  };
                    
  class And : public BinaryExpr {
  public:
    And(Expr *e1, Expr *e2) : BinaryExpr("and", e1, e2) {}
    LTL_BINARY_VISITOR
  };

  class Or : public BinaryExpr {
  public:
    Or(Expr *e1, Expr *e2) : BinaryExpr("or", e1, e2) {}
    LTL_BINARY_VISITOR
  };

  /// U operator
  class Until : public BinaryExpr {
  public:
    Until(Expr *e1, Expr *e2) : BinaryExpr("until", e1, e2) {}
    LTL_BINARY_VISITOR
  };

  /// WU / weak U operator
  class WeakUntil : public BinaryExpr {
  public:
    WeakUntil(Expr *e1, Expr *e2) : BinaryExpr("weakUntil", e1, e2) {}
    LTL_BINARY_VISITOR
  };

  /// R operator
  class Release : public BinaryExpr {
  public:
    Release(Expr *e1, Expr *e2) : BinaryExpr("release", e1, e2) {}
    LTL_BINARY_VISITOR
  };


  /**
   * A well-formed LTL formula.
   * Performs basic grammar checking on an LTL expressions.
   */
  class Formula {
  public:
    /**
     * Construct a new formula and make sure all atomic expressions are properly quantified.
     *
     * Note: this may be a misconception, but I do not understand what
     * "oX" is supposed to mean if it is encountered
     * unquantified. E.g., what's the difference between "oX" and "G
     * oX", otherwise?
     */
    Formula(const Expr& e1) :e(e1), expr_size(ltl_label) {
      class WellFormedVisitor: public TopDownVisitor {
      public:
        struct Attr: public InheritedAttribute { 
          Attr(bool q) : quantified(q) {}
          bool quantified; 
        };

	// FIXME: is there a less ugly way to implement generic attributes?
        static Attr* getAttr(IAttr a) { return static_cast<Attr*>(a.get()); }
        static IAttr newAttr(bool b)  { return IAttr((InheritedAttribute*)new Attr(b)); }

        IAttr visit(const InputSymbol* e, IAttr a) {
          if (!getAttr(a)->quantified) throw "unquantified input operation"; 
          return a;
        }
        IAttr visit(const OutputSymbol* e, IAttr a) {
          if (!getAttr(a)->quantified) throw "unquantified output operation";
          return a;
        }

        IAttr visit(const Next* e,       IAttr a) { return newAttr(true); }
        IAttr visit(const Eventually* e, IAttr a) { return newAttr(true); }
        IAttr visit(const Globally* e,   IAttr a) { return newAttr(true); }
        IAttr visit(const Until* e,      IAttr a) { return newAttr(true); }
        IAttr visit(const WeakUntil* e,  IAttr a) { return newAttr(true); }
        IAttr visit(const Release* e,    IAttr a) { return newAttr(true); }

        IAttr visit(const Not* e,        IAttr a) { return a; }
        IAttr visit(const And* e,        IAttr a) { return a; }
        IAttr visit(const Or* e,         IAttr a) { return a; }

      };

      WellFormedVisitor v;
      e1.accept(v, WellFormedVisitor::newAttr(false));
    }

    operator std::string () const { return std::string(e); }
    operator const Expr& () const { return e; }
    short size() const { return expr_size; }

  protected:
    const Expr& e;
    short expr_size;
  };

};

#endif
