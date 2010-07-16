int& foo(int& a)
{
    return a = 0;
}


int i = 0, j = 0, k = 0;
int a[100];
int b[100];

enum Color
{
    red = 1,
    blud,
    yellow
};

void test_if()
{
    if (int i = 0);
    if (++i > j--);
    if ((i += j) > k);
    if (foo(i));
    if (++i);
}

void test_type()
{
    //Color c = 0;
#if 1
    {}
#endif
}

void test_preprocessor()
{
    if(
#if 1
            int i = 0
#endif
      );
}

void test_and_or()
{
    i || j;
    i || j++;
    (i, j) || k;
    i || (j, k);
    (i ? j : k) || i;
    i || (j ? k : i);

    // (i ? (true || i++) : ((t = i, ++i, t) || i++)
    (i || i++) || i++;
    i || (i++ || i++);

    i = (j || k++);
}

void test_index()
{
    a[i++] = b[j++];
    int *p = &a[0];
    i = (p++)[j++];
}

void test_declaration()
{
    int i = j++;
    int iii(j++);
    int ii = (j, k);
    int k = j++ ? 1 : 2;

    if (int i = j++);
    switch(int i = j++);
    while (int i = j++);
#if 1
    for(int i = 0; i < 10; ++i);
    for (int i = j++, k = i++; int t = i;);
    for (; int t = i;);
#endif
    //for (int t = i;;);
    {
        int t = i;
    for (;;);
    }
}

void test_conditional()
{
    (i ? j : k) = 10;
    -(i ? j : k);
    i++ ? j : k;

    (i ? (j, k) : (k, j)) = 10;
    (i ? (j ? i : k) : k) = 10;
    (i ? j : k) ? j : k;

}

void test_logical()
{
    (i = 0) || (j = 0);
    if (i || j || k);
    i = ( j || ( k = (i && (j = 0 ) ) ) );
}

void test0()
{
    /*****************************************/
    i = j = k;
    i = j++, i = j = k;
    (j += (k += 1));
    i += (j += (k += 1));
    i = (k *= 2, j += (k += 1));
}

void test1()
{
    /*****************************************/
    (++i, j) = k;
    (++i) = ++j;
}

void test2()
{

    /*****************************************/
    (i += 1) + (j += 1);
    if (1)
        i++ + j++;
}

void test3()
{
    /*****************************************/
    i = ++j;
    i = j++;
}

void test4()
{
    /*****************************************/
    ++++i;
    ++++++++i;
}

void test5()
{
    /*****************************************/
    i = (j++) * 2;
    j = i + ++i;
    j = i++ + i;
    (j = i, j = ++i);
}

void test6()
{
    /*****************************************/
    i = (j++, (k++, k = 4));
}

void test7()
{
    /*****************************************/
    if (i = j);
    if (i == (j = k));
    if (i = j++);
    if (int i = j++);
}

void test8()
{
    /*****************************************/
    i = (i ? ++j : k++);
}

void test9()
{
    /*****************************************/
    ((i += 1, i += 2), (i += 3, i += 4));
}

void test10()
{
    /*****************************************/
    (i = j) || (j = k = i);
    (i = j) + (j = k = i);
    k = (i++ || j++);
}

void test11()
{
    // Unary test.
    ++(i = j);
    !(i = j);
}

void test12()
{
    // Logical or and and
    (i += j) || (j += k);
}

void test13()
{
    // Function calls.
    //i = foo(j);
    //++foo(j);

    // In the following expression, i is undefined.
    a[i++] = i;
}


void foo() { int i = 2; int j = i++;}
