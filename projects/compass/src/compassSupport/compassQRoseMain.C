#include "compass.h"
#include "checkers.h"
#include "qrose.h"

using namespace std;
using namespace rose;

void computeResultTable();


// Global arrays
vector<const Compass::Checker*> compassTestArray;
// vector<Compass::FactoryBase*> compassFactoryArray;

SgProject* sageProject = NULL;



// Note this can't be a local variable since I can't access it using QRGUI::getWidget<QRTable>("Compass Results");
// QRTable* compassResultsTable = NULL;

// Function to call emacs for file associated with AST IR node.
void callEditor(SgNode *node, int x, int y, int width, int height);


class roseRow
   : public QRRow
   {
     public:	
          roseRow(SgNode *node, QRTable *parent, string label) : QRRow(parent, label, QRRow::checkbox_ctrl) 
             {
            // I think these don't highlight properly! Bug in QRose
            // So build a pointer to the parent which is more likely to highlight properly.
               if (isSgInitializedName(node) != NULL)
                  {
                    node = node->get_parent();
                    ROSE_ASSERT(node != NULL);
                  }

               m_node = node;
             }

          roseRow(SgNode *node, QRRow *parent, string label) : QRRow(parent, label, QRRow::checkbox) 
             {
            // I think these don't highlight properly! Bug in QRose
            // So build a pointer to the parent which is more likely to highlight properly.
               if (isSgInitializedName(node) != NULL)
                  {
                    node = node->get_parent();
                    ROSE_ASSERT(node != NULL);
                  }

               m_node = node;
             }
	
          void doubleClickHandler(int col)
             {
               printf ("roseRow::doubleClickHandler(col = %d) m_node = %p = %s = %s \n",
                    col,m_node,m_node->class_name().c_str(),SageInterface::get_name(m_node).c_str());
            // QRGUI::broadcastNode(m_node);
               if (m_table->getKeyModifier() == Qt::Key_Control)
                    callEditor(m_node, 10, 20, 100, 200);
                 else 
                    QRGUI::broadcastNode(m_node);
             }

     protected:
          SgNode* m_node;	 
   };

class compassRow: public QRRow
   {
  // This class binds the QRRow to the Compass object that does the checking.
     public:
          compassRow(const Compass::Checker* localChecker, QRTable *parent, string label)
		       : QRRow(parent, label, QRRow::checkbox_ctrl) 
             {
               checker = localChecker;
            // ROSE_ASSERT(checker != NULL);
             }
          compassRow(const Compass::Checker* localChecker, QRRow *parent, string label)
             : QRRow(parent, label, QRRow::checkbox) 
             {
               checker = localChecker;
            // ROSE_ASSERT(checker != NULL);
             }
         const Compass::Checker* getChecker() 
             {
            // ROSE_ASSERT(checker != NULL);
               return checker;
             }

#if 1
          void singleClickHandler(int col)
             {
            // The semantics of this should be to select the current row (same as clicking the box).
               printf ("Need to define a QRose QRRow::singleClickHandler() member function for column = %d of row = %s \n",col,getChecker()->getName().c_str());
            // setState(true);

            // Selection  sets the check box: Set the row as checked if not already checked, else mark as unchecked.
               setChecked(!getState());
             }
#endif
#if 1
          void doubleClickHandler(int col)
             {
            // The semantics of what double click should mean is not yet fully implemented, 
            // but it should swelect the current row AND run the specified single test.
               printf ("Need to define a QRose QRRow::setState() member function for column = %d of row = %s \n",col,getChecker()->getName().c_str());

            // I think there should be a setState(bool) function so that we can set the state directly (so that then the 
            // computeResultTable() will have the same semantics as if the current row had been selected alone and 
            // the "Run" Button had been pressed.
            // setState(true);
               setChecked(true);

               computeResultTable();

            // int status = QRGUI::exec("1");
               QRGUI::showDialog(1);
             }
#endif

     protected:
          const Compass::Checker* checker;
   };

