

void foobar()
   {
     int x;
     if (x)
        {
          goto end;
        }
     x = 6;
// Not allowed to be associated with a label.
// __declspec( selectany )
     end:
     x = 5;
   }
