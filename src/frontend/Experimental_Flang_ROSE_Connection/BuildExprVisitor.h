#pragma once

namespace Rose::builder {

class BuildExprVisitor {
public:
  BuildExprVisitor(SgExpression* expr) : pre_{expr}, post_{nullptr} {}

  // In nearly all cases, this code avoids defining Boolean-valued Pre()
  // callbacks for the parse tree walking framework in favor of two void
  // functions, Before() and Build(), which imply true and false return
  // values for Pre() respectively.
  template <typename T> void Before(T &) {}
  template <typename T> double Build(T &); // not void, never used

  template <typename T> bool Pre(T &x) {
    if constexpr (std::is_void_v<decltype(Build(x))>) {
      // There is a local definition of Build() for this type.  It
      // overrides the parse tree walker's default Walk() over the descendents.
      Before(x);
      Build(x);
      Post(x);
      return false; // Walk() does not visit descendents
    }
    else {
      Before(x);
      return true; // there's no Build() defined here, Walk() the descendents
    }
  }

  template <typename T> void Post(T &) {}

  // Call back to the traversal framework.
  template <typename T> void Walk(T &x) {
    Fortran::parser::Walk(x, *this);
  }

  // IntrinsicOperator
  void Build(Fortran::parser::Expr::Power &);
  void Build(Fortran::parser::Expr::Multiply &);
  void Build(Fortran::parser::Expr::Divide &);
  void Build(Fortran::parser::Expr::Add &);
  void Build(Fortran::parser::Expr::Subtract &);
  void Build(Fortran::parser::Expr::Concat &);
  void Build(Fortran::parser::Expr::LT &);
  void Build(Fortran::parser::Expr::LE &);
  void Build(Fortran::parser::Expr::EQ &);
  void Build(Fortran::parser::Expr::NE &);
  void Build(Fortran::parser::Expr::GE &);
  void Build(Fortran::parser::Expr::GT &);
  void Build(Fortran::parser::Expr::NOT &);
  void Build(Fortran::parser::Expr::AND &);
  void Build(Fortran::parser::Expr::OR &);
  void Build(Fortran::parser::Expr::EQV &);
  void Build(Fortran::parser::Expr::NEQV &);

  void Build(Fortran::parser::Name &);
  void Build(Fortran::parser::IntLiteralConstant &);
  void Build(Fortran::parser::RealLiteralConstant &);

  // ArraySpec ...
  void Build(Fortran::parser::AssumedImpliedSpec &);
  void Build(Fortran::parser::ExplicitShapeSpec &);
  void Build(Fortran::parser::AssumedShapeSpec &);

  void Build(Fortran::parser::CharBlock &x) {
    // For future use with token-based unparsing?
#if 0
    startSource_ = x.interval_.start_;
    sizeSource_ = x.interval_.size_;
#endif
  }

  void Done() const {}

  // Build expressions for binary operators
  template <typename T> void BuildExpressions(T &x, SgExpression* &lhs, SgExpression* &rhs);

  // Access functions for synthesized attributes
  void get(SgExpression* &expr) {
    expr = post_;
    post_ = nullptr;
  }
  void set(SgExpression* expr) {
    ASSERT_not_null(expr);
    ASSERT_require(post_ == nullptr);
    post_ = expr;
  }

private:
  SgExpression* pre_; // expression attribute (probably not needed)
  SgExpression* post_; // synthesized expression attribute
  const char* startSource_; // start of Fortran::common::Interal expression source
  size_t sizeSource_; // size of Fortran::common::Interal expression source
}; // BuildExprVisitor

// Walk using BuildExprVisitor
template <typename T>
void WalkExpr(T &root, SgExpression* &expr) {
  BuildExprVisitor visitor{expr};
  Walk(root, visitor);
  visitor.Done();
  visitor.get(expr); // synthesized expression attribute
}

} // namespace Rose::builder
