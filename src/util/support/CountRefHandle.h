
#ifndef COUNT_REF_HANDLE
#define COUNT_REF_HANDLE

#include <stdlib.h>

template  <class T>
class CountRefHandle 
{
   T *obj;
   int *count;

   void Init() { count = new int; *count = 1; }
   void IncreaseUse() { if (count != 0) (*count)++; }
   void DecreaseUse() 
       { 
         if (count == 0);
         else if ((*count) > 1) (*count)--; 
         else {
           delete count;
           delete obj;
           count = 0;
           obj = 0; 
         }
       }
   int RefCount() { return (count == 0)? 0 : *count; }
  
 protected:
   const T* ConstPtr() const { return obj;}
   T* UpdatePtr()
    { if (RefCount() > 1) {
        DecreaseUse();
        obj = obj->Clone();
        Init();
      }
      return obj;
     }

   const T& ConstRef() const { return *obj; }
   T& UpdateRef() { return *UpdatePtr(); }

 public:
   CountRefHandle <T> () {obj = 0; count = 0; }
   CountRefHandle <T> (const T &t) { obj = t.Clone(); Init(); }
   CountRefHandle<T>(T* t) { obj = t; Init(); }
   CountRefHandle <T> ( const CountRefHandle <T> &that) 
      { obj = that.obj; count = that.count; IncreaseUse(); }
   CountRefHandle<T> & operator = ( const CountRefHandle <T> &that)
      { 
        DecreaseUse(); 
        obj = that.obj; count = that.count;
        IncreaseUse(); return *this; 
      }
   ~CountRefHandle <T> () 
        {  DecreaseUse(); }
   void Reset() { DecreaseUse(); obj = 0; count = 0; }
   void Reset( T* t) { obj = t; Init(); }

   bool operator ==( const CountRefHandle <T> &that) const
     { return obj == that.obj; }
};  

#endif