// A QRose specific output object which provides the information required to support the QRose GUI.
class QRoseOutputObject
   : public Compass::OutputObject
   {
  // This output object adds the ability to collect the output violations so that they can be presented in the GUI after all runs are complete.

     protected:
       // Container of violations (locations in the source code of violations) for the associated checker
       // std::vector<Compass::OutputViolationBase*> outputList;

     public:
          QRoseOutputObject() {}

          virtual void addOutput(Compass::OutputViolationBase* theOutput)
             {
               outputList.push_back(theOutput);

            // cout << "In QRoseOutputObject::addOutput(): " << theOutput->getString() << std::endl;
               printf ("In QRoseOutputObject::addOutput(): getString() = %s \n",theOutput->getString().c_str());
             }

      // Moved from base class where they are not required
      // std::vector<Compass::OutputViolationBase*> getOutputList() { return outputList; }
      // void clear() { outputList.erase(outputList.begin(),outputList.end()); }
   };


void displayResults();
vector<compassRow*> flattenTable (QRTable* table);
vector<compassRow*> flattenTable (vector<compassRow*> table);
// vector<bool> buildStatusList(vector<QRRow*> & rowList);
void computeResultTable();

void compassInputsHandler ( QRButtons & compassHeader, QRButtons::Event event, int buttonId )
   {
     switch(buttonId)
        {
       // Run button
          case 0:
             {
            // displayResults();
            // QRGUI::getDialog(0)->hide();

            // tableBuilder tb(table);
            // tb.traverseInputFiles(project, NULL);

               computeResultTable();

               QRGUI::showDialog(1);
               break;
             }

       // Exit button
          case 1:
             {
               QRGUI::quit(0);
               break;
             }

          default:
             {
               printf ("Button not handled in compassInputsHandler: buttonId = %d \n",buttonId);
               ROSE_ASSERT(false);
             }
        }
   }

void callEditor(SgNode *node, int x, int y, int width, int height)
   {
  // This function should get the $EDITOR environment variable if it is defined.
  // In this way it should be independent of any specific editor, though we might 
  // have to address how it is that specific editors use the command line.

     char cmd[100];
     Sg_File_Info *info = node->get_file_info();
     ROSE_ASSERT(info != NULL);

  // Later we need to sanitize the filename to avoid exec of commands embedded in the filename.
     string filename = info->get_filenameString();

  // Avoid possible buffer overflow
     snprintf(cmd, 100, "emacs +%d:%d %s &",info->get_line(),info->get_col(),filename.c_str());
     printf ("editor command: cmd = %s \n",cmd);
     system(cmd);
   }


void compassResultsHandler ( QRButtons & compassHeader, QRButtons::Event event, int buttonId )
   {
     switch(buttonId)
        {
       // Run button
          case 0:
             {
            // displayResults();
            // QRGUI::getDialog(0)->hide();

            // tableBuilder tb(table);
            // tb.traverseInputFiles(project, NULL);

               computeResultTable();

            // int status = QRGUI::exec("1");
               QRGUI::showDialog(1);
               break;
             }

       // Exit button
          case 1:
             {
            // QRGUI::quit(0);
               QRGUI::hideDialog(1);
            // int status = QRGUI::exec("1");
               break;
             }

          default:
             {
               printf ("Button not handled in compassResultsHandler: buttonId = %d \n",buttonId);
               ROSE_ASSERT(false);
             }
      }
      
   }

void compassResultsListHandler ( QRButtons & compassHeader, QRButtons::Event event, int buttonId )
   {
     switch(buttonId)
        {
       // Run button
          case 0:
             {
               printf ("Button #0 \n");
               if (compassHeader.getState(0) == true)
                    compassHeader.setCaption(0,"Unhide Tests");
                 else
                    compassHeader.setCaption(0,"Hide Tests");
               break;
             }

       // Exit button
          case 1:
             {
               printf ("Button #1 \n");
               break;
             }
          default:
             {
               printf ("Button not handled in compassResultsListHandler: buttonId = %d \n",buttonId);
               ROSE_ASSERT(false);
             }
      }
      
   }


