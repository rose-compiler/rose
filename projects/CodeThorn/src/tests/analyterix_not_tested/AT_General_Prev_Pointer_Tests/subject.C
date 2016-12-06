struct A1 { void foo1(); };
struct B1 { A1* operator->(); };
struct C1 { B1 operator->(); };
struct D { C1 operator->(); };
int main1()
{
    D d;
    d->foo1();
} 
int foo2()
{
    int *p, **q;
    int vals  = 5;
    p = &vals;
    q = &p;
    vals = *p + **q;
    return vals;
}
int q = 6;

int foo3(int** p)
{
    *p = &q;
    return **p;
}

int main3()
{
    int x, *p, *r;
    int **q;
    q = &p;
    x = 5;
    p = &x;
    r = 0;
    int val = *p + foo3(&p) + *p;
    *(q + 2) = &val;
    *((p = &x) + 2) = 0;
    val = x + (*p = 5);
    *(int*)(p-r) = 5;
    return val;
}
struct A4
{
    int a;
    float b;
};

int foo4()
{
    int *p, **q;
    A4 sA4;
    int vals  = 5;
    float f, *fp;
    f = 4.0;
    fp = &f;
    p = &sA4.a;
    q = &p;
    return *p;
}
struct A5
{
    int a;
    float b;
    A5() : a(0), b(0.0) { }
    A5(int _a, float _b) : a(_a), b(_b) { }
};

int gvar = 4.0;

struct A5 a(4,0.1);

int foo51()
{
    int *p, **q;
    A5 sA5, *sAp;
    int vals  = 5;
    float f, *fp;
    f = 4.0;
    fp = &f;
    p = &sA5.a;
    q = &p;
    sAp = &sA5;
    return 0;
}
struct A6
{
    int a;
    float b;
    A6* next;
};

int foo61()
{

    A6 sA6, *sAp;
    sA6.a = 5;
    sA6.b = 4.0;
    sAp = 0;
    sA6.next = sAp;
    ((sA6.next)+1)->next = sAp;
    return 0;
}
int add(int first, int second)
{
    return first + second;
}
 
int subtract(int first, int second)
{
    return first - second;
}
 
int operation(int first, int second, int (*functocall)(int, int))
{
    return (*functocall)(first, second);
}
 
int main7()
{
    int  a, b;
    int  (*plus)(int, int) = add;
    a = operation(7, 5, plus);
    b = operation(20, a, subtract);
    return 0;
}
struct A8
{
    int a;
    float b;
    A8* next;
};

void bar8(A8&& rval_ref);

int foo81()
{
    bar8(A8());
    A8&& a1 = A8();
    //bar8(a1);
    A8& a2 = *(new A8());
    return 0;
}
void foo9()
{
    int *p, *q, val, **r;
    p = &val;
    q = &(*p);
    r = &(*(&p));
}
void foo10()
{
    int arr[10];
    int arr2[10];
    int *p = arr;
    int *q = &arr[4];
    *(arr + *p) = 5;
    *(arr[2] + arr) = 0;
}
// test defuse querying

int main11()
{
    int a = 0, b = 1, c;
    c = b + (a = 2);
    return a+c;
}
struct A12
{
    int a12;
    int* p12;
    float f12;
} sA12 = { sA12.a12=0, sA12.p12 = 0, sA12.f12=0.0};



int main12()
{
    int array1[10];
    int *p = array1;
    int i;
    array1[i = 0] = 10;
    return array1[0];
}
bool func(bool val)
{
    return val;
}

int main13()
{
    int a, b;
    return func( (a=0) && (b=1) );
}
struct A14
{
    int type;
    int blah;
};

