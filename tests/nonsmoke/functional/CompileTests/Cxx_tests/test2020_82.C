
class A
   {
     public:
          virtual bool isError()
             {
               return true;
             }
   };

class B
   {
     protected:
          virtual bool isError();
   };

class D
   {
     public:
          void foo1()
             {
               abData;
             }

     private:
          int abData;
   };

class E
   {
     public:
          virtual bool isError() = 0;
   };

class F: public E, public B
   {
   };

class G: public F
   {
     public:
          virtual bool bafoo2(void)
             {
               return value;
             }
          virtual bool isError()
             {
            // This is unparsed incorrectly as:
            // return (this) -> F:: isError ();
            // return B::isError();
            // return E::isError();
               return B::isError();
             }

     private:
          bool value;
   };

