#pragma once

namespace Rose::builder {

class BuildVisitor {
public:
  BuildVisitor() : cooked_{nullptr} {}
  BuildVisitor(Fortran::parser::AllCookedSources &cooked) : cooked_{&cooked}, type_{nullptr} {
  }

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
#define USE_FROM_FLANG 0
#if USE_FROM_FLANG
    else if constexpr (HasTypedExpr<T>::value) {
      // Format the expression representation from semantics
      if (asFortran_ && x.typedExpr) {
        // Probably not useful for high-level builder?
        //asFortran_->expr(out_, *x.typedExpr);
        return false;
      } else {
        return true;
      }
    }
#endif
    else {
      Before(x);
      return true; // there's no Build() defined here, Walk() the descendents
    }
  }
  template <typename T> void Post(T &) {}

  void setKindSelectorType(std::optional<Fortran::parser::KindSelector> &x) {
    // KindSelector std::variant<ScalarIntConstantExpr, StarSize> u;
    using namespace Fortran;
    if (x) {
      common::visit(common::visitors {
          [&] (parser::KindSelector::StarSize &y) {
                 if (type_ != nullptr) {
                   type_->set_hasTypeKindStar(true);
                 }
             },
          [&] (auto &y) { return; }
        },
        x->u);
      }
  }

  void Post(Fortran::parser::IntrinsicTypeSpec &x) {
    // IntrinsicTypeSpec std::variant<IntegerTypeSpec, Real, DoublePrecision, Complex, Character, Logical, DoubleComplex> u;
    using namespace Fortran;
    common::visit(common::visitors {
        [&] (Fortran::parser::IntrinsicTypeSpec::DoublePrecision &y) { return; },
        [&] (Fortran::parser::IntrinsicTypeSpec::Character &y) { return; },
        [&] (Fortran::parser::IntrinsicTypeSpec::DoubleComplex &y) { return; },
        [&] (Fortran::parser::IntegerTypeSpec &y) { setKindSelectorType(y.v); },
        [&] (auto &y) { setKindSelectorType(y.kind); }
      },
      x.u);
  }

  // Call back to the traversal framework.
  template <typename T> void Walk(T &x) {
    Fortran::parser::Walk(x, *this);
  }

  // Call back to the traversal framework for Expr
  template <typename T> void Walk(/*const*/ T &x, SgExpression* sage) {
    std::cerr << "--> TODO: make an expression visitor???\n";
    Fortran::parser::Walk(x, *this);
  }

  // Call back to the traversal framework.
  template <typename T, typename S> void Walk(T &x, S* sage) {
    std::cerr << "--> TODO: What is this for???\n";
    Fortran::parser::Walk(x, *this);
  }

  // ProgramUnit
  void Build(Fortran::parser::MainProgram &);
  void Build(Fortran::parser::Module &);
  void Build(Fortran::parser::Submodule &);
  void Build(Fortran::parser::FunctionSubprogram &);
  void Build(Fortran::parser::SubroutineSubprogram &);
  void Build(Fortran::parser::InternalSubprogramPart &);
  void Build(Fortran::parser::ModuleSubprogramPart &);
  void Build(Fortran::parser::BlockData &);
  void Build(Fortran::parser::ContainsStmt &);

  // SpecificationPart
  void Build(Fortran::parser::ImplicitStmt &);
  void Build(Fortran::parser::CommonStmt &);
  void Build(Fortran::parser::TypeDeclarationStmt &);

  // SpecificationConstruct
  void Build(Fortran::parser::DataStmt &);
  void Build(Fortran::parser::DerivedTypeDef &);

  void Build(Fortran::parser::IntegerTypeSpec &);
  void Build(Fortran::parser::IntrinsicTypeSpec::Real &);
  void Build(Fortran::parser::IntrinsicTypeSpec::DoublePrecision &);
  void Build(Fortran::parser::IntrinsicTypeSpec::Complex &);
  void Build(Fortran::parser::IntrinsicTypeSpec::DoubleComplex &);
  void Build(Fortran::parser::IntrinsicTypeSpec::Character &);
  void Build(Fortran::parser::IntrinsicTypeSpec::Logical &);

  // ExecutionPart
  void Build(Fortran::parser::AssignmentStmt &);
  void Build(Fortran::parser::StopStmt &);

  void Done() const { std::cerr << "Done()\n"; }

  void BuildPrefix(std::list<Fortran::parser::PrefixSpec> &,
                   LanguageTranslation::FunctionModifierList &, SgType* &);
  void BuildSuffix(Fortran::parser::Suffix &, std::string &);

  // Build types using a synthesized attribute
  void BuildType(Fortran::parser::DeclarationTypeSpec &x, SgType* &type) {
    Walk(x);
    this->get(type); // get synthesized attribute
  }

  // Access functions for synthesized attributes
  void get(SgType* &type) {
    type = type_;
    type_ = nullptr;
  }
  void set(SgType* type) {
    ASSERT_not_null(type);
    ASSERT_require(type_ == nullptr);
    type_ = type;
  }

private:
  Fortran::parser::AllCookedSources* cooked_;
  SgType* type_; // synthesized attribute

}; // BuildVisitor

} // namespace Rose::builder
