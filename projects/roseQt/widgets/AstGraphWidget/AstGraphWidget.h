
#ifndef AstGRAPHWIDGET_H
#define AstGRAPHWIDGET_H


#include <QGraphicsView>
#include "DisplayNode.h"


class SgNode;
class AstFilterInterface;

class AstGraphWidget : public QGraphicsView
{
	Q_OBJECT

	public:
		AstGraphWidget(QWidget * parent);
		virtual ~AstGraphWidget();


	public slots:
		void setNode(SgNode * sgNode);
		void setFilter(AstFilterInterface * filter);
		void setFileFilter(int id);
	signals:
		void clicked(SgNode * node);

		void clicked(const QString & filename, int line, int column);


	protected:
		QGraphicsScene * scene;
		DisplayNode * root;

		// Zoom
		virtual void wheelEvent(QWheelEvent *event);
		virtual void scaleView(qreal scaleFactor);
		virtual void mousePressEvent(QMouseEvent *event);


		AstFilterInterface * curFilter;
		SgNode * curSgTreeNode;
};



#endif
