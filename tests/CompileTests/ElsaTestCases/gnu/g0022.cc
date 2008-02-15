// addresses of labels and computed goto

// originally found in package procps

// error: Parse error (state 499) at &&

// ERR-MATCH: Parse error .*at &&

int main()
{
    void *a = &&x;
    goto *a;
x:

    int i;
    //ERROR(1): goto *i;     // 'i' is not a pointer

    return 0;
}
