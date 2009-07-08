#include "KiviatView.h"

#include <QRadialGradient>
#include <QPainter>
#include <QDebug>
#include <QBrush>

#include <QGraphicsPolygonItem>
#include <QMouseEvent>

#include <cmath>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif


// ------------------------- KIVIAT DATA ------------------------------------

class KiviatView::KiviatData
{
   public:
      KiviatData(const KiviatView * view, const QVector<float>  _data, QColor color);
      ~KiviatData();

      /// Active Items are displayed on top and non-transparent
      void setActive(bool active);

      QGraphicsPolygonItem * getPolygon() const  { return polygon; }
      const QVector<float> & getData()    const  { return data;    }

      void setToolTip( int axisNr, const QString& string )
      { cornerPoints[axisNr]->setToolTip( string ); }

      void changeDataPoint(int axisNr, float newValue);

      QColor getColor()
      {
          return polygonColor;
      }

      void setColor( const QColor& color )
      {
          polygonColor = color;
      }

      void redraw();

      void addToScene(QGraphicsScene & scene)    { scene.addItem(polygon); }

      /// Changes dimension of data-array (if axisCount changed)
      /// if dim is increased the values are filled with zeros
      void resizeData(int newSize);

   protected:
      const KiviatView * view;

      QColor polygonColor;

      /// data polygon
      QGraphicsPolygonItem * polygon;

      /// black dots on intersection of axis and polygon
      QVector<QGraphicsEllipseItem*> cornerPoints;

      /// using float fix here (not templated) because there is only
      /// QPolygonF , so double makes no sense
      QVector<float> data;

      /// All Items in the Scene have a Z-Value, highest Z-Value is displayed on top
      /// these are the zValue of active/inactive data-polygons
      static const int ACTIVE_ZVALUE;
      static const int INACTIVE_ZVALUE;
};

const int KiviatView::KiviatData::ACTIVE_ZVALUE   = 5;
const int KiviatView::KiviatData::INACTIVE_ZVALUE = 4;

KiviatView::KiviatData::KiviatData(const KiviatView * _view, const QVector<float>  _data, QColor color)
   : view(_view), polygon(NULL), data(_data)
{
   polygonColor=color;

   polygon = new QGraphicsPolygonItem();

   redraw();
}

KiviatView::KiviatData::~KiviatData()
{
   // cornerPoints are children of polygon
   // --> are deleted automatically
   delete polygon;
}


void KiviatView::KiviatData::resizeData(int newSize)
{
   data.resize(newSize);
   redraw();
}


void KiviatView::KiviatData::redraw()
{
   qDeleteAll(cornerPoints);
   cornerPoints.clear();
   cornerPoints.resize(data.size());
   cornerPoints.fill(NULL);


   QPolygonF polygonF;
   for(int i=0; i< view->getAxisCount(); i++)
   {
      Q_ASSERT(data[i]>=0 && data[i] <=1.0);

      QPointF curPoint = view->calcPosition(i,data[i]);
      polygonF << curPoint;

      int rad=view->cornerPointRadius();
      QRectF circleRect(0,0,rad,rad);
      circleRect.moveCenter(curPoint);

      cornerPoints[i]= new QGraphicsEllipseItem (circleRect,polygon);
      cornerPoints[i]->setBrush(QBrush(Qt::black));
   }
   polygon->setPolygon(polygonF);
   polygon->setBrush(QBrush(polygonColor));

   /*
   //Shadow:
   polygon.translate(6,6);
   QGraphicsPolygonItem * shadow =  scene.addPolygon(polygon);
   shadow->setBrush(QBrush(QColor(0,0,0,100)));
   polygon.translate(-6,-6);
   */
}

void KiviatView::KiviatData::setActive(bool active)
{
   //Change Alpha Value of polygon and corner points
   int alpha = active ? view->activeAlpha() : view->inactiveAlpha();

   QColor before=polygon->brush().color();
   before.setAlpha(alpha);
   polygon->setBrush(QBrush(before));

   for(int i=0; i<cornerPoints.size(); i++)
      cornerPoints[i]->setBrush(QColor(0,0,0,alpha));


   // Change zOrder of Polygon
   // (not need to set for the points, they are children of polygon)
   polygon->setZValue(active ? ACTIVE_ZVALUE: INACTIVE_ZVALUE);
}


void KiviatView::KiviatData::changeDataPoint(int axisNr, float newValue)
{
   data[axisNr]=newValue;

   QPointF newPoint = view->calcPosition(axisNr,newValue);


   //Adjust corner point
   int rad=view->cornerPointRadius();
   QRectF circleRect(0,0,rad,rad);
   circleRect.moveCenter(newPoint);
   cornerPoints[axisNr]->setRect(circleRect);

   //Adjust polygon
   QPolygonF fPoly = polygon->polygon();
   fPoly[axisNr]=newPoint;
   polygon->setPolygon(fPoly);
}