void compassASTHandler ( QRButtons & compassHeader, QRButtons::Event event, int buttonId )
   {
     switch(buttonId)
        {
       // Run button
          case 0:
             {
               printf ("Button #0 \n");
               break;
             }

       // Exit button
          case 1:
             {
               printf ("Button #1 \n");
               break;
             }
          default:
             {
               printf ("Button not handled in compassASTHandler: buttonId = %d \n",buttonId);
               ROSE_ASSERT(false);
             }
      }
      
   }


void displayResults()
   {
  // This function debuging support.
     QRTable* compassTestsTable = QRGUI::getWidget<QRTable>("Compass Tests",0);

     printf ("\nResults from selection of inputs or outputs \n");

  // Get the states of the hierarchy of compass tests. 
     vector<QRRow*> compassTestsRows = compassTestsTable->getRows();
     for (vector<QRRow*>::iterator i = compassTestsRows.begin(); i != compassTestsRows.end(); i++)
        {
          printf ("%s = %s \n",(*i)->getText().c_str(),(*i)->getState() ? "true" : "false");

          vector<QRRow*> compassTestsRows_test1 = (*i)->getRows();
          for (vector<QRRow*>::iterator i = compassTestsRows_test1.begin(); i != compassTestsRows_test1.end(); i++)
             {
               printf ("%s = %s \n",(*i)->getText().c_str(),(*i)->getState() ? "true" : "false");
             }
        }

     printf ("displayResults: DONE!\n\n\n");
   }

vector<compassRow*>
flattenTable (vector<compassRow*> rowVector)
   {
     printf ("Calling flattenTable rowVector.size() = %d \n",rowVector.size());

     vector<compassRow*> resultVector;
     for (unsigned int i = 0; i < rowVector.size(); i++)
        {
          printf ("rowVector[i]->childCount() = %d \n",rowVector[i]->childCount());
          if (rowVector[i]->childCount() > 1)
             {
               vector<QRRow*> tempQRRowVector = rowVector[i]->getRows();
               vector<compassRow*> tempCompassVector;
               for (vector<QRRow*>::iterator j = tempQRRowVector.begin(); j != tempQRRowVector.end(); j++)
                  {
                    tempCompassVector.push_back(dynamic_cast<compassRow*>(*j));
                 // tempCompassVector.push_back(*j);
                  }
            // vector<compassRow*> tempVector = flattenTable(rowVector[i]->getRows());
               vector<compassRow*> tempVector = flattenTable(tempCompassVector);
               resultVector.insert(resultVector.end(),tempVector.begin(),tempVector.end());
             }
            else
             {
               resultVector.push_back(rowVector[i]);
             }
        }

     return resultVector;
   }

vector<compassRow*>
flattenTable (QRTable* table)
   {
     printf ("Calling flattenTable table->getRows().size() = %d \n",table->getRows().size());

  // Write a conversion function to handle conversion of vector<QRRow*> to vector<compassRow*>
     vector<QRRow*> tempQRRowVector = table->getRows();
     vector<compassRow*> tempCompassVector;
     for (vector<QRRow*>::iterator j = tempQRRowVector.begin(); j != tempQRRowVector.end(); j++)
        {
          compassRow* tempCompassRow = dynamic_cast<compassRow*>(*j);
          ROSE_ASSERT(tempCompassRow != NULL);
          tempCompassVector.push_back(tempCompassRow);
        }
     vector<compassRow*> resultVector = flattenTable(tempCompassVector);

     return resultVector;
   }

#if 0
vector<bool>
buildStatusList(vector<QRRow*> & rowList)
   {
     vector<bool> returnVector;
     for (unsigned int i = 0; i < rowList.size(); i++)
        {
          bool status = rowList[i]->getState();
          returnVector.push_back(status);
        }

     return returnVector;
   }
#endif

