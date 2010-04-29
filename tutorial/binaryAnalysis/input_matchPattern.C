
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
