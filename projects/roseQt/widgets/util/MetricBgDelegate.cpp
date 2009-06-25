#include "rose.h"

#include "MetricBgDelegate.h"

#include "ItemModelHelper.h"


#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QToolButton>
#include <QLinearGradient>

#ifdef ROSEQT_EXPERIMENTAL
#include "qtgradientdialog.h"
#endif

#include "ui_MetricBgDelegateConfig.h"

#include <QPainter>
#include <QDebug>

MetricBgDelegate::MetricBgDelegate( QObject * par, const QString& configureId, MetricsConfig *globalConfig )
    : QItemDelegate(par),
      configId( configureId ),
      metricsConfig( configureId, globalConfig ),
      metricsName( "<none>" ),
      gradient( QLinearGradient( 0.0, 0.0, 1.0, 0.0 ) ),
      colorMap( 100, 1, QImage::Format_ARGB32 )
{
    /*MetricsConfig::iterator begin( metricsConfig.begin() );
    MetricsConfig::iterator end( metricsConfig.end() );
    if( begin != end )
        metricsInf = *begin;*/

    gradient.setColorAt( 0.0, Qt::white );
    gradient.setColorAt( 1.0, Qt::white );

    QPainter painter( &colorMap );
    painter.fillRect( 0, 0, 100, 1, QBrush( Qt::white ) );
}

QColor MetricBgDelegate::getColor(double val) const
{
    Q_ASSERT(val >= 0 && val<=1);

    /*QColor zero(Qt::white);
    QColor one  = QColor(Qt::yellow).lighter(150);

    double valInv = 1-val;*/

    //qDebug() << one.greenF() << zero.greenF();
    //result.setRedF(val * one.redF() + valInv * zero.redF() );
    //result.setGreenF(val * one.greenF() + valInv * zero.greenF() );
    //result.setBlueF(val * one.blueF() + valInv * zero.blueF() );

    //qDebug() << zero << one <<   "Value " << val << result;

    QRgb pix = colorMap.pixel( (int)(val * 99), 0 );

    QColor result( colorMap.pixel( (int)(val * 99), 0 ) );

    //result.setAlpha( qAlpha( pix ) );

    return result;
}

void MetricBgDelegate::setupConfigWidget( QWidget *parent )
{
#ifdef ROSEQT_EXPERIMENTAL
    Ui::MetricBgDelegateConfig configWidget;

    configWidget.setupUi( parent );

    QtGradientDialog     *gradientDialog( new QtGradientDialog( parent ) );

    connect( configWidget.metricsConfigButton, SIGNAL( clicked( bool ) ),
             &metricsConfig                  , SLOT  ( configureSingle() ) );

    connect( configWidget.gradientButton, SIGNAL( clicked( bool ) ),
             gradientDialog             , SLOT  ( exec() ) );
    connect( gradientDialog, SIGNAL( finished( int ) ),
             this          , SLOT  ( gradientFinished( int ) ) );

    //QPalette p( configWidget.gradientButton->palette() );
    //p.setBrush( QPalette::Background, QBrush( gradient ) );
    //p.setBrush( QPalette::Button, QBrush( gradient ) );
    //configWidget.gradientButton->setPalette( p );
    QPixmap iconPix( 100, 16 );
    QPainter painter( &iconPix );
    painter.fillRect( 0, 0, 100, 100, QBrush( gradient ) );
    configWidget.gradientButton->setIconSize( QSize( 100, 16 ) );
    configWidget.gradientButton->setIcon( QIcon( iconPix ) );

    gradientDialog->setGradient( gradient );

    QComboBox *infoBox( configWidget.infoBox );

    MetricsConfig::iterator itr( metricsConfig.begin() );
    MetricsConfig::iterator end( metricsConfig.end() );
    for( ; itr != end; ++itr )
    {
        infoBox->addItem( itr->caption, itr.name() );
    }

    int bgId = infoBox->findData( metricsName );
    if( bgId == -1 ) bgId = 0;
    infoBox->setCurrentIndex( bgId );
#endif
}

void MetricBgDelegate::applyConfigWidget( QWidget *parent )
{
#ifdef ROSEQT_EXPERIMENTAL
    QComboBox *infoBox( parent->findChild<QComboBox *>() );
    assert( infoBox );
    QtGradientDialog *gradientDialog( parent->findChild<QtGradientDialog *>() );
    assert( gradientDialog );

    if( gradientDialog->result() )
    {
        gradient = gradientDialog->gradient();
        QPainter painter( &colorMap );

        //painter.setBrush( gradient );
        painter.eraseRect( 0, 0, 100, 1 );
        painter.fillRect( 0, 0, 100, 1, QBrush( gradient ) );
    }

    int idx( infoBox->currentIndex() );

    //qDebug() << metricsInf.caption;
    if( idx > 0 )
    {
        metricsName = infoBox->itemData( idx ).toString();
        metricsInf = metricsConfig.getMetricsInfo( metricsName );
    }
#endif
}

