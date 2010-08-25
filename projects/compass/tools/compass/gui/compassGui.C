#if ROSE_MPI
#include <mpi.h>
#endif
#include "rose.h"
#include "compassChecker.h"

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <QFileDialog>

#include <unistd.h>


#include "compassGui.h"
#include "icons.h"

#include "disks.xpm"


//For fork()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>


#define EMACS

using namespace qrs;

typedef enum 
{ 
  TB_RUN, TB_REFRESH, TB_RESET, TB_SEL_ALL, TB_SEL_EN, TB_SEL_REV, TB_UNSEL_ALL, 
  TB_SORT_NAME, TB_SORT_TIMING, TB_SORT_VIOLATIONS, TB_SAVE, TB_QUIT
} ButtonID;

static void tableCellActivated(int col, int row, int oldCol, int oldRow) 
{
  CompassGui *instance = QROSE::cbData<CompassGui *>();
  instance->unhighlightRow(oldRow);
  instance->highlightRow(row);

  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void toolbarClick(int id)
{
  CompassGui *instance = QROSE::cbData<CompassGui *>();
  switch (id) 
  {      
    case TB_RUN:                instance->run(); instance->select(TB_SEL_ALL); instance->refresh();      break;
    case TB_REFRESH:		instance->refresh();      break;
    case TB_RESET:		instance->reset();    break;
    case TB_SEL_ALL:          
    case TB_SEL_EN:
    case TB_SEL_REV:
    case TB_UNSEL_ALL:		instance->select(id); break;
    case TB_SORT_NAME:
    case TB_SORT_TIMING:
    case TB_SORT_VIOLATIONS:	instance->sort(id);   break;
    case TB_SAVE: 		instance->save();     break;
    case TB_QUIT: QROSE::exit(0); break;
    default: eAssert(0, ("internal error: invalid selection type!"));
  } //switch (id)

  return;
} //toolbarClick(int id)

static void violationDblCliked(int row)
{
  CompassGui *instance = QROSE::cbData<CompassGui *>();
  instance->launchEditor(row);

  return;
} //violationDblCliked(int row)

static void violationClicked(int row)
{
  CompassGui *instance = QROSE::cbData<CompassGui *>();
  instance->writeCodeWidget(row);

  return;
} //violationClicked(int row)

//------------------------------------------------------------------------------

CompassGui::CompassGui( CompassInterface & ci ) :
  window(0),
  tableWidget(0),
  violationsWidget(0),
  descriptionWidget(0),
  codeWidget(0),
  compassInterface(ci),
  sortType(-1),
  activeRow(-1)
{
  window = new QRWindow( "mainWindow", QROSE::TopDown );

  {
    //--------------------------------------------------------------------------

    QRToolBar *toolbar = (*window)["toolbar"] << new QRToolBar(QROSE::LeftRight, true, true, true);

    // icons are defined in <icons.h>
    toolbar->addButton("regenerate");   toolbar->setPicture(0, iconRun); 
    toolbar->addButton("refresh");   toolbar->setPicture(1, iconRun); 
    toolbar->addButton("reset"); toolbar->setPicture(2, iconReset);
    toolbar->addSeparator();
    toolbar->addButton("select all"); toolbar->setPicture(3, iconSelectAll);
    toolbar->addButton("select enabled"); toolbar->setPicture(4, iconSelectEnabled);
    toolbar->addButton("select reverse"); toolbar->setPicture(5, iconSelectReversed);         
    toolbar->addButton("unselect all");  toolbar->setPicture(6, iconUnselectAll);
    toolbar->addSeparator();
    toolbar->addButton("sort by name"); toolbar->setPicture(7, iconSortByName);
    toolbar->addButton("sort by time");  toolbar->setPicture(8, iconSortByTime);
    toolbar->addButton("sort by violations"); toolbar->setPicture(9, iconSortByViolations);
    toolbar->addSeparator();
    toolbar->addButton("save"); toolbar->setPicture(10, disks_xpm);
    toolbar->addButton("quit"); toolbar->setPicture(11, iconQuit);
//    toolbar->addButton("regenerate"); toolbar->setPicture(12, iconRun);

    QROSE::link(toolbar, SIGNAL(clicked(int)), &toolbarClick, this);

    //--------------------------------------------------------------------------

    QRProgress *progress = (*window)["progress"] << new QRProgress(true,true);
    progress->set(compassInterface.numCheckers());
    
    //--------------------------------------------------------------------------

    QRPanel &tiledPanel = *window << *( new QRPanel(QROSE::TopDown, QROSE::UseSplitter) );

    {
      QRPanel &upperInnerTiledPanel = tiledPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);

      {
        tableWidget = upperInnerTiledPanel << new QRTable( 3, "Checker", "Timing", "Violations" );
        QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableCellActivated, this);

        //----------------------------------------------------------------------

        violationsWidget = upperInnerTiledPanel << new QRSelect(QRSelect::Box);
        QROSE::link(violationsWidget, SIGNAL(clicked(int)), &violationClicked,this);
        QROSE::link(violationsWidget, SIGNAL(dblClicked(int)), &violationDblCliked, this);

        //----------------------------------------------------------------------

        descriptionWidget = upperInnerTiledPanel << new QREdit(QREdit::Box);
        descriptionWidget->setFont(QFont("helvetica"));
        descriptionWidget->setBgColor(QColor(253,255,204));
      } //upperInnerTiledPanel
      upperInnerTiledPanel.setTileSize(50,30,20);

      codeWidget = tiledPanel << new QTextEdit;//new QREdit(QREdit::Box);
      codeWidget->setReadOnly(true);

    } //tiledPanel
    tiledPanel.setTileSize(40,60);
  } //window 

  window->setGeometry(0,0,1280,800);
  window->setTitle("CompassMainGui (QROSE)");

  sort(TB_SORT_VIOLATIONS); // sort by number of violations
  tableWidget->setChecked(true,0);
} //CompassGui::CompassGui()

