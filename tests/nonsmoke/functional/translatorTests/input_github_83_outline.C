int main(int argc, char **argv) {
        float foo[10] = { 0.0 };

        #pragma omp target data map(tofrom: foo)
        {
                foo[3] = 4.2;
        }

        return 0;
}