void MetricBgDelegate::paint ( QPainter * painter,
                                const QStyleOptionViewItem & option,
                                const QModelIndex & ind ) const
{
    QVariant bgRole = ind.data(Qt::BackgroundRole);
    if(qVariantCanConvert<QBrush>(bgRole))
    {
        QItemDelegate::paint(painter,option,ind);
        return;
    }

    QColor colorRowAbove;
    QColor colorThisRow;
    QColor colorRowBelow;

    // Middle Row
    double val=metricsInf.eval( qvariant_cast<SgNode*>(ind.data(SgNodeRole)), metricsName );
    colorThisRow = getColor(val);

    // Row above
    if(ind.row()>0)
    {
        QModelIndex indAbove = ind.model()->index(ind.row()-1,ind.column(),ind.parent());
        double val=metricsInf.eval( qvariant_cast<SgNode*>(indAbove.data(SgNodeRole)) ,metricsName );
        colorRowAbove = getColor(val);
    }
    else
        colorRowAbove=colorThisRow;

    // Row below
    if(ind.row()+1 < ind.model()->rowCount(ind.parent()))
    {
        QModelIndex indBelow = ind.model()->index(ind.row()+1,ind.column(),ind.parent());
        double val=metricsInf.eval( qvariant_cast<SgNode*>(indBelow.data(SgNodeRole)), metricsName );
        colorRowBelow = getColor(val);
    }
    else
        colorRowBelow=colorThisRow;

    float halfHeight = option.rect.height() /2;
    QPointF gradStart=option.rect.topLeft();
    gradStart.ry() -= halfHeight;

    QPointF gradStop =option.rect.bottomLeft();
    gradStop.ry() += halfHeight;

    QLinearGradient grad( gradStart, gradStop);
    grad.setColorAt(0,colorRowAbove);
    grad.setColorAt(0.5,colorThisRow);
    grad.setColorAt(1,colorRowBelow);

    painter->fillRect(option.rect, QBrush(grad));

    if (option.showDecorationSelected && (option.state & QStyle::State_Selected))
    {
            QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                                      ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
                cg = QPalette::Inactive;


            QBrush b = option.palette.brush(cg, QPalette::Highlight);
            QColor hlColor= b.color();
            hlColor.setAlpha(0);
            hlColor = QColor(0,0,255,10);
            //painter->fillRect(option.rect, QBrush(hlColor));
    }

    //Setup Options (copied from qt-source)
    QStyleOptionViewItem opt = option;
    {
        // set font
        QVariant value = ind.data(Qt::FontRole);
        if (value.isValid()){
            opt.font = qvariant_cast<QFont>(value).resolve(opt.font);
            opt.fontMetrics = QFontMetrics(opt.font);
        }

        // set text alignment
        value = ind.data(Qt::TextAlignmentRole);
        if (value.isValid())
            opt.displayAlignment = (Qt::Alignment)value.toInt();

        // set foreground brush
        value = ind.data(Qt::ForegroundRole);
        if (qVariantCanConvert<QBrush>(value))
            opt.palette.setBrush(QPalette::Text, qvariant_cast<QBrush>(value));
    }

    QString text = qvariant_cast<QString>(ind.data(Qt::DisplayRole));
    drawDisplay(painter,opt,opt.rect, text );
}

void MetricBgDelegate::gradientFinished( int result )
{
#ifdef ROSEQT_EXPERIMENTAL
    if( result != QDialog::Accepted || sender() == NULL )
        return;

    QObject *parent = sender()->parent();
    if( parent == NULL ) return;

    QPushButton *gradientButton( parent->findChild<QPushButton *>( "gradientButton" ) );
    if( gradientButton == NULL ) return;

    QtGradientDialog *gradientDialog = dynamic_cast<QtGradientDialog *>( sender() );
    if( gradientDialog == NULL ) return;

    QPixmap iconPix( 100, 16 );
    QPainter painter( &iconPix );
    painter.fillRect( 0, 0, 100, 100, QBrush( gradientDialog->gradient() ) );
    gradientButton->setIconSize( QSize( 100, 16 ) );
    gradientButton->setIcon( QIcon( iconPix ) );
#endif
}
