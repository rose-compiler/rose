
int main() {
    class A { };
    
    A  a1, a2;
    auto l = [=]() mutable {
        A* ap2 = &a2;
        a1;
    };
    
    A a4, a5;
    auto l2 = [&]() {
        A* ap5 = &a5;
        a4;
    };
    
    class B {
        void mf() {
            auto l4 = [=]() {
                this;
            };
            auto l5 = [&]() {
                this;
            };
            auto l6 = [this]() {
                this;
            };
        }
    };
    
}

