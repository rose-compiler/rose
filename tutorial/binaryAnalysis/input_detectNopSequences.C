int foobar()
   {
     int result = 0;
     result++;
     return result;
   }


int main()
   {
     int result = 0;
     result++;
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");
     asm ("nop");

     result++;
     return result;
   }
