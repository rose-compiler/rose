namespace NS {
    template<typename T>
    void
    mm() {
        return;
    }
}


void foo() {
    NS::mm<int>();
}
