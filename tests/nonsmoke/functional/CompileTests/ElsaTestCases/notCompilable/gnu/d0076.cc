// Test "restrict" which I understand is a C99-ism but right now we
// only lex "__restrict__" as a gnu extension so I'll put it here with
// gnu stuff.

int const * const __restrict__ volatile x;
