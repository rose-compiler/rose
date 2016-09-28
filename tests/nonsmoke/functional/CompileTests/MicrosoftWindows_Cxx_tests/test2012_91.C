template <int count>
class Outer
{
public:
   template <int count_>
   class Inner
   {
   };
};

int main()
{
   Outer<3>::Inner<1> in;
}

// Produces:
// int main()
// {
//    Inner<1> in;
// }

