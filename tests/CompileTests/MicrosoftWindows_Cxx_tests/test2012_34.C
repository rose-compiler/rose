namespace std
   {
//   template <typename T> bool operator== (T,int);
     namespace B
        {

        }

   }


namespace std
   {
     template <typename T> bool operator== (T,int);
#if 1
     using namespace B;
#endif
   }

#if 1
namespace std
   {
     template <typename T> bool operator== (T,int);
     template <typename T> bool operator== (T,int) {}
     namespace B
        {
        }

     template <typename T1, typename T2> bool operator== (T1,T2);
     template <typename T1, typename T2> bool operator== (T1,T2) {}
     namespace B
        {
        }

   }
#endif

#if 1
namespace std
   {
     template <typename T> bool operator== (T,int);
#if 1
     using namespace B;
#endif
   }
#endif