// ------------------------- KIVIAT VIEW ------------------------------------

const int KiviatView::AXIS_LENGTH = 300;

KiviatView::KiviatView( QWidget * p, int _axisCount)
   : QGraphicsView(p),
     axisCount(_axisCount)
{
   //Connect Scene to View
   setScene(&scene);

   //Turn of scrollbars ( the kiviat-diagram is resized, see also resizeEvent)
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   //Init Properties with default values
   prop_markerCount=5;
   prop_activeAlpha = 220;
   prop_inactiveAlpha= 80;
   prop_cornerPointRadius=10;
   prop_angleOffset=0;

   curDataItem=-1;

   labelFont.setPointSize(labelFont.pointSize()*1.5);

   setRenderHint(QPainter::Antialiasing,true);
   redrawAll();
};


void KiviatView::redrawData()
{
   for(int i=0; i < dataset.size(); i++)
   {
      dataset[i]->redraw();
      dataset[i]->setActive(curDataItem==i);
   }
}

void KiviatView::removeData(int dataHandle)
{
   Q_ASSERT(dataHandle >= 0 &&
         dataHandle < dataset.size());

   if(curDataItem==dataHandle)
      curDataItem=-1;

   delete dataset[dataHandle];
   dataset.remove(dataHandle);
}

void KiviatView::setDataPoint(int dataHandle, int axisNr, float newValue)
{
   Q_ASSERT(newValue>=0 && newValue<=1);
   dataset[dataHandle]->changeDataPoint(axisNr,newValue);
}


void KiviatView::setMarkerCount(int newMarkerCount)
{
   prop_markerCount=newMarkerCount;

   //Delete old markers
   qDeleteAll(markers);
   markers.clear();


   markers.reserve(prop_markerCount);

   float frac=1.0/prop_markerCount;
   for(int i=1; i<=prop_markerCount; i++)
      markers.push_back(addMarker(i*frac));

}

QGraphicsPolygonItem * KiviatView::addMarker(float position)
{
   QPolygonF polygon;

   for(int i=0; i<axisCount; i++)
      polygon << calcPosition(i,position);

   QGraphicsPolygonItem * polyItem = scene.addPolygon(polygon);
   polyItem->setZValue(0);

   return polyItem;
}

void KiviatView::setupAxesCaptions()
{
   int curCount=axisLabels.size();
   int newCount=axisCount;
   if(curCount==newCount)
      return;

   //remove old ones
   for(int i=curCount-1; i>=newCount; i--)
      delete axisLabels[i];

   axisLabels.resize(newCount);

   //add new ones
   for(int i=curCount; i<newCount; i++)
      axisLabels[i] = scene.addText(QString("Axis %1").arg(i),labelFont);

}

void KiviatView::redrawAll()
{
   //Delete markers and axes,keep the captions
   qDeleteAll(markers);
   qDeleteAll(axes);
   markers.clear();
   axes.clear();

   setupAxesCaptions();

   //Paint Axes and adjust label position
   for(int i=0; i<axisCount; i++)
   {
      //Draw the axis-line
      QGraphicsLineItem * line= scene.addLine(QLineF(QPointF(0,0),calcPosition(i,1)));
      line->setZValue(1);
      axes.push_back(line);

      // Why Position 1.08?  Pos 1.0 means end of axis
      // label is positioned a litte bit further away from center
      // also needed in setAxisLabel
      QPointF lPos = calcPosition(i, 1.08);
      axisLabels[i]->setPos(QPoint(0,0));
      axisLabels[i]->moveBy(lPos.x(),lPos.y());

      // If the axis is on the left side, the text has to be moved to the left
      if( axisLabels[i]->pos().x()<0 )
         axisLabels[i]->moveBy(- axisLabels[i]->boundingRect().width(),0);
   }

   //Call setMarkerCount with current count, to paint them
   setMarkerCount(prop_markerCount);

   redrawData();

}
void KiviatView::setAxisCount(int newAxisCount)
{
   if( newAxisCount < 0 )
      return;

   //Deletes the data
   axisCount=newAxisCount;

   foreach(KiviatData * d, dataset)
   {
      d->resizeData(axisCount);
   }

   redrawAll();
}
void KiviatView::removeAxis( int axisID )
{
   if( axisID < 0 || axisID >= axisCount )
      return;

   --axisCount;
   dataset.remove( axisID );

   foreach(KiviatData * d, dataset)
   {
      d->resizeData(axisCount);
   }

   redrawAll();
}

