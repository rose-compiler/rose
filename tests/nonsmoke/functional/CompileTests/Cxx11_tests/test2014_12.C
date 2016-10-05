
// int get_five(); error
constexpr int get_five();
constexpr int get_five() {return 5;}
 
int some_value[get_five() + 7]; // Create an array of 12 integers. Legal C++11

