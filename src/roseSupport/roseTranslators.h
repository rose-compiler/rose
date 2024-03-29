#ifndef ROSETRANSLATORS_H
#define ROSETRANSLATORS_H

// MSTL
#include <Translator.h>

//#include <sage3.h>
#include <AstDOTGeneration.h>

#include <AstJSONGeneration.h>

#include <AstConsistencyTests.h>

//! Default Cpp2Cpp Translator. Translates a C++ program by
//! invoking the EDG frontend and the ROSE backend, to a slightly reformatted C++
//! program. The default midend is the identity transformation.
//! This Translator can be customized by overriding frontend/midend/backend.
//! Examples are CppToDotTranslator, CppToJsonTranslator, CppTestTranslator.
class ROSE_DLL_API CppToCppTranslator : public Translator<int> {

 public:
  CppToCppTranslator();
  virtual ~CppToCppTranslator();
  virtual void setOptions(int argc,char** argv);
  virtual void setOptions(const std::vector <std::string>& argvList);
  virtual void frontend();
  virtual void midend();
  virtual void backend();
  //! the status code allows to report front/mid/backend warnings and errors.
  virtual int status();
  //! messages to the user (stdout).
  //! Handles the impact of command line flags on printing of messages to stdout. For example, it is suppressed in preprocessing mode.
  virtual void printMessage(std::string s);

 protected:
  //! This function is used to get the root of the Ast which is
  //! generated by the frontend
  SgProject* getAstRoot();
  //! The AST root is set by the frontend
  void setAstRoot(SgProject*);
  void setStatusCode(int code) { statusCode=code; }
 private:
  SgProject* sageProject;
  int statusCode;
};

class ROSE_DLL_API CppToVendorTranslator : public CppToCppTranslator {
protected:
  /*! the backend unparses and invokes the vendor compiler */
  virtual void backend();
};

class ROSE_DLL_API CppToJsonTranslator : public CppToCppTranslator {
 public:
    static bool dumpFullAST; // a flag to indicate if full AST (including parts from headers) should be dumped
 protected:
  /*! the backend generates a JSON file (instead of a C++ file). */
  virtual void backend();
};

class ROSE_DLL_API CppToDotTranslator : public CppToCppTranslator {
 protected:
  /*! the backend generates a DOT file (instead of a C++ file). */
  void backend();
};


class ROSE_DLL_API RoseTestTranslator : public CppToVendorTranslator {
 protected:
  //! The frontend and backend are default, the midend implements
  //! a test on the ROSE infrastructure
  virtual void midend();
};

#endif
