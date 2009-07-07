
#ifndef COMPILER_OUTPUT_WIDGET
#define COMPILER_OUTPUT_WIDGET


#include <QTreeWidget>
#include <QItemDelegate>
#include <QListView>
#include <QToolButton>


class TaskModel;
class TaskView;


/**
 * Widget which shows error/warnings/messages
 * adapted from Qt-Creators TaskWidget
 */
class CompilerOutputWidget : public QWidget
{
    Q_OBJECT

    public:
        enum PatternType { Unknown, Warning, Error };


        CompilerOutputWidget(QWidget * par = NULL);
        ~CompilerOutputWidget();

        void clearContents();

        void addItem(PatternType type,const QString &description, const QString &file, int line);

        int numberOfTasks() const;
        int numberOfErrors() const;

        void gotoFirstError();

    signals:
        void taskClicked(const QString & file, int line);
        void tasksChanged();

    private slots:
        void showTaskInFile(const QModelIndex &index);
        void copy();

    private:
        int sizeHintForColumn(int column) const;

        int m_errorCount;
        int m_currentTask;

        TaskModel *m_model;
        TaskView *m_listview;
};

#include <QStyledItemDelegate>

class TaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
        TaskDelegate(QObject * parent = 0);
        ~TaskDelegate();
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

        // TaskView uses this method if the size of the taskview changes
        void emitSizeHintChanged(const QModelIndex &index);

    public slots:
        void currentChanged(const QModelIndex &current, const QModelIndex &previous);

    private:
        void generateGradientPixmap(int width, int height, QColor color, bool selected) const;
};



#endif
