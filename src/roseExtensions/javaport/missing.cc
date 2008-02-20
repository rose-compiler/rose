#include "rose.h"
#include <list>

void SgCtorInitializerList::insert_ctor_initializer(Rose_STL_Container<SgInitializedName*>::iterator const&, SgInitializedName*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void SgFunctionCallExp::insert_arg(Rose_STL_Container<SgExpression*>::iterator const&, SgExpression*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void SgAggregateInitializer::insert_initializer(Rose_STL_Container<SgExpression*>::iterator const&, SgExpression*)  {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void SgMemberFunctionDeclaration::insert_ctor_initializer(Rose_STL_Container<SgInitializedName*>::iterator const&, SgInitializedName*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

std::string ROSE::identifyVariant(int) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

#if 0
std::string ROSE::stringCopyEdit(std::string, std::string, std::string) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
#endif

SgPragma* ROSE::getPragma(SgBinaryOp*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

ROSE::ROSE() {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

SgPragma* ROSE::getPragma(SgExpression*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

std::string ROSE::getPragmaString(SgExpression*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

std::string ROSE::getPragmaString(SgStatement*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void writeFile(char const*, char const*, char const*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void generatePDFofEDG(SgProject const&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

SgPragma* ROSE::getPragma(SgStatement*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void ROSE::ROSE_Unparse(SgFile*, std::ostream*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

char* copyEdit(char*, char const*, char const*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void TestExpressionTypes::test(SgNode*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

void dummyTests(SgProject*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}

#if 0
void Unparser::unparseStructType(SgType*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::unparseParForStmt(SgStatement*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::run_unparser() {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::unparseParStmt(SgStatement*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::outputDirective(PreprocessingInfo*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::unparseUnionType(SgType*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::unparseElseIfStmt(SgStatement*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
void Unparser::unparseDeclStmt(SgStatement*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
bool Unparser::RemoveArgs(SgExpression*) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
#endif
void Unparser::unparseProject(SgProject*, SgUnparse_Info&) {
  std::cerr << "missing function" << std::endl;
  exit(1);
}
