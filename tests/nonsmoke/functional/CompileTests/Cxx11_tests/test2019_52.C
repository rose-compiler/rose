
enum E 
   {
  // BUG: this is no longer a bug in the frontend, but it is unparsed as "e=8;" which might not be quite correct.
  // Need to look into this later, but it is likely semantically equivalent.
     e = sizeof( (struct A*)0 )
   };