void foo14()
{
    A14 arr[10], arrB[10], a;
    A14 *a_ptr1, *a_ptr2, *x, *y;
    x = arr; y = arrB;
    (a_ptr1 = arr)[2].type = 2;
    (a_ptr2 = x + 4)[2].type = 4;
    (true? a_ptr1 = arr : a_ptr1 = arrB)[2].type = 2;
    (true? a_ptr1 = arr + 10 : a_ptr2 = arrB + 10)[2].type = 2;
    (true? a_ptr1 = x++ : a_ptr1 = x+sizeof(arr))[2].type = 0;
    (true? x : y) = 0;
} 
void foo15()
{
    int arr[10];
    int *p, **q;
    q = &p;
    p = &arr[4];
    q = &(*q);
}
int main16()
{
    int arr[10][10];
    int (*p_arr)[10];
    int i, j;
    i = j = 1;
    p_arr = arr;
    *(*(arr + i) + (j = 2)) = 10;
    return arr[1][1];
}
struct A17;

struct A17
{
    int val;
    A17() { }
};

void bar17(A17 a)
{
    a.val = (int)'b';
}

void foo17()
{
    A17 a, a_next;
    a_next.val = a.val;
    a_next = a;
    bar17(a_next);
}
/* moved into own test:
struct A18;

struct A18
{
    int val;
    A18& next;
    A18() : next(*this) { }
    A18(A18& _next) : next(_next) { }
};

void foo18()
{
    A18 last;
    A18 list(last);
    list.next.val = list.val;
}
*/
struct A19 { void foo19(); };
struct B19 { A19* operator->(); };

int main19()
{
    B19 b;
    b->foo19();
} 
void foo20()
{
    int arr[10], arrB[10], a;
    int *a_ptr1, *a_ptr2, *x, *y, (*arr_ptr)[10];
    x = arr; y = arrB;
    (a_ptr1 = arr)[2] = 2;
    (a_ptr2 = x + 4)[2] = 4;
    (true? a_ptr1 = arr : a_ptr1 = arrB)[2] = 2;
    (true? a_ptr1 = arr + 10 : a_ptr2 = arrB + 10)[2] = 2;
    (true? a_ptr1 = x++ : a_ptr1 = x+sizeof(arr))[2] = 0;
    (true? x : y) = 0;
    a_ptr1 = &((true? a_ptr1 = arr : a_ptr1 = arrB)[2]);
    arr_ptr = &(a_ptr2? arr : arrB);
} 
int foo21()
{
    int a, b1, c1;
//    a = b + c;
//    a = b = 10;
//    a = b + (c = 10);
    a = (b1 = 10) && c1 + 1;
    return a + b1 + c1;
}
void foo22()
{
    int *p, arr[10];
    int i;
    p = arr;
    *(p + (i = 4)) = 12;
    *(p + (i = 4)) = arr[arr[i = 4]];
    arr[i=4] = 19;
}
/* moved into own test:
void foo23()
{
    int arr[10][10], b[10][10], c[10][10];
    int i, j, k;
    arr[i+1][j=0] = b[j][k=1] + c[k+1][j-1];
}
*/
void bar24()
{
    int r1, *q;
    q = &r1;
}

void foo24()
{
    int arr[10], x, y, *p;
    if(true)
        p = &x;
    else
        p = &y;
    arr[*p] = x + y;
}
void foo25()
{
    int i,j,k;
    k++;
    j--;
    (++i)++;
}
struct A26;

struct A26
{
    int val;
    A26() { }
    A26(int _val) : val(_val) { }
};

void foo26()
{
    A26 a, a_next;
    A26 a1(4);
    a_next.val = a.val;
    a_next = a;
}
class A27
{
public:
    int i;
    // moved into own test: static float& f;
};

int& bar27(int *x)
{
    return *x;
}

void foo27()
{
    int *p, *q;
    int a, b;
    int (* arr_p)[10];
    int arr[10][10];
    p = &(a+1, (a+1, b));
    p = &(arr[2][3]);
    arr_p = &(arr[2]);
    void (*f)() = &foo27;
    q = &(*(new int[5]));
    int A27::*i_ptr = &A27::i;
    // moved into own test: float* f_ptr = &A27::f;
    q = &(bar27(p));
    //p = &(*p? a : b);
}
int main28()
{
    int x = 1,y = 2;
    int &x_ref = x;
    int &y_ref = y;
    int *p, *q;

    p = &x_ref; q = &y_ref;
    *p = 5;
    *q = 10;
    return *p + *q;
}
void foo29(int& ref)
{
    int *p = &ref;
    *p = 10;
}

