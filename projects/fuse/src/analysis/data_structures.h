#pragma once
#include "abstract_object.h"

namespace fuse {
class EqualFunctor {
  public: 
    virtual bool operator()(AbstractObjectPtr obj1, AbstractObjectPtr obj2, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis)=0;// { return * obj1 == * obj2; };
  };
  
  class MayEqualFunctor : public EqualFunctor {
  protected:
    bool mayEqual(AbstractObjectPtr objPtr1, AbstractObjectPtr objPtr2, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);

  public:
    virtual bool operator()(AbstractObjectPtr obj1, AbstractObjectPtr obj2, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis) { 
      // Using may equal
      return mayEqual(obj1, obj2, pedge, comp, analysis); 
    };
  };

  class MustEqualFunctor : public EqualFunctor {
  protected:
    bool mustEqual(AbstractObjectPtr objPtr1, AbstractObjectPtr objPtr2, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
  
  public:
    virtual bool operator()(AbstractObjectPtr obj1, AbstractObjectPtr obj2, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis) { 
      // Using must equal
      return mustEqual(obj1, obj2, pedge, comp, analysis); 
    };
  };
    
  typedef boost::shared_ptr<EqualFunctor> EqualFunctorPtr;
}; // namespace fuse

