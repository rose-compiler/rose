// demonstrates that stack-allocation constructors with multiple
// arguments don't render correctly.
class Goo {
    public:
    int a;
};

class RefBox {
    public:
    int x;                       
    int &yref;                   
    Goo &z;                      

    RefBox(int x0, int &yref0, Goo &z0) : x(x0), yref(yref0), z(z0) {}
};

int main() {
    Goo g;
    g.a = 3;
    int b = 4;
    RefBox r(5, b, g);
}
