
#include <numeric>

#include "MatrixTypeTransformer.h"

#include "rose.h"
#include "sageGeneric.h"

#include "utility/utils.h"

namespace sb = SageBuilder;
namespace ru = RoseUtils;


namespace MatlabToCpp
{
  // \note see below for an alternative approach
  struct MatrixTypeSetter : AstSimpleProcessing
  {
    typedef std::map<SgType*, SgType*> instantiation_map;

    instantiation_map           memory;

    MatrixTypeSetter()
    : memory()
    {}

    virtual void visit(SgNode* n);
  };


  struct TemplateArgListBuilder
  {
    bool first;

    TemplateArgListBuilder()
    : first(true)
    {}

    std::string operator()(std::string s, SgType* t)
    {
      if (first) { first = false; s += ", "; }

      return s += t->unparseToString();
    }
  };


  struct MatrixVarTypeSetter
  {
    typedef MatrixTypeSetter::instantiation_map instantiation_map;

    instantiation_map&          memory;

    explicit
    MatrixVarTypeSetter(instantiation_map& m)
    : memory(m)
    {}

    std::string genTemplateTypeName(SgTypeTuple& t)
    {
      return std::accumulate( t.get_types().begin(),
                              t.get_types().end(),
                              std::string("Tuple<"),
                              TemplateArgListBuilder()
                            ) + ">";
    }

    std::string genTemplateTypeName(SgTypeMatrix& m)
    {
      std::string res = "Matrix<";

      res += m.get_base_type()->unparseToString();
      res += ">";

      return res;
    }

    template <class T>
    void convert_type(SgInitializedName& n, T& t)
    {
      SgType*& prevconv = memory[&t];

      if (!prevconv)
      {
        std::string       tyname = genTemplateTypeName(t);
        SgScopeStatement* scope = sg::ancestor<SgScopeStatement>(&n);
        ROSE_ASSERT(scope);

        prevconv = sb::buildOpaqueType(tyname, scope);
      }

      n.set_type(prevconv);
    }

    void handle(SgNode& n) {}

    void handle(SgInitializedName& n)
    {
      if (isSgTypeMatrix(n.get_type()))
        convert_type(n, *isSgTypeMatrix(n.get_type()));
      else if (isSgTypeTuple(n.get_type()))
        convert_type(n, *isSgTypeTuple(n.get_type()));
      else
      {
        std::cerr << ":: " << n.get_name()
                  << ": "  << ru::str(n.get_type()) << std::flush
                  << " "   << typeid(*n.get_type()).name()
                  << std::endl;
        // ROSE_ASSERT(false);
      }
    }
  };


  void MatrixTypeSetter::visit(SgNode* n)
  {
    sg::dispatch(MatrixVarTypeSetter(memory), n);
  }

  void transformMatrixType(SgProject* project)
  {
    MatrixTypeSetter matrixTraversal;

    matrixTraversal.traverse(project, preorder);
  }
} /* namespace MatlabTransformation */


/*********             *******/
/********* end of file *******/
/*********             *******/
/*****************************/
/*********             *******/
/********* old stuff   *******/
/********* below       *******/
/*********             *******/



/*
struct PowerOpPattern : sg::DispatchHandler <std::vector<SgPowerOp*> >
{
  void handle(SgNode& n) {}
  void handle(SgPowerOp& n) { res.push_back(&n); }
};

struct PowerOpTransformer : AstSimpleProcessing
{
  void visit(SgNode* ) ROSE_OVERRIDE;
};

PowerOpTransformer::visit(SgNode* n)
{
  std::vector<SgPowerOp*>           powerops = sg::dispatch(PowerOpPattern(), n);
  std::vector<SgPowerOp*>::iterator aa = powerops.begin();
  std::vector<SgPowerOp*>::iterator zz = powerops.end();

  while (aa != zz)
  {
    SgExpression*      lhs = (**aa).get_lhs_operand();
    SgExpression*      rhs = (**aa).get_rhs_operand();
    SgExpression*      lcp = si::deepCopy(lhs);
    SgExpression*      rcp = si::deepCopy(rhs);
    SgVarRefExpr*      var = sb::buildOpaqueVarRefExp("pow");

    SgFunctionCallExp* powcall = sb::buildFunctionCallExpr(var);

  }
}
*/



