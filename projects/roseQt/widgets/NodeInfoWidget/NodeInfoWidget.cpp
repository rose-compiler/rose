#include "rose.h"

#include "NodeInfoWidget.h"



NodeInfoWidget::NodeInfoWidget(QWidget * par)
	: PropertyTreeWidget(par)
{

}

NodeInfoWidget::~NodeInfoWidget()
{
}


void NodeInfoWidget::printDeclModifier(const QModelIndex & par, const SgDeclarationModifier & declMod)
{
	addEntry(par,"isUnknown",declMod.isUnknown());
	addEntry(par,"isDefault",declMod.isDefault());
	addEntry(par,"isFriend",declMod.isFriend());
	addEntry(par,"isTypedef",declMod.isTypedef());
	addEntry(par,"isExport",declMod.isExport());
	addEntry(par,"isThrow",declMod.isThrow());
}


void NodeInfoWidget::printFuncModifier(const QModelIndex & par,
										const SgFunctionModifier & funcMod)
{
	addEntry(par,"isUnknown",funcMod.isUnknown());
	addEntry(par,"isDefault",funcMod.isDefault());
	addEntry(par,"isInline",funcMod.isInline());
	addEntry(par,"isVirtual",funcMod.isVirtual());
	addEntry(par,"isPureVirtual",funcMod.isPureVirtual());
	addEntry(par,"isExplicit",funcMod.isExplicit());
	addEntry(par,"isPure",funcMod.isPure());
}

void NodeInfoWidget::printSpecialFuncModifier(const QModelIndex & par,
											  const SgSpecialFunctionModifier& funcMod)
{
	addEntry(par,"isUnknown",funcMod.isUnknown());
	addEntry(par,"isDefault",funcMod.isDefault());
	addEntry(par,"isNotSpecial",funcMod.isNotSpecial());
	addEntry(par,"isConstructor",funcMod.isConstructor());
	addEntry(par,"isDestructor",funcMod.isDestructor());
	addEntry(par,"isConversion",funcMod.isConversion());
	addEntry(par,"isOperator",funcMod.isOperator());
}



QString NodeInfoWidget::getTraversalName(SgNode * node)
{
    SgNode * par = node->get_parent();
    if(par)
    {
        //TODO is there a better way to get this info?
         unsigned int i;
         for(i=0; i<par->get_numberOfTraversalSuccessors(); i++)
             if(par->get_traversalSuccessorByIndex(i)==node)
                 break;

         if( i >= par->get_numberOfTraversalSuccessors())
             return QString(tr("Unknown"));

         return QString(par->get_traversalSuccessorNamesContainer()[i].c_str());
    }
    else
        return QString();
}


