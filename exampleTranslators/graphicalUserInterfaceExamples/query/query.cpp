#include <rose.h>
#include <config.h>

#include <qrose.h>

class QueryCollection {
public:    
    bool findPublicVarMembers (SgNode *node, string &str) {
	SgVariableDeclaration *decl;
	if (decl = isSgVariableDeclaration(node)) {
	    SgDeclarationModifier &dfm = decl->get_declarationModifier();
	    if (dfm.get_accessModifier().isPublic()) {
		// check if it belongs to a class
		SgStatement *block = ((SgVariableDeclaration *) node)->get_scope();
		SgClassDefinition *classDef;
		if (classDef = isSgClassDefinition(block)) {
		    if (classDef->get_declaration()->get_class_type() == SgClassDeclaration::e_class) {
                        str = classDef->get_qualified_name().getString();		   
                        return true;
                    }
		}
	    } 
	}
	return false;
    }    
    
    bool findCallsWithFuncArgs(SgNode *node, string &str) {		
	SgExprStatement *statement;
	if (statement = isSgExprStatement(node)) {
	    SgFunctionCallExp *call_exp;	
            if (call_exp = isSgFunctionCallExp(statement->get_the_expr())) {
                m_nodes.clear(); m_nodes.insert(call_exp);
                m_domain.expand(&m_nodes, QRQueryDomain::all_children);
                m_domain.getNodes()->erase(call_exp);
                NodeQuery::VariantVector vector(V_SgFunctionCallExp);	       
                QRQueryOpVariant op(vector);
                op.performQuery(&m_domain, &m_range);
                unsigned hits = m_range.countRange();
                if (hits) {
                    sprintf(m_buffer, "%d", hits);
                    str = m_buffer;
                    return true;
                }
            }
	}
	
	return false;
    }
    
    bool findCallsWithFuncArgs2(SgNode *node, string &str) {		
	SgExprStatement *statement;
	if (statement = isSgExprStatement(node)) {
	    SgFunctionCallExp *call_exp;	
            if (call_exp = isSgFunctionCallExp(statement->get_the_expr())) {
                m_nodes.clear(); m_nodes.insert(call_exp);
                m_domain.expand(&m_nodes, QRQueryDomain::all_children);
                m_domain.getNodes()->erase(call_exp);
                NodeQuery::VariantVector vector (V_SgFunctionCallExp);
                QRQueryOpVariant op(vector);
                op.performQuery(&m_domain, &m_range);
                unsigned hits = m_range.countRange();
                if (hits) {
                    set<SgNode *> *rnodes = m_range.getNodes();
                    for (set<SgNode *>::iterator iter = rnodes->begin();
                         iter != rnodes->end(); )
                    {
                        SgFunctionCallExp *exp = (SgFunctionCallExp *) *iter; iter++;
                        SgExpression *funcexpr = exp->get_function();
                        str += funcexpr->unparseToString();
                        if (iter != rnodes->end()) {
                            str += ", ";
                        } 
                    }
                    return true;
                }
            }
	}
	
	return false;
    }
    
protected:
    QRQueryDomain m_domain;
    QRQueryRange m_range;
    set<SgNode *> m_nodes;
    char m_buffer[10];
};


int main(int argc, char **argv) {
    SgProject *project = frontend(argc, argv);
    
    QRGUI::init(argc, argv, project);    
    
    QRQueryBox *query = new QRQueryBox();    
    
    query->insertVariantQuery("Loops", NodeQuery::VariantVector(V_SgDoWhileStmt) + 
                              NodeQuery::VariantVector(V_SgForStatement) + 
                              NodeQuery::VariantVector(V_SgWhileStmt));

    query->insertVariantQuery("Function Declarations", NodeQuery::VariantVector(V_SgFunctionDeclaration));    
    
    query->insertCustomQuery<QueryCollection>("find public variable members", &QueryCollection::findPublicVarMembers);
    query->insertCustomQuery<QueryCollection>("find calls with function arguments I", &QueryCollection::findCallsWithFuncArgs);
    query->insertCustomQuery<QueryCollection>("find calls with function arguments II", &QueryCollection::findCallsWithFuncArgs2);
    
                
    new QRCodeBox();
 // new QRTreeBox();
    
    QRGUI::dialog()->setGeometry(0,0,1000,1000);
    QRGUI::dialog()->sizes(50);
    
    QRGUI::exec();    
}
