// Example from Andreas

#undef unix

#define unix

// This is not allowed (since it is a predefined macro)
// #undef __cplusplus

int main()
   {
     return 0;
   };

