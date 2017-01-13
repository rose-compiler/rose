
void foo()
   {
     int x = 0;
  // Unparses as: for (int i = 0; i < 10; 0x2b1a8d271010::i++) 
     for (int i = 0; i < 10; i++) 
        {
           x = 1;
        }
   }
