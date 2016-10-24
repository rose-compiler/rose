/*
 __is_union (type)
 If type is a cv union type ([basic.compound]) the trait is true, else it is false.

 */

struct SimpleStruct{
    int i;
};


class SimpleClass{
    int i;
};

union SimpleUnion{
    int i;
    int j;
};


void Foo(){
    // if we have a NOT in if's conditional, then the test expects for false from the triat else expects true
    if(!__is_union(int));
    if(!__is_union(SimpleStruct));
    if(__is_union(SimpleUnion));
    
}