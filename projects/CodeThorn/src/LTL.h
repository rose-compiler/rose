// -*- C++ -*-
#ifndef __LTL_H__
#define __LTL_H__

// LTL Abstract syntax tree (AST)
namespace ltl {
  class Expr {
  public:
    std::string id;
    Expr(std::string _id="not implemented"): id(_id) {}
    virtual operator std::string () { return id; }
  };

  class UnaryExpr : public Expr {
  public:
    Expr *expr;
    UnaryExpr(std::string _id, Expr *e): expr(e), Expr(_id) {
      assert(e);
    }
    operator std::string () {
      std::stringstream s;
      s << id << "(" << std::string(*expr) << ")" ;
      return s.str();
    }
  };

  class BinaryExpr : public Expr {
  public:
    Expr *expr1, *expr2;
    BinaryExpr(std::string _id, Expr *e1, Expr *e2): expr1(e1), expr2(e2), Expr(_id) {
      assert(e1 && e2);
    }
    operator std::string () {
      std::stringstream s;
      s << id << "(" << std::string(*expr1) << ", " << std::string(*expr2) << ")" ;
      return s.str();
    }
  };

  class InputSymbol : public Expr {
  public:
    char c;
    InputSymbol(int _c) { c = (char)_c; id = "input("+std::string(1, c)+")"; }
  };

  class OutputSymbol : public Expr {
  public:
    char c;
    OutputSymbol(int _c) { c = (char)_c; id = "output("+std::string(1, c)+")"; }
  };
                    
  class Not : public UnaryExpr {
  public:
    Not(Expr *e) : UnaryExpr("not", e) {}
  };

  class Next : public UnaryExpr {
  public:
    Next(Expr *e) : UnaryExpr("next", e) {}
  };

  class Eventually : public UnaryExpr {
  public:
    Eventually(Expr *e) : UnaryExpr("eventually", e) {}
  };

  class Globally : public UnaryExpr {
  public:
    Globally(Expr *e) : UnaryExpr("globally", e) {}
  };
                    
  class And : public BinaryExpr {
  public:
    And(Expr *e1, Expr *e2) : BinaryExpr("and", e1, e2) {}
  };

  class Or : public BinaryExpr {
  public:
    Or(Expr *e1, Expr *e2) : BinaryExpr("or", e1, e2) {}
  };

  class Until : public BinaryExpr {
  public:
    Until(Expr *e1, Expr *e2) : BinaryExpr("until", e1, e2) {}
  };

  class WeakUntil : public BinaryExpr {
  public:
    WeakUntil(Expr *e1, Expr *e2) : BinaryExpr("weakUntil", e1, e2) {}
  };

  class Release : public BinaryExpr {
  public:
    Release(Expr *e1, Expr *e2) : BinaryExpr("release", e1, e2) {}
  };

};

#endif
