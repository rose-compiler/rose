unsigned bit_12() {
    unsigned i, j=1, count=12;
    for (i = 0; i < count; i++)
        j = j << 1;
    return j;
}

int factorial(int n) {
    return n<=1 ? 1 : n * factorial(n-1);
}

int fibonocci(int n) {
    int a=0, b=1, sum=0, i;
    for (i=0; i<n; ++i) {
        int c = a + b;
        a = b;
        b = c;
    }
    return a;
}

int main(){return 0;}