void KiviatView::clear()
{
   curDataItem=-1;

   //calls delete for every item in container
   qDeleteAll(dataset);
   dataset.clear();

   emit cleared();
}

void KiviatView::resizeEvent(QResizeEvent * ev)
{
   // zoom the view according to widget size
   //float radius=1.3*AXIS_LENGTH;
   //fitInView(-radius,-radius,2*radius,2*radius,Qt::KeepAspectRatio);
   fitInView( scene.sceneRect(), Qt::KeepAspectRatio );

   QGraphicsView::resizeEvent( ev );
}


void KiviatView::mousePressEvent(QMouseEvent *ev)
{
   QGraphicsPolygonItem * item = dynamic_cast<QGraphicsPolygonItem*> (itemAt(ev->pos()));

   if(item==NULL)
      return;

   for(int i=0; i<dataset.size(); i++)
   {
      if(dataset[i]->getPolygon() == item)
      {
         emit clickedOnData(i);
         break;
      }
   }

   QGraphicsView::mousePressEvent(ev);
}

int KiviatView::addData(const QVector<float> & dat, QColor color)
{
   Q_ASSERT(dat.size() == axisCount);

   dataset.push_back( new KiviatData(this,dat,color) );
   dataset.back()->addToScene(scene);

   //Make the newly added to the active item
   setActiveItem(dataset.size()-1);

   return dataset.size()-1;
}

int KiviatView::addData(const QVector<int> & dat, int valToNormalize, QColor color )
{
   QVector<float> fDat (dat.size(), 0);
   for(int i=0; i<dat.size(); i++)
   {
      fDat[i]=dat[i]/valToNormalize;
      Q_ASSERT(fDat[i]>=0 && fDat[i]<=1.0);
   }
   return addData(fDat,color);
}

void KiviatView::setDataColor( int dataHandle, const QColor& color )
{
    if( dataset.size() == 0 ) return;

    dataset[dataHandle]->setColor( color );
    redrawData();
}

QColor KiviatView::getDataColor( int dataHandle )
{
    return dataset[dataHandle]->getColor( );
}

void KiviatView::setAxisLabel(int axisNr, const QString & name)
{
   Q_ASSERT(axisNr>=0 && axisNr<axisCount);
   axisLabels[axisNr]->setPlainText(name);

   // update text position
   // see redrawAll
   QPointF lPos = calcPosition(axisNr, 1.08);
   axisLabels[axisNr]->setPos(QPoint(0,0));
   axisLabels[axisNr]->moveBy(lPos.x(),lPos.y());

   // If the axis is on the left side, the text has to be moved to the left
   if( axisLabels[axisNr]->pos().x()<0 )
      axisLabels[axisNr]->moveBy(- axisLabels[axisNr]->boundingRect().width(),0);

}

QPointF KiviatView::calcPosition(int axisNr, float d) const
{
   Q_ASSERT(axisNr>=0 && axisNr < axisCount);

   float angle = 2*M_PI / (float) axisCount * (float) axisNr;
   angle += (float)prop_angleOffset/360.0 * 2.0*M_PI;

   return QPointF  (std::cos(angle)*AXIS_LENGTH*d,
         std::sin(angle)*AXIS_LENGTH*d);

}


void KiviatView::setGradientBackground(bool on)
{
   if(!on)
   {
      setBackgroundBrush(Qt::NoBrush);
      return;
   }
   else
   {
      int alpha=200;

      QRadialGradient gradient(QPoint(0,0),AXIS_LENGTH,QPoint(0,0));

      gradient.setColorAt(0,   QColor(0,255,0,alpha));
      gradient.setColorAt(0.5, QColor(255,255,0,alpha));
      gradient.setColorAt(0.9, QColor(255,0,0,alpha));
      gradient.setColorAt(1,   QColor(255,255,255));

      setBackgroundBrush(QBrush(gradient));
   }
}





const QVector<float> & KiviatView::getData(int id)
{
   return dataset[id]->getData();
}


void KiviatView::setActiveItem(int newId)
{
   if(curDataItem>=0 && curDataItem<dataset.size())
   {
      dataset[curDataItem]->setActive(false);
   }

   curDataItem=newId;


   if(curDataItem>=0 && curDataItem<dataset.size())
      dataset[curDataItem]->setActive(true);


   update();
}

void KiviatView::setToolTip( int dataHandle, int axisNr, const QString& string )
{
    dataset[dataHandle]->setToolTip( axisNr, string );
}




