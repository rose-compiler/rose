#include "PropertyTreeWidget.h"

#include "PropertyTreeModel.h"


#include <QPalette>
#include <QDebug>
#include <QPainter>
#include <QHeaderView>


PropertyTreeWidget::PropertyTreeWidget(QWidget * p)
	: QTreeView(p)
{
	model = new PropertyTreeModel(this);
	setModel(model);


    QVector<QColor> temp;
    temp.reserve(6);
    temp.push_back(QColor(255, 255, 191));
    temp.push_back(QColor(255, 230, 191));
    temp.push_back(QColor(191, 255, 191));
    temp.push_back(QColor(199, 255, 255));
    temp.push_back(QColor(234, 191, 255));
    temp.push_back(QColor(255, 191, 239));

    predefColors.reserve(temp.count());
    for (int i = 0; i < temp.count(); i++) {
        QColor c = temp.at(i);
        predefColors.push_back(qMakePair(c, c.lighter(110)));
    }


	//Set Background to transparent -> background is drawn manually
	QPalette pal = palette();
	pal.setColor(QPalette::Base,QColor(255,255,255,0));
	//pal.setColor(QPalette::Window,QColor(255,255,255,0));
	pal.setColor(QPalette::AlternateBase,QColor(255,255,255,0));
	setPalette(pal);
	setRootIsDecorated(false);
	header()->setVisible(true);
}

PropertyTreeWidget::~PropertyTreeWidget()
{
}


int PropertyTreeWidget::addSection(const QString & sectionName, const QColor & col, const QColor & altCol)
{
	int secId=model->addSection(sectionName);
	colors.push_back(qMakePair(col,altCol));
	expand(model->index(secId,0));
	header()->adjustSize();

	return secId;
}

int PropertyTreeWidget::addSection(const QString & sectionName, int colorNr)
{
	int secId=model->addSection(sectionName);

	if(colorNr == -1)
		colorNr=secId;

	colors.push_back(predefColors[colorNr % predefColors.size()] );
	expand(model->index(secId,0));
	header()->adjustSize();

	return secId;
}

QModelIndex PropertyTreeWidget::addEntryToSection(int sectionId, const QString & prop,
                                                  const QVariant & val)
{
	QModelIndex id=  model->addEntryToSection(sectionId,prop,val);
	header()->adjustSize();
    ItemTreeModel::setupView(this);

	return id;
}

QModelIndex PropertyTreeWidget::addEntry (  const QModelIndex & par,
											const QString & prop,
											const QVariant & val)
{
    ItemTreeModel::setupView(this);
    return model->addEntry(par,prop,val);
}


//Custom Paint Event to set the colors
void PropertyTreeWidget::drawRow (QPainter * p,
                                  const QStyleOptionViewItem & viewItem,
                                  const QModelIndex & ind) const
{
	bool ok;
	int section=model->data(ind,Qt::UserRole).toInt(&ok);
	if(!ok)
	{
		//no UserRole data provided -> draw normal
		QTreeView::drawRow(p,viewItem,ind);
		return;
	}
	Q_ASSERT(ok);
	Q_ASSERT(section>=0 && section < colors.size());


	QColor col = (ind.row() % 2) ? colors[section].first : colors[section].second;

	p->setPen(Qt::NoPen);
	p->setBrush(QBrush(col));
	p->drawRect(viewItem.rect);
	QTreeView::drawRow(p,viewItem,ind);

}

void PropertyTreeWidget::clear()
{
	model->clear();
	colors.clear();
	update();
}


