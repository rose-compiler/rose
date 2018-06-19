#ifndef DIMENSION_AST_ATTRIBUTE
#define DIMENION_AST_ATTRIBUTE

class LLVMAstAttributes;
class FunctionAstAttribute;

class DimensionAstAttribute : public RootAstAttribute { 
public: 

 DimensionAstAttribute(SgInitializedName *declaration_, LLVMAstAttributes *ast_attributes_, FunctionAstAttribute *function_attribute_, SgArrayType *array_type_)
      : declaration(declaration_),
        ast_attributes(ast_attributes_),
        function_attribute(function_attribute_),
        array_type(array_type_)
    {}

    int numDimensionExpressions() { return dimensionExpressions.size(); }
    SgExpression *getDimensionExpression(int i) { return dimensionExpressions[i]; }
    void insertDimensionExpression(SgExpression *exp) {
        dimensionExpressions.push_back(exp);
    }

    std::stringstream &getBuffer() { return buffer; }

    SgInitializedName *getVariableDeclaration() { return declaration; }
    LLVMAstAttributes *getAstAttributes() { return ast_attributes; }
    FunctionAstAttribute *getFunctionAttribute() { return function_attribute; }
    SgArrayType *getArrayType() { return array_type; }


 private:

    SgInitializedName *declaration;
    LLVMAstAttributes *ast_attributes;
    FunctionAstAttribute *function_attribute;
    SgArrayType *array_type;

    std::vector<SgExpression *> dimensionExpressions;
    std::stringstream buffer;
};

#endif


