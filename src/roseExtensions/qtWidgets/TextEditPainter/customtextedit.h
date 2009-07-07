#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>

class QPainter;
class CustomTextEdit : public QTextEdit
{
    Q_OBJECT
    public:
        CustomTextEdit(QWidget * parent=0);
        virtual ~CustomTextEdit() {}


        void setArrowBegin(const QTextCursor & c1)  { cursor1 = c1;  viewport()->update();}
        void setArrowEnd  (const QTextCursor & c2)  { cursor2 = c2; viewport()->update(); }


    public slots:
        void setCurvature(double newCurv)    { curv=newCurv; viewport()->update(); }

    protected:
        virtual void paintEvent (QPaintEvent * pEv);

        void drawArrowBezier(const QPoint & p1, const QPoint & p2, QPainter * painter, float curvature);


    private:
        void normalizeVec(QPointF & vec) const;
        QPointF getOrtho(const QPointF & input,bool direction=true) const;


        QTextCursor cursor1;
        QTextCursor cursor2;

        float curv;
};

#endif
