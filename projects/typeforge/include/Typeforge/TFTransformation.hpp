#ifndef TFTRANSFORMATION_H
#define TFTRANSFORMATION_H

#include "AstTerm.h"

#include <string>

class SgNode;

namespace Typeforge {

class TFTransformation;

class TransformationSpec{
  protected:
    TransformationSpec(SgFunctionDefinition* def);
    SgFunctionDefinition* funDef;
  public:
    virtual int run(SgProject* project, RoseAst ast, TFTransformation* tf) = 0;
};

class ADTransformation : public TransformationSpec{
  public:
    ADTransformation(SgFunctionDefinition* def);
    int run(SgProject* project, RoseAst ast, TFTransformation* tf);
};

class ArrayStructTransformation : public TransformationSpec{
  private:
    SgType* type;
  public:
    ArrayStructTransformation(SgFunctionDefinition* def, SgType* accessType);
    int run(SgProject* project, RoseAst ast, TFTransformation* tf);
};

class ReadWriteTransformation : public TransformationSpec{
  private:
    SgType* type;
  public:
    ReadWriteTransformation(SgFunctionDefinition* def, SgType* accessType);
    int run(SgProject* project, RoseAst ast, TFTransformation* tf);
};

class PragmaTransformation : public TransformationSpec{
  private:
    std::string fromString;
    std::string toString;
  public:
    PragmaTransformation(std::string from, std::string to);
    int run(SgProject* project, RoseAst ast, TFTransformation* tf);
};

class IncludeTransformation : public TransformationSpec{
  private:
    std::string includeFile;
    bool systemHeader;
    SgSourceFile* source;
  public:
    IncludeTransformation(std::string include, bool system, SgSourceFile* sourceFile);
    int run(SgProject* project, RoseAst ast, TFTransformation* tf);
};

class ReplacementString{
  public:
    std::string prepend;
    std::string replace;
    std::string append;
    std::string generate(SgNode* node);
    ReplacementString(std::string before, std::string overwrite, std::string after);
};

class TFTransformation {
  public:
    void transformHancockAccess(SgType* exprTypeName, SgNode* root);
    int readTransformations=0;
    int writeTransformations=0;
    int statementTransformations=0;
    int arrayOfStructsTransformations=0;
    int adIntermediateTransformations=0;
    bool trace=false;

    void transformRhs(SgType* accessType, SgNode* rhsRoot);
    void checkAndTransformVarAssignments(SgType* accessType,SgNode* root);
    void checkAndTransformNonAssignments(SgType* accessType,SgNode* root);
    void transformArrayAssignments(SgType* accessType,SgNode* root);
    void transformArrayOfStructsAccesses(SgType* accessType,SgNode* root);
    //Transformation ad_intermediate
    void instrumentADIntermediate(SgNode* root);
    int instrumentADDecleration(SgInitializer* init);
    void instrumentADGlobals(SgProject* project, RoseAst ast);
    void addADTransformation(SgFunctionDefinition* funDef);
    void addArrayStructTransformation(SgFunctionDefinition* funDef, SgType* accessType);
    void addReadWriteTransformation(SgFunctionDefinition* funDef, SgType* accessType);
    void addPragmaTransformation(std::string from, std::string to);
    void addIncludeTransformation(std::string includeFile, bool systemHeader, SgSourceFile* source);
    void insertInclude(std::string includeFile, bool systemHeader, SgSourceFile* source);
    void prependNode(SgNode* node, std::string newCode);
    void replaceNode(SgNode* node, std::string newCode);
    void appendNode(SgNode* node, std::string newCode);

  public:
    void transformationAnalyze();
    void transformationExecution();

  private:
    std::list<TransformationSpec*> _transformationList;
    std::list<std::tuple<std::string,bool,SgSourceFile*>> _newHeaders;
    std::map<SgNode*, ReplacementString*> _transformations;
    
};

}

#endif
