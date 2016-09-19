class B {
    void mf() {
        auto l4 = [=]() {
            const B* bp = this;
        };
    }
};

int main() {
    
}
