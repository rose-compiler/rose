/*
Hi Dan,

The attached test case does not pass AstPostProcessing.  This is due to
an AST bug on the destructor's SgCtorInitializerList.  It seems that the
definingDeclaration is set incorrectly on the non-defining declaration
(it points back to itself).

Thanks,
-- Peter
*/

template <typename T>
class foo
{
    public:
    ~foo() {}
};

int main()
{
     foo<int> f;
}

