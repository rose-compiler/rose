// ambiguous overload "char* const*" vs "char const* const*"

// originally found in package aconnectgui

// ERR-MATCH: ambiguous overload

struct S1
{
    S1(char * const * D)
    {
    }
    S1(char const * const * D)
    {
    }
};

int func(char * const * D)
{
}
int func(char const * const * D)
{
}

char **strings;

int main() {
    func(strings);

    S1 s1(strings);
}
