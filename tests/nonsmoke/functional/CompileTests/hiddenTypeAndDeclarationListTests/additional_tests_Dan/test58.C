// test for declaration matching: C++ Standard page 219

class B {
        public:
                int f(int);
};

class D : public B {
        public:
                int f(char*); // hides, not overloades, B's f !!!!

};

int main() {

        return 0;

}
