static int INSERT_HERE;                 // a marker used by the injectSnippet test

// This needs to be available.
int tmp;

int
ipoint1()
{
    int x = 1;
    int y = 2;
 // int z = 3;
    INSERT_HERE;
    return x;
}
