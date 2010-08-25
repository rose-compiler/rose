#ifndef _CREATESLICE_H_
#define _CREATESLICE_H_

/* ! \class CreateSlice

   This class is a traversal which marks elements that should be in a slice.
   It does so by accepting a list of nodes that belong in the slice.
   Currently, it's basically a destructive operation: the ast itself is
   changed to produce the correct slice.

   @todo The way that we create the slice is rather... simple. We completely
   destroy any attendant file info by simply marking everything as compiler
   generated and then marking the nodes of the slice as to be output. It may
   be better in the future to create a second AST containing just the nodes of 
   the slice. */
// #include "rose.h"
#include <set>

class BooleanSafeKeeper
{
	public:
	BooleanSafeKeeper():boolean(false){}
	BooleanSafeKeeper(bool boolVal):boolean(boolVal){};
	bool boolean;
};


class CreateSlice:public AstTopDownBottomUpProcessing < BooleanSafeKeeper,BooleanSafeKeeper>
{

  public:

        CreateSlice(std::set < SgNode * >saveNodes):_toSave(saveNodes)
        {
        }

        // bool traverse(SgNode * node) {return traverse(node, false);}
        bool traverse(SgNode * node)
        {
                currentFile=NULL;
//		return traverse(node,BooleanSafeKeeper(false));
                return AstTopDownBottomUpProcessing <BooleanSafeKeeper,BooleanSafeKeeper>::traverse(node,BooleanSafeKeeper( false)).boolean;
        }
  protected:
        SgFile  * currentFile;

        virtual BooleanSafeKeeper evaluateInheritedAttribute(SgNode * node, BooleanSafeKeeper partOfSlice);

        virtual BooleanSafeKeeper evaluateSynthesizedAttribute(SgNode * node, BooleanSafeKeeper inherited,
                                                               SubTreeSynthesizedAttributes atts);
        BooleanSafeKeeper defaultSynthesizedAttribute(BooleanSafeKeeper inh)
        {
	        // std::cout<<"DEFAULT SYNTHESIZED ATTRIBUTE CONSTRUCTOR CALLED\n";
                return inh;
        }
        std::stack<std::list<SgNode *> > delayedRemoveListStack;
        /*    virtual bool defaultSynthesizedAttribute()
          {
          return false;
          }	
	  virtual bool defaultInheritedAttribute()
	  {
	  return false;
	  } */
  private:

        std::set < SgNode * >_toSave;

};

#endif
