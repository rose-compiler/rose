#include "cindent.h"
#include "qregexp.h"
//#include "qeditor_part.h"
#include "qeditor.h"
//#include "indentconfigpage.h"

#include <qvbox.h>
/*#include <kdialogbase.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kinstance.h>
*/

extern int indentForBottomLine( const QStringList& program, QChar typedIn );
extern void configureCIndent( const QMap<QString, QVariant>& values );

using namespace std;

CIndent::CIndent( QEditor* ed )
    : QEditorIndenter( ed )
{
/*    KConfig* config = QEditorPartFactory::instance()->config();
    config->setGroup( "C++ Indent" );

    m_values[ "TabSize" ] = ed->tabStop();
    m_values[ "IndentSize" ] = config->readNumEntry( "IndentSize", 4 );
    m_values[ "ContinuationSize" ] = config->readNumEntry( "ContinuationSize", 4 );
    m_values[ "CommentOffset" ] = config->readNumEntry( "CommentOffset", 2 );

    configureCIndent( m_values ); */
}

CIndent::~CIndent()
{
    /*
    KConfig* config = QEditorPartFactory::instance()->config();
    config->setGroup( "C++ Indent" );
    config->writeEntry( "IndentSize", m_values[ "IndentSize" ].toInt() );
    config->writeEntry( "ContinuationSize", m_values[ "ContinuationSize" ].toInt() );
    config->writeEntry( "CommentOffset", m_values[ "CommentOffset" ].toInt() );

    config->sync();
    */
}

int CIndent::indentForLine( int line )
{
    QStringList code;

    for( int i=0; i<=line; ++i )
        code << editor()->text( i );

    return indentForBottomLine( code, QChar::null );
}

/*
QWidget* CIndent::createConfigPage( QEditorPart* part, KDialogBase* parentWidget,
                                    const char* widgetName )
{
    QVBox* vbox = parentWidget->addVBoxPage( i18n("Indent") );

    IndentConfigPage* page = new IndentConfigPage( vbox, widgetName );
    page->setPart( part );
    QObject::connect( parentWidget, SIGNAL(okClicked()),
                      page, SLOT(accept()) );
    return page;
}
*/

void CIndent::updateValues( const QMap<QString, QVariant>& values )
{
    QEditorIndenter::updateValues( values );
    configureCIndent( values );
}
