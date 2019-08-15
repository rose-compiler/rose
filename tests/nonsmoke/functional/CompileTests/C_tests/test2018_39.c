static void foobar( int i) /*  block scope (set to 19 from call above) */
   {

  // union_tag, in prototype scope, disappears 
  // void nonexist(union union_tag *junk);

  // re-declares union_tag
     i = sizeof(struct union_tag { int stuff; });

        {
          struct union_tag s = {1};
        }


   }
