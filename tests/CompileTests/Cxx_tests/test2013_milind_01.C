/*
This test used to fail extract function call normalization, since the
get_type() on SgFunctionCallExpr did not strip TYPEDEF and other
decorators.
*/

int  A(){};
int  * Foo(int a){

        typedef int (*FP)();
        FP fp [] = {A};

        Foo(fp[0]());
}