CompassGui::~CompassGui()
{
  if( window ) delete window;

  sourceCodesMap.clear();
} //CompassGui::~CompassGui()

void CompassGui::updateTableWidget()
{
  // unlink activation callback, which otherwise would trigger each
  // time we add a row in the table.
  QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));

  while(tableWidget->rowCount()) tableWidget->removeRow(0);

  compassInterface.sort(sortType);
  const CompassCheckers_v & compassCheckers = compassInterface.getCompassCheckers();
  static std::vector<int> columnWidths( compassCheckers.size(), -1 );
  bool allDisabled = compassInterface.numEnabledCheckers() == 0;

  int row = 0;
  for( CompassCheckers_v::const_iterator itr = compassCheckers.begin();
       itr != compassCheckers.end(); itr++, row++ )
  {
    tableWidget->addRows(1);
    tableWidget->setText((*itr)->getName(), 0, row);

    int &cw = columnWidths.at(row);
    if( cw < 0 )
      tableWidget->setColumnWidth(0,380);
    else
      tableWidget->setColumnWidth(0,cw);

    bool isEnabled = (*itr)->isEnabled();
    bool hasErrorMsg = (*itr)->hasErrorMsg();

    if( isEnabled )
    {
      if (hasErrorMsg) 
      {
        tableWidget->setText("-", 1, row);
        tableWidget->setText("error", 2, row);
        // a little icon on the cell
        tableWidget->setPicture(iconError, 2, row);
      } //if (hasErrorMsg)
      else 
      {
        tableWidget->setText(QROSE::format("%4.3fs", (*itr)->getEllapsedTime()), 1, row);
        tableWidget->setText(QROSE::format("%d", (*itr)->getNumViolations()), 2, row);
        tableWidget->clearPicture(2, row);
      } //else
    } //if( (*itr)->isEnabled() )
    else
    {
      tableWidget->setText("-",1,row);
      tableWidget->setText("-",2,row);
      tableWidget->clearPicture(2, row);
    } //else

    tableWidget->setType(QRTable::Check, 0, row);
    tableWidget->setChecked(isEnabled, 0, row);

    cw = tableWidget->columnWidth(0);

    //----Colors----
    if (allDisabled) 
    {
       tableWidget->setBgColor(QColor("white"), QRTable::All, row);
    } //if (allDisabled) 
    else 
    {
      if (isEnabled) 
      {
        if ((*itr)->getNumViolations() == 0 && !(*itr)->hasErrorMsg()) 
        {
          // QColor(R=210,G=235,B=210) => green if there no violations
          tableWidget->setBgColor(QColor(210, 235, 210), QRTable::All, row);
        }//if
        else 
        {
          // QColor(R=235,G=210,B=210) => red if there are violations
          tableWidget->setBgColor(QColor(235, 210, 210), QRTable::All, row);
        }//else
      }//if (isEnabled)
      else 
      {
        // QColor(R=235,G=235,B=235) => grey if disabled
        tableWidget->setBgColor(QColor(235, 235, 235), QRTable::All, row);
      }//else
    } //else
  } //for, itr

  tableWidget->setHAlignment(true, false, 0); // left horizontal alignment
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), 
              &tableCellActivated, this);

  return;
} //CompassGui::updateTableWidget()

