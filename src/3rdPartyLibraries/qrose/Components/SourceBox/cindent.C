  /**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file COPYING included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "cindent.h"
#include "qregexp.h"
#include "qeditor.h"
#include <Qt3Support>
#include <QtGui>
#include <QtCore>

using namespace std;

namespace qrs {

extern int indentForBottomLine( const QStringList& program, QChar typedIn );
extern void configureCIndent( const QMap<QString, QVariant>& values );

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

}
