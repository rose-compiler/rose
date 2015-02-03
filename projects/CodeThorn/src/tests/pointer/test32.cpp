struct A
{
    int *p;
    A() : p(0)  { }
    A(int *_p) : p(_p) { }

    A& operator=(const A& other) {
        if(other.p) p = other.p;
        return *this;
    }
};

struct B
{
    int val;
    B() : val(0) { }
    B(int _val) : val(_val) { }
    B operator+(const B& other) {
         val += other.val;
         return *this;
     }
    B& operator=(const B& other) {
        val = other.val;
        return *this;
    }
    friend B operator+(const B& b1, const B& b2);

};

B operator+(const B& b1, const B& b2)
{
    return B(b1.val + b2.val);
}

void foo()
{
    int val;
    int *p = &val;
    A a1(p);
    A a2, a3;
    a2 = a3 = a1;
    B b1, b2(2), b3(3);
    b1 = b2 + b3;
}
