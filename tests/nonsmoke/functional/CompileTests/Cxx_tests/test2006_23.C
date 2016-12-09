#if 0
compiling the following code with ROSE:
class A {
    virtual void print()const {}
};

class B {
    virtual void print()const {}
};

class C: public A, public B {

    void print()const {}
};


int main()
{
    B* b = new B;
    if(C* startNode=dynamic_cast<C*>(b));
}

gives the following error:
identityTranslator:
/home/saebjornsen1/ROSE-October/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:15799:
SgExpression* sage_gen_expr(an_expr_node*, int): Assertion
"result->get_file_info()->ok()" failed.

Andreas
#endif



