#ifndef AGGREGATE_AST_ATTRIBUTE
#define AGGREGATE_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class AggregateAstAttribute : public RootAstAttribute { 
public: 
    AggregateAstAttribute(SgAggregateInitializer *aggregate_) : aggregate_init(aggregate_), type(NULL) {} 
    AggregateAstAttribute(SgArrayType *array_type_) : aggregate_init(NULL), type(array_type_) {} 
    AggregateAstAttribute(SgClassType *class_type_) : aggregate_init(NULL), type(class_type_) {} 

    AggregateAstAttribute(AggregateAstAttribute *a) : aggregate_init(a -> aggregate_init), type(a -> type) {} 

    SgAggregateInitializer *getAggregate() { return aggregate_init; }
    SgArrayType *getArrayType() { 
        return isSgArrayType(type);
    }
    SgClassType *getClassType() {
        return isSgClassType(type);
    }

private:

    SgAggregateInitializer *aggregate_init;
    SgType *type;
};

#endif
