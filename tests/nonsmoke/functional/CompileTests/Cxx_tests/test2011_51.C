
namespace X
   {
     class Y
        {
        };
   }

class Z
   {
     X::Y foo();
     X::Y foobar()
        {
        }
   };

X::Y Z::foo()
{
}

