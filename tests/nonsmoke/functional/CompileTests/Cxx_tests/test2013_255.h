// We need to place this into a sepeerate file because the 
// aggregate initialization unparsing detects when the initializer
// is from a seperate file and avoids unparsing those entries 
// (because they may come from  #include file).

namespace XMLURL
   {
     enum Protocols
        {
          File, HTTP, FTP, HTTPS
        };

      const int Protocols_Count = 4;
   }
