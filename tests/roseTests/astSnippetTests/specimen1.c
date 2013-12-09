static int INSERT_HERE;                 // a marker used by the injectSnippet test
char *strdup(const char*);
void *malloc(unsigned);
char *strcpy(char*, const char*);
char *strcat(char*, const char*);

int
ipoint1()
{
    int x = 1;
    int y = 2;
    return x;
}

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

void
ipoint3()
{
    double f1 = 3.14;
    double f2 = 299792458.0;
    double f3 = f1 + f2;

    INSERT_HERE;
    return f3;
}


int
main(int argc, char *argv[]) 
{
    return 0;
}
