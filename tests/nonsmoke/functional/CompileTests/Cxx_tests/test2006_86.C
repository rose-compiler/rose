/*
Hi Dan,

It seems that the unparser does not handle enums in templates correctly;
specifically unparsing a reference to such an enum value causes an
assertion error.  Attached is a patch and test case for this problem.

Thanks,
-- Peter
*/

template <typename T>
struct foo
{
    enum {
        one,
        two,
        three,
    };
};

int main()
{
     foo<int>::one;
}


