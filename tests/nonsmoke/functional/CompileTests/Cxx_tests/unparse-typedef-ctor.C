
struct Range {};

struct Array {
  Array(Range range) {}
};

typedef Range Index_Range_1;

Array arr_1(Index_Range_1());
Array arr_2((Index_Range_1()));

