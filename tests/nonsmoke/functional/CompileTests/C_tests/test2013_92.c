int main() 
   {
     int y, z;

  /* Test 4: dangling else is possible if code is not unparsed correctly */
     if (1)
          if (y == 2)
               z = 7;
            else;
       else
          z = 8;

  return 0;
}