string
findCommonPath( vector<Compass::OutputViolationBase*> & outputList )
   {
  // This function returns the longest common path to the filenames associated with the files of violated tests

  // ROSE_ASSERT(outputList.empty() == false);     

     string randomEntry = "no path";

     vector<string> setOfFileNames;
     for (vector<Compass::OutputViolationBase*>::iterator j = outputList.begin(); j != outputList.end(); j++)
        {
          SgNode* targetNode = (*j)->getNode();
          SgLocatedNode* locateNode = isSgLocatedNode(targetNode);
          if (locateNode != NULL)
             {
            // ROSE_ASSERT(locateNode != NULL);
               ROSE_ASSERT(locateNode->get_startOfConstruct() != NULL);
               string filename = locateNode->get_startOfConstruct()->get_filenameString();
               printf ("In findCommonPath(): filename = %s \n",filename.c_str());

               setOfFileNames.push_back(filename);

               randomEntry = filename;
             }
            else
             {
               printf ("In findCommonPath(): targetNode = %p = %s \n",targetNode,targetNode != NULL ? targetNode->class_name().c_str() : "NULL");
               Sg_File_Info* fileInfo = targetNode->get_file_info();

               string filename = fileInfo->get_filenameString();
               printf ("In findCommonPath(): filename = %s \n",filename.c_str());

               setOfFileNames.push_back(filename);

               randomEntry = filename;
             }
        }

     string lastPossibleCommonSubstring = randomEntry;
     string possibleCommonSubstring = StringUtility::getPathFromFileName(randomEntry);
     printf ("randomEntry = %s possibleCommonSubstring = %s \n",randomEntry.c_str(),possibleCommonSubstring.c_str());

     bool isCommon = false;
     while (isCommon == false)
        {
          bool isCommonOverWholeSet = true;
          int i = 0;
          while (isCommonOverWholeSet == true && i < setOfFileNames.size())
             {
               string filename = setOfFileNames[i];
               printf ("filename = %s possibleCommonSubstring = %s \n",filename.c_str(),possibleCommonSubstring.c_str());
               if (filename.find(possibleCommonSubstring,0) != string::npos)
                  {
                    isCommonOverWholeSet = true;
                  }
                 else
                  {
                    isCommonOverWholeSet = false;
                  }

               i++;
             }

          isCommon = isCommonOverWholeSet;
       // Save the last string that failed (I don't think we will need this)
          lastPossibleCommonSubstring = possibleCommonSubstring;
          possibleCommonSubstring = StringUtility::getPathFromFileName(possibleCommonSubstring);
        }

     printf ("lastPossibleCommonSubstring = %s \n",lastPossibleCommonSubstring.c_str());
     printf ("possibleCommonSubstring     = %s \n",possibleCommonSubstring.c_str());

     if (lastPossibleCommonSubstring == "/")
        {
          lastPossibleCommonSubstring = "";
        }

     printf ("lastPossibleCommonSubstring = %s \n",lastPossibleCommonSubstring.c_str());
     return lastPossibleCommonSubstring;
   }

