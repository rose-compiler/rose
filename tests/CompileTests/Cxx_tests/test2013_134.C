
const bool FALSE = 0;

enum X_HighlightedItem
{
  X_HLI_None=0,
};

void startFile(int &ol,const int* name,const int *manName,const int* title,X_HighlightedItem hli=X_HLI_None,bool additionalIndices=FALSE,const char *altSidebarName=0);

void writeGraphInfo(int &ol)
   {
   // Unparsed as: startFile(ol,(&ol),0,(&ol),FALSE,0);
      startFile(ol,&ol,0,&ol);
   }