void CompassGui::refresh()
{
  CompassCheckers_v checkers = compassInterface.getCompassCheckers();
  QRProgress *progress = (*window)["progress"];

  compassInterface.getResult()->reset();

  int row = 0;
  progress->tick();
  for( CompassCheckers_v::iterator itr = checkers.begin(); 
       itr != checkers.end(); itr++, row++ )
  {
    progress->tick((*itr)->getName());
    bool enable = tableWidget->isChecked(0,row);
    (*itr)->process(enable,compassInterface);
  } //for, itr

  progress->tick();
  updateTableWidget();

  return;
} //CompassGui::refresh()


#ifdef HAVE_SQLITE3

#include "sqlite3x.h"
#include <boost/lexical_cast.hpp>

#endif


struct BuildLine{
  int build_index;
  std::string compile_line;
  std::vector<std::pair<std::string, std::string> > argv;
};

void CompassGui::run()
{
#ifdef HAVE_SQLITE3
 std::vector<BuildLine> buildLines;


  if ( Compass::UseDbOutput == true )
  {

    //Make sure that the violations table exists and is empty
    try {
      Compass::con.executenonquery("create table IF NOT EXISTS violations( row_number INTEGER PRIMARY KEY, checker_name TEXT,  error_body TEXT, filename TEXT, line INTEGER, short_description TEXT )");
    } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}



    try { // Read each build_index from the database
      /* Read in from database here */
      sqlite3x::sqlite3_command cmd(Compass::con, "SELECT row_number,compile_line from build_interpreted" );
      sqlite3x::sqlite3_reader r = cmd.executereader();
      while (r.read()) {
           int build_index = r.getint(0);
           std::string compile_line = r.getstring(1);

           BuildLine commandLine;
           commandLine.build_index = build_index;
           commandLine.compile_line= compile_line;

           buildLines.push_back(commandLine);
           std::cout << compile_line << std::endl;
      }

    } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}


    //For each build index find the argv in the database
    for(unsigned int i = 0; i < buildLines.size(); i++)
    {
      BuildLine& commandLine = buildLines[i];
      try {
        /* Read in from database here */
        sqlite3x::sqlite3_command cmd(Compass::con, "SELECT first,second from build_interpreted_split where build_index=? ORDER BY row_number" );
        cmd.bind(1,commandLine.build_index);
        sqlite3x::sqlite3_reader r = cmd.executereader();
        while (r.read()) {
          std::string first = r.getstring(0);
          std::string second = r.getstring(1);

          commandLine.argv.push_back(std::pair<std::string,std::string>(first,second) );

        }

      } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}


    }

  }


  if(buildLines.size() != 0) // If there are something to build, build it.
  {
    



    QRProgress *progress = (*window)["progress"];


    compassInterface.getResult()->reset();
    progress->set(buildLines.size());


    progress->set(0,buildLines.size());

    for(unsigned int i=0; i < buildLines.size() ; i++)
    {
        progress->tick();
        std::string runCompassPath = std::string(ROSE_COMPILE_TREE_PATH) + "/projects/compass/tools/compass/compassMain";

        BuildLine& commandLine = buildLines[i];
        std::vector<char*> argv;
        argv.push_back(strdup(runCompassPath.c_str()));
        for(int j =0; j < commandLine.argv.size(); j++ )
        {
          std::string cur_arg = commandLine.argv[j].first + commandLine.argv[j].second;
          argv.push_back( strdup( (char*) cur_arg.c_str() ));

        }
       
        argv.push_back("--outputDb");
        argv.push_back(strdup(Compass::outputDbName.c_str()));

        std::cout << "Executing " << commandLine.compile_line << std::endl;


	//Execute compass to find violations

	std::cout << std::endl << "normal compile: ";
	std::string runningROSE;
	for(int i=0; i<argv.size(); i++)
	{
		runningROSE+= std::string(argv[i]) + " ";

		//std::cout << argv[i] << " ";


	};
	std::cout << std::endl;


        //AS(1/9/09) execv(..) does not work on Dan's machine so we have to use system(..)
	int errorCodeROSE = system(runningROSE.c_str());

	if(errorCodeROSE)
	{
		runningROSE+=" -rose:skip_rose ";

   // DQ (1/24/2009): Added debugging code
      printf ("runningROSE = %s \n",runningROSE.c_str());

		errorCodeROSE = system(runningROSE.c_str()) ;

   // DQ (1/24/2009): Added a ROSE_ASSERT().
                ROSE_ASSERT(errorCodeROSE == 0);

   // DQ (1/24/2009): It might be more helpful if this as a ROSE_ASSERT(false) instead of "abort();"
                if(errorCodeROSE) abort();


	}


	std::cerr << "Status: " << errorCodeROSE << std::endl;
	std::cerr << "Done execution " << commandLine.compile_line  << std::endl;


    }

    updateTableWidget();
  }

