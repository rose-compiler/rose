
#ifndef CMDLINEEDITDIALOG_H
#define CMDLINEEDITDIALOG_H

#include <QDialog>
#include <QStringList>


namespace Ui { class CmdLineEditDialog; }

/**
 * \brief Simple Line-Editor for editing CommandLineOptions
 *
 *  Used by ProjectManager to set the command-line parameters for the ROSE frontend
 */
class CmdLineEditDialog  : public QDialog
{
    Q_OBJECT

    public:
        CmdLineEditDialog(QWidget * parent=NULL);


        void setCurrentCmdLine(const QStringList & l);

        QStringList getResult();

    protected slots:
        void on_cmdAdd_clicked();
        void on_cmdRemove_clicked();

    protected:

        Ui::CmdLineEditDialog * ui;
};


#endif
