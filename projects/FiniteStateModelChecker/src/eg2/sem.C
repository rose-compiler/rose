class Semaphore
   {

     static int semaphore_count;

     public:
     Semaphore()
	{
	  semaphore_count++;
	}

     ~Semaphore()
	{
	  semaphore_count--;
	}

     operator bool()
	{
	  return true;
	}

   };

int Semaphore::semaphore_count;

int main()
   {
     if ((Semaphore() && Semaphore()) || ((Semaphore() && Semaphore())))
	{
	  return 0;
	}
     return 1;
   }
