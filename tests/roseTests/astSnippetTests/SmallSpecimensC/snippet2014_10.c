// Declared at global scope because it's needed in more than one snippet
struct Struct1 {
    int int_member;
    char char_member;
    const char *const_string_member;
    char* string_member;
    double double_member;
};

void storeStructString(const char *from)
{
    struct Struct1 storage;
    storage.const_string_member = from;
}

void loadStructString(const char *to, struct Struct1 from)
{
    to = from.const_string_member;
}
