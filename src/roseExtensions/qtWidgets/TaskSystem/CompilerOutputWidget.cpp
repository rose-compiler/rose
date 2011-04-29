
#include "CompilerOutputWidget.h"

#include <QDir>
#include <QKeyEvent>
#include <QHeaderView>
#include <QListView>
#include <QPainter>
#include <QAbstractItemModel>
#include <QApplication>
#include <QClipboard>
#include <QFont>
#include <QFontMetrics>
#include <QTextLayout>



class TaskModel;


class TaskView : public QListView
{
    public:
        TaskView(QWidget *parent = 0);
        ~TaskView();
        void resizeEvent(QResizeEvent *e);
        void keyPressEvent(QKeyEvent *e);
};



class TaskModel : public QAbstractItemModel
{
    public:

        typedef CompilerOutputWidget::PatternType PatternType;
        struct TaskItem
        {
            QString description;
            QString file;
            int line;
            bool fileNotFound;
            PatternType type;
        };


        // Model stuff
        TaskModel();
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &child) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        void clear();
        void addTask(PatternType type, const QString &description, const QString &file, int line);
        int sizeOfFile();
        int sizeOfLineNumber();
        QModelIndex firstError() const;
        void setFileNotFound(const QModelIndex &index, bool b);

        enum Roles { File = Qt::UserRole, Line, Description, FileNotFound, Type }; /*NO_STRINGIFY*/

        QIcon iconFor(PatternType type);

    private:
        QList<TaskItem> m_items;
        int m_maxSizeOfFileName;
        QIcon m_errorIcon;
        QIcon m_warningIcon;
        QIcon m_unspecifiedIcon;
};





////
//  TaskView
////

TaskView::TaskView(QWidget *parent)
    : QListView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

TaskView::~TaskView()
{

}

void TaskView::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    static_cast<TaskDelegate *>(itemDelegate())->emitSizeHintChanged(selectionModel()->currentIndex());
}

void TaskView::keyPressEvent(QKeyEvent *e)
{
    if (!e->modifiers() && e->key() == Qt::Key_Return) {
        emit activated(currentIndex());
        e->accept();
        return;
    }
    QListView::keyPressEvent(e);
}

/////
// TaskModel
/////

TaskModel::TaskModel()
{
    m_maxSizeOfFileName = 0;
    m_errorIcon = QIcon(":/images/compile_error.png");
    m_warningIcon = QIcon(":/images/compile_warning.png");
    m_unspecifiedIcon = QIcon(":/images/compile_unspecified.png");
}

void TaskModel::addTask(PatternType type, const QString &description, const QString &file, int line)
{
    TaskItem task;
    task.description = description;
    task.file = file;
    task.line = line;
    task.type = type;
    task.fileNotFound = false;

    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.append(task);
    endInsertRows();

    QFont font;
    QFontMetrics fm(font);
    QString filename = task.file;
    int pos = filename.lastIndexOf("/");
    if (pos != -1)
        filename = file.mid(pos +1);
    m_maxSizeOfFileName = qMax(m_maxSizeOfFileName, fm.width(filename));
}

void TaskModel::clear()
{
    if (m_items.isEmpty())
        return;
    beginRemoveRows(QModelIndex(), 0, m_items.size() -1);
    m_items.clear();
    endRemoveRows();
    m_maxSizeOfFileName = 0;
}


QModelIndex TaskModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row, column, 0);
}

QModelIndex TaskModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.count();
}

int TaskModel::columnCount(const QModelIndex &parent) const
{
        return parent.isValid() ? 0 : 1;
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size() || index.column() != 0)
        return QVariant();

    if (role == TaskModel::File)
        return m_items.at(index.row()).file;
    else if (role == TaskModel::Line)
        return m_items.at(index.row()).line;
    else if (role == TaskModel::Description)
        return m_items.at(index.row()).description;
    else if (role == TaskModel::FileNotFound)
        return m_items.at(index.row()).fileNotFound;
    else if (role == TaskModel::Type)
        return (int)m_items.at(index.row()).type;
    return QVariant();
}

QIcon TaskModel::iconFor(PatternType type)
{
    if (type == CompilerOutputWidget::Error)
        return m_errorIcon;
    else if (type == CompilerOutputWidget::Warning)
        return m_warningIcon;
    else
        return m_unspecifiedIcon;
}

int TaskModel::sizeOfFile()
{
    return m_maxSizeOfFileName;
}

int TaskModel::sizeOfLineNumber()
{
    QFont font;
    QFontMetrics fm(font);
    return fm.width("8888");
}

