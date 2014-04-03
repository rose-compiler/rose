
namespace io
   {
     class Tokenizer
        {
          public:
               Tokenizer (int x);
        };
   }


class ParserImpl
   {
     public:
          ParserImpl ()
             : tokenizer_(42)
             {
             }

     public:
          io::Tokenizer tokenizer_;

   };



