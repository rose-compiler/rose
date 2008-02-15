// anonymous inline struct fields

// originally found in package elinks_0.9.3-1

// a.i:4:62: error: field `dummy2' is not a class member

// ERR-MATCH: is not a class member

int main () {
    // invalid, rejected by gcc-3.4.3
    //ERROR(1): int x = (int) &((struct {int dummy1; int dummy2;} *) 0)->dummy2;
    
    
    // all these are invalid
    //ERROR(2): (struct { int x; }*)0;
    //ERROR(3): const_cast<struct { int x; }*>(0);
    //ERROR(4): static_cast<struct { int x; }*>(0);
    //ERROR(5): dynamic_cast<struct { int x; }*>(0);
    //ERROR(6): reinterpret_cast<struct { int x; }*>(0);
}