QModelIndex TaskModel::firstError() const
{
    int size = m_items.size();
    for (int i=0; i<size; ++i) {
        if (m_items.at(i).type == CompilerOutputWidget::Error) {
            return index(i, 0);
        }
    }
    return QModelIndex();
}

void TaskModel::setFileNotFound(const QModelIndex &idx, bool b)
{
    if (idx.isValid() && idx.row() < m_items.size()) {
        m_items[idx.row()].fileNotFound = b;
        emit dataChanged(idx, idx);
    }
}

/////
// TaskWindow
/////

#include <QVBoxLayout>

CompilerOutputWidget::CompilerOutputWidget(QWidget * par)
    : QWidget(par)
{
    m_model = new TaskModel;
    m_listview = new TaskView(this);


    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(m_listview);

    m_listview->setModel(m_model);
    m_listview->setWindowTitle(tr("Compiler Output"));
    m_listview->setSelectionMode(QAbstractItemView::SingleSelection);
    TaskDelegate *tld = new TaskDelegate(this);
    m_listview->setItemDelegate(tld);
    //m_listview->setWindowIcon(QIcon(":/qt4projectmanager/images/window.png"));
    m_listview->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_listview->setAttribute(Qt::WA_MacShowFocusRect, false);


    connect(m_listview->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            tld, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));

    connect(m_listview, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(showTaskInFile(const QModelIndex &)));
    connect(m_listview, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(showTaskInFile(const QModelIndex &)));


    m_errorCount = 0;
    m_currentTask = -1;
}

CompilerOutputWidget::~CompilerOutputWidget()
{
    delete m_model;
}


void CompilerOutputWidget::clearContents()
{
    m_errorCount = 0;
    m_currentTask = -1;
    m_model->clear();
    emit tasksChanged();
}



void CompilerOutputWidget::addItem(CompilerOutputWidget::PatternType type,
                         const QString &description, const QString &file, int line)
{
    m_model->addTask(type, description, file, line);
    if (type == CompilerOutputWidget::Error)
        ++m_errorCount;
    emit tasksChanged();
}

void CompilerOutputWidget::showTaskInFile(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QString file = index.data(TaskModel::File).toString();
    int line = index.data(TaskModel::Line).toInt();
    if (file.isEmpty() || line == -1)
        return;

    QFileInfo fi(file);
    if (fi.exists()) {
        emit taskClicked(file,line);
    }
    else
        m_model->setFileNotFound(index, true);

    m_listview->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    m_listview->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void CompilerOutputWidget::copy()
{
    QModelIndex index = m_listview->selectionModel()->currentIndex();
    QString file = index.data(TaskModel::File).toString();
    QString line = index.data(TaskModel::Line).toString();
    QString description = index.data(TaskModel::Description).toString();
    QString type;
    switch (index.data(TaskModel::Type).toInt()) {
    case CompilerOutputWidget::Error:
        type = "error: ";
        break;
    case CompilerOutputWidget::Warning:
        type = "warning: ";
        break;
    }

    QApplication::clipboard()->setText(file + ':' + line + ": " + type + description);
}

int CompilerOutputWidget::numberOfTasks() const
{
    return m_model->rowCount(QModelIndex());
}

int CompilerOutputWidget::numberOfErrors() const
{
    return m_errorCount;
}

void CompilerOutputWidget::gotoFirstError()
{
    QModelIndex idx = m_model->firstError();
    if (idx.isValid())
        showTaskInFile(idx);
}


/////
// Delegate
/////

TaskDelegate::TaskDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

TaskDelegate::~TaskDelegate()
{
}

QSize TaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    QFontMetrics fm(option.font);
    int fontHeight = fm.height();
    int fontLeading = fm.leading();

    QSize s;
    s.setWidth(option.rect.width());
    const QAbstractItemView * view = qobject_cast<const QAbstractItemView *>(opt.widget);
    TaskModel *model = static_cast<TaskModel *>(view->model());
    int width = opt.rect.width() - model->sizeOfFile() - model->sizeOfLineNumber() - 12 - 22;
    if (view->selectionModel()->currentIndex() == index) {
        QString description = index.data(TaskModel::Description).toString();
        // Layout the description
        int leading = fontLeading;
        int height = 0;
        QTextLayout tl(description);
        tl.beginLayout();
        while (true) {
            QTextLine line = tl.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(width);
            height += leading;
            line.setPosition(QPoint(0, height));
            height += static_cast<int>(line.height());
        }
        tl.endLayout();

        s.setHeight(height + leading + fontHeight + 3);
    } else {
        s.setHeight(fontHeight + 3);
    }
    return s;
}