int main29()
{
    int x = 1,y = 2;
    foo29(x); 
    foo29(y);
    return x+y;
}
void foo30(int &x_ref)
{
    int *p = &x_ref;
    *p = 10;
}

void bar30(int &y_ref)
{
    int *q = &y_ref;
    *q = 20;
}

int main30()
{
    int x=2, y=1;
    void (*f_ptr)(int&);
    f_ptr = foo30;
    if(x > y) f_ptr = foo30;
    else      f_ptr = bar30;
    f_ptr(x);
    return x+y;
}
void foo31()
{
  int arr[4];
  int (*p1_arr)[4];
  p1_arr = &arr;
  *p1_arr[3] = 0;
  int **p;
  p = new int*[10];
  p[0] = new int[10];
  p[0][4] = 4;
}

struct A32
{
    int p;
    A32(int _p) : p(_p) { }
};

void foo32()
{
    A32  a(4);
    a.p = 5;
}
struct A33
{
    int *p;
    A33() : p(0)  { }
    A33(int *_p) : p(_p) { }

    A33& operator=(const A33& other) {
        if(other.p) p = other.p;
        return *this;
    }
};

struct B33
{
    int val;
    B33() : val(0) { }
    B33(int _val) : val(_val) { }
    B33 operator+(const B33& other) {
         val += other.val;
         return *this;
     }
    B33& operator=(const B33& other) {
        val = other.val;
        return *this;
    }
    friend B33 operator+(const B33& b1, const B33& b2);

};

B33 operator+(const B33& b1_ref, const B33& b2_ref)
{
    return B33(b1_ref.val + b2_ref.val);
}

void foo33()
{
    int val;
    int *p = &val;
    A33 a1(p);
    A33 a2, a3;
    a2 = a3 = a1;
    B33 b1, b2(2), b3(3);
    b1 = b2 + b3;
}
struct B34 { 
    int _b;

    B34* operator->() {
        return this;
    }
    B34* operator=(const B34& other) {
        _b = other._b;
        return this;
    }

    void foo34() { }
};

int main34()
{
    B34 b, c0;
    b->foo34(); b.foo34();
    b = c0;
} 
struct A35 {
    void foo35() {
    }
};

struct B35 { 
    int _b;
    A35* a_ptr;
    B35() : _b(0), a_ptr(0) { }
    B35(A35* _ap) : a_ptr(_ap) { }

    A35* operator->() {
        return a_ptr;
    }
    void foo35() { 
    }
};

struct C35 {
    int _c;
    void foo35() { }
    C35 operator=(const C35& other) {
        C35 r_c = {other._c};
        return r_c;
    }
};

int main35()
{
    A35 a;
    B35 b, *b_p;
    b_p = &b;
    b->foo35();
    b_p->foo35();
    C35 c1={4}, c2;
    c2 = c1;    
} 
void foo36()
{
    int i, *p, **q;
    float *pf, ifl;
    ++i = 0;
    p = &(++++i);
    p = &(--i);
}
/* Moved to a own test:
int bar37()
{
    return -1;
}

void foo37()
{
    throw;
    throw "hello";
    try {
        throw bar37();
    }
    catch(int val) {
        return;
    }
}
*/
class A38
{
public:
    int val;
    float fval;
public:
    A38() { }
    void foo38() { }
    int bar38() { return 0; }
};

typedef int (A38::*f_ptr)();

int main38()
{
    int A38::*aval_p = &A38::val;
    f_ptr p_bar38 = &A38::bar38;
    A38 a0, *ap;
    int *p = &a0.val;
    a0.*aval_p = 4;
    ap = &a0;
    ap->*aval_p = 5;
    (a0.*p_bar38)();
    (ap->*p_bar38)();
    return a0.val;
}
#include <cstdarg>

