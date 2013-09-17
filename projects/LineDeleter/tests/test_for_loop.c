typedef union UInt64{
    double *b;
}UInt64;

double uInt64_to_double (UInt64* n)
{
    int i;
    double base = 1.0;
    double sum = 0.0;
    for (i = 0; i < 8; i++)
    {
        sum += base * (double) (n->b[i]);
        base *= 256.0;
    }
    return sum;
}

int main(){
    return 0;
}

