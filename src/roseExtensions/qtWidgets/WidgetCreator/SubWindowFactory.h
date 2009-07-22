
#ifndef SUBWINDOWFACTORY_H
#define SUBWINDOWFACTORY_H

#include <QMdiArea>

class WidgetCreatorInterface;
class QMenu;

class SubWindowFactory
    : public QMdiArea
{
    Q_OBJECT

    public:
        SubWindowFactory( QWidget *parent = NULL )
            : QMdiArea( parent )
        {}

        virtual ~SubWindowFactory();

        void registerSubWindow( WidgetCreatorInterface *winInterface );

        void fillMenu( QMenu *menu );

    private slots:
        void addSubWindow();

    private:

        QList<WidgetCreatorInterface *> interfaces;
};

#endif
