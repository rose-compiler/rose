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
  try
  {
    if (!n) return;

    si::Ada::FlatArrayType res = si::Ada::getArrayTypeInfo(n->get_type());

    if (!res.type()) { return; }

    os << "Found ArrayType: " << n->unparseToString() << std::flush;

    for (SgExpression* exp : res.dims())
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
  catch (const std::exception& err)
  {
    os << "Err: " << n->unparseToString() << std::endl;
    throw;
  }
  catch (...)
  {
    os << "Unknown Error" << std::endl;
    throw;
  }

  void checkExpr(std::ostream& os, SgAdaAttributeExp* n)
  {
    if (!n) return;

    SgRangeExp* rangeExpr = si::Ada::range(n);
    const bool  rangeAttr = boost::to_upper_copy(n->get_attribute().getString()) == "RANGE";

    std::string out = "<null>";

    if (rangeExpr) out = rangeExpr->unparseToString();

    os << "Found Attribute: " << n->unparseToString() << " " << rangeAttr
       << " = " << out
       << std::endl;
  }

  template <class V>
  struct Vector
  {
    std::vector<V>& vec;
  };

  std::ostream& operator<<(std::ostream& os, const si::Ada::RecordField& rec)
  {
    return os << rec.originalSymbol().get_name()
              << (rec.inherited() ? " [inh]" : "")
              //~ << (rec.discriminant() ? " [dsc]" : "")
              ;
  }

  template <class V>
  std::ostream& operator<<(std::ostream& os, const Vector<V>& vecwrap)
  {
    for (const V& v : vecwrap.vec) os << v << ", ";

    return os;
  }

  void checkGetAllRecordFields(std::ostream& os, const SgClassDefinition* n)
  {
    if (!n) return;

    const SgClassDeclaration&         dcl = SG_DEREF( isSgClassDeclaration(n->get_parent()) );
    std::vector<si::Ada::RecordField> fields = si::Ada::getAllRecordFields(*n);

    os << "* record " << dcl.get_name() << "\n"
       << Vector<si::Ada::RecordField>{fields} << "\n"
       << std::endl;
  }

  struct SageInterfaceAdaCheck : AstSimpleProcessing
  {
    void resetStream()
    {
      output.str(std::string{});
      output.clear();
    }

    void visit(SgNode* n) override
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
      checkGetAllRecordFields(output, isSgClassDefinition(n));
      //~ checkGetAllRecordFields(std::cerr, isSgClassDefinition(n));

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
/*
  struct ConversionOnTypes
  {
    template <class SageNode>
    auto _handle(SageNode& n) -> decltype(n.get_type())
    {

    }

    void _handle(SgNode& n) {}

    template <class SageNode>
    void handle(SageNode& n) { _handle(n); }
    {

    }
  };
*/

  void checkConvertToOperatorRepOnTypes()
  {
    VariantVector vv{V_SgType};

    for (SgNode* el : NodeQuery::queryMemoryPool(vv))
      si::Ada::convertToOperatorRepresentation(el);
  }


}

int main( int argc, char * argv[] )
{
  int errCode = 0;

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

  // try to prevent "terminate called recursively" errors from the ASIS frontend
  try
  {
    Rose::failedAssertionBehavior(Rose::throwOnFailedAssertion);

    SgProject* project = frontend(argc,argv);

    // AST consistency tests (optional for users, but this enforces more of our tests)
    AstTests::runAllTests(project);

    check<SageInterfaceAdaCheck>(project);

    // last: check conversion functions
    si::Ada::convertToOperatorRepresentation(project, false /* convert operators that have the syntax flag set */);
    // std::cerr << "checked si::Ada::convertToOperatorRepresentation(project, false)" << std::endl;

    si::Ada::convertToOperatorRepresentation(project, true /* convert all operators */, true /* resolve named args */);
    si::Ada::convertToCaseSensitiveSymbolTables(project);

    checkConvertToOperatorRepOnTypes();
  }
  catch (...)
  {
    errCode = 1;
  }

  return errCode;
}
