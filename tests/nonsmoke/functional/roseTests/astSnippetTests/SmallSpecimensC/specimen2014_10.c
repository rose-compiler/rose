static int INSERT_HERE;                 // a marker used by the injectSnippet test

#if 0
struct Struct1 {
    int int_member;
    char char_member;
    const char *const_string_member;
    char* string_member;
    double double_member;
};
#endif

int
ipoint1()
{
    int x = 1;
    INSERT_HERE;
    return 0;
}
