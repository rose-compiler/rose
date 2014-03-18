/* Basic insertion test.  The three declarations should be inserted at the beginning of the insertionPoint scope and the three
 * other statements should be inserted right after the insertionPoint statement. */
void
basic() 
{
    int a;
    int b=0;
    int c=1, d=2;
    a = b + c;
    ++a;
    a += d;
}

/* Tests that arguments can be substituted. */
void
decrement(int a)
{
    --a;
}

/* Test two arguments */
void
swap(int a, int b)
{
    int tmp = a;
    a = b;
    b = tmp;
}
