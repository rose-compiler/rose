#ifndef ALIAS_ANALYSIS
#define ALIAS_ANALYSIS

class SgExpression;

namespace CodeThorn {

  class AliasAnalysis {
  public:
    AliasAnalysis();
    virtual void initialize()=0;
    virtual void run()=0;
    
    // two expressions that may refer to the same memory location.
    // an analysis that cannot determine results for some provided pair of expressions this function should return true;
    virtual bool isMayAlias(SgExpression*, SgExpression*)=0;
    
    // two expressions that definitely refer to the same memory location.
    // for an analysis that cannot determine must analysis results this function always returns false.
    virtual bool isMustAlias(SgExpression*, SgExpression*)=0;
    
    virtual ~AliasAnalysis();
  };
  
} // end namespace CodeThorn

#endif
