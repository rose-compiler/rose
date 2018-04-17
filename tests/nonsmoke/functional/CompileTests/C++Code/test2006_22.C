#if 0
compiling the following code in ROSE:
template <char sep>
class ContainerDescriptor
{};

class SymbolicParamListDescriptor
  : public ContainerDescriptor<','>
{};
gives the error:
identityTranslator:
/home/saebjornsen1/ROSE-October/NEW_ROSE/src/midend/astDiagnostics/AstConsistencyTests.C:1339:
virtual void TestAstForProperlyMangledNames::visit(SgNode*): Assertion "mangledName.find('\'') == string::npos" failed.

Andreas
#endif


template <char sep>
class ContainerDescriptor
   {};

class SymbolicParamListDescriptor
   : public ContainerDescriptor<','>
   {};


