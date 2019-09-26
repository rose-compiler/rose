template <char...a>
int operator "" _a();

// Should be unparsed as: int a = 2_a;
// Is unparsed as: int a = _a <  > ;
int a = 2_a;

