// tps: the following line causes the frontend to misbehave!!!
// that is totally weird!
//#include <boost/algorithm/string.hpp>
#include "BinQGui.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "icons.h"
#include "folder.xpm"

#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "Clone.h"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;




void clicked1() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  QListWidgetItem *item= instance->listWidget->currentItem(); 
  QString te = item->text();
  string t = te.toStdString();
  //const char *t = (const char *)text;
  cerr << " Selected : " << t << endl;

  if (t=="Andreas's Diff") {
    // this part is to find the added and removed code (from Andreas)
    FindInstructionsVisitor vis;
    scoped_array<scoped_array<size_t> > C;
    vector_start_at_one<SgNode*> insnsA;
    AstQueryNamespace::querySubTree(instance->fileA, std::bind2nd( vis, &insnsA ));
    vector_start_at_one<SgNode*> insnsB;
    AstQueryNamespace::querySubTree(instance->fileB, std::bind2nd( vis, &insnsB ));

    LCSLength(C,insnsA,insnsB);
    std::vector<pair<int,int> > addInstr,minusInst;
    printDiff(C,insnsA, insnsB,insnsA.size(),insnsB.size(),addInstr,minusInst);
    //    cerr << " found adds on left side : " << addInstr.size() << endl;
    //cerr << " found subbs on left side : " << minusInst.size() << endl;
    
    QString res = QString("Found adds:  %1.  Found subbs: %2. ")
      .arg(addInstr.size())
      .arg(minusInst.size());
    instance->analysisResult->append(res);  


    for (unsigned int k=0;k<addInstr.size();++k) {
      std::pair<int,int> p = addInstr[k];
      int a = p.first;
      int b = p.second;
      SgAsmInstruction* instA = isSgAsmInstruction(insnsA[a]);
      SgAsmInstruction* instB = isSgAsmInstruction(insnsB[b]);
#if 0
      cerr << i << " Found ADD in A  (a:" << a <<",b:"<<b<<") : " << endl << 
	"     " << RoseBin_support::HexToString(instA->get_address()) << "  " <<
	instA->get_mnemonic() <<endl <<
	"     " << RoseBin_support::HexToString(instB->get_address()) << "  " <<
	instB->get_mnemonic() <<endl;
#endif

      int myPosA=0;
      int myPosB=0;
      for(size_t i=0; i < instance->itemsFileA.size(); i++ )    {
	SgAsmStatement* stmts = instance->itemsFileA[i]->statement;
	SgAsmInstruction* inst = isSgAsmInstruction(stmts);
	if (inst && inst->get_address()==instA->get_address()) {
	  myPosA=instance->itemsFileA[i]->row;
	  //  instance->itemsFileA[i]->plus=true;
	  instance->itemsFileA[i]->bg=QColor(135,206,255);
	  for (int j=1;j<instance->maxrows;j++)
	    instance->codeTableWidget->setBgColor(instance->itemsFileA[i]->bg,j,i);
	}
      }
      for(size_t i=0; i < instance->itemsFileB.size(); i++ )    {
	SgAsmStatement* stmts = instance->itemsFileB[i]->statement;
	SgAsmInstruction* inst = isSgAsmInstruction(stmts);
	if (inst && inst->get_address()==instB->get_address()) {
	  myPosB=instance->itemsFileB[i]->row;
	  //instance->itemsFileB[i]->plus=true;
	  instance->itemsFileB[i]->bg=QColor(135,206,255);
	  for (int j=1;j<instance->maxrows;j++)
	    instance->codeTableWidget2->setBgColor(instance->itemsFileB[i]->bg,j,i);
	}
      }

      QString res = QString("%1 Found ADD in A  (a:%2,b:%3) (a:%4,b:%5)  %6 %7   %8 %9")
	.arg(k)
	.arg(a)
	.arg(b)
	.arg(myPosA)
	.arg(myPosB)
	.arg(QString(RoseBin_support::HexToString(instA->get_address()).c_str()))
	.arg(QString(instA->get_mnemonic().c_str()))
	.arg(QString(RoseBin_support::HexToString(instB->get_address()).c_str()))
	.arg(QString(instB->get_mnemonic().c_str()));
      instance->analysisResult->append(res);  

    }
  } // andreas diff


  if (t=="Align Functions") {
    
#if 1
  // ------------------------------ Sync statments between itemsFileA amd itemsFileB
  // add padding 
  int position=-1, offset=0, currentPos=0;
  bool fileAPadd = instance->findPosWhenFunctionsAreNotSync(position, offset,currentPos);
  cerr << " 1 Found PosWhenFunctionsAre not sync : " << position << "   offset : " << offset << 
    " A? " << fileAPadd << "  currentPos : " << currentPos << endl;
  //int count=0;
  while (position!=-1) {
    // lets add padding
    if (fileAPadd) {
      Item* oldItem = *(instance->itemsFileA.begin()+position);
      int oldPos = oldItem->pos+oldItem->length;
      cerr << "    A: old item at : " << (position) << "  oldPos : " << oldPos << endl;
      vector<Item*>::iterator it = instance->itemsFileA.begin()+position+1;
      int length=currentPos-oldPos;
      for (int i=0; i<offset;++i) {
	Item* item =NULL;
	if (i==0)
	  item = new Item(false,NULL,0,4,(position+i+1),length,(oldPos));
	else
	  item = new Item(false,NULL,0,4,(position+i+1),0,(oldPos)+length);
	it = instance->itemsFileA.insert(it,item);
	++it;
	cerr << "    A: adding NULL item at : " << (position+i+1) << "  pos : " << (oldPos+i) <<endl;
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
      cerr << "    B: old item at : " << (position) << "  oldPos : " << oldPos << endl;
      vector<Item*>::iterator it = instance->itemsFileB.begin()+position+1;
      int length=currentPos-oldPos;
      for (int i=0; i<offset;++i) {
	Item* item =NULL;
	if (i==0)
	  item = new Item(false,NULL,0,4,(position+i+1),length,(oldPos));
	else
	  item = new Item(false,NULL,0,4,(position+i+1),0,(oldPos)+length);
	it = instance->itemsFileB.insert(it,item);
	++it;
	cerr << "    B: adding NULL item at : " << (position+i+1) << "  pos : " << oldPos+i<<endl;
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
    fileAPadd = instance->findPosWhenFunctionsAreNotSync(position, offset,currentPos);
    cerr << " 2 Found PosWhenFunctionsAre not sync : " << position << "   offset : " << offset << 
      " A? " << fileAPadd << "  currentPos : " << currentPos << endl;
    //    count++;
    //if (count==5) break;
  }

  instance->updateByteItemList();
#endif

  }

} 

// ----------------------------------------------------------------------------------------------
void toolbarClick(int action) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  switch(action){
  case 0:
    instance->run();
    break;
  case 1:
    instance->reset();
    break;
  case 2: 		
    instance->open();     
    break;
  case 3: 		
    QROSE::exit(0); break;
    break;
  default:
    eAssert(0, ("internal error: invalid selection type!"));
  }
}



static void tableWidgetCellActivatedA(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightFunctionRow(oldRow, true);
  instance->highlightFunctionRow(row,true);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void codeTableWidgetCellActivatedA(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightInstructionRow(oldRow, true);
  instance->highlightInstructionRow(row, true);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void tableWidgetCellActivatedB(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightFunctionRow(oldRow, false);
  instance->highlightFunctionRow(row,false);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void codeTableWidgetCellActivatedB(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightInstructionRow(oldRow, false);
  instance->highlightInstructionRow(row, false);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)




// ----------------------------------------------------------------------------------------------


void BinQGUI::highlightFunctionRow(int row, bool fileA) {
  if(row >= 0)    {         
    if (fileA) {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(true);
      tableWidget->setFont(f, 0, row);
      tableWidget->setBgColor(QColor(255,255,0),0,row);
      //showFileA(row);
      SgAsmFunctionDeclaration* func = funcsFileA[row];
      std::vector<Item*>::iterator it = itemsFileA.begin();
      int offset=0;
      for (;it!=itemsFileA.end();++it) {
	Item* item = *it;
	SgAsmStatement* stat = item->statement;
	if (func==isSgAsmFunctionDeclaration(stat)) {
	  offset=item->row;
	  break;
	}
      }
      //cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
      codeTableWidget->setCurrentCell(offset,0);
    } else {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(true);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,0),0,row);
      //      showFileB(row);
      SgAsmFunctionDeclaration* func = funcsFileB[row];
      std::vector<Item*>::iterator it = itemsFileB.begin();
      int offset=0;
      for (;it!=itemsFileB.end();++it) {
	Item* item = *it;
	SgAsmStatement* stat = item->statement;
	if (func==isSgAsmFunctionDeclaration(stat)) {
	  offset=item->row;
	  break;
	}
      }
      //      cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
      codeTableWidget2->setCurrentCell(offset,0);
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightFunctionRow(int row, bool fileA) {
  if (row >= 0)     {
    if (fileA) {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(false);
      tableWidget->setFont(f, 0, row);
      tableWidget->setBgColor(QColor(255,255,255),0,row);
    } else {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(false);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,255),0,row);
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




void BinQGUI::highlightInstructionRow(int row, bool fileA) {
  if(row >= 0)    {         
    if (fileA) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(true);
      codeTableWidget->setCurrentCell(row,0);
      Item* item = itemsFileA[row];
      for (int j=1;j<maxrows;j++) {
	codeTableWidget->setFont(f, j, row);
	if (item->function) 
	  codeTableWidget->setBgColor(QColor(120,120,120),j,row);
	else 
	  codeTableWidget->setBgColor(QColor(255,255,0),j,row);
      }
    } else {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(true);
      codeTableWidget2->setCurrentCell(row,0);
      Item* item = itemsFileB[row];
      for (int j=1;j<maxrows;j++) {
	codeTableWidget2->setFont(f, j, row);
	if (item->function) 
	  codeTableWidget2->setBgColor(QColor(120,120,120),j,row);
	else
	  codeTableWidget2->setBgColor(QColor(255,255,0),j,row);
      }
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightInstructionRow(int row,bool fileA) {
  if (row >= 0)     {
    if (fileA) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(false);
      Item* item = itemsFileA[row];
      for (int j=1;j<maxrows;j++) {      
	codeTableWidget->setFont(f, j, row);
	codeTableWidget->setBgColor(item->bg,j,row);
      }
    } else {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(false);
      Item* item = itemsFileB[row];
      for (int j=1;j<maxrows;j++) {      
	codeTableWidget2->setFont(f, j, row);
	codeTableWidget2->setBgColor(item->bg,j,row);
      }
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




// ----------------------------------------------------------------------------------------------

void BinQGUI::updateByteItemList() {
  // update byteItemList
  for (unsigned int i=0;i<itemsFileA.size();++i) {
    Item* a = itemsFileA[i];
    int pos = a->pos;
    int length = a->length;
    for (int k=0; k<length;++k)
      byteItemFileA[pos+k]=a;
  }
  for (unsigned int i=0;i<itemsFileB.size();++i) {
    Item* b = itemsFileB[i];
    int pos = b->pos;
    int length = b->length;
    for (int k=0; k<length;++k)
      byteItemFileB[pos+k]=b;
  }
  slide->colorize();
  showFileA(0);
  showFileB(0);
  //  codeTableWidget->viewport()->update();
  //codeTableWidget2->viewport()->update();

}

bool 
BinQGUI::findPosWhenFunctionsAreNotSync(int& position, int& offset, int& currentPos) {
  unsigned int max = itemsFileA.size();
  if (itemsFileB.size()>max) 
    max=itemsFileB.size();
  for (unsigned int i=0;i<max;++i) {
    Item* itemA =NULL;
    Item* itemB =NULL;
    if (i<itemsFileA.size())
      itemA=itemsFileA[i];
    if (i<itemsFileB.size())
      itemB=itemsFileB[i];
    if (itemA && itemB) {
      SgAsmFunctionDeclaration* fa = isSgAsmFunctionDeclaration(itemA->statement);
      SgAsmFunctionDeclaration* fb = isSgAsmFunctionDeclaration(itemB->statement);
      if (fa && !fb) {
	// fa occured but fb is further away, need padding for fa
	for (unsigned int k=i;k<itemsFileB.size();++k) {
	    itemB=itemsFileB[k];
	    fb = isSgAsmFunctionDeclaration(itemB->statement);
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
	for (unsigned int k=i;k<itemsFileA.size();++k) {
	    itemA=itemsFileA[k];
	    fa = isSgAsmFunctionDeclaration(itemA->statement);
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

void
BinQGUI::insertFileInformation() {
  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;

  SgAsmInterpretationPtrList& interps = file->get_interpretations();
  SgAsmInterpretationPtrList::iterator it = interps.begin();
  for (;it!=interps.end();++it) {
    SgAsmInterpretation* mainInt = *it;
    SgAsmGenericHeader* header = mainInt->get_header();
    fileInfo->append(header->class_name().c_str());
    SgAsmElfFileHeader* elf = isSgAsmElfFileHeader(header);
    if (elf) {
      fileInfo->append(file->get_name().c_str());
      fileInfo->append(elf->format_name());
      string num = RoseBin_support::ToString(elf->get_e_machine());
      fileInfo->append( QString("Machine: %1")
			.arg(num.c_str()) );
      SgAsmElfSegmentTable *segments = elf->get_segment_table();
      string seg = RoseBin_support::ToString(segments);
      fileInfo->append( QString("ElfSegmentTable: %1")
			.arg(seg.c_str()) );
      SgAsmElfSectionTable *sections = elf->get_section_table();
      string sec = RoseBin_support::ToString(sections);
      fileInfo->append( QString("ElfSectionTable: %1")
			.arg(sec.c_str()) );
    }
  }
  SgAsmGenericFile *genericF = file->get_genericFile() ;
  string size = RoseBin_support::ToString(genericF->get_orig_size());
  fileInfo->append( QString("Orig Size of AsmGenericFile: %1")
		    .arg(size.c_str()) );

  SgAsmGenericHeaderPtrList headers = genericF->get_headers()->get_headers();
  fileInfo->append(QString("HEADERS ------------- %1")
		   .arg(RoseBin_support::ToString(headers.size()).c_str()));	 
  for (unsigned int i=0;i<headers.size();++i) {
    SgAsmGenericHeader* h = headers[i];
    fileInfo->append(h->class_name().c_str());	 
    fileInfo->append(h->format_name());	 
  }

  SgAsmGenericSectionPtrList sectionsList = genericF->get_sections();
  fileInfo->append(QString("SECTIONS ------------- %1")
		   .arg(RoseBin_support::ToString(sectionsList.size()).c_str()));	 
  for (unsigned int i=0;i<sectionsList.size();++i) {
    SgAsmGenericSection* h = sectionsList[i];
    SgAsmElfSection* elfSec = isSgAsmElfSection(h);
    if (elfSec) {
      SgAsmElfSectionTableEntry* entry= elfSec->get_section_entry();
      if (entry) {
	rose_addr_t addr = entry->get_sh_addr();
	rose_addr_t size = entry->get_sh_size();
	string addrS = RoseBin_support::HexToString(addr);
	string sizeS = RoseBin_support::HexToString(size);
	fileInfo->append( QString("%1 Addr: %2  Size: %3")
			  .arg(h->class_name().c_str())
			  .arg(addrS.c_str())
			  .arg(sizeS.c_str()));	 
      }
    } else {
      fileInfo->append( QString("%1")
			.arg(h->class_name().c_str()));
    }
  }
  SgFileContentList fileList = genericF->get_data();
  fileInfo->append(QString("DATA ---------------- %1 ")
		   .arg(fileList.size()));	 
#if 0
  string data="";
  for (unsigned int i=0;i<fileList.size();++i) {
    data.append(RoseBin_support::ToString(fileList[i]));	
  }
  fileInfo->append( data.c_str());	 
#endif
  // doesnt work
  SgFileContentList contentList = genericF->content();
  fileInfo->append(QString("CONTENT ------------- %1 ")
		   .arg(contentList.size()));	 
#if 0
  string content="";
  for (unsigned int i=0;i<contentList.size();++i) {
    data += RoseBin_support::ToString(contentList[i]);	
  }
  fileInfo->append( content.c_str());	 
#endif


}


BinQGUI::BinQGUI(std::string fA, std::string fB ) :  window(0), fileNameA(fA), fileNameB(fB) {
  window = new QRWindow( "mainWindow", QROSE::TopDown );
  binqsupport= new BinQSupport();
  maxrows=5;
  init();
  createGUI();
}

void BinQGUI::init(){
  fileA = binqsupport->disassembleFile(fileNameA);
  fileB = binqsupport->disassembleFile(fileNameB);


  // this part writes the file out to an assembly file
  SgBinaryFile* binaryFileA = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file1 = binaryFileA != NULL ? binaryFileA->get_binaryFile() : NULL;
  SgAsmInterpretation* interpA = SageInterface::getMainInterpretation(file1);

  unparseAsmStatementToFile("unparsedA.s", interpA->get_global_block());

  if(is_directory( fileNameB  ) == false ) {
    SgBinaryFile* binaryFileB = isSgBinaryFile(isSgProject(fileB)->get_fileList()[0]);
    SgAsmFile* file2 = binaryFileB != NULL ? binaryFileB->get_binaryFile() : NULL;
    SgAsmInterpretation* interpB = SageInterface::getMainInterpretation(file2);
    unparseAsmStatementToFile("unparsedB.s", interpB->get_global_block());
  }
  // --------------------------------------------

  itemsFileA.clear();
  itemsFileB.clear();
  
  // ---------------------- Create itemsFileA and itemsFileB , containing all statements
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcsFileA ));
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVis, &funcsFileB ));

  FindStatementsVisitor visStat;
  std::vector<SgAsmStatement*> stmts;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat, &stmts ));
  vector<SgAsmStatement*>::iterator it= stmts.begin();
  int pos=0;
  int row=0;
  for (;it!=stmts.end();++it) {
    Item* item;
    int length=1;
    if (isSgAsmFunctionDeclaration(*it)) {
      FindStatementsVisitor visStat2;
      std::vector<SgAsmStatement*> stmts2;
      AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat2, &stmts2 ));
      int funcsize= stmts2.size();
      item = new Item(true,*it,funcsize,2,row,length, pos);
    } else if (isSgAsmBlock(*it)) {
      continue;
      //item = new Item(false,*it,0,1,row,0);
    } else {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      item = new Item(false,*it,0,0,row,length,pos);
    }
    // color code pushes as an example
    if (isSgAsmx86Instruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      if (isSgAsmx86Instruction(*it)->get_kind() == x86_push)
	item = new Item(false,*it,0,3,row,length,pos);
    }
    row++;
    itemsFileA.push_back(item);
    pos+=length;
  }


  stmts.clear();
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat, &stmts ));
  it= stmts.begin();
  pos=0;
  row=0;
  for (;it!=stmts.end();++it) {
    Item* item;
    int length=1;
    if (isSgAsmFunctionDeclaration(*it)){
      FindStatementsVisitor visStat2;
      std::vector<SgAsmStatement*> stmts2;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat2, &stmts2 ));
      int funcsize= stmts2.size();
      item = new Item(true,*it,funcsize,2,row,length,pos);
    }    else if (isSgAsmBlock(*it)) {
      continue;
      //item = new Item(false,*it,0,1,row,0);
    } else {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      item = new Item(false,*it,0,0,row,length,pos);
    }
    //example
    if (isSgAsmx86Instruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      if (isSgAsmx86Instruction(*it)->get_kind() == x86_push)
	item = new Item(false,*it,0,3,row,length,pos);
    }
    row++;
    itemsFileB.push_back(item);
    pos+=length;
  }

}

  void BinQGUI::createGUI() {
    QDesktopWidget *desktop = QApplication::desktop();


  screenWidth = desktop->width()-50;
  screenHeight = desktop->height()-150;
  if (screenWidth>1424) screenWidth=1424;
  if (screenHeight>1224) screenHeight=1224;


  {
    //--------------------------------------------------------------------------
    QRToolBar *toolbar = (*window)["toolbar"] << new QRToolBar(QROSE::LeftRight, true, true, true);
    // icons are defined in <icons.h>
    toolbar->addButton("run");   toolbar->setPicture(0, iconRun); 
    toolbar->addButton("reset"); toolbar->setPicture(1, iconReset);
    toolbar->addSeparator();
    toolbar->addButton("open"); toolbar->setPicture(2, folder_xpm);
    toolbar->addButton("quit"); toolbar->setPicture(3, iconQuit);
    QROSE::link(toolbar, SIGNAL(clicked(int)), &toolbarClick, this);
  } //window 

  QRPanel &mainPanel = *window << *( new QRPanel(QROSE::TopDown, QROSE::UseSplitter) );
  {
    QRPanel &topPanels = mainPanel << *new QRPanel(QROSE::TopDown, QROSE::UseSplitter);
    {
      QGroupBox *selectGroup =  topPanels <<  new QGroupBox(("Binary File Analysis Information"));
      {
        QGridLayout *echoLayout =  new QGridLayout;
	slide = new Slide(this);
	echoLayout->addWidget(slide, 0, 0 );
        selectGroup->setLayout(echoLayout);

      }
      selectGroup->setFixedHeight(70);
      QRPanel &analysisPanel = topPanels << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	QTabWidget *qtabwidgetL =  analysisPanel << new QTabWidget( );
	listWidget = new QListWidget;
	new QListWidgetItem(("Align Functions"), listWidget);
	new QListWidgetItem(("Andreas's Diff"), listWidget);

	QROSE::link(listWidget, 
		    SIGNAL(itemSelectionChanged()), 
		    &clicked1, this);

	qtabwidgetL->insertTab(0,listWidget,"Analyses");
	
	QTabWidget *qtabwidget =  analysisPanel << new QTabWidget( );
	analysisPanel.setTileSize(50,50);

	analysisResult = new QTextEdit;//new QREdit(QREdit::Box);
	analysisResult->setReadOnly(true);
	analysisResult->setText("Initializing GUI");

	fileInfo = new QTextEdit;//new QREdit(QREdit::Box);
	fileInfo->setReadOnly(true);
	insertFileInformation();

	qtabwidget->insertTab(0,analysisResult,"Analysis Results");
	qtabwidget->insertTab(1,fileInfo,"File Info");
      }
      //      topPanels.setFixedHeight(300);
    }
      
    QRPanel &bottomPanel = mainPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      QRPanel &bottomPanelLeft = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	tableWidget = bottomPanelLeft << new QRTable( 1, "function" );
	QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivatedA, this);
	tableWidget2 = bottomPanelLeft << new QRTable( 1, "function" );
	QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivatedB, this);

	//	bottomPanelLeft.setTileSize(20,20);
      }

      QRPanel &bottomPanelRight = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	codeTableWidget = bottomPanelRight << new QRTable( 7, "row","address","instr","operands","comment","pos","byte" );
	QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);
	codeTableWidget2 = bottomPanelRight << new QRTable( 7, "row","address","instr","operands","comment","pos","byte" );
	QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedB, this);

	//	bottomPanelRight.setTileSize(0,80);
      }
      bottomPanelLeft.setFixedWidth(screenWidth/5 );
      //bottomPanelRight.setFixedWidth(screenWidth/2 );

    } //mainPanel
    mainPanel.setTileSize(30);
  } //window 



  window->setGeometry(0,0,screenWidth,screenHeight);
  window->setTitle("BinQ");
  analysisResult->append("Initializing done.");
  QString res = QString("A: Total functions  %1.  Total statements: %2. ")
    .arg(funcsFileA.size())
    .arg(itemsFileA.size());
  analysisResult->append(res);  
  QString res2 = QString("B: Total functions  %1.  Total statements: %2. ")
    .arg(funcsFileB.size())
    .arg(itemsFileB.size());
  analysisResult->append(res2);  


  updateByteItemList();
  // ------------------------------------


} //BinQGUI::BinQGUI()


