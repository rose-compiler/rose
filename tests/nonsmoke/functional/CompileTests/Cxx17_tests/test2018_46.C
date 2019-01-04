// Selection statements with initializer

typedef int status_code;
int SUCCESS = 1;
int bar();

status_code foo() 
   {   
     if (status_code c = bar(); c != SUCCESS)
        {
          return c;
       // ... 
        }
   }

