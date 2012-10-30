// This is a more complex switch statment (closer to duff's device).
// This code is extracted from test2012_132.c taken from postfix 
// C application: file: memcache_proto.c

void foobar()
   {
     int x;
     switch (x)
        {
          case '\r':
             if (0)
                {
                }
               else
                {
                  default:
                     return (-1);
                }

          case '\n':
               return (0);
        }
   }

