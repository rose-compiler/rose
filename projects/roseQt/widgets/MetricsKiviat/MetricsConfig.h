
#ifndef ATTRIBUTE_CONFIG_H
#define ATTRIBTUE_CONFIG_H

#if 0
#include <QDialog>
#include <QModelIndexList>

#include "MetricsKiviat.h"

class QListWidgetItem;

namespace Ui { class MetricsConfig; }

class MetricsConfig : public QDialog
{
   Q_OBJECT

   public:
      //MetricsConfig( const QVector<MetricsKiviat::info>& attributes, QWidget *parent = NULL );

      /*const QVector<MetricsKiviat::info>& attributes()
      {
          return attributes_;
      }*/
      
      int  count();
      int  getAttributeIndex( int );

   private slots:
      void itemChanged( int itemID );
      void normalizeByRange( bool );
      void minChanged( double min );
      void maxChanged( double max );
      void normalizeValueChanged( const QString& normalize );
      void enableItem();
      void disableItem();
      void captionChanged( QListWidgetItem *item );

   protected:
      Ui::MetricsConfig *ui_;
      //QVector<MetricsKiviat::info> attributes_;
      int currentId;
};
#endif

#endif
