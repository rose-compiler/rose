class Call {};

int main()
   {
     Call* e;

     if (Call* c = dynamic_cast<Call*>(e))
        {
          c = 0L;
        }

     return 0;
   }
