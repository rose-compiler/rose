enum fmt { V = 0x100 | 0x200 };

void foo(enum fmt f)
   {
     switch (f)
        {
          case V:
             break;
        }
   }

