#ifndef COMPASS_GUI_VIOLATION_H
#define COMPASS_GUI_VIOLATION_H

#include <compass.h>
#include <string>

class CompassViolation
{
public:
   CompassViolation(Compass::OutputViolationBase *outputViolation);
   CompassViolation(std::string d, std::string f, int lStart, int lEnd,
       int cStart, int cEnd);

   ~CompassViolation();

   std::string getDesc() const;
   std::string getFilename() const;
   int getLineStart() const;
   int getLineEnd() const;
   int getColStart() const;
   int getColEnd() const;

  protected:
    std::string desc;         // description
    std::string filename;     // source-file
    int lineStart, lineEnd;  // coordinates
    int colStart, colEnd;
}; //class CompassViolation

#endif
