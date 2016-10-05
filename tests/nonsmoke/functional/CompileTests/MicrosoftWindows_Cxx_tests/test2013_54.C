// Check that we get the statements in the case option blocks in the correct place.
// It would be nice to have an automated test for this, not sure how to build
// that.  This test was added when i noticed that there was a bug on this topic
// that made it almost all the way through our testing, failing only in the 
// data flow anaysis tests (and only in two of the tests).

void
foobar()
   {
     int n;
     switch (n)
        {
          case 0:
               10;

          case 1:
               11;

          default:
               20;
        }
   }