void
computeResultTable()
   {
     printf ("Inside of computeResultTable() \n");

     QRTable* compassTestsTable = QRGUI::getWidget<QRTable>("Compass Tests",0);
     ROSE_ASSERT(compassTestsTable != NULL);

     QRTable* compassResultsTable = QRGUI::getWidget<QRTable>("Compass Results",1);
     ROSE_ASSERT(compassResultsTable != NULL);

     QRProgress *progress = QRGUI::getWidget<QRProgress>("progress", 0);
     ROSE_ASSERT(progress != NULL);

  // Remove the existing entries (no need to edit them)
     compassResultsTable->clear();

  // debugging code
  // displayResults();

  // Note that once displayed, the rows are sorted. So we attach the checkers 
  // to the rows to simplify the design and use of QRose in Compass.
     vector<compassRow*> rowList = flattenTable (compassTestsTable);

  // Set the number of ticks used in the progress widget
     progress->set(rowList.size());

     printf ("rowList.size() = %d \n",rowList.size());

  // Empty the lists for each checker before rebuilding it in the next run
     for (int i = 0; i != rowList.size(); i++)
        {
          ROSE_ASSERT(rowList[i] != NULL);

       // The row can just be a header for a nested list of rows
          if (rowList[i]->getChecker() != NULL)
             {
            // ROSE_ASSERT(rowList[i]->getChecker() != NULL);
               ROSE_ASSERT(rowList[i]->getChecker()->getOutput() != NULL);
               rowList[i]->getChecker()->getOutput()->clear();
             }
        }

  // Run the tests (all together so that we can combine them and run them in parallel)
     for (int i = 0; i != rowList.size(); i++)
        {
          string checkerName = rowList[i]->getText();
          char msg[1000];
          snprintf(msg, 1000, "processing checker name = %s %d of %d ...",checkerName.c_str(),i,rowList.size());			
          progress->tick(msg);

          if (rowList[i]->getState() == true)
             {
               printf ("Running checkerName = %s \n",checkerName.c_str());

               if (rowList[i]->getChecker() != NULL)
                  {
                 // We should alternatively gather all the test to be run so that they can be combined.
                    rowList[i]->getChecker()->run(sageProject);

                    ROSE_ASSERT(rowList[i] != NULL);
                 // printf ("Testing: rowList[i]->getChecker() \n");
                    ROSE_ASSERT(rowList[i]->getChecker() != NULL);
                 // printf ("Testing: rowList[i]->getChecker()->getOutput() \n");
                    ROSE_ASSERT(rowList[i]->getChecker()->getOutput() != NULL);
                 // printf ("Testing: rowList[i]->getChecker()->getOutput()->getNode() \n");
                 // ROSE_ASSERT(rowList[i]->getChecker()->getOutput()->getNode() != NULL);

                    printf ("***** rowList[i]->getChecker()->getOutput()->getNode() = %d \n",rowList[i]->getChecker()->getOutput()->getOutputList().size());

                 // Need to gather the results
                  }
             }
            else
             {
               printf ("Skip running checkerName = %s \n",checkerName.c_str());
             }
        }

  // sleep(2);
     progress->tick(); // add an extra tick to hide progress widget

     for (int i = 0; i != rowList.size(); i++)
        {
       // printf ("(*i)->getName() = %s \n",(*i)->getName().c_str());
       // string checkerName = (*i)->getName();
          string checkerName = rowList[i]->getText();

          printf ("checkerName        = %s \n",checkerName.c_str());

       // if (statusList[i] == true)
          if (rowList[i]->getState() == true)
             {
               printf ("Processing checkerName = %s \n",checkerName.c_str());
               QRRow* checker = new QRRow(compassResultsTable,checkerName,QRRow::normal);

            // We should alternatively gather all the test to be run so that they can be combined (done above).
            // rowList[i]->run();

               set<string> setOfFileNames;

            // vector<Compass::OutputViolationBase*> outputList = rowList[i]->getChecker()->getOutput()->getOutputList();
               vector<Compass::OutputViolationBase*> outputList;
               if (rowList[i]->getChecker() != NULL)
                  {
                    outputList = rowList[i]->getChecker()->getOutput()->getOutputList();
                  }
#if 0
                 else
                  {
                    QRRow* checker = new QRRow(commonPathRow,filenameWithoutPath,QRRow::normal);
                  }
#endif
               string commonPath = findCommonPath(outputList);
               printf ("commonPath = %s \n",commonPath.c_str());
            // ROSE_ASSERT(false);

            // QRRow* commonPathRow = new QRRow(checker,commonPath,QRRow::normal);
               QRRow* commonPathRow = new QRRow(checker,commonPath,QRRow::normal);

               for (vector<Compass::OutputViolationBase*>::iterator j = outputList.begin(); j != outputList.end(); j++)
                  {
                 // string targetLocation = (*j)->getString();
                    SgNode* node = (*j)->getNode();
                    ROSE_ASSERT(node != NULL);
                    Sg_File_Info* fileInfo = node->get_file_info();
                    ROSE_ASSERT(fileInfo != NULL);
                    string filename = fileInfo->get_filenameString();
                    if (setOfFileNames.find(filename) == setOfFileNames.end())
                       {
                      // QRRow* commonPathRow = new QRRow(checker,commonPath,QRRow::normal);

                      // Output a row with just the filename.
                         string filenameWithoutPath = StringUtility::getPathFromFileName(filename);
                         QRRow* checker = new QRRow(commonPathRow,filenameWithoutPath,QRRow::normal);
                       }

                 // string targetLocation = (*j)->getString();
                    string targetLocation = "Violation at line = " + StringUtility::numberToString(fileInfo->get_line()) + 
                                            " column = " + StringUtility::numberToString(fileInfo->get_col());
                    printf("node = %p \n",node);
                 // roseRow* result = new roseRow(node,checker,targetLocation);
                    roseRow* result = new roseRow(node,commonPathRow,targetLocation);
                  }

            // Build a new  row based on what was selected from the compassTestsTable
            // compassRow* checker = new compassRow(rowList[i]->getChecker(),compassResultsTable,checkerName,QRRow::normal);

            // string count = "0";
               string count = StringUtility::numberToString(outputList.size());
               checker->setText(1,count);
             }
            else
             {
               printf ("Skip processing checkerName = %s \n",checkerName.c_str());
             }
        }
   }

