
namespace foo
   {
     namespace bar
        {
        }
   }

// This will be unparsed as "bar" instead of "foo::bar"
// using namespace bar;
using namespace foo::bar;

// Commenting this out makes it a little simpler...
// using namespace foo;


