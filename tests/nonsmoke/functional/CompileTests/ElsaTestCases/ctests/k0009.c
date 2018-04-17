// func returning array of function pointers with old-style parm list

// originally found in package eli_4.4.1-1

// a.i:4:1: Parse error (state 312) at int

long (*func(var))[42]
    int var;
{
    return 0;
}