void BinQGUI::open() {
#if 0
  char buf[4096] = "\0";
  std::string database = QFileDialog::getOpenFileName( 0, "Open As", getcwd(buf, 4096), "ASCII (*.sql)").toStdString();
  if( database.empty() ) return;
  if(exists(database) == true)    {
    //codeWidget->setReadOnly(true);
    //codeWidget->setPlainText(QString("foobar\nBar\nFoobari3"));
  }
#endif
} //CompassGui::open()

void BinQGUI::reset() {
  init();
  updateByteItemList();
} //CompassGui::open()




void
BinQGUI::run( ) {
  QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));
  QROSE::unlink(tableWidget2, SIGNAL(activated(int, int, int, int)));
  while(tableWidget->rowCount()) 
    tableWidget->removeRow(0);
  while(tableWidget2->rowCount()) 
    tableWidget2->removeRow(0);

  for (size_t row = 0; row < funcsFileA.size(); ++row) {
    tableWidget->addRows(1);
    tableWidget->setText(boost::lexical_cast<std::string>(funcsFileA[row]->get_name()), 0, row);
    //tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.size), 1, row);
    tableWidget->setVDim(row,18);
  }
  for (size_t row = 0; row < funcsFileB.size(); ++row) {
    tableWidget2->addRows(1);
    tableWidget2->setText(boost::lexical_cast<std::string>(funcsFileB[row]->get_name()), 0, row);
    tableWidget2->setVDim(row,18);
  }
  tableWidget->setHAlignment(true, false, 0); // left horizontal alignment
  //tableWidget->setHAlignment(true, false, 1); // left horizontal alignment
  tableWidget->setTextColor(QColor(0,0,255),0);
  tableWidget->setHDim(0,140);
  //tableWidget->setHDim(1,40);
  tableWidget->setShowGrid(false);

  tableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
  tableWidget2->setTextColor(QColor(0,0,255),0);
  tableWidget2->setHDim(0,140);
  //tableWidget2->setHDim(1,40);
  tableWidget2->setShowGrid(false);

  
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivatedA, this);
  QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivatedB, this);
  showFileA(0);
  showFileB(0);
}


