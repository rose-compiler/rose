
extern int x;

void incrementXInOtherFile();

void incrementX()
   {
      x++;
   }

int main()
   {
     incrementX();
     incrementX();
     return x;
   }