void buildRuleSelectionDialog()
   {
     QRDialog* dialogInputs = QRGUI::getDialog(0);
     dialogInputs->setTitle("Compass: Rule Selection",true);

  // new QRCodeBox(sageProject, dialogInputs);

  // QRPanel* compassPanel = new QRPanel(dialogInputs, "Compass Panel");

     QRButtons* compassInputRuleSelectionButtons = new QRButtons(QRGUI::horizontal,3,QRButtons::toggle,dialogInputs);
     compassInputRuleSelectionButtons->setCaption(0,"Hide Tests");
     compassInputRuleSelectionButtons->setCaption(1,"Exclude List");
     compassInputRuleSelectionButtons->setCaption(2,"Build New Checker");
     compassInputRuleSelectionButtons->setCallback(compassResultsListHandler);

     compassInputRuleSelectionButtons->setCellSize(0,75,100);
     compassInputRuleSelectionButtons->setCellSize(1,75,100);
     compassInputRuleSelectionButtons->setCellSize(1,150,200);

  // QRTable* compassTestsTable = new QRTable(dialog, "Compass Tests");
     QRTable* compassTestsTable = new QRTable("Checker Name", -1, dialogInputs, "Compass Tests");
     compassTestsTable->addColumn("Checker Description");

  // NOTE: Need to display initially with all top level children expanded
     int checkerCounter = 0;

#if 0
  // Use compassRow object uniformally so that flattening the table is as simple as possible.
  // QRRow* all_tests = new QRRow(compassTestsTable,"All Tests",QRRow::checkbox_ctrl);
     compassRow* all_tests = new compassRow(NULL,compassTestsTable,"All Tests");
     for (vector<Compass::FactoryBase*>::iterator i = compassFactoryArray.begin(); i != compassFactoryArray.end(); i++)
        {
       // printf ("(*i)->getName() = %s \n",(*i)->getName().c_str());
          string checkerName = (*i)->getName();
          string checkerDescription = (*i)->getHelp();

          printf ("checkerName        = %s \n",checkerName.c_str());
          printf ("checkerDescription = %s \n",checkerDescription.c_str());

       // QRose does not handle CR's well
          while (checkerDescription.find("\n") != string::npos)
             {
               string::size_type location = checkerDescription.find("\n");
               checkerDescription[location] = ' ';
             }

       // QRRow* checker = new QRRow(all_tests,checkerName,QRRow::checkbox);
          QRRow* checker = new compassRow(compassTestArray[checkerCounter++],all_tests,checkerName);

          ROSE_ASSERT(checker != NULL);
          ROSE_ASSERT(dynamic_cast<compassRow*>(checker) != NULL);
          ROSE_ASSERT(dynamic_cast<compassRow*>(checker)->getChecker() != NULL);
          ROSE_ASSERT(dynamic_cast<compassRow*>(checker)->getChecker()->getOutput() != NULL);

       // Set the short description (see the manual for the more complete documentation)
       // In the future tooltips will handle present the text appearing after the first "\n".
          checker->setText(1,checkerDescription);
        }
#endif

  // QRGUI::exec("0");

  // vector<QRRow*> rowList = flattenTable (compassTestsTable);
  // vector<bool> statusList = buildStatusList(rowList);

  // Add progress widget
     new QRProgress(true, true, dialogInputs, "progress");

     QRButtons* compassFooterInputs = new QRButtons(QRGUI::horizontal,2,QRButtons::normal,dialogInputs);
     compassFooterInputs->setCaption(0,"Run");
     compassFooterInputs->setCaption(1,"Exit");
     compassFooterInputs->setCallback(compassInputsHandler);

  // Make the "Exit" button and the "Run" button small and cute. 
     compassFooterInputs->setCellSize(0,50,75);
     compassFooterInputs->setCellSize(1,50,75);

  // Specification: X,Y location then size (width,hieght)
     dialogInputs->setGeometry(20,20,1400,800);

  // Let the table be the largest percentage (99%) of the dialog box
     dialogInputs->setDimChildren(1,98);
   }