void BinQGUI::showFileA(int row) {
  QROSE::unlink(codeTableWidget, SIGNAL(activated(int, int, int, int)));

  while(codeTableWidget->rowCount()) 
    codeTableWidget->removeRow(0);

  ROSE_ASSERT(fileA != NULL);
  ROSE_ASSERT(fileB != NULL);

  QString res = QString("FILE A : Looking at function  %1  row: %2  size ")
    .arg(funcsFileA[row]->get_name().c_str())
    .arg(row);
  //    .arg(elem.size);
  analysisResult->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileA.size(); i++ )    {
    SgAsmStatement* stmts = itemsFileA[i]->statement;
    int length=1;    
    bool addRow=false;
    //posRowA[posC]=-1;
    if (isSgAsmx86Instruction(stmts)) {
      codeTableWidget->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();
      //      if (itemsFileA[i]->plus)
      //	itemsFileA[i]->bg=QColor(135,206,255);

      itemsFileA[i]->bg=QColor(255,255,255);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(0,0,0);
      QColor front = itemsFileA[i]->fg;

      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
      }
      codeTableWidget->setTextColor(front,0,i);
      codeTableWidget->setTextColor(QColor(255,0,0),1,i);
      codeTableWidget->setTextColor(QColor(0,0,255),2,i);
      codeTableWidget->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget->setTextColor(QColor(0,155,0),4,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 2, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) )+", ";
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(opsName), 3, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_comment() ), 4, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
      //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget->addRows(1);
      codeTableWidget->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget->setTextColor(QColor(128,128,128),1,i);
      codeTableWidget->setTextColor(QColor(255,255,0),2,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("***"), 2, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts)) {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(0,0,0);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(255,255,255);
      QColor front = itemsFileA[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
	codeTableWidget->setTextColor(front,j,i);
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("FUNC"), 2, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 3, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      addRow=true;
    } else {
      codeTableWidget->addRows(1);
      /*
      cerr << " FOUND UNEXPECTED NODE " << endl;
      if (stmts)
	cerr << "      NODE IS : " << stmts->class_name() << endl;
      else 
	cerr << "      NODE IS : NULL " << endl;
      */
      itemsFileA[i]->bg=QColor(128,128,128);
      QColor back = itemsFileA[i]->bg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
      }
      if (itemsFileA[i]->row) {
	codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
	codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
	codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      }
      addRow=true;
    }
    if (addRow) {
      codeTableWidget->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 3); // left horizontal alignment
      
      codeTableWidget->setVDim(i,18);
      codeTableWidget->setHDim(0,30);
      codeTableWidget->setHDim(1,80);
      codeTableWidget->setHDim(2,50);
      codeTableWidget->setHDim(3,190);
      codeTableWidget->setHDim(4,110);
      codeTableWidget->setHDim(5,30);
      codeTableWidget->setHDim(6,30);
      
      //posRowA[posC]=rowC;
      //      cerr << "added at pos:" << posC << "  rowC:" << rowC<<endl;
      rowC++;
      posC+=length;
    }
  }

  codeTableWidget->setShowGrid(false);
  codeTableWidget->setCurrentCell(row,0);

  QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);

}


