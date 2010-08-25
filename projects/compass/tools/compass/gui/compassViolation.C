#include "rose.h"
#include "compassViolation.h"

CompassViolation::CompassViolation(std::string d, std::string f, int lStart, int lEnd,
       int cStart, int cEnd)
: desc(d), filename(f), lineStart(lStart), lineEnd(lEnd), colStart(cStart), colEnd(cEnd)
{
  
};


CompassViolation::CompassViolation(
  Compass::OutputViolationBase *outputCompassViolation ) 
{
   // we collect all information we need from Compass::OutputCompassViolationBase
   desc = outputCompassViolation->getShortDescription();

   SgLocatedNode* locatedNode = isSgLocatedNode(outputCompassViolation->getNode());

   std::string sourceCodeLocation;
   if (locatedNode != NULL) {
      Sg_File_Info* start = locatedNode->get_startOfConstruct();
      ROSE_ASSERT(start != NULL);
      lineStart = start->get_line();
      colStart = start->get_col();
      filename = start->get_filenameString();
      Sg_File_Info* end   = locatedNode->get_endOfConstruct();
      if (end) {
         lineEnd = end->get_line();
         colEnd = end->get_col();
      } else {
         lineEnd = colEnd = -1;
      }
   } else {
      Sg_File_Info* start = outputCompassViolation->getNode()->get_file_info();
      // tps : Jan 22 2009 changed this because this does not hold for binaries!
      //ROSE_ASSERT(start != NULL);
      if (start) {
        lineStart = start->get_line();
	colStart = start->get_col();
	filename = start->get_filenameString();
	lineEnd = colEnd = -1;
      }
    }
}

CompassViolation::~CompassViolation()
{
}

std::string CompassViolation::getDesc() const {
   return desc;
}

std::string CompassViolation::getFilename() const {
   return filename;
}

int CompassViolation::getLineStart() const {
   return lineStart;
}

int CompassViolation::getLineEnd() const {
   return lineEnd;
}

int CompassViolation::getColStart() const {
   return colStart;
}

int CompassViolation::getColEnd() const {
   return colEnd;
}
