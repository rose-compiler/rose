// 7.3.2.cc

namespace Company_with_very_long_name { /*...*/ }
namespace CWVLN = Company_with_very_long_name;
namespace CWVLN = Company_with_very_long_name;      // OK: duplicate
namespace CWVLN = CWVLN;
