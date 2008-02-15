#include "linenumberwidget.h"
#include "qeditor.h"
#include "paragdata.h"

#include <private/qrichtext_p.h>

LineNumberWidget::LineNumberWidget( QEditor* editor, QWidget* parent, const char* name )
	: QWidget( parent, name, WRepaintNoErase | WStaticContents | WResizeNoErase ),
	  m_editor( editor )
{
	setFixedWidth( 50 );

	connect( m_editor->verticalScrollBar(), SIGNAL( valueChanged( int ) ),
			 this, SLOT( doRepaint() ) );
	connect( m_editor, SIGNAL( textChanged() ),
			 this, SLOT( doRepaint() ) );
        doRepaint();
}

LineNumberWidget::~LineNumberWidget()
{
}

void LineNumberWidget::paintEvent( QPaintEvent* /*e*/ )
{
	buffer.fill();

	QTextParagraph *p = m_editor->document()->firstParagraph();
	QPainter painter( &buffer );
	int yOffset = m_editor->contentsY();
	while ( p ) {
		if ( !p->isVisible() ) {
			p = p->next();
			continue;
		}
		if ( p->rect().y() + p->rect().height() - yOffset < 0 ) {
			p = p->next();
			continue;
		}
		if ( p->rect().y() - yOffset > height() )
			break;
		//ParagData *paragData = (ParagData*)p->extraData();

		painter.drawText( 0, p->rect().y() - yOffset,
				  buffer.width() - 10, p->rect().height(),
				  AlignRight | AlignBottom,
				  QString::number(p->paragId()+1) );
		p = p->next();
	}

	painter.end();
	bitBlt( this, 0, 0, &buffer );
}

void LineNumberWidget::resizeEvent( QResizeEvent *e )
{
	buffer.resize( e->size() );
	QWidget::resizeEvent( e );
}

#include "linenumberwidget.moc"