void BinQGUI::showFileB(int row) {
  QROSE::unlink(codeTableWidget2, SIGNAL(activated(int, int, int, int)));

  while(codeTableWidget2->rowCount()) 
    codeTableWidget2->removeRow(0);

  ROSE_ASSERT(fileA != NULL);
  ROSE_ASSERT(fileB != NULL);

  QString res = QString("FILE B : Looking at function  %1  row: %2  size ")
    .arg(funcsFileB[row]->get_name().c_str())
    .arg(row);
  //    .arg(elem.size);
  analysisResult->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileB.size(); i++ )    {
    SgAsmStatement* stmts = itemsFileB[i]->statement;
    int length=1;    
    bool addRow=false;
    //posRowB[posC]=-1;
    if (isSgAsmx86Instruction(stmts)) {
      codeTableWidget2->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();

      itemsFileB[i]->bg=QColor(255,255,255);
      //      if (itemsFileB[i]->plus)
      //	itemsFileB[i]->bg=QColor(135,206,255);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(0,0,0);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
      }
      codeTableWidget2->setTextColor(front,0,i);
      codeTableWidget2->setTextColor(QColor(255,0,0),1,i);
      codeTableWidget2->setTextColor(QColor(0,0,255),2,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),4,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 2, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) )+", ";
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(opsName), 3, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_comment() ), 4, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
      //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget2->addRows(1);
      codeTableWidget2->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget2->setTextColor(QColor(128,128,128),1,i);
      codeTableWidget2->setTextColor(QColor(255,255,0),2,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>("***"), 2, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts)) {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(0,0,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>("FUNC"), 2, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      addRow=true;
    } else {
      codeTableWidget2->addRows(1);
      /*
      cerr << " FOUND UNEXPECTED NODE " << endl;
      if (stmts)
	cerr << "      NODE IS : " << stmts->class_name() << endl;
      else 
	cerr << "      NODE IS : NULL " << endl;
      */
      itemsFileB[i]->bg=QColor(128,128,128);
      QColor back = itemsFileB[i]->bg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
      }

      if (itemsFileB[i]->row) {
	codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
	codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
	codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
    }
      addRow=true;
    }
    if (addRow) {
      codeTableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 3); // left horizontal alignment
      
      codeTableWidget2->setVDim(i,18);
      codeTableWidget2->setHDim(0,30);
      codeTableWidget2->setHDim(1,80);
      codeTableWidget2->setHDim(2,50);
      codeTableWidget2->setHDim(3,190);
      codeTableWidget2->setHDim(4,110);
      codeTableWidget2->setHDim(5,30);
      codeTableWidget2->setHDim(6,30);
      
      //posRowB[posC]=rowC;
      //      cerr << "added at pos:" << posC << "  rowC:" << rowC<<endl;
      rowC++;
      posC+=length;
    }
  }

  codeTableWidget2->setShowGrid(false);
  codeTableWidget2->setCurrentCell(row,0);

  QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedB, this);

}


BinQGUI::~BinQGUI(){
  if( window ) delete window;
} //BinQGUI::~BinQGUI()



