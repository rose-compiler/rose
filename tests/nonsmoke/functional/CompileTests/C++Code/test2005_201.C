// Examples from: http://gcc.gnu.org/onlinedocs/gcc-3.2.2/gcc/Designated-Inits.html#Designated%20Inits

// Standard C89 requires the elements of an initializer to appear in a fixed order, the same as the order of the elements in the array or structure being initialized.

// In ISO C99 you can give the elements in any order, specifying the array indices or structure field names they apply to, and GNU C allows this as an extension in C89 mode as well. This extension is not implemented in GNU C++.

// To specify an array index, write [index] = before the element value. For example,

     int a[6] = { [4] = 29, [2] = 15 };
     

// is equivalent to

     int a[6] = { 0, 0, 15, 0, 29, 0 };
     

// The index values must be constant expressions, even if the array being initialized is automatic.

// An alternative syntax for this which has been obsolete since GCC 2.5 but GCC still accepts is to write [index] before the element value, with no =.

// To initialize a range of elements to the same value, write [first ... last] = value. This is a GNU extension. For example,

     int widths[] = { [0 ... 9] = 1, [10 ... 99] = 2, [100] = 3 };

// If the value in it has side-effects, the side-effects will happen only once, not for each initialized field by the range initializer.

// Note that the length of the array is the highest value specified plus one.

// In a structure initializer, specify the name of a field to initialize with .fieldname = before the element value. For example, given the following structure,

     struct point { int x, y; };
     

// the following initialization

     struct point p = { .y = yvalue, .x = xvalue };
     

// is equivalent to

     struct point p = { xvalue, yvalue };
     

// Another syntax which has the same meaning, obsolete since GCC 2.5, is fieldname:, as shown here:

     struct point p = { y: yvalue, x: xvalue };
     

// The [index] or .fieldname is known as a designator. You can also use a designator (or the obsolete colon syntax) when initializing a union, to specify which element of the union should be used. For example,

     union foo { int i; double d; };
     
     union foo f = { .d = 4 };
     

// will convert 4 to a double to store it in the union using the second element. By contrast, casting 4 to type union foo would store it into the union as the integer i, since it is an integer. (See Cast to Union.)

// You can combine this technique of naming elements with ordinary C initialization of successive elements. Each initializer element that does not have a designator applies to the next consecutive element of the array or structure. For example,

     int a[6] = { [1] = v1, v2, [4] = v4 };
     

// is equivalent to

     int a[6] = { 0, v1, v2, 0, v4, 0 };
     

// Labeling the elements of an array initializer is especially useful when the indices are characters or belong to an enum type. For example:

     int whitespace[256]
       = { [' '] = 1, ['\t'] = 1, ['\h'] = 1,
           ['\f'] = 1, ['\n'] = 1, ['\r'] = 1 };
     

// You can also write a series of .fieldname and [index] designators before an = to specify a nested subobject to initialize; the list is taken relative to the subobject corresponding to the closest surrounding brace pair. For example, with the struct point declaration above:

     struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };
     

// If the same field is initialized multiple times, it will have value from the last initialization. If any such overridden initialization has side-effect, it is unspecified whether the side-effect happens or not. Currently, gcc will discard them and issue a warning. 
