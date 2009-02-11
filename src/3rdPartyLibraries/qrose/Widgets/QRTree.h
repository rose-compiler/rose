/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho                                   
    jgfc@doc.ic.ac.uk                                                     
    Class: QRTree     
    Description: 
 ***************************************************************************/

#ifndef QRTREE_H
#define QRTREE_H

#include <QRMacros.h>
#include <QtGui>

namespace qrs {

class QRTree;

 class TableModel : public QAbstractListModel {
	Q_OBJECT

 friend class QRTree;

 public:
     TableModel(QRTree *tree);
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     QVariant data(const QModelIndex &index, int role) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;

signals:
   void queryRowCount() const;
   void queryData(int) const;

protected:
   void setRowCount(int rowCount);
   void setData(std::string data);

protected:
   QRTree *_tree;
   int _rowCount;
   std::string _data;
 };


class QRTree: public QTableView {

   Q_OBJECT
   
public:
   QRTree();
   void modelReset();
   
   void setRowCount(int rowCount);
   void setData(std::string data);

signals:
   void queryRowCount();
   void queryData(int);

protected:
   TableModel *_model;
};

} // namespace qrs
#endif
