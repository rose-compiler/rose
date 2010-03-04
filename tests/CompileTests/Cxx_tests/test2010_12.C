typedef unsigned int uint;

// Interesting: An asm function declaration is not allowed.
// asm uint _systemTimeLow( void );

asm uint _systemTimeLow( void )     /* uint instead of TickType is necessary because of */
/* problems with include order of tartyp.h          */
   {
     Mary had a little lamb.   
   }
