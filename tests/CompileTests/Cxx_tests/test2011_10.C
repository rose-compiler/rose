/*
On 12/20/10 4:56 PM, Daniel J. Quinlan wrote:
> This summer I implemented global type tables, so in
> general any type should exist only once and be reused
> (any exceptions are likely a bug and I am interested it that). 
Hi Dan,

Here's an instance of two SgFunctionType* instances created in ROSE for exactly the same function type. This is pretty
easily reproduced with the following ROSE translator:

translatorInput.C
---------------------
void foo(bool b)
{
}
---------------------


functionTypeTranslator.C
---------------------
int main(int argc, char** argv)
{
    SgProject* project = frontend(argc, argv);

    vector<SgFunctionDeclaration*> funcs = SageInterface::querySubTree<SgFunctionDeclaration>(project,
V_SgFunctionDeclaration);
    BOOST_FOREACH (SgFunctionDeclaration* func, funcs)
    {
        if (func->get_name() != "foo")
            continue;

        SgTypeBool* boolType = SageBuilder::buildBoolType();
        SgTypeVoid* voidType = SageBuilder::buildVoidType();
        SgFunctionParameterTypeList* argTypes = SageBuilder::buildFunctionParameterTypeList();
        argTypes->get_arguments().push_back(boolType);
        SgFunctionType* functionType = SageBuilder::buildFunctionType(voidType, argTypes);

        //This assertion doesn't trigger because the functions types are equivalent
        ROSE_ASSERT(func->get_type()->unparseToString() == functionType->unparseToString());

        //However, this assertion triggers, because they are two different SgType* instances
        ROSE_ASSERT(func->get_type() == functionType);
    }
}
---------------------

As noted in the translator source code, there are two different instances for the type "void (bool)".

I noticed that the symbol table lookups for functions rely on the assumption that function types are unique (because
they compare type pointers directly when checking for equality). Hence, this example probably subtly breaks a number
of other things.

-George

*/

void foo(bool b)
   {
   }
