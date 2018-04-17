// DQ (1/14/2014): This only fails for C99, but it is redundantly represented as a test code here.
static int vid_read_packet(int x)
   {
     unsigned char block_type;
     switch(1)
        {
          case 2:
               if(0)
                    block_type = 0;
               return 7;

          default:
               return 42;
        }
   }
