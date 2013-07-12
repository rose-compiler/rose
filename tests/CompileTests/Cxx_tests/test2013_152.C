
#define uint32 unsigned int
#define uint8  unsigned char

// ===================================================================
// emulates google3/util/endian/endian.h
//
// TODO(xiaofeng): PROTOBUF_LITTLE_ENDIAN is unfortunately defined in
// google/protobuf/io/coded_stream.h and therefore can not be used here.
// Maybe move that macro definition here in the furture.
uint32 ghtonl(uint32 x) 
   {
#if 0
     class 
        {
        };
#endif
#if 1
     union
        {
          uint32 result;
          uint8 result_array[4];
        };
#endif
#if 0
     result_array[0] = static_cast<uint8>(x >> 24);
     result_array[1] = static_cast<uint8>((x >> 16) & 0xFF);
     result_array[2] = static_cast<uint8>((x >> 8) & 0xFF);
     result_array[3] = static_cast<uint8>(x & 0xFF);
#endif

     return result;
   }
