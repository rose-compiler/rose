
// test for marking of compiler generated SgBasicBlock IR nodes.

void foo()
   {
  // Case: if (true);
     if (true);

  // Case: if (true) foo();
     if (true)
        foo();

  // Case: if (false){}
     if (false)
        {
        }
   
  // Case: if (false){} else;
     if (false)
        {
        }
       else;
   
  // Case: if (false){} else {}
     if (false)
        {
        }
       else
        {
        }

  // Case: for(;;);
     for (;;);

  // Case: for(;;) foo();
     for (;;)
        foo();

  // Case: for(;;){}
     for (;;){}

  // Case: for(;;){};
     for (;;){};

  // Case: while(true);
     while(true);

  // Case: while(true){}
     while(true) {};

  // Case: do {} while(true);
     do {} while(true);

  // Case: do foo(); while(true);
     do foo(); while(true);

     int x;

  // Case: switch(x);
     switch(x);

  // Case: switch(x) foo();
     switch(x) foo();

  // Case: switch(x) {}
     switch(x){}

  // Case: switch(x) case 1: {}
     switch(x)
          case 1:
             {
             }

  // Case: switch(x) { case 1:{ break; } case 2: break; case 3: case4: default: }
     switch(x)
        {
          case 1:
             {
               break;
             }
          case 2:
             break;
          case 3:
          case 4:
          default:
// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ >= 3)
             0;
#endif
        }

  // Case: switch(x) { case 1:{ break; } case 2: break; case 3: case4: default:{} }
     switch(x)
        {
          case 1:
             {
               break;
             }
          case 2:
             break;
          case 3:
          case 4:
          default:
           {
           }
        }

  // End of function foo()
   }
