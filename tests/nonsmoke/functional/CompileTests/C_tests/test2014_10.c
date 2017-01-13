// Example code from busy-box, use of enum in switch body before first case statement.

void foo()
   {
     int x;

#if 0
  // Alternative fix for this in the source code.
     enum { CWD_LINK, EXE_LINK };
#endif

     switch (x)
        {
#if 1
       // Note enum type declaration at top of switch before first case statement.
          enum { CWD_LINK, EXE_LINK };
#endif
          case CWD_LINK:
               break;

          case EXE_LINK:
               break;

          default:
               break;
        }
   }
