#ifndef __linenumberwidget_h
#define __linenumberwidget_h

#include <qwidget.h>
#include <qpixmap.h>

class QEditor;

class LineNumberWidget: public QWidget
{
    Q_OBJECT
public:
    LineNumberWidget( QEditor*, QWidget* =0, const char* =0 );
    virtual ~LineNumberWidget();

public slots:
    void doRepaint() { repaint( false ); }

protected:
    virtual void resizeEvent( QResizeEvent* );
    virtual void paintEvent( QPaintEvent* );

private:
    QEditor* m_editor;
    QPixmap buffer;
};

#endif // __linenumberwidget_h
