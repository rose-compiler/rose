int foobar()
   {
     int err;
     int value;
     
     if (err)
          goto fail;

	  switch (value) 
        {
          case 42:
               return 0;

          default:
             {
               int x;
            fail:
               x = 1;
               break;
             }
        }
   }
