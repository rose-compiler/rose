// #include directives don't work yet, so declare the stuff we need. None of
// this stuff needs to be injected (because we'd inject the #include directvies
// instead if that actually worked).
void assert(int);
void free(void*);
void* malloc(unsigned long);
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, unsigned, unsigned long);
unsigned long strlen(const char*);

// Snippet support declarations should be in a header file which is not injected
void snippet_inject(const char*, ...);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The rest of the file is snippets
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notNull(const void *x) 
{
    assert(x != (const void*)0);
}

void checkedCopy(void *dst, void *src, unsigned nbytes)
{
    notNull(dst);
    notNull(src);
    if (dst != src)
        memcpy(dst, src, nbytes);
}

void storeHeapString(const char *from)
{
    char *storage;
    unsigned nbytes;

    nbytes = strlen(from) + 1;
    storage = malloc(nbytes);
    checkedCopy(storage, from, nbytes);
}

void loadHeapString(char *to, char *from)
{
    unsigned nbytes;

    nbytes = strlen(from) + 1;
    checkedCopy(to, from, nbytes);
    free(from);
}

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
