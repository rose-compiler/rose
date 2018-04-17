
template<class T>
class X
   {
      T abc;
   };

// This class must be names something different from "X" to be legal C++.
template<int T1, int T2>
class X2
   {
     int abc(int b = T1);
     int xyz(int y = T2);
   };