void buildResultDialog()
   {
  // Build second dialog box (and fill in the rows later)
     QRDialog* dialogResults = QRGUI::getDialog(1);
     dialogResults->setTitle("Compass: Results View",true);

  // QRButtons* compassHeaderResults = new QRButtons(QRGUI::horizontal,2,QRButtons::normal,dialogResults);
     QRButtons* compassHeaderResults = new QRButtons(QRGUI::horizontal,2,QRButtons::radio,dialogResults);
     compassHeaderResults->setCaption(0,"Show AST");
     compassHeaderResults->setCaption(1,"Action 2");
     compassHeaderResults->setCallback(compassASTHandler);

  // It seems that this is at too low a level (don't know how to add Qt specific input types)
  // QRToolBar* compassHeaderResults = new QRToolBar(dialogResults,"Compass Toolbar");

     compassHeaderResults->setCellSize(0,50,75);
     compassHeaderResults->setCellSize(1,50,75);

     QRPanel* compassPanel = new QRPanel(dialogResults, "Compass Results Panel");
     QRPanel* compassResultsListPanel = new QRPanel(compassPanel, "Compass Results List Panel");

  // QRButtons* compassHeaderResultsList = new QRButtons(QRGUI::horizontal,2,QRButtons::normal,compassResultsListPanel);
     QRButtons* compassHeaderResultsList = new QRButtons(QRGUI::horizontal,2,QRButtons::toggle,compassResultsListPanel);
     compassHeaderResultsList->setCaption(0,"Unhide Violations");
     compassHeaderResultsList->setCaption(1,"Exclude Paths");
     compassHeaderResultsList->setCallback(compassResultsListHandler);

  // compassHeaderResultsList->setCellSize(0,125);
  // compassHeaderResultsList->setCellSize(1,125);
     compassHeaderResultsList->setMinimumHeight(40);
     compassHeaderResultsList->setMaximumHeight(40);

  // Note this can't be a local variable since I can't access it using QRGUI::getWidget<QRTable>("Compass Results");
  // QRTable* compassResultsTable = new QRTable("Checker Results Name", -1, compassPanel, "Compass Results");
     QRTable* compassResultsTable = new QRTable("Checker Results Name", -1, compassResultsListPanel, "Compass Results");
  // compassResultsTable = new QRTable(compassPanel, "Compass Results");
     ROSE_ASSERT(compassResultsTable != NULL);
     compassResultsTable->addColumn("Count");

     compassResultsListPanel->setDimChildren(1);

  // Build the source code view
  // QRCodeBox* SourceCodeBox = new QRCodeBox(sageProject,dialog,"Source Code View");
     QRCodeBox* SourceCodeBox = new QRCodeBox(sageProject,compassPanel,"Source Code View");

     compassPanel->setDimChildren(40);

     QRButtons* compassFooterResults = new QRButtons(QRGUI::horizontal,2,QRButtons::normal,dialogResults);
     compassFooterResults->setCaption(0,"Run");
     compassFooterResults->setCaption(1,"Exit");
     compassFooterResults->setCallback(compassResultsHandler);

  // Make the "Exit" button and the "Run" button small and cute. 
     compassFooterResults->setCellSize(0,50,75);
     compassFooterResults->setCellSize(1,50,75);

     QRTreeBox* astDialog = new QRTreeBox(sageProject,compassPanel,"AST Results");

  // This might be a bug in QRose, but is required to see the AST.
     astDialog->initView();

  // Specify first top level child to dialog box size to be 1% of the dialog box size
  // Put this call after all children of the top level dialog box have been specified.
     dialogResults->setDimChildren(1,98);

  // Specification: X,Y location then size (width,hieght)
     dialogResults->setGeometry(50,50,1400,800);
   }