void NodeInfoWidget::setNode(SgNode * node)
{
	clear();
	if(node==NULL)
		return;

	int colorNr=0;

	int generalSection = addSection(tr("General Info"),colorNr++);
	SgNode * par=node->get_parent();
	if(par)
	{
        addEntryToSection(generalSection,"Class Name",node->class_name().c_str());
        addEntryToSection(generalSection,"Desc Name",getTraversalName(node));

	    addEntryToSection(generalSection,"Parent Class Name",par->class_name().c_str());
        addEntryToSection(generalSection,"Parent Desc",getTraversalName(par));

        SgNode * parpar = par->get_parent();
        if(parpar)
        {
            addEntryToSection(generalSection,"Grandparent Class Name",parpar->class_name().c_str());
            addEntryToSection(generalSection,"Grandparent Desc",getTraversalName(parpar));
        }
	}


	// Located Node
	SgLocatedNode* sgLocNode = isSgLocatedNode(node);
	if(sgLocNode)
	{
		int locNodeSec= addSection(tr("Located Node"), colorNr);

		Sg_File_Info* fi = sgLocNode->get_file_info();

		QModelIndex idx = addEntryToSection(locNodeSec,"File Information","");

		//Filename
		addEntry(idx,"File",QString(fi->get_filenameString().c_str()));
		//Line
		addEntry(idx,"Line",fi->get_line());
		//Column
		addEntry(idx,"Column",fi->get_col());
		//FileId
		addEntry(idx,"FileId",fi->get_file_id());


		//Transformation
		addEntryToSection(locNodeSec,"IsTransformation",fi->isTransformation());
		//Output in CodeGen
		addEntryToSection(locNodeSec,"IsOutputInCodeGeneration",fi->isOutputInCodeGeneration());


		// Declaration Statement
		SgDeclarationStatement * sgDeclNode =isSgDeclarationStatement(node);
		if(sgDeclNode)
			addEntryToSection(locNodeSec,"Decl Mangled Name",QString(sgDeclNode->get_mangled_name().getString().c_str()));


		// Expression Type
		SgExpression * sgExprNode = isSgExpression(node);
		if(sgExprNode)
			addEntryToSection(locNodeSec,"Expression Type",QString(sgExprNode->get_type()->unparseToString().c_str()));
	}
	colorNr++;


	//RTI information from SgNode
    RTIReturnType rti=node->roseRTI();
	int rtiNodeSec= addSection(tr("RTI from SgNode"),colorNr);

    for(RTIReturnType::iterator i=rti.begin(); i<rti.end(); i++)
    {
		if (strlen(i->type) >= 7 &&
		strncmp(i->type, "static ", 7) == 0)
		{
			continue; // Skip static members
		}

		QString propName (i->name);
		QString propValue(i->value.c_str());


		//  Declaration Modifier Special Treatment
		if(propName=="p_declarationModifier")
		{
			SgDeclarationStatement * declStatement = isSgDeclarationStatement(node);
			if(declStatement)
			{
				QModelIndex idx = addEntryToSection(rtiNodeSec,"Declaration Modifier","");
				printDeclModifier(idx,declStatement->get_declarationModifier());
			}
			continue;
		}

		if(propName=="p_functionModifier")
		{
			SgFunctionDeclaration * funcDecl = isSgFunctionDeclaration(node);
			if(funcDecl)
			{
				QModelIndex idx = addEntryToSection(rtiNodeSec,tr("Function Modifier"),"");
				printFuncModifier(idx,funcDecl->get_functionModifier());
			}
			continue;
		}

		if(propName=="p_specialFunctionModifier")
		{
			SgFunctionDeclaration * funcDecl = isSgFunctionDeclaration(node);
			if(funcDecl)
			{
				QModelIndex idx = addEntryToSection(rtiNodeSec,tr("Special Function Modifier"),"");
				printSpecialFuncModifier(idx,funcDecl->get_specialFunctionModifier());
			}
			continue;
		}

		addEntryToSection(rtiNodeSec,propName,propValue);
    }
    colorNr++;

    // Attributes
    if (node->get_attributeMechanism() != NULL)
    {
    	int attrNodeSec = addSection(tr("Attributes"),colorNr);

		AstAttributeMechanism::AttributeIdentifiers aidents = node->get_attributeMechanism()->getAttributeIdentifiers();

		QModelIndex metricNodeSection;
		for (AstAttributeMechanism::AttributeIdentifiers::iterator it = aidents.begin(); it != aidents.end(); ++it )
		{
			AstAttribute * attr = node->getAttribute(*it);
			MetricAttribute * metricAttr = dynamic_cast<MetricAttribute * >(attr);
			if(metricAttr)
			{
				if(! metricNodeSection.isValid())
					metricNodeSection = addEntryToSection(attrNodeSec,tr("Metric Attributes"),"");

				addEntry(metricNodeSection,(*it).c_str(), metricAttr->getValue() );
			}
			else
			{
				QString name (it->c_str());
				QString attributeValue (node->getAttribute(*it)->toString().c_str());
				addEntryToSection(attrNodeSec,name,attributeValue);
			}
		}
    }
    colorNr++;

}
