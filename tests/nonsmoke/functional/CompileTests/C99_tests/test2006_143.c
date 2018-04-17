
// test for _Pragma in global scope (this fails if in all caps in EDG)
// _Pragma ( "STDC CX_LIMITED_RANGE OFF" );
_Pragma ( "STDC CX_LIMITED_RANGE off" );

// This works in EDG (but is a warning (unrecognized STDC pragma)
_Pragma ( "STDC cx_limited_range OFF" );

// This works as well
_Pragma ( "STDC align(power)" );

// This is an error
// _Pragma ( );

// This is the #pragma equivalent to the _Pragma ( "STDC cx_limited_range off" );
#pragma STDC cx_limited_range off

