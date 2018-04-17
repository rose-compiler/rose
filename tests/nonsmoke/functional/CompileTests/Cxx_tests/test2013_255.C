#if 0
namespace XMLURL
   {
     enum Protocols
        {
          File, HTTP, FTP, HTTPS
        };

      const int Protocols_Count = 4;
   }
#else
   #include "test2013_255.h"
#endif

enum XMLCh
   {
     chLatin_f, chLatin_i, chLatin_l, chLatin_e,
     chLatin_h, chLatin_t, chLatin_p, chLatin_s, chNull
   };

struct ProtoEntry
   {
     XMLURL::Protocols   protocol;
     const XMLCh*        prefix;
     unsigned int        defPort;
   };


static const XMLCh  gFileString[] =
{
        chLatin_f, chLatin_i, chLatin_l, chLatin_e, chNull
};

static const XMLCh gFTPString[]  =
{
        chLatin_f, chLatin_t, chLatin_p, chNull
};

static const XMLCh gHTTPString[] =
{
        chLatin_h, chLatin_t, chLatin_t, chLatin_p, chNull
};

static const XMLCh gHTTPSString[] =
{
        chLatin_h, chLatin_t, chLatin_t, chLatin_p, chLatin_s, chNull
};


// Unparsed as: 
// static struct ProtoEntry gProtoList[XMLURL::Protocols_Count] = 
// {
//    {(gFileString), (0)}, 
//    {(gHTTPString), (80)}, 
//    {(gFTPString), (21)}, 
//    {(gHTTPSString), (443)}};

static ProtoEntry gProtoList[XMLURL::Protocols_Count] =
{
        { XMLURL::File     , gFileString    , 0  }
    ,   { XMLURL::HTTP     , gHTTPString    , 80 }
    ,   { XMLURL::FTP      , gFTPString     , 21 }
    ,   { XMLURL::HTTPS    , gHTTPSString   , 443 }
};
