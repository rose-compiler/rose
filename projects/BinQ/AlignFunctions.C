#include "rose.h"
#include "BinQGui.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>


#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "AlignFunctions.h"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;

std::string AlignFunction::name() {
  return "Align Functions";
}

bool 
AlignFunction::findPosWhenFunctionsAreNotSync(int& position, int& offset, int& currentPos) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  unsigned int max = instance->itemsFileA.size();
  if (instance->itemsFileB.size()>max) 
    max=instance->itemsFileB.size();
  for (unsigned int i=0;i<max;++i) {
    Item* itemA =NULL;
    Item* itemB =NULL;
    if (i<instance->itemsFileA.size())
      itemA=instance->itemsFileA[i];
    if (i<instance->itemsFileB.size())
      itemB=instance->itemsFileB[i];
    if (itemA && itemB) {
      SgAsmFunction* fa = isSgAsmFunction(itemA->statement);
      SgAsmFunction* fb = isSgAsmFunction(itemB->statement);
      if (fa && !fb) {
	// fa occured but fb is further away, need padding for fa
	for (unsigned int k=i;k<instance->itemsFileB.size();++k) {
	    itemB=instance->itemsFileB[k];
	    fb = isSgAsmFunction(itemB->statement);
	    if (fb) {
	      // We need a padding in B at pos i with length (k-i)
	      position=i-1;
	      offset=k-i;
	      currentPos = itemB->pos;
	      return true; //left (A)
	    }
	}
      } else if (fb && !fa) {
	// fa occured but fb is further away, need padding for fa
	for (unsigned int k=i;k<instance->itemsFileA.size();++k) {
	    itemA=instance->itemsFileA[k];
	    fa = isSgAsmFunction(itemA->statement);
	    if (fa) {
	      // We need a padding in A at pos i with length (k-i)
	      position=i-1;
	      offset=k-i;
	      currentPos = itemA->pos;
	      return false; //right (B)
	    }
	}
      }

    }
  }
  return true; // should not be reached
}

std::string 
AlignFunction::getDescription() {
  return "This Analysis aligns functions of two binaries. It is a simple algorithm that aligns functions based on their first encounter.";
}


void
AlignFunction::run(SgNode* fileA, SgNode* fileB) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();

#if 1
  // ------------------------------ Sync statments between itemsFileA amd itemsFileB
  // add padding 
  int position=-1, offset=0, currentPos=0;
  bool fileAPadd = findPosWhenFunctionsAreNotSync(position, offset,currentPos);
  //cerr << " 1 Found PosWhenFunctionsAre not sync : " << position << "   offset : " << offset << 
  //  " A? " << fileAPadd << "  currentPos : " << currentPos << endl;
  //int count=0;
  while (position!=-1) {
    // lets add padding
    if (fileAPadd) {
      Item* oldItem = *(instance->itemsFileA.begin()+position);
      int oldPos = oldItem->pos+oldItem->length;
      //cerr << "    A: old item at : " << (position) << "  oldPos : " << oldPos << endl;
      vector<Item*>::iterator it = instance->itemsFileA.begin()+position+1;
      int length=currentPos-oldPos;
      for (int i=0; i<offset;++i) {
	Item* item =NULL;
	if (i==0)
	  item = new Item(0,NULL,4,(position+i+1),length,length,(oldPos),"",0);
	else
	  item = new Item(0,NULL,4,(position+i+1),0,0,(oldPos)+length,"",0);
	it = instance->itemsFileA.insert(it,item);
	++it;
	//	cerr << "    A: adding NULL item at : " << (position+i+1) << "  pos : " << (oldPos+i) <<endl;
      }
      // need to adjust the remainder
      int c=1;
      for (; it!=instance->itemsFileA.end();++it) {
	Item* item = (*it);
	//cerr << "    changing row at : " << (item->row) << "  to : " << (position+offset+c) <<endl;
	item->row = position+offset+c;
	item->pos = item->pos+length;
	c++;
      }
    }

    if (!fileAPadd) {
      Item* oldItem = *(instance->itemsFileB.begin()+position);
      int oldPos = oldItem->pos+oldItem->length;
      //cerr << "    B: old item at : " << (position) << "  oldPos : " << oldPos << endl;
      vector<Item*>::iterator it = instance->itemsFileB.begin()+position+1;
      int length=currentPos-oldPos;
      for (int i=0; i<offset;++i) {
	Item* item =NULL;
	if (i==0)
	  item = new Item(0,NULL,4,(position+i+1),length,length,(oldPos),"",0);
	else
	  item = new Item(0,NULL,4,(position+i+1),0,0,(oldPos)+length,"",0);
	it = instance->itemsFileB.insert(it,item);
	++it;
	//cerr << "    B: adding NULL item at : " << (position+i+1) << "  pos : " << oldPos+i<<endl;
      }
      // need to adjust the remainder
      int c=1;
      for (; it!=instance->itemsFileB.end();++it) {
	Item* item = (*it);
	//cerr << "    changing row at : " << (item->row) << "  to : " << (position+offset+c) <<endl;
	item->row = position+offset+c;
	item->pos = item->pos+length;
	c++;
      }
    }
    position=-1;
    offset=0;
    currentPos=0;
    fileAPadd = findPosWhenFunctionsAreNotSync(position, offset,currentPos);
    //cerr << " 2 Found PosWhenFunctionsAre not sync : " << position << "   offset : " << offset << 
    //  " A? " << fileAPadd << "  currentPos : " << currentPos << endl;
    //    count++;
    //if (count==5) break;
  }

  instance->updateByteItemList();
