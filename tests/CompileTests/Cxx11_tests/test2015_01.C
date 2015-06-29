template<class T, unsigned int C, unsigned char... RestD>
class Tensor // : public std::function<T&(unsigned int,decltype(RestD)...)>
   {
     public:
       // Tensor(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<T&(unsigned int,decltype(RestD)...)>(a_input) { }
          Tensor() { }
       // void operator=(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) {((std::function<T&(unsigned int,decltype(RestD)...)>&)(*this)) = a_input;}
   };


// Original code: void consToPrimF(Tensor<double,5> & a_Q)
// Generated code: void consToPrimF(class Tensor< double  , 5 , /* varadic template argument */ ... > &a_Q)
void consToPrimF(Tensor<double,5> & a_Q)
   {
   }
