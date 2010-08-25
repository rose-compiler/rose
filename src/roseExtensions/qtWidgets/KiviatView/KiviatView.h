
#ifndef KIVIAT_VIEW_H
#define KIVIAT_VIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtDesigner/QDesignerExportWidget>

#include <QVariant>

class QGraphicsPolygonItem;


/**
 * \brief Generic Qt-Widget for displaying Kiviats (polar diagrams)
 *
 * \image html KiviatTester.jpg
 *
 * This class doesn't depend on rose, the rose specific version for
 * displaying MetricAttributes is MetricsKiviat
 */
class QDESIGNER_WIDGET_EXPORT KiviatView : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY(int AxisCount           READ getAxisCount          WRITE setAxisCount  )
    Q_PROPERTY(int MarkerCount         READ markerCount           WRITE setMarkerCount)
    Q_PROPERTY(int ActiveAlpha         READ activeAlpha           WRITE setActiveAlpha)
    Q_PROPERTY(int InactiveAlpha       READ inactiveAlpha         WRITE setInactiveAlpha)
    Q_PROPERTY(int CornerPointRadius   READ cornerPointRadius     WRITE setCornerPointRadius)
    Q_PROPERTY(int AngleOffset         READ angleOffset           WRITE setAngleOffset)
    Q_PROPERTY(bool GradientBackground READ hasGradientBackground WRITE setGradientBackground)


    class KiviatData;
    friend class KiviatData;     ///needed because of calcPosition

    public:


        KiviatView(QWidget * parent=NULL,int axisCount=8);
        virtual ~KiviatView() {}



        /** Adds a dataset to the diagram
            @param data Float Data (between 0 and 1), size of vector has to be axisCount
                                   std::vectors can be converted with QVector::fromStdVector()
                    @param color color for displaying this dataset (to distinguish datasets, if more than
                    one are displayed in one single kiviat)
            @return handle to modify or delete data
        */
        int addData(const QVector<float> & data, QColor color=Qt::blue);



        /** Adds Data from an integer-vector,
         * @param data Float Data (between 0 and 1), size of vector has to be axisCount
                   std::vectors can be converted with QVector::fromStdVector()
         * @param valueToNormalize the value which represents 100%
         *                         data is calulated as data[i]/valueToNormalize
         *  @param color color for displaying this dataset (to distinguish datasets, if more than
         *           one are displayed in one single kiviat)
         *   @return handle to modify or delete data
         */
        int addData(const QVector<int> & data, int valueToNormalize, QColor color=Qt::blue);


        /** Removes dataset with given handle */
        void removeData(int dataHandle);

        /** Modifies a single entry in already existing data */
        void  setDataPoint(int dataHandle, int axisNr, float newValue);
        float getDataPoint(int dataHandle, int axisNr) { return getData(dataHandle)[axisNr]; }

        QColor getDataColor( int dataHandle );

        const QVector<float> & getData(int dataHandle);

        /// Returns the axis count
        int getAxisCount() const { return axisCount; }

        /** Returns number of dataSETS, whereas each dataSet contains axisCount entries */
        int getDataCount()    const { return dataset.size(); }

        void setToolTip( int dataHandle, int axisNr, const QString& string );

        // ---------- Getter for Graphical Properties --------------------
        // for documentation look at member variables declaration
        int  cornerPointRadius()     const   { return prop_cornerPointRadius; }
        int  markerCount()           const   { return prop_markerCount;       }
        int  inactiveAlpha()         const   { return prop_inactiveAlpha;     }
        int  activeAlpha()           const   { return prop_activeAlpha;       }
        int  angleOffset()           const   { return prop_angleOffset;       }
        bool hasGradientBackground() const   { return backgroundBrush()!=Qt::NoBrush; }

    public slots:
        // ---------- Setter for Graphical Properties --------------------
        // for documentation look at member variables declaration
        void setMarkerCount      (int newMarkerCount);
        void setInactiveAlpha    (int newVal)  { prop_inactiveAlpha=newVal;     redrawData(); }
        void setActiveAlpha      (int newVal)  { prop_activeAlpha=newVal;       redrawData(); }
        void setCornerPointRadius(int newVal)  { prop_cornerPointRadius=newVal; redrawData(); }
        void setAngleOffset      (int newVal)  { prop_angleOffset=newVal;       redrawAll();  }

        void setDataColor( int dataHandle, const QColor& color );

        /// Change/Set the Label of given axis
        void setAxisLabel(int axisNr, const QString & name);

        /// Sets the Item which is displayed in the foreground (i.e. the "selected")
        void setActiveItem(int dataHandle);

        /// Turns the gradient background on and off
        /// color gradient from green (center) over yellow to red (on outside)
        void setGradientBackground(bool on=true);
        void clear();

        /// Set axis count
        void setAxisCount(int newAxisCount);
        void removeAxis( int axisID );

    signals:
        void clickedOnData(int dataId);
        void cleared();

    protected:
        // Redraws the axes and markers and then calles redrawData
        void redrawAll();

        // Redraws the Datasets (call if some properties like alpha values, or color have changed
        void redrawData();

    	void setupAxesCaptions();

        /// Overwritten Resize Event, used for scaling of scene
        virtual void resizeEvent(QResizeEvent * ev);

        /// Used for selecting active dataset, and generating
        /// signal clicked_onData
        virtual void mousePressEvent(QMouseEvent *ev);

        //// Adds Marking ( lines with equal %-values)
        /// @param position Value between 0 and 1 (where to draw the marking)
        QGraphicsPolygonItem * addMarker(float position);

        /// Calculates the Cartesian coordinates, of a point on axis with given nr
        /// @param axisNr
        /// @param normedDistance 0 means center of diagram, 1 end of the axis line
        QPointF calcPosition(int axisNr, float normedDistance) const;


        /// Index of active data-item, or -1 if none is active
        int curDataItem;

        /// Number of Axis
        int axisCount;

        /// Markers are the lines which connect the points of equal value
        /// Number of markers i.e. 5 would be: draw 20%, 40%... line
        int prop_markerCount;

        /// Transparency value of active and inactive dataset
        /// (between 0 and 255, where 0 is fully transparent)
        int prop_activeAlpha;
        int prop_inactiveAlpha;

        /// Radius of the cornerPoints (dots on intersection of axis and polygon
        int prop_cornerPointRadius;

        /// Angle Offset (rotates the diagram) (set between 0 and 360)
        int prop_angleOffset;


        QFont labelFont;

        /// Axis Length in pixel
        static const int AXIS_LENGTH;

        /// KiviatData manages the storing and drawing of the data
        QVector<KiviatData*> dataset;

        QGraphicsScene scene;
        QVector<QGraphicsTextItem*> axisLabels;
        QVector<QGraphicsLineItem*> axes;
        QVector<QGraphicsPolygonItem*> markers;
};

#endif
