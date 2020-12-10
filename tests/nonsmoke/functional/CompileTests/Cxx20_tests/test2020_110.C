template<class T1, class T2>
struct P {
 T1 x1;
 T2 x2;
 friend auto operator<=>(const P&, const P&) = default;
};