#if 0

too complicated and does not work, as there is no Matrix template when we parse
the Matlab code.

==> solution: opaque types!

/// function family to find a template class declaration with a given name
struct TemplateClassFinder : sg::DispatchHandler<SgTemplateClassDeclaration*>
{
  SgName name;

  explicit
  TemplateClassFinder(SgName n)
  : name(n)
  {}

  void handle(SgNode&) { /* only looking for template class decls */ }

  void handle(SgTemplateClassDeclaration& n)
  {
    if ((n.get_name() == name) && true /* \todo \pp should be scope test */)
    {
      res = &n;
    }
  }
};


/// \brief Traverses the AST and finds a matrix template.
///        The found template can be instantiated with a float type.
struct FindMatrixTemplate : AstSimpleProcessing
{
  SgTemplateClassDeclaration* matrix;
  SgTemplateClassDeclaration* tuple;

  FindMatrixTemplate()
  : matrix(NULL), tuple(NULL)
  {}

  virtual void visit(SgNode* n);
};

void FindMatrixTemplate::visit(SgNode* n)
{
  static const std::string matrix_template_name("matrix");
  static const std::string tuple_template_name("tuple");

  if (!matrix) matrix = sg::dispatch(TemplateClassFinder(matrix_template_name), n);
  if (!tuple)  tuple = sg::dispatch(TemplateClassFinder(tuple_template_name), n);
}

/// Traverses AST and replaces every SgMatrixType with a template
/// instantiation of matrix<float>.
struct MatrixTypeSetter : AstSimpleProcessing
{
  typedef std::map<SgType*, SgType*> instantiation_map;

  instantiation_map           memory;
  SgTemplateClassDeclaration* matrix_t;
  SgTemplateClassDeclaration* tuple_t;

  MatrixTypeSetter(SgTemplateClassDeclaration* m, SgTemplateClassDeclaration* t)
  : memory(), matrix_t(m), tuple_t(t)
  {
    ROSE_ASSERT(m && t);
  }

  virtual void visit(SgNode* n);
};

struct MatrixVarTypeSetter
{
  typedef MatrixTypeSetter::instantiation_map instantiation_map;

  instantiation_map&          memory;
  SgTemplateClassDeclaration* matrix_t;
  SgTemplateClassDeclaration* tuple_t;

  MatrixVarTypeSetter( instantiation_map& mem,
                       SgTemplateClassDeclaration* m,
                       SgTemplateClassDeclaration* t
                     )
  : memory(mem), matrix_t(m), tuple_t(t)
  {}

  SgTemplateClassDeclaration* baseTemplate(const SgTypeTuple&)   { return matrix_t; }
  SgTemplateClassDeclaration* baseTemplate(const SgTypeMatrix&) { return tuple_t; }

  SgNodePtrList genTemplateArgumentList(SgTypeTuple& t)
  {
    SgNodePtrList lst(t.get_types().begin(), t.get_types().end());

    return lst;
  }

  SgNodePtrList genTemplateArgumentList(SgTypeMatrix& m)
  {
    SgNodePtrList res;

    res.push_back(m.get_base_type());
    return res;
  }

  template <class T>
  void convert_type(SgInitializedName& n, T& t)
  {
    SgType*& prevconv = memory[&t];

    if (!prevconv)
    {
      SgNodePtrList targs = genTemplateArgumentList(t);

      prevconv = sb::buildClassTemplateType(baseTemplate(t), targs);
    }

    n.set_type(prevconv);
  }

  void handle(SgNode& n) {}

  void handle(SgInitializedName& n)
  {
    if (isSgTypeMatrix(n.get_type()))
      convert_type(n, *isSgTypeMatrix(n.get_type()));
    else if (isSgTypeTuple(n.get_type()))
      convert_type(n, *isSgTypeTuple(n.get_type()));
  }
};

#endif
