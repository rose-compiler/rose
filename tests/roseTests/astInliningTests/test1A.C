int x = 1;

void incrementXInOtherFile();

void incrementX()
   {
      x++;
   }

int main()
   {
     incrementX();
     incrementXInOtherFile();
     return 0;
   }
