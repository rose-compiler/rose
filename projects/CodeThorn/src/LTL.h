// -*- C++ -*-
#ifndef __LTL_H__
#define __LTL_H__

//#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

extern FILE* ltl_input;
extern int ltl_parse();
extern bool ltl_eof;
extern unsigned short ltl_label;

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
namespace CodeThorn {
namespace LTL {
  
  /// Inherited Attribute for visitor pattern
  class InheritedAttribute {};

  //MS: removed smart pointers
  //typedef boost::shared_ptr<InheritedAttribute> IAttr;
  typedef InheritedAttribute* IAttr;

  class Expr;
  class InputSymbol;
  class OutputSymbol;
  class NegInputSymbol;
  class NegOutputSymbol;
  class Not;
  class Next;
  class Eventually;
  class Globally;
  class And;
  class Or;
  class Until;
  class WeakUntil;
  class Release;
  class True;
  class False;

  enum NodeType { 
    e_Error=0, 
    e_InputSymbol, e_OutputSymbol, e_NegInputSymbol, e_NegOutputSymbol, 
    e_True, e_False,
    e_Not, e_Next, e_Eventually, e_Globally, e_And, e_Or, e_Until,
    e_WeakUntil, e_Release };

  /**
   * Abstract visitor pattern Visitor base class for LTL expressions.
   */
  class BottomUpVisitor {
  public:
    virtual void visit(const InputSymbol* e) {}
    virtual void visit(const OutputSymbol* e) {}
    virtual void visit(const NegInputSymbol* e) {}
    virtual void visit(const NegOutputSymbol* e) {}
    virtual void visit(const Not* e) {}
    virtual void visit(const Next* e) {}
    virtual void visit(const Eventually* e) {}
    virtual void visit(const Globally* e) {}
    virtual void visit(const And* e) {}
    virtual void visit(const Or* e) {}
    virtual void visit(const Until* e) {}
    virtual void visit(const WeakUntil* e) {}
    virtual void visit(const Release* e) {}
    virtual void visit(const True* e) {}
    virtual void visit(const False* e) {}
  };

  /**
   * Abstract visitor pattern Visitor base class for LTL expressions,
   * which supports the calculation of inherited attributes.
   */
  class TopDownVisitor {
  public:
    virtual IAttr visit(InputSymbol* e,     IAttr a) { return a; }
    virtual IAttr visit(OutputSymbol* e,    IAttr a) { return a; }
    virtual IAttr visit(NegInputSymbol* e,  IAttr a) { return a; }
    virtual IAttr visit(NegOutputSymbol* e, IAttr a) { return a; }
    virtual IAttr visit(Not* e,             IAttr a) { return a; }
    virtual IAttr visit(Next* e,            IAttr a) { return a; }
    virtual IAttr visit(Eventually* e,      IAttr a) { return a; }
    virtual IAttr visit(Globally* e,        IAttr a) { return a; }
    virtual IAttr visit(And* e,             IAttr a) { return a; }
    virtual IAttr visit(Or* e,              IAttr a) { return a; }
    virtual IAttr visit(Until* e,           IAttr a) { return a; }
    virtual IAttr visit(WeakUntil* e,       IAttr a) { return a; }
    virtual IAttr visit(Release* e,         IAttr a) { return a; }
    virtual IAttr visit(True* e,            IAttr a) { return a; }
    virtual IAttr visit(False* e,           IAttr a) { return a; }
  };

#define LTL_ATOMIC_VISITOR                        \
  void accept(BottomUpVisitor& v)         const { v.visit(this); }    \
  void accept(TopDownVisitor& v, IAttr a)       { v.visit(this, a); }

#define LTL_UNARY_VISITOR                        \
  void accept(BottomUpVisitor& v)         const { expr1->accept(v); v.visit(this); } \
  void accept(TopDownVisitor& v, IAttr a)       { expr1->accept(v, v.visit(this, a)); }

#define LTL_BINARY_VISITOR                    \
  void accept(BottomUpVisitor& v) const                    \
  {                                \
    expr1->accept(v);                        \
    expr2->accept(v);                        \
    v.visit(this);                        \
  }                                \
                                \
  void accept(TopDownVisitor& v, IAttr a)            \
  {                                \
    IAttr a1 = v.visit(this, a);                \
    expr1->accept(v, a1);                    \
    expr2->accept(v, a1);                    \
  }



