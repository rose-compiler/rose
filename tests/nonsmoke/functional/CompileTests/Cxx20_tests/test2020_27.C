void f1(int i)
{
    int const N = 20;
    auto m1 = [=] {
            int const M = 30;
            auto m2 = [i] {
                    int x[N][M]; // N and M are not odr-used 
                                 // (ok that they are not captured)
                    x[0][0] = i; // i is explicitly captured by m2
                                 // and implicitly captured by m1
            };
    };
 
    struct s1 // local class within f1()
    {
        int f;
        void work(int n) // non-static member function
        {
            int m = n * n;
            int j = 40;
            auto m3 = [this, m] {
                auto m4 = [&, j] { // error: j is not captured by m3
                        int x = n; // error: n is implicitly captured by m4
                                   // but not captured by m3
                        x += m;    // ok: m is implicitly captured by m4
                                   // and explicitly captured by m3
                        x += i;    // error: i is outside of the reaching scope
                                   // (which ends at work())
                        x += f;    // ok: this is captured implicitly by m4
                                   // and explicitly captured by m3
                };
            };
        }
    };
}


