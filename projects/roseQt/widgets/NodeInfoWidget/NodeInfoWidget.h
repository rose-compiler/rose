#ifndef NODEINFOVIEW_H
#define NODEINFOVIEW_H

#include "PropertyTreeWidget/PropertyTreeWidget.h"


class SgNode;
class SgFunctionModifier;
class SgDeclarationModifier;
class SgSpecialFunctionModifier;


class NodeInfoWidget : public PropertyTreeWidget
{
	Q_OBJECT

	public:
		NodeInfoWidget(QWidget * parent=0);
		virtual ~NodeInfoWidget();

	public slots:
		void setNode(SgNode * node);

	protected:
		void printDeclModifier(const QModelIndex & par,
							  const SgDeclarationModifier & declMod);

		void printFuncModifier(const QModelIndex & par,
							   const SgFunctionModifier & funcMod);

		void printSpecialFuncModifier(const QModelIndex & par,
									  const SgSpecialFunctionModifier& funcMod);


		QString getTraversalName(SgNode * node);
};

#endif
