// apparently this is legal C (at least gcc -pedantic accepts it)

// originally found in package e2fsprogs

// ERR-MATCH: Parse error.*at long$

typedef int fooint;

// sm: I copied this test to in/gnu/dC0014.c; now the
// present test is to ensure this syntax is *not* allowed
// in ANSI mode
//ERROR(1): typedef fooint long /*omitted*/;
//ERROR(2): typedef fooint long blah;
