
const bool FALSE = 5;

enum X_HighlightedItem
{
  X_HLI_None=7
};

void startFile(const int* title,X_HighlightedItem hli=X_HLI_None,bool additionalIndices=FALSE);

void writeGraphInfo(int &ol)
   {
   // Unparsed as: startFile((&ol),FALSE);
      startFile(&ol);
   }