  /**
   * Base Expression
   */
  class Expr {
  public:
    bool quantified;
    short label;
    std::string id;
    enum NodeType type;
    Expr(std::string _id="not implemented", NodeType _type=e_Error): id(_id), type(_type) {}
    virtual operator std::string () const { return id; }
    virtual void accept(BottomUpVisitor& v) const = 0;
    virtual void accept(TopDownVisitor& v, IAttr a) = 0;
    virtual ~Expr() {}
  };

  class UnaryExpr : public Expr {
  public:
    Expr *expr1;
    UnaryExpr(std::string _id, NodeType _type, Expr *e): Expr(_id, _type), expr1(e) {
      assert(e);
      label = ltl_label++;
    }
    operator std::string () const {
      std::stringstream s;
      s << id << "(" << std::string(*expr1) << ")";//^"<<label;
      return s.str();
    }
  };

  class BinaryExpr : public Expr {
  public:
    Expr *expr1, *expr2;
    BinaryExpr(std::string _id, NodeType _type, Expr *e1, Expr *e2): 
      Expr(_id, _type), expr1(e1), expr2(e2) {
      assert(e1 && e2);
      label = ltl_label++;
    }
    operator std::string () const {
      std::stringstream s;
      s << id << "(" << std::string(*expr1) << ", " << std::string(*expr2) << ")";//^"<<label;
      return s.str();
    }
  };

  class InputSymbol : public Expr {
  public:
    char c;
    InputSymbol(int _c) { 
      label = ltl_label++;
      type = e_InputSymbol;
      c = (char)_c; 
      std::stringstream s;
      s << "input("<<std::string(1, c)<<")";//^"<<label;
      id = s.str();
    }
    LTL_ATOMIC_VISITOR
  };

  class OutputSymbol : public Expr {
  public:
    char c;
    OutputSymbol(int _c) { 
      label = ltl_label++;
      type = e_OutputSymbol;
      c = (char)_c;
      std::stringstream s;
      s << "output("<<std::string(1, c)<<")";//^"<<label;
      id = s.str();
    }
    LTL_ATOMIC_VISITOR
  };

  /// special case for !iX
  class NegInputSymbol : public InputSymbol {
  public:
    NegInputSymbol(int _c) : InputSymbol(_c) {
      type = e_NegInputSymbol;
      std::stringstream s;
      s << "neg_input("<<std::string(1, c)<<")";//^"<<label;
      id = s.str();
    }
    LTL_ATOMIC_VISITOR
  };

  /// special case for !oX
  class NegOutputSymbol : public OutputSymbol {
  public:
    NegOutputSymbol(int _c) : OutputSymbol(_c) {
      type = e_NegOutputSymbol;
      std::stringstream s;
      s << "neg_output("<<std::string(1, c)<<")";//^"<<label;
      id = s.str();
    }
    LTL_ATOMIC_VISITOR
  };

  /// atom true
  class True : public Expr {
  public:
    True() {
      label = ltl_label++;
      type = e_True;
      id = "true";
    }
    LTL_ATOMIC_VISITOR
  };

  /// atom false
  class False : public Expr {
  public:
    False() {
      label = ltl_label++;
      type = e_False;
      id = "false";
    }
    LTL_ATOMIC_VISITOR
  };

                    
  /// ! / ¬ operator
  class Not : public UnaryExpr {
  public:
    Not(Expr *e) : UnaryExpr("not", e_Not, e) {}
    LTL_UNARY_VISITOR
  };

  /// X / ○ operator
  class Next : public UnaryExpr {
  public:
    Next(Expr *e) : UnaryExpr("next", e_Next, e) {}
    LTL_UNARY_VISITOR
  };

  /// F / ◇ operator
  class Eventually : public UnaryExpr {
  public:
    Eventually(Expr *e) : UnaryExpr("eventually", e_Eventually, e) {}
    LTL_UNARY_VISITOR
  };

  /// G / □ operator
  class Globally : public UnaryExpr {
  public:
    Globally(Expr *e) : UnaryExpr("globally", e_Globally, e) {}
    LTL_UNARY_VISITOR
  };
                    
  class And : public BinaryExpr {
  public:
    And(Expr *e1, Expr *e2) : BinaryExpr("and", e_And, e1, e2) {}
    LTL_BINARY_VISITOR
  };

