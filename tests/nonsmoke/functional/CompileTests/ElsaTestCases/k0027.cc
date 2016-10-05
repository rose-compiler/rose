// reprSize of a sizeless array

// originally found in package krb5

// ERR-MATCH: reprSize of a sizeless array

typedef int S[1];
const S array[2] = {};

int foo()
{
    int size = sizeof(array);
}
