
class A {
    public:

        double* d;

        double& operator[]( int index ) {
            double& rv = d[ index ];
            return rv;
        }
};


int main() {

    A a;
    a.d = new double[ 2 ];

    a[ 1 ] = 10;
    a[ 2 ] = 42;    // illegal write, bounds error

    return 0;
}
