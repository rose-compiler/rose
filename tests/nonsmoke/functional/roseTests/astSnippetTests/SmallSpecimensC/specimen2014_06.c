static int INSERT_HERE;                 // a marker used by the injectSnippet test

// This function declaration must be present in order to insert a function call that would reference this function. 
// void fputs(v1, stderr);
// typedef int FILE;
// int fputs(const char *s, FILE *stream);
// int *stderr;

int
ipoint1()
{
    int x = 1;
    INSERT_HERE;
    return 0;
}

int main()
   {
     return 0;
   }
