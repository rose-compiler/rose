namespace XXX {
   void foobar(const int def[3]); // (const int def[3]);
} // namespace XXX

namespace XXX {
// namespace {
namespace Y {

const unsigned size = 3;

struct AAA
   {
     bool isPeriodic;
   };

} // unnamed namespace, back to XXX

#if 0
   void foobar(const int def[3]);
#else
   void foobar(const int def[Y::size]);
#endif
#if 0
   {
     Y::AAA abc;
  // abc.isPeriodic = false;
   }
#endif
} // namespace XXX
