// This example shows how seeds may be specified to
// be either expression, statements, or lists of statements.
// This is a way to tailor the seeding process and make 
// it more complex it a well defined (specifiable) way.

void foobar(int x)
   {
     int n;
     int array[10];

#pragma expression-seed
     n;

#pragma statement-seed
     n+1;

#pragma statement-seed
     {
       foobar(n);
     }

#pragma statement-list-seed
     {
       foobar(n);
       n+=1;
       foobar(n+1);
     }

  // Use the expression seed to transform the seeding of the buffer overflow security flaw.
     array[0] = 0;
   }
