// computed array size expression as an assignment

// originally found in package centericq_4.13.0-2

// (expected-token info not available due to nondeterministic mode)
// a.ii:5:15: Parse error (state -1) at =

// ERR-MATCH: Parse error.*at =$

int main() {
    int i;
    char str[i=4];
    
    // does gcc/icc allow commas too?
    //
    // interesting; gcc does *not*, while icc *does*!
    //
    // I will reject
    //ERROR(1): char str2[4/*ignored*/, 6];
    
    // what about throw?  they both parse it but then reject
    // for tcheck reasons; I will do the same (since it is the
    // behavior that naturally falls out of my implementation)
    //
    // er, I guess my tchecker isn't strict enough
    //char str3[throw 4];

    return 0;
}
