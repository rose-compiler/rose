
namespace std
   {
     class string {};
   }

template <class T> void linearIn(T& a_outputT, const void* const inBuf);
// template <class T> void linearIn(T& a_outputT);

template <> void linearIn(std::string& a_outputT, const void* const a_inBuf);
// template <> void linearIn(std::string& a_outputT);

#if 1
template <>
void linearIn<std::string>(std::string& a_outputT, const void* const a_inBuf)
   {
#if 0
     int* intBuffer = (int*)a_inBuf;
     int length = intBuffer[0];
     if (length > 0)
        {
          const char* charBuffer = (const char*)(&intBuffer[1]);
       // a_outputT.assign(charBuffer, length);
        }
    // else a_outputT = "";
#endif
   }
#endif
