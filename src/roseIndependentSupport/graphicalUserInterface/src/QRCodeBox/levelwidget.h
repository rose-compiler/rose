#ifndef __levelwidget_h
#define __levelwidget_h

#include <qwidget.h>
#include <qpixmap.h>

class QEditor;
class QTextParagraph;

class LevelWidget: public QWidget
{
    Q_OBJECT
public:
    LevelWidget( QEditor*, QWidget* =0, const char* =0 );
    virtual ~LevelWidget();
    
signals:
    void expandBlock( QTextParagraph* );
    void collapseBlock( QTextParagraph* );

public slots:
    void doRepaint() { repaint( false ); }

protected:
    virtual void resizeEvent( QResizeEvent* );
    virtual void paintEvent( QPaintEvent* );
    virtual void mousePressEvent( QMouseEvent* );

private:
    QEditor* m_editor;
    QPixmap buffer;
};

#endif // __levelwidget_h