#else
// refresh();
#endif

 return;
} //CompassGui::refresh()






void CompassGui::reset()
{
  compassInterface.reset();
  updateTableWidget();
  tableWidget->activateCell(0,0);

  return;
} //CompassGui::reset()

void CompassGui::select(int actionId)
{
  const CompassCheckers_v &checkers = compassInterface.getCompassCheckers();
  int row = 0;

  for (CompassCheckers_v::const_iterator itr = checkers.begin(); 
       itr != checkers.end(); itr++) 
  {
    switch (actionId) 
    {
      case TB_SEL_ALL:  tableWidget->setChecked(true, 0, row); break;
      case TB_SEL_EN:   tableWidget->setChecked((*itr)->isEnabled(), 0, row); break;
      case TB_SEL_REV:  tableWidget->setChecked(!tableWidget->isChecked(0, row), 0, row); break;
      case TB_UNSEL_ALL: tableWidget->setChecked(false, 0, row); break;
      default: eAssert(0, ("internal error: invalid selection type!"));
    } //switch (actionId)
    row++;
  } //for, itr

  return;
} //CompassGui::select(int actionId)

void CompassGui::sort(int sortId)
{
  switch (sortId) 
  {
    case TB_SORT_NAME:       sortType = CompassInterface::SORT_NAME;   break;
    case TB_SORT_TIMING:     sortType = CompassInterface::SORT_TIMING; break;
    case TB_SORT_VIOLATIONS: sortType = CompassInterface::SORT_VIOLATIONS;  break;
    default: eAssert(0, ("internal error: invalid selection type!"));
  } //switch (sortId)

  updateTableWidget();

  return;
} //CompassGui::sort(int sortId)

void CompassGui::save()
{
  char buf[4096] = "\0";

  std::string path = QFileDialog::getSaveFileName( 0, "Save As", getcwd(buf, 4096), "ASCII (*.txt, *.xml)").toStdString();

  if( path.empty() ) return;

  unsigned int dotpos = path.find_first_of( "." );
  std::string ext;

  if( dotpos != std::string::npos )
    ext = path.substr( dotpos );

  if( ext == ".xml" )
  {
    Compass::outputTgui( path, compassInterface.getCheckers(), compassInterface.getResult() );
  } //if( ext == ".xml" )
  else
  {
    std::ofstream fout( path.c_str(), std::ios::out );

    if( !fout.good() )
    {
      std::cerr << "Warning: " << path << " not written! Bad file stream\n";
      fout.close(); return;
    } //if( !fout.good() )

    fout << *compassInterface.getResult() << std::endl;
    fout.close();
  } //else

  return;
} //CompassGui::save()

void CompassGui::highlightRow(int row)
{
  activeRow = -1;

  if(row >= 0)
  {         
    QFont f = tableWidget->getFont(0, row);
    f.setBold(true);
    tableWidget->setFont(f, 0, row);

    activeRow = row;

    showDescription(row);
    showViolations(row);
  } //if(row >= 0)

  return;
} //CompassGui::highlightRow(int row)

void CompassGui::unhighlightRow(int row)
{
  if (row >= 0) 
  {
    QFont f = tableWidget->getFont(0, row);
    f.setBold(false);
    tableWidget->setFont(f, 0, row);
    violationsWidget->clear();
    descriptionWidget->clear();
  } //if (row >= 0)

  return;
} //CompassGui::unhighlightRow(int row)

void CompassGui::showDescription(int ch)
{
  const CompassChecker *checker = compassInterface.getCompassChecker(ch);
  eAssert(checker, ("internal error: cannot get checker [%d]", ch));
  // text supports a subset of HTML
  descriptionWidget->setText(QROSE::format("<h3>%s</h3>%s", checker->getName().c_str(), checker->getLongDescription().c_str()));

  return;
} //CompassGui::showDescription(int checker)

