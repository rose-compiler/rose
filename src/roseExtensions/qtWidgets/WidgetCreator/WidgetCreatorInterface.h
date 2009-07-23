
#ifndef WINDOW_INTERFACE_H
#define WINDOW_INTERFACE_H

#include <QObject>
#include <QAction>
#include <QString>
#include <QIcon>

class WidgetCreatorInterface
    : public QObject
{
    public:
        WidgetCreatorInterface( const QIcon &icon, const QString &text )
            : create_action( new QAction( icon, text, 0 ) ),
              counter( 0 )
        {}

        virtual void loadState()
        {}
        virtual void saveState()
        {}
        
        QString getName() const
        { return create_action->text(); }
        QIcon   getIcon() const
        { return create_action->icon(); }

        QAction *createAction() const
        { return create_action; }


        QWidget *addWidget( QWidget *parent = NULL );

    protected:
        virtual QWidget *createWidget( QWidget *parent = NULL ) = 0;

        QAction *create_action;

        size_t counter;
};

#endif
