#include <QRTree.h>

using namespace std;

#define N 40000000

namespace qrs {


TableModel::TableModel(QRTree *tree) {
   _tree = tree;	
   _rowCount = 0;
}
 
int TableModel::rowCount(const QModelIndex &parent) const {
   emit queryRowCount();
   return _rowCount;
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
   if (!index.isValid())
      return QVariant();

   if (index.row() >= _rowCount)
      return QVariant();

   if (role == Qt::DisplayRole) {
      emit queryData(index.row());
      return QVariant(QString(_data.c_str()));
   } else
      return QVariant();   
}

void TableModel::setRowCount(int rowCount) {
   _rowCount = rowCount;
}

void TableModel::setData(std::string data) {
   _data = data;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
   return QVariant();
}

QRTree::QRTree() {
    _model = new TableModel(this);    
    connect(_model, SIGNAL(queryRowCount()), this, SIGNAL(queryRowCount()));
    connect(_model, SIGNAL(queryData(int)), this, SIGNAL(queryData(int)));
    
    setModel(_model);
}


void QRTree::modelReset() {
   _model->reset();
}

void QRTree::setRowCount(int rowCount) {
   _model->setRowCount(rowCount);
}

void QRTree::setData(std::string data) {
   _model->setData(data);
}
   
} // namespace qrs




