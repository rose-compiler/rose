
class QCString {};

class TagAnchorInfo
   {
     public:
      // The support for matching declarations checks that the line numbers are the same as the declaration and this is false if the thrid parameter is on another line.
#if 0
         TagAnchorInfo(const QCString &t = QCString()) {}
#else
      // This version will cause an error.
         TagAnchorInfo(
                       const QCString &t = QCString()) {}
#endif
   };

void foobar()
   {
     TagAnchorInfo* tag = new TagAnchorInfo();
   }
