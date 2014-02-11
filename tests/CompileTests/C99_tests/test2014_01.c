// DQ (1/14/2014): This only fails for C99, but it is redundantly represented as a test code here.

// This test code generates the incorrect AST where the statement in the default is put into the first case.
// the more general case of this bug is that all of the statements, across all of the cases, are put
// into the first case.  This is a bug in ROSE processing of ffmpeg.

int vid_read_packet(int x)
   {
     int block_type;
     switch(1)
        {
          case 2:
            // This if statement is required to demonstrate the bug.
               if(0)
                    block_type = 0;
               return 7;

          case 3:
               return 12;

          default:
               return 42;
        }
   }
