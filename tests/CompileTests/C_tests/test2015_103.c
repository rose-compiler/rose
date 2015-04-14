// This test code demonstrates a bug where the ";" is 
// unparsed after the macro expansion, and AFTER the 
// header file CPP include directive: #include "test2015_103.h"

#define FILE_LICENCE( _sym )	char _sym[0]

// It might be the the source position for this variable declaration is incorrect.
FILE_LICENCE ( BSD2 );

int abc;

#if 1
#include "test2015_103.h"
#else
typedef unsigned char		uint8_t;
#endif

typedef uint8_t u8;


FILE_LICENCE ( BSD2 );