void CompassGui::showViolations(int ch)
{
  const CompassChecker *c = compassInterface.getCompassChecker(ch);
  eAssert(c, ("internal error: cannot get ch [%d]", ch));
  violationsWidget->clear();

  if (c->hasErrorMsg()) 
  {
    violationsWidget->addItem(QROSE::format("ERROR: %s", c->getErrorMsg().c_str()));
  } 
  else 
  {
    const std::vector<CompassViolation> *violations = c->getViolations();
    int n = 1;
    for ( std::vector<CompassViolation>::const_iterator i = violations->begin(); 
          i != violations->end(); i++, n++) 
    {
      const CompassViolation &v = *i;
      std::string filename = v.getFilename();
      unsigned pos = filename.find_last_of('/');
      violationsWidget->addItem(QROSE::format("[%d] %s:%d.%d - %s",
                                n, filename.substr(pos+1).c_str(),
                                v.getLineStart(), v.getColStart(),
                                v.getDesc().c_str()));
    } //for
  } //else

  return;
} //CompassGui::showViolations(int ch)

void CompassGui::launchEditor(unsigned int violationId)
{
  if(activeRow < 0) return;

  const CompassChecker *checker = compassInterface.getCompassChecker(activeRow);
  const std::vector<CompassViolation> *violations = checker->getViolations();

  if( violationId >= violations->size() ) return;

  const CompassViolation &v = violations->at(violationId);

#ifdef KATE
      system(QROSE::format("kate --column %d --line %d %s",
               v.getColStart()-1, v.getLineStart() -1, 
               v.getFilename().c_str()
             ).c_str());
#elif defined(EMACS)
      system(QROSE::format("emacs +%d:%d %s", 
               v.getLineStart(), v.getColStart(), v.getFilename().c_str()
               ).c_str());
#else
#error "please define EMACS or KATE for launching editor!"
#endif

  return;
} //CompassGui::launchEditor(int row)

void CompassGui::writeCodeWidget(unsigned int violationId)
{
  if(activeRow < 0) return;

  const CompassChecker *checker = compassInterface.getCompassChecker(activeRow);  const std::vector<CompassViolation> *violations = checker->getViolations();

  if( violationId >= violations->size() ) return;

  const CompassViolation &v = violations->at(violationId);

  std::string fname = v.getFilename();

  std::map<std::string, std::string*>::iterator srcItr = sourceCodesMap.find(fname);
  if( srcItr == sourceCodesMap.end() )
  {
    std::string *src = new std::string();
    sourceCodesMap.insert( std::make_pair( fname, src ) );

    std::ifstream fin( fname.c_str(), std::ios::in );

    std::string s;
    for( int numLine = 1; getline(fin, s); numLine++ )
    {
      boost::algorithm::replace_all(s, " ", "&nbsp;");
      std::stringstream ss;

      ss << "<div id=\"" << numLine << "\"><font color=\"black\">";

      if( numLine < 10 )
        ss << numLine << ":&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << s;
      else if( numLine < 100 )
        ss << numLine << ":&nbsp;&nbsp;&nbsp;&nbsp;" << s;
      else if( numLine < 1000 )
        ss << numLine << ":&nbsp;&nbsp;" << s;
      else 
        ss << numLine << ":&nbsp;" << s;

      ss << "</font></div>" << std::endl;

      src->append( ss.str() );
    } //

    fin.close();
  } //if( sourceCodesMap.find(fname) == sourceCodesMap.end() )

  srcItr = sourceCodesMap.find(fname); 
  assert( srcItr != sourceCodesMap.end() );

  std::string src(*srcItr->second);
  int lineStart = v.getLineStart();
  int lineEnd = v.getLineEnd();

  for( int i = lineStart; i <= lineEnd; i++ )
  {
    std::stringstream ss;

    ss << "div id=\"" << i << "\"><font color=\"";

    std::string tag(ss.str() + "black\">");
    std::string repl(ss.str() + "red\">");
    boost::algorithm::replace_all(src, tag, repl);
  } //for, i

  codeWidget->setHtml(QString(src.c_str()));

  QTextCursor cursor = codeWidget->textCursor();
  cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
  cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineStart);
  codeWidget->setTextCursor(cursor);

  return;
} //CompassGui::writeCodeWidget(unsigned int violationId)