double foo39(int a[], ...)
{
    va_list args_list, args_list_copy;
    va_start(args_list, a);
    double v = va_arg(args_list, double);
    va_end(args_list);    
    return v;
}

int bar39(int a[], ...)
{
    int _v;
    va_list args_list;
    va_start(args_list, a);
    _v = va_arg(args_list, int);
    va_end(args_list);
    return _v;    
}

int foo39bar39(int a, ...)
{
    int* _vp, _v;
    va_list args_list, args_list_copy;
    va_start(args_list, a);
    _vp = va_arg(args_list, int*);
    va_copy(args_list_copy, args_list);
    _v = va_arg(args_list_copy, int);
    va_end(args_list);
    return *_vp;
}

int main39()
{
    double f = 10.0;
    int a[5] = {1, 2, 3, 4, 5};
    foo39(a, f);
    int val = 7;
    bar39(a, val);
    return foo39bar39(val, a, val); 
}
/* va_arg example */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

int FindMax (int n, ...)
{
    int i,val,largest;
    va_list vl;
    va_start(vl,n);
    largest=va_arg(vl,int);
    for (i=1;i<n;i++)
    {
        val=va_arg(vl,int);
        largest=(largest>val)?largest:val;
    }
    va_end(vl);
    return largest;
}

int main40 ()
{
    int m;
    m= FindMax (7,702,422,631,834,892,104,772);
    return m;
}
void foo41()
{
    int a, b, i, j;
    (a, b)= (i=0, j=0);
    for(i=0, j=0; i < 10, j < 10; i=j+1, j=i+1) { 
    }
}
struct A42
{
    int **p;
};

typedef struct A42 sA42;
typedef int MYINT;
typedef int MYARR[10];

int foo42()
{
    int **p;
    MYINT m_arr[10][10];
    MYARR my_arr[10];
    int* arr_p[10];
    sA42 sa;
    int arr[10][10];
    p[0] = arr[0];
    arr_p[0] = arr[0];
    p[1][0] = 2;
    sa.p[0] = arr[0];
    arr_p[0][3] = 3;
    (sa.p)[1][2] = 0;
    m_arr[0][2] = 4;
    my_arr[0][0] = 5;
    return arr[1][0] + arr[1][2];
}
typedef struct A43 sA43;
typedef int MYINT;
typedef int MYARR[10];

int foo43()
{

    MYINT m_arr[10][10];
    MYARR my_arr[10];
    m_arr[0][0] = 4;
    my_arr[0][0] = 5;
}
void foo44(int (&arr_ref)[10][10])
{
    arr_ref[0][0] = 1;
}

void bar44()
{
    int arr[10][10];
    foo44(arr);
}

/* Commented out because of template function in include
#include <vector>

struct A45
{
    int _val;
    A45() { }
    A45(int v) : _val(v) { }
};

typedef struct A45 SA;

int main45()
{
    std::vector<SA> savec;
    for(int i = 0; i < savec.size(); ++i) {
        savec[i] = 0;
    }
    return 0;
}
*/

//#include <iostream>
//#include <list>

/* Moved to own test:

// test to skip templates

template <class S>
S myfunc(S s) {
    return s;
}

template <class T>
class TmplClass
{
public:
    T data;
    TmplClass(T _data) : data(_data) { }
    T* getData() {
        T t = myfunc<T>(data);
        return &data;
    }
};

template <>
class TmplClass <char>
{
public:
    char data[10];
    TmplClass(char (&_data)[10]) { 
        for(int i=0; i < sizeof(data); ++i)
            data[i] = _data[i];
    }
};

// copied from cplusplus.com
template <class T>
T* GetMax (T a, T b) {
    T result;
    result = (a>b)? a : b;
    return (&result);
}

int main47()
{
    int i = 10, j = -1;
    int* result = GetMax<int>(i, j);
    TmplClass<int> tmplClass(4);
    tmplClass.getData();
    TmplClass<float> tmplFloat(5.0);
    char mydata[] = { "123456789" };
    TmplClass<char> tmplChar (mydata);
    float* tmplData = tmplFloat.getData();
    return 0;
}*/
