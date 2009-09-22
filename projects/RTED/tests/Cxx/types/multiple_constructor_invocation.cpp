
class Base {
    public:
        Base() {}
};
class Sub : public Base {
    public:
        Sub() { }

        int x[ 3 ];
};



int main() {
    Sub s;
    // simple bounds error
    int y = s.x[ 3 ];
}
