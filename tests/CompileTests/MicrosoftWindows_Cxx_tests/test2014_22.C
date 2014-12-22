// Here the declspec attribute is lost.
struct __declspec(uuid("00000000-0000-0000-c000-000000000046")) IUnknown;

#if 0
// This works fine.
struct __declspec(uuid("00000000-0000-0000-c000-000000000046")) IUnknown {};
#endif
