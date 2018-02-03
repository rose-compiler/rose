class Y { };

// nonmember postfix y++
void operator++(Y&, int) { };

void foobar()
   {
     Y y;

  // calls operator++(y, 0)
     y++;

  // explicit call to non-member postfix y++
     operator++(y, 0);
   }


