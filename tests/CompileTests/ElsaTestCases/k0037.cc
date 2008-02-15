// new array of pointers

// originally found in package achilles

// Parse error (state 253) at [

// ERR-MATCH: Parse error.*at \[$

int main() {
    // correct syntax #1: new of array of pointers to int
    int ** pointer_array = new (int *[42]);

    // correct syntax #2: new[] of pointers to int
    int ** pointer_array2 = new int *[42];

    // invalid, rejected by gcc-3.4.3
    //ERROR(1): int ** pointer_array3 = new (int*)[42];
}