int 
main(int argc, char** argv)
   {
     sageProject = frontend(argc,argv);
#if 0
#if 0
     compassFactoryArray.push_back(new CompassAnalyses::LineLengthCheckerNamespace::LineLengthCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::EllipsisDetectorNamespace::EllipsisDetectorFactory());
     compassFactoryArray.push_back(new CompassAnalyses::StaticConstructorInitializationDetectorNamespace::StaticConstructorInitializationDetectorFactory());
     compassFactoryArray.push_back(new CompassAnalyses::ConstCastCheckerNamespace::ConstCastCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::DefaultCaseCheckerNamespace::DefaultCaseCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::DuffsDeviceDetectorNamespace::DuffsDeviceDetectorFactory());
     compassFactoryArray.push_back(new CompassAnalyses::ExplicitCopy::ExplicitCopyFactory());
     compassFactoryArray.push_back(new CompassAnalyses::functionDeclarationPrototypeCheckerNamespace::functionDeclarationPrototypeCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::NoGoto::NoGotoFactory());
     compassFactoryArray.push_back(new CompassAnalyses::IndVarUpdateCheckerNamespace::IndVarUpdateCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::LOC_perFunctionCheckerNamespace::LOC_perFunctionCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::MultipleCasesOnSameLineCheckerNamespace::MultipleCasesOnSameLineCheckerFactory());
     compassFactoryArray.push_back(new CompassAnalyses::MPIBufferTypeChecker::Factory());
#else
  // Tests a single checker to get the interface correct
     compassFactoryArray.push_back(new CompassAnalyses::StaticConstructorInitializationDetectorNamespace::StaticConstructorInitializationDetectorFactory());
#endif

     Compass::Parameters params(Compass::findParameterFile());

     for (vector<Compass::FactoryBase*>::iterator i = compassFactoryArray.begin(); i != compassFactoryArray.end(); i++)
        {
       // Generate separate outputObjects for each checker (to collect output from each checker)
          QRoseOutputObject* output = new QRoseOutputObject();
          ROSE_ASSERT(output != NULL);
          compassTestArray.push_back((*i)->create(params, output));
        }

     QRGUI::init(argc,argv);

     buildRuleSelectionDialog();

     buildResultDialog();
#if 0
  // Handoff control to QRose (Qt) event loop.
     int status = QRGUI::exec("0");
     printf ("Program Terminated Normally! \n");
     return status;
#else
     for (vector<const Compass::Checker*>::iterator i = compassTestArray.begin(); i != compassTestArray.end(); i++)
        {
       // Generate separate outputObjects for each checker (to collect output from each checker)
          (*i)->run(sageProject);
        }
#endif
#else
  // New way to build the checkers
     Compass::Parameters params(Compass::findParameterFile());
     QRoseOutputObject* output = new QRoseOutputObject();

  // Need to make this a function to setup all the checkers.
     compassTestArray.push_back(staticConstructorInitializationChecker);

     for (vector<const Compass::Checker*>::iterator i = compassTestArray.begin(); i != compassTestArray.end(); i++)
        {
       // Generate separate outputObjects for each checker (to collect output from each checker)
          Compass::runCheckerWithPrereqs(*i, sageProject, params, output);
        }
#endif

     printf ("Program Terminated Normally! \n");
     return 0;
   }