  class Or : public BinaryExpr {
  public:
    Or(Expr *e1, Expr *e2) : BinaryExpr("or", e_Or, e1, e2) {}
    LTL_BINARY_VISITOR
  };

  /// U operator
  class Until : public BinaryExpr {
  public:
    Until(Expr *e1, Expr *e2) : BinaryExpr("until", e_Until, e1, e2) {}
    LTL_BINARY_VISITOR
  };

  /// WU / weak U operator
  class WeakUntil : public BinaryExpr {
  public:
    WeakUntil(Expr *e1, Expr *e2) : BinaryExpr("weakUntil", e_WeakUntil, e1, e2) {}
    LTL_BINARY_VISITOR
  };

  /// R operator
  class Release : public BinaryExpr {
  public:
    Release(Expr *e1, Expr *e2) : BinaryExpr("release", e_Release, e1, e2) {}
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
     * As a side effect, the quantified attribute of each expression node is set.
     *
     * Note: this may be a misconception, but I do not understand what
     * "oX" is supposed to mean if it is encountered
     * unquantified. E.g., what's the difference between "oX" and "G
     * oX", otherwise?
     */
    Formula(Expr& e1) :e(e1), expr_size(ltl_label) {
      class WellFormedVisitor: public TopDownVisitor {
      public:
        struct Attr: public InheritedAttribute { 
          Attr(bool q) : quantified(q) {}
          bool quantified; 
        };

    // FIXME: is there a less ugly way to implement generic attributes?
    // MS: needed to remove the shared ptr
        static Attr* getAttr(IAttr a) { return static_cast<Attr*>(a); }
        static IAttr newAttr(bool b)  { return new Attr(b); }

        IAttr visit(InputSymbol* e, IAttr a) {
      e->quantified=getAttr(a)->quantified; 
          if (!getAttr(a)->quantified) throw "unquantified input operation"; 
          return a;
        }
        IAttr visit(OutputSymbol* e, IAttr a) {
      e->quantified=getAttr(a)->quantified; 
          if (!getAttr(a)->quantified) throw "unquantified output operation";
          return a;
        }
        IAttr visit(NegInputSymbol* e, IAttr a) {
      e->quantified=getAttr(a)->quantified; 
          if (!getAttr(a)->quantified) throw "unquantified input operation"; 
          return a;
        }
        IAttr visit(NegOutputSymbol* e, IAttr a) {
      e->quantified=getAttr(a)->quantified; 
          if (!getAttr(a)->quantified) throw "unquantified output operation";
          return a;
        }

        IAttr visit(Next* e,       IAttr a) { e->quantified=getAttr(a)->quantified; return newAttr(true); }
        IAttr visit(Eventually* e, IAttr a) { e->quantified=getAttr(a)->quantified; return newAttr(true); }
        IAttr visit(Globally* e,   IAttr a) { e->quantified=getAttr(a)->quantified; return newAttr(true); }
        IAttr visit(Until* e,      IAttr a) { e->quantified=getAttr(a)->quantified; return newAttr(true); }
        IAttr visit(WeakUntil* e,  IAttr a) { e->quantified=getAttr(a)->quantified; return newAttr(true); }
        IAttr visit(Release* e,    IAttr a) { e->quantified=getAttr(a)->quantified; return newAttr(true); }

        IAttr visit(Not* e,        IAttr a) { e->quantified=getAttr(a)->quantified; return a; }
        IAttr visit(And* e,        IAttr a) { e->quantified=getAttr(a)->quantified; return a; }
        IAttr visit(Or* e,         IAttr a) { e->quantified=getAttr(a)->quantified; return a; }

        IAttr visit(True* e,       IAttr a)  { e->quantified=getAttr(a)->quantified; return a; }
        IAttr visit(False* e,      IAttr a) { e->quantified=getAttr(a)->quantified; return a; }

      };

      WellFormedVisitor v;
      e1.accept(v, WellFormedVisitor::newAttr(false));
    }

    operator std::string () const { return std::string(e); }
    operator const Expr& () const { return e; }
    size_t size() const { return expr_size; }

  protected:
    Expr& e;
    size_t expr_size;
  };

} // namespace LTL
 
} // namespace CodeThorn

#endif
