template<typename... Args>
bool all(Args... args) { return (... && args); }
 
bool b = all(true, true, true, false);
 // within all(), the unary left fold expands as
 //  return ((true && true) && true) && false;
 // b is false

