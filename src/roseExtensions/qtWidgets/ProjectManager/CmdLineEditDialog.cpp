#include "CmdLineEditDialog.h"

#include "ui_CmdLineEditDialog.h"

CmdLineEditDialog::CmdLineEditDialog(QWidget * par)
    : QDialog(par)
{
    ui = new Ui::CmdLineEditDialog();
    ui->setupUi(this);
}


void CmdLineEditDialog::setCurrentCmdLine(const QStringList & l)
{
    ui->listWidget->clear();
    foreach(const QString & element, l)
        ui->listWidget->addItem(new QListWidgetItem(element));
}

QStringList CmdLineEditDialog::getResult()
{
    QStringList res;
    for(int i=0; i< ui->listWidget->count(); i++)
        res << ui->listWidget->item(i)->text();

    return res;
}

void CmdLineEditDialog::on_cmdAdd_clicked()
{
    ui->listWidget->addItem(new QListWidgetItem(ui->lineEdit->text()));
}

void CmdLineEditDialog::on_cmdRemove_clicked()
{
    qDeleteAll( ui->listWidget->selectedItems());
}
