#define MAX_UNKNOWNS 3

int unknown[MAX_UNKNOWNS];

/* skip some of the tests to focus on others? */
#define SKIP_OPERATORS 0
#define SKIP_POINTERS 0
#define SKIP_CONDITIONS 0

/* naming this function "operators" messes up the ROSE unparser... */
void ops(void)
{
#if !SKIP_OPERATORS
    int a;
    int b;

    a = (unknown[0] ? 23 : 25);
    b = (unknown[1] ? 40 : 42);

    a = a + b;
    b = b - a;
    a = a * b;
    b = b / a;

    a = (b = (unknown[0] ? 4 : 44));

    a += 2;
    a += b;
    a += (b -= 2);

    a *= 2;
    a /= 3;
    b = a % 17;
    a %= 4;

    a = (unknown[0] ? 0x100 : 0x010) | (unknown[1] ? 0x0ff : 0xfff);
    b = (unknown[0] ? 0x101 : 0x0ff) & (unknown[1] ? 0x7f7 : 0x801);
    a = ~(unknown[1] ? 0x0fe : 0x100);
    b = b ^ a;
    a = (-1) & 1;
    a = (-2147483648) & 1;

    a = (unknown[2] ? 0xfe0 : 0xff8);
    b = 2;
    b |= -a;
    a &= -0xf;
    a = -(unknown[2] ? 0xfe0 : 0xff8);
    a &= 0xf;
    b ^= 0xff;

    a = (unknown[0] ? 50 : 55);
    b = (unknown[1] ? 5 : 4);

    a >>= (b >> 1);
    a <<= (b << 1);

    a = (unknown[2] ? 22 : 24);
    b = a--;
    b = --a;
    b = a++;
    b = ++a;

    b = sizeof (int) + sizeof a;

    b = a;
    a = +b;
#endif
}

void pointers(void)
{
#if !SKIP_POINTERS
    int x = 23;
    int y = 42;

    int *p = (unknown[2] ? &x : &y);

    int z = 13;
    int *q = &z;

    int arr[3] = { 0, 1, 2, 3, 4 };
    int *a = arr;

    if (a) a = a; else z = z;

    *q = *p;
    *p = (unknown[0] ? 1 : 0);

    z += *p;
    z += *a++;

    int brr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int *base = brr;
    int *pb = base;
    int *end = base + 10;
    int s = 0;
    while (pb < end)
        s += *pb++;

    pb = (unknown[1] ? base : end);
    if (pb) a = a; else z = z;
#endif
}

void conditions(void)
{
#if !SKIP_CONDITIONS
    /* primary goal of these tests: update variables in conditionals as well
     * as we can */
    int a = (unknown[0] ? 23 : 42);
    int b = (unknown[1] ? 2 : 7);
    int c = (unknown[2] ? 2 : 42);
    int d = 0;
    int e = (unknown[0] ? 0 : 4);

    if (a) a = a; else b = b;

    if (a < b) a = a; else b = b;

    if (a < 30) a = a; else b = b;
    if (30 < a) a = a; else b = b;

    if (b >= 5) a = a; else b = b;
    if (5 <= b) a = a; else b = b;

    if (a + b > 0) a = a; else b = b;

    if (0 < a + b) a = a; else b = b;

    if (a + b > 30) a = a; else b = b;

    if (30 < a + b) a = a; else b = b;

    if (0 == b) a = a; else b = b;

    if (a + 7 == b) a = a; else b = b;

    if (a == c) a = a; else b = b;

    if (d++ > 0) a = a; else b = b;

    if (++d > 1) a = a; else b = b;

    if (e < c) a = a; else b = b;
    if (e <= c) a = a; else b = b;
#endif
}

int glob;
int glob_init = 23;
int glob_array_nosize[] = { 5, 7, 9 };
int glob_array_size[5] = { 100, 200, 300, 400, 500 };

void globals(void)
{
    int a = glob_init;
    int sum = 0;
    int i;
    if (glob_init == 23)
        glob_init++;
    for (i = 0; i < 5; i++)
        sum += glob_array_size[i];
    /* after the loop, sum > 0 and i == 5 must hold -- can we somehow
     * approximate this requirement by inserting branches? */
    if (sum == 0) a = a; else sum = sum;
    if (i > 5) a = a; else sum = sum;
}

void loops(void)
{
    int i = 2;
    int loopsum = 0;

    for (i = 0; i < 55; i++)
        loopsum += i;

#if 1
    for (i = 60; i >= 0; --i)
    {
        i = i;
    }

    while (10 > i++)
        i = i;

    do i -= 3; while (i >= 7);

    while (i-- > 0)
        i = i;
#endif
}


#include <stdio.h>

void print_unknowns(const char *name)
{
    int i;
    printf("interval stress test: test run %-10s ", name);
    for (i = 0; i < MAX_UNKNOWNS; i++)
        printf("%c", '0' + unknown[i]);
    printf("\n");
}

void run(int low, int limit, void (*func)(void), const char *name)
{
    if (low < limit)
    {
        unknown[low] = 0;
        run(low+1, limit, func, name);
        unknown[low] = 1;
        run(low+1, limit, func, name);
    }
    else if (low == limit)
    {
        print_unknowns(name);
        func();
    }
}

int main(void)
{
    run(0, MAX_UNKNOWNS, ops, "operators");
    run(0, MAX_UNKNOWNS, pointers, "pointers");
    run(0, MAX_UNKNOWNS, conditions, "conditions");
    run(0, MAX_UNKNOWNS, globals, "globals");
    run(0, MAX_UNKNOWNS, loops, "loops");
    return 0;
}
