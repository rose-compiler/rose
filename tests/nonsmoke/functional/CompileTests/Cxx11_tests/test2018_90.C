
class duration {};

duration operator/ ( duration & __d, int  __s );

void func1()
   {
     duration var1;
     duration var2;
     int      var3;
     var1 = var2 / ( var3 - 1 );
   }

