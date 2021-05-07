// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"
#include "sageInterfaceAda.h"
#include "sageGeneric.h"

#include <sstream>

namespace si = SageInterface;

namespace
{
  template <class TypedSageNode>
  void checkType(std::ostream& os, TypedSageNode* n)
  {
    if (!n) return;

    si::ada::FlatArrayType res = si::ada::getArrayTypeInfo(n->get_type());

    if (!res.first) { return; }

    os << "Found ArrayType: " << n->unparseToString() << std::flush;

    for (SgExpression* exp : res.second)
    {
      // test that the range is given in one of the known forms
      ROSE_ASSERT(  isSgRangeExp(exp)
                 || isSgTypeExpression(exp)
                 || isSgAdaAttributeExp(exp)
                 );

      // test printing capabilities
      os << ", " << SG_DEREF(exp).unparseToString();
    }

    os << std::endl;
  }

  void checkExpr(std::ostream& os, SgAdaAttributeExp* n)
  {
    if (!n) return;

    SgRangeExp* rangeExpr = si::ada::range(n);
    const bool  rangeAttr = boost::to_upper_copy(n->get_attribute().getString()) == "RANGE";

    std::string out = "<null>";

    if (rangeExpr) out = rangeExpr->unparseToString();

    os << "Found Attribute: " << n->unparseToString() << " " << rangeAttr
       << " = " << out
       << std::endl;
  }

  struct SageInterfaceAdaCheck : AstSimpleProcessing
  {
    void resetStream()
    {
      output.str(std::string{});
      output.clear();
    }

    void visit(SgNode* n) ROSE_OVERRIDE
    {
/*
      if (!n) return;
      switch (n->variantT())
      {
      // types    
      case V_SgInitializedName:
        {
          SgInitializedName* init_name = isSgInitializedName(n);

          // transAutoType(init_name);

          // call Peter's interface function to grab type information
          std::pair<SgArrayType*, std::vector<SgExpression*> > type_info = SageInterface::ada::getArrayTypeInfo(init_name->get_type());

          if (type_info.first)
          {
            std::cout<<"Found an array type for "<<init_name->get_name() <<std::endl;
          }
          break;
        }
      
      default: ;
      }
*/
      checkType(output, isSgExpression(n));
      checkType(output, isSgInitializedName(n));
      checkExpr(output, isSgAdaAttributeExp(n));

      resetStream();
    }

    std::stringstream output;
  };
  

  template<class Checker>
  void check(SgProject* n)
  {
    ROSE_ASSERT(n);

    Checker checker;

    for (SgFile* file : n->get_files())
    {
      if (SgSourceFile* srcfile = isSgSourceFile(file))
        checker.traverse(srcfile, preorder);
    }
  }
}

int main( int argc, char * argv[] )
{
  // Initialize and check compatibility. See Rose::initialize
  ROSE_INITIALIZE;

  // DQ (3/5/2017): Add message logging by to be on by default for testing (disable conditional support for testing).
  if (Rose::Diagnostics::isInitialized() == true)
  {
       // DQ (3/6/2017): Test API to set frontend and backend options for tools (minimal output from ROSE-based tools).
       // Note that the defaults are for minimal output from ROSE-based tools.
          Rose::global_options.set_frontend_notes(false);
          Rose::global_options.set_frontend_warnings(false);
          Rose::global_options.set_backend_warnings(false);
  }

  SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
  AstTests::runAllTests(project);

  check<SageInterfaceAdaCheck>(project);

  // last check, tests if symbol table conversion to case sensitive succeeds.
  si::ada::convertToCaseSensitiveSymbolTables(project);
  return 0;
}
