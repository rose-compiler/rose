struct A {
    int a;
    int* p;
    float f;
    struct B {
        int b;
    } sB;
};

struct A sA = { .a = 0, .p = 0, .f=0.0, .sB.b = 0};

int arr[5] = {0, 1, [3] = 2, [4]= 5 };

void foo()
{
    struct A sa;
}
