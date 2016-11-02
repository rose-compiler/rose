static int INSERT_HERE;                 // a marker used by the injectSnippet test

// This function declaration must be present in order to insert a function call that would reference this function. 
// void fputs(v1, stderr);
// typedef int FILE;
// struct __FILE {};
// typedef struct __FILE FILE;
// int fputs(const char *s, FILE *stream);
// int *stderr;

int
ipoint1()
{
    int x = 1;
    INSERT_HERE;
    return 0;
}

#if 0
char *
ipoint2()
{
    char *s1, *s2, *s3;
    s1 = strdup("Hello, world!");
    s2 = strdup("Goodbye, cruel world.\n");
    s3 = malloc(strlen(s1) + strlen(s2) + 1);

    INSERT_HERE;
    strcpy(s3, s1);
    strcat(s3, s2);
    return s3;
}
#endif