#endif

}

void
AlignFunction::test(SgNode* fileA, SgNode* fileB) {

#if 0
  // ------------------------------ Sync statments between itemsFileA amd itemsFileB
  // add padding 
  int position=-1, offset=0, currentPos=0;
  bool fileAPadd = findPosWhenFunctionsAreNotSync(position, offset,currentPos);
  //cerr << " 1 Found PosWhenFunctionsAre not sync : " << position << "   offset : " << offset << 
  //  " A? " << fileAPadd << "  currentPos : " << currentPos << endl;
  //int count=0;
  while (position!=-1) {
    // lets add padding
    if (fileAPadd) {
      Item* oldItem = *(itemsFileA.begin()+position);
      int oldPos = oldItem->pos+oldItem->length;
      //cerr << "    A: old item at : " << (position) << "  oldPos : " << oldPos << endl;
      vector<Item*>::iterator it = itemsFileA.begin()+position+1;
      int length=currentPos-oldPos;
      for (int i=0; i<offset;++i) {
	Item* item =NULL;
	if (i==0)
	  item = new Item(0,NULL,4,(position+i+1),length,length,(oldPos),"",0);
	else
	  item = new Item(0,NULL,4,(position+i+1),0,0,(oldPos)+length,"",0);
	it = itemsFileA.insert(it,item);
	++it;
	//	cerr << "    A: adding NULL item at : " << (position+i+1) << "  pos : " << (oldPos+i) <<endl;
      }
      // need to adjust the remainder
      int c=1;
      for (; it!=itemsFileA.end();++it) {
	Item* item = (*it);
	//cerr << "    changing row at : " << (item->row) << "  to : " << (position+offset+c) <<endl;
	item->row = position+offset+c;
	item->pos = item->pos+length;
	c++;
      }
    }

    if (!fileAPadd) {
      Item* oldItem = *(itemsFileB.begin()+position);
      int oldPos = oldItem->pos+oldItem->length;
      //cerr << "    B: old item at : " << (position) << "  oldPos : " << oldPos << endl;
      vector<Item*>::iterator it = itemsFileB.begin()+position+1;
      int length=currentPos-oldPos;
      for (int i=0; i<offset;++i) {
	Item* item =NULL;
	if (i==0)
	  item = new Item(0,NULL,4,(position+i+1),length,length,(oldPos),"",0);
	else
	  item = new Item(0,NULL,4,(position+i+1),0,0,(oldPos)+length,"",0);
	it = itemsFileB.insert(it,item);
	++it;
	//cerr << "    B: adding NULL item at : " << (position+i+1) << "  pos : " << oldPos+i<<endl;
      }
      // need to adjust the remainder
      int c=1;
      for (; it!=itemsFileB.end();++it) {
	Item* item = (*it);
	//cerr << "    changing row at : " << (item->row) << "  to : " << (position+offset+c) <<endl;
	item->row = position+offset+c;
	item->pos = item->pos+length;
	c++;
      }
    }
    position=-1;
    offset=0;
    currentPos=0;
    fileAPadd = findPosWhenFunctionsAreNotSync(position, offset,currentPos);
    //cerr << " 2 Found PosWhenFunctionsAre not sync : " << position << "   offset : " << offset << 
    // " A? " << fileAPadd << "  currentPos : " << currentPos << endl;
    //    count++;
    //if (count==5) break;
  }


#endif

}