void TaskDelegate::emitSizeHintChanged(const QModelIndex &index)
{
    emit sizeHintChanged(index);
}

void TaskDelegate::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit sizeHintChanged(current);
    emit sizeHintChanged(previous);
}

void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setRenderHint(QPainter::TextAntialiasing,true);

    QFontMetrics fm(opt.font);
    QColor backgroundColor;
    QColor textColor;


    const QAbstractItemView * view = qobject_cast<const QAbstractItemView *>(opt.widget);
    bool selected = view->selectionModel()->currentIndex() == index;

    if (selected) {
        painter->setBrush(opt.palette.highlight().color());
        backgroundColor = opt.palette.highlight().color();
    } else {
        painter->setBrush(opt.palette.background().color());
        backgroundColor = opt.palette.background().color();
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(opt.rect);

    // Set Text Color
    if (selected)
        textColor = opt.palette.highlightedText().color();
    else
        textColor = opt.palette.text().color();

    painter->setPen(textColor);

    TaskModel *model = static_cast<TaskModel *>(view->model());
    CompilerOutputWidget::PatternType type = CompilerOutputWidget::PatternType(index.data(TaskModel::Type).toInt());
    QIcon icon = model->iconFor(type);
    painter->drawPixmap(2, opt.rect.top() + 2, icon.pixmap(16, 16));

    int width = opt.rect.width() - model->sizeOfFile() - model->sizeOfLineNumber() - 12 - 22;
    if (!selected) {
        // in small mode we lay out differently
        QString bottom = index.data(TaskModel::Description).toString();
        painter->drawText(22, 2 + opt.rect.top() + fm.ascent(), bottom);
        if (fm.width(bottom) > width) {
            // draw a gradient to mask the text
            int gwidth = opt.rect.right() - width;
            QLinearGradient lg(QPoint(width, 0), QPoint(width+gwidth, 0));
            QColor c = backgroundColor;
            c.setAlpha(0);
            lg.setColorAt(0, c);
            lg.setColorAt(20.0/gwidth, backgroundColor);
            painter->fillRect(width, 2 + opt.rect.top(), gwidth, fm.height() + 1, lg);
        }
    } else {
        // Description
        QString description = index.data(TaskModel::Description).toString();
        // Layout the description
        int leading = fm.leading();
        int height = 0;
        QTextLayout tl(description);
        tl.beginLayout();
        while (true) {
            QTextLine line = tl.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(width);
            height += leading;
            line.setPosition(QPoint(0, height));
            height += static_cast<int>(line.height());
        }
        tl.endLayout();
        tl.draw(painter, QPoint(22, 2 + opt.rect.top()));
        //painter->drawText(22, 2 + opt.rect.top() + fm.ascent(), description);

        QColor mix;
        mix.setRgb( static_cast<int>(0.7 * textColor.red()   + 0.3 * backgroundColor.red()),
                static_cast<int>(0.7 * textColor.green() + 0.3 * backgroundColor.green()),
                static_cast<int>(0.7 * textColor.blue()  + 0.3 * backgroundColor.blue()));
        painter->setPen(mix);

        QString directory = index.data(TaskModel::File).toString();
        int secondBaseLine = 2 + fm.ascent() + opt.rect.top() + height + leading; //opt.rect.top() + fm.ascent() + fm.height() + 6;
        if (index.data(TaskModel::FileNotFound).toBool()) {
            QString fileNotFound = tr("File not found: %1").arg(directory);
            painter->setPen(Qt::red);
            painter->drawText(22, secondBaseLine, fileNotFound);
        } else {
            painter->drawText(22, secondBaseLine, directory);
        }
    }

    painter->setPen(textColor);
    // Assemble string for the right side
    // just filename + linenumer
    QString file = index.data(TaskModel::File).toString();
    int pos = file.lastIndexOf("/");
    if (pos != -1)
        file = file.mid(pos +1);
    painter->drawText(width + 22 + 4, 2 + opt.rect.top() + fm.ascent(), file);

    QString topRight = index.data(TaskModel::Line).toString();
    painter->drawText(opt.rect.right() - fm.width(topRight) - 6 , 2 + opt.rect.top() + fm.ascent(), topRight);
    // Separator lines
    painter->setPen(QColor::fromRgb(150,150,150));
    painter->drawLine(0, opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
    painter->restore();
}



