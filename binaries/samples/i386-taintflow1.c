int main(int argc, char *argv[]) {
    return 0;
}

int f1(int a) {                                         // a tainted by definition (esp_0+4 for 1 byte)
    int b = a + 1;                                      // b tainted (esp_0-8 for 4 bytes); status-bits tainted
    return b;                                           // retval tainted (eax)
}

int f2(int a) {                                         // a tainted by definition (esp_0+4 for 1 byte)
    int i;                                              // i bottom (esp_0-12 for 4 bytes)
    int acc=0;                                          // acc untainted (esp_0-8 for 4 bytes)

    for (i=1;                                           // i untainted
         i<=a;                                          // status-bits top
         ++i)                                           // i untainted
        acc += i;                                       // acc untainted
    return acc;                                         // retval untainted (eax)
}

int f3(int a,                                           // a tainted by definition (esp_0+4 for 1 byte)
       int b) {                                         // b bottom (esp_0+8 for 4 bytes)
    int c = a + b;                                      // c tainted (esp_0-8)
    return c;                                           // retval tainted (eax)
}

int f4(int a,                                           // a tainted by definition (esp_0+4 for 1 byte)
       int b,                                           // b bottom (esp_0+8 for 4 bytes)
       int c) {                                         // c bottom (esp_0+12 for 4 bytes)
    int d =                                             // d bottom (esp_0-8 for 4 bytes)
        a < 0 ?                                         // status-bits tainted
        b : c;
    return d;                                           // retval bottom (eax)
}

int f5(int a,                                           // a tainted by definition (esp_0+4 for 1 byte)
       int b) {                                         // b bottom (esp_0+8 for 4 bytes)
    int c = 0;                                          // c untainted (esp_0-12 for 4 bytes)
    int d =                                             // d top (esp_0-8 for 4 bytes)
        b < 0 ?                                         // status-bits bottom
        a : c;                                          // (tainted ^ untainted) = top
    return d;                                           // retval top (eax)
}

void f6(int a) {                                        // a tainted by definition (esp_0+4 for 1 byte)
    int b[3];                                           // b[0] bottom; b[1] bottom; b[3] bottom
    b[0] = a;                                           // b[0] tainted (esp_0-16 for 4 bytes)
    b[1] = 0;                                           // b[1] untainted (esp_0-12 for 4 bytes)
    b[2] = b[0];                                        // b[2] tainted (esp_0-8 for 4 bytes)
}

int g1;
int f7(int a) {                                         // a tainted by definition (esp_0+4 for 1 byte)
    int b;                                              // b bottom
    g1 = 0;                                             // g untainted
    b = a;                                              // b tainted; g tainted (iff over approximating)
    return b;                                           // retval tainted (eax)
}

    
