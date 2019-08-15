extern "C" {
    void f();             // C linkage
    extern "C++" {
        void g();         // C++ linkage
        extern "C" void h(); // C linkage
        void g2();        // C++ linkage
    }
    extern "C++" void k();// C++ linkage
    void m();             // C linkage
}

