
int main() {

    double* d;
    double*& ref = d;

    ref = new double[ 2 ];

    // out of bounds
    double e = ref[ 3 ];

    return 0;
}
