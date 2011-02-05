#include "sage3basic.h"

#include "RoseFileComboBox.h"

#include <QFileInfo>
#include <QDebug>

#include "AstFilters.h"


RoseFileComboBox::RoseFileComboBox(QWidget * par)
        : QComboBox(par),
          project(NULL),
          dispOnlyFilename(true)
{
        connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(curIndexChanged(int)));
}

RoseFileComboBox::~RoseFileComboBox()
{
}


void RoseFileComboBox::setProject(SgProject * proj)
{
        clear(); //delete all combobox entries

        project=proj;

        if(project==NULL)
                return;



        //Add Entries to the combobox
        for(int i=0; i < project->numberOfFiles(); i++)
        {
                Sg_File_Info * fi = (*project)[i]->get_file_info();

                QFileInfo qFi;
                QVariant fileId (fi->get_file_id());

                qFi.setFile(fi->get_filenameString().c_str());

                if(dispOnlyFilename)
                        addItem(qFi.fileName(),-1);
                else
                        addItem(qFi.filePath(),-1);


                std::map< int, std::string > map = fi->get_fileidtoname_map();

                typedef std::map<int,std::string>::iterator MapIter;
                for( MapIter iter = map.begin(); iter != map.end(); ++iter )
                {
                        qFi.setFile( iter->second.c_str());
                        QString dispString;
                        if(dispOnlyFilename)
                                dispString=qFi.fileName();
                        else
                                dispString=qFi.filePath();

                        addItem(QString("   ")+dispString,iter->first);
            }
        }
}

void RoseFileComboBox::curIndexChanged ( int index )
{
    if(index==-1)
        return;

    int fileId = itemData( index ).toInt();

    emit selectedFileChanged( fileId );
    emit selectedFileChanged( itemText(index) );

    if( fileId == -1 )
    {
        // fileId == -1 means show all, existing widgets expect a NULL pointer here
        emit selectedFileChanged( (AstFilterFileById *)NULL );
    }
    else
    {
        AstFilterFileById filter( fileId );
        emit selectedFileChanged( &filter );
    }
}

void RoseFileComboBox::setDisplayOnlyFilename(bool newValue)
{
    dispOnlyFilename=newValue;
    //update display
    setProject(project);
}

