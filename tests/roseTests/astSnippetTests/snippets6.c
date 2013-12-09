// Tests for variable renaming. Any variable whose name begins with "tmp" is renamed
// so it doesn't conflict with other variables that might be visible at the insertion point.
int random_boolean(void);

void randomOffByOne(int arg1)
{
    typedef int typeof_arg1;
    typeof_arg1 offset = random() % 3 - 1;
    arg1 += offset;
}

void addWithError(int addend1, int addend2, int result)
{
    typedef int typeof_result;
    randomOffByOne(addend1);
    randomOffByOne(addend2);
    result = addend1 + addend2;
}


void add(int a, int b, int c) 
{
    int tmp1;
    a = b + c;
}

void mult(int a, int b, int d)
{
    int tmp4;
    a = b + d;
}

void dot(int a, int b1, int b2, int c1, int c2)
{
    int tmp1, tmp2, tmp3;
    mult(tmp1, b1, c2);
    mult(tmp2, b2, c1);
    add(tmp3, tmp1, tmp2);
}
