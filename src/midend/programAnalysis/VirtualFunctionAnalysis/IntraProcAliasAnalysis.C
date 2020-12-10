#include "sage3basic.h"
#include "IntraProcAliasAnalysis.h"


void CompactRepresentation::computeAliases (SgGraphNode *node, int derefLevel, vector<SgGraphNode *> & sol) {
    
    if(node == NULL) return;
    
    if(derefLevel == 0) {
        sol.push_back(node);
    } else {
        
        std::set<SgDirectedGraphEdge *> edges = graph->computeEdgeSetOut(node);
        foreach(SgDirectedGraphEdge *ed, edges) {
            computeAliases(ed->get_to(), derefLevel-1, sol);
        }
        
    }
}

SgGraphNode* CompactRepresentation::getGraphNode(SgNode *node) {
    
        SgGraphNode *g_node;
        
        if(all_nodes.count(node) == 0) {
            g_node = new SgGraphNode;
            g_node->set_SgNode(node);
            graph->addNode(g_node);
            all_nodes[node] = g_node;
        } else
            g_node = all_nodes.at(node);
        return g_node;
}


void CompactRepresentation::merge(SgIncidenceDirectedGraph *thatGraph) {

        set<SgGraphNode *> nodes = thatGraph->computeNodeSet();
        SgGraphNode *g_from;
        
        BOOST_FOREACH(SgGraphNode *node, nodes) {
            std::set<SgDirectedGraphEdge *> edges = thatGraph->computeEdgeSetOut(node);
            g_from = getGraphNode(node->get_SgNode());
            BOOST_FOREACH(SgDirectedGraphEdge *ed, edges) {
                addEdge(g_from, getGraphNode(ed->get_to()->get_SgNode()));
            }
        }
    
}
void CompactRepresentation::merge(const CompReprBase &that) {
        if(getHash() == that.getHash()) return;
    
        if(that.getHash() == 0) return;
        
        SgIncidenceDirectedGraph *thatGraph = that.getGraph();
        
        merge(thatGraph);
        
    }


void CompactRepresentation::updateHash(SgNode *from, SgNode *to) {
        const long mask = 0xffff;
        unsigned long f_add = (unsigned long)from;
        unsigned long t_add = (unsigned long)to;
        hash ^= (((f_add+1) & mask) * (t_add & mask) & mask);
 }

void CompactRepresentation::addEdge(SgGraphNode *g_from, SgGraphNode *g_to) {
        
         if(graph->checkIfDirectedGraphEdgeExists(g_from, g_to) == false) {
                graph->addDirectedEdge(new SgDirectedGraphEdge(g_from, g_to));
                updateHash(g_from->get_SgNode(), g_to->get_SgNode());
        }
}


void CompactRepresentation::init() {
        graph = new SgIncidenceDirectedGraph;
        hash = 0;
        this->all_nodes.clear();
}

CompactRepresentation::CompactRepresentation(const CompactRepresentation& copy) {
        if(graph != NULL) {
                delete graph;
                graph = NULL;
        }
        
        init();
        this->merge(copy.getGraph());
}

CompactRepresentation& CompactRepresentation::operator=(const CompactRepresentation& p) {
        assert(this != NULL);

        if(this != &p) {
            delete graph;
            graph = NULL;
            init();
            this->merge(p.getGraph());
            this->all_nodes.clear();
     
        }
        return *this;
}
void CompactRepresentation::computeAliases (SgVariableSymbol *var, int derefLevel, vector<SgGraphNode *> &nodes) {
        computeAliases(all_nodes[var], derefLevel, nodes);
    }

void CompactRepresentation::addMustAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right) {
        
        std::vector<SgGraphNode *> leftNodes; 
        std::vector<SgGraphNode *> rightNodes;
        SgGraphNode *g_from, *g_to;
        unsigned int i, j;
        
        if(left.var == NULL) 
            return; // function call as parameter not resolved yet
        
        if(left.var == right.var) 
            return;
        
        g_from = getGraphNode(left.var);
        g_to = getGraphNode(right.var);

//        std::cout << "Add a relation from " << left.var->get_name().getString() << " "<<right.var->get_name().getString() << std::endl;
                
        computeAliases(g_to, right.derefLevel+1, rightNodes);
        computeAliases(g_from, left.derefLevel, leftNodes);
        
        for(i=0; i< leftNodes.size(); i++) {
            std::set<SgDirectedGraphEdge *> edges = graph->computeEdgeSetOut(leftNodes[i]);
            if(edges.size() == 1) {

                updateHash((*(edges.begin()))->get_from()->get_SgNode(),(*(edges.begin()))->get_to()->get_SgNode());
                graph->removeDirectedEdge(*(edges.begin()));
                
            }
            edges = graph->computeEdgeSetOut(leftNodes[i]);
        }
        
        
        for(i=0; i < leftNodes.size(); i++ ) {
            for(j=0 ; j < rightNodes.size(); j++) {
                addEdge(leftNodes[i], rightNodes[j]);
            }
           std::set<SgDirectedGraphEdge *> edges = graph->computeEdgeSetOut(leftNodes[i]);
        }
    
}


void CompactRepresentation::addMayAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right) {
        
        std::vector<SgGraphNode *> leftNodes; 
        std::vector<SgGraphNode *> rightNodes;
        SgGraphNode *g_from, *g_to;
        unsigned int i, j;
        if(left.var == NULL) return; // function call as parameter not resolved yet
        
        if(left.var == right.var) 
            return;
        
        g_from = getGraphNode(left.var);
        g_to = getGraphNode(right.var);

        computeAliases(g_to, right.derefLevel+1, rightNodes);
        computeAliases(g_from, left.derefLevel, leftNodes);
        
        for(i=0; i < leftNodes.size(); i++ ) {
            for(j=0 ; j < rightNodes.size(); j++) {
                addEdge(leftNodes[i], rightNodes[j]);
            }
        }
    
 }

bool CompactRepresentation::operator==(const CompactRepresentation &that) const {
    ROSE_ASSERT(this != NULL);
    return (getHash() == that.getHash());
}

bool CompactRepresentation::operator !=(const CompactRepresentation &that) const {
        return !(*this == that);
}


void CompactRepresentation::toDot(const std::string& file_name) {
    
        assert(this != NULL);
        
        std::ofstream ofile(file_name.c_str(), std::ios::out);
        
        set<SgGraphNode *>nodes = graph->computeNodeSet();
        
        ofile << "digraph defaultName {\n";
        std::set<SgGraphNode*> explored;
        foreach(SgGraphNode *node, nodes)
                processNodes(ofile, node, explored);
                
        ofile << "}\n";
}

void CompactRepresentation::processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored) {
        if (explored.count(n) > 0)
            return;
        explored.insert(n);

        printNodePlusEdges(o, n);

        std::set<SgDirectedGraphEdge*> out_edges = graph->computeEdgeSetOut(n);
        foreach (SgDirectedGraphEdge* e, out_edges)
            processNodes(o, e->get_to(), explored);

        std::set<SgDirectedGraphEdge*> in_edges = graph->computeEdgeSetIn(n);
        foreach (SgDirectedGraphEdge* e, in_edges)
            processNodes(o, e->get_from(), explored);
    }

void CompactRepresentation::printNodePlusEdges(std::ostream & o, SgGraphNode* node) {
       
    ROSE_ASSERT(node != NULL); 
    
    printNode(o, node);

    std::set<SgDirectedGraphEdge*> out_edges = graph->computeEdgeSetOut(node);
    foreach (SgDirectedGraphEdge* e, out_edges)
        printEdge(o, e, false);

}

void CompactRepresentation::printNode(std::ostream & o, SgGraphNode* node) {
    
    std::string nodeColor = "black";

    assert(node != NULL);

    std::string label = escapeString(((SgVariableSymbol *)node->get_SgNode())->get_scope()->unparseToCompleteString()+"\n\n"+((SgVariableSymbol *)node->get_SgNode())->get_name().getString());
    o << "n_"<<node << " [label=\""  <<  label << "\", color=\"" << nodeColor <<
        "\", style=\"" <<"solid" << "\"];\n";
}

void CompactRepresentation::printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge) {
    SgGraphNode *from = edge->get_from();
    SgGraphNode *to = edge->get_to();

    o << "n_"<<from  << " -> n_" << to << " [label=\"" << " " <<
            "\", style=\"" << "solid" << "\"];\n";
}

bool CompReprPtr::operator==(const CompReprPtr &that) const {
        CompactRepresentation *a = (static_cast<CompactRepresentation *>(ptr.get()));
        CompactRepresentation *b = (static_cast<CompactRepresentation *>(that.get()));
        if(a == NULL && b == NULL) return true;
        if(a == NULL || b == NULL) return false;
        return *a == *b;
}

void CompReprPtr::operator+=(const CompReprPtr &that) const {
    if(that.get() != NULL) {
            this->get()->merge(*(that.get()));
    }
}


AliasInfoGenerator::AliasInfoGenerator() {
        ins.clear();
        outs.clear();
        returnStmts.clear();
        aliasRelations.clear();
}

void AliasInfoGenerator::init(SgGraphNode *n) {
        std::vector<std::pair<AliasRelationNode, AliasRelationNode> > rel;
        ins[n] = CompReprPtr();
        outs[n] = CompReprPtr();
        aliasRelations[n] = rel;
}

void AliasInfoGenerator::addNewAliasRelation(SgGraphNode *node, std::pair<AliasRelationNode, AliasRelationNode> a_relation) {
      bool found = false;
      for(unsigned int i=0; i < aliasRelations.at(node).size(); i++) {
          std::pair<AliasRelationNode, AliasRelationNode> relation = aliasRelations.at(node)[i];
          if(relation.first == a_relation.first && relation.second.var == a_relation.second.var) {
              found = true;
              break;
          }
      }
      
      if(!found)
        aliasRelations.at(node).push_back(a_relation);
}
CompReprPtr AliasInfoGenerator::getExitData(SgGraphNode *node) {  
    /** This is to for the bug in StaticCFG look at file test2001_14.C*/    
    //ROSE_ASSERT(outs.count(node) != 0);
    if(outs.count(node) == 0)
            outs[node] = CompReprPtr(new CompactRepresentation);
        return outs.at(node);    
}
CompReprPtr AliasInfoGenerator::getEntryData(SgGraphNode *node) {  
    /** This is for  the bug in StaticCFG look at file test2001_14.C*/    
    //ROSE_ASSERT(ins.count(node) != 0);
    if(ins.count(node) == 0)
            ins[node] = CompReprPtr(new CompactRepresentation);
        return ins.at(node);    
}


std::vector <std::pair<AliasRelationNode, AliasRelationNode> > AliasInfoGenerator::getAliasRelations(SgGraphNode *node){
      return aliasRelations.at(node);
}


void IntraProcAliasAnalysis::buildCFG() {
#if 0        
        printf ("In IntraProcAliasAnalysis::buildCFG(): \n");
#endif
        SgFunctionDefinition *defn = isSgFunctionDefinition(isSgFunctionDeclaration(isSgFunctionDeclaration(head)->get_definingDeclaration())->get_definition());

        ROSE_ASSERT(defn != NULL);
#if 0
        printf ("In IntraProcAliasAnalysis::buildCFG(): StaticCFG::CustomFilteredCFG constructor \n");
#endif
        cfg = new StaticCFG::CustomFilteredCFG<AliasCfgFilter>(defn);
#if 0
        printf ("In IntraProcAliasAnalysis::buildCFG(): calling buildFilteredCFG() \n");
#endif
        cfg->buildFilteredCFG();
#if 0
        printf ("In IntraProcAliasAnalysis::buildCFG(): done calling buildFilteredCFG() \n");
#endif
        // run a bfs and list the nodes
        cfgNodes.clear();
        std::queue<SgGraphNode *> workQ;
        //boost::unordered_map<SgGraphNode* , bool> visited;
        vector<SgGraphNode *>visited;
        workQ.push(cfg->getEntry());
        visited.push_back(cfg->getEntry());
#if 0
        printf ("In IntraProcAliasAnalysis::buildCFG(): call while loop \n");
#endif
        while(!workQ.empty()) {
            SgGraphNode *node = workQ.front();
            workQ.pop();
            cfgNodes.push_back(node);
            

            std::vector<SgDirectedGraphEdge*>outs = StaticCFG::outEdges(node);
#if 0
             printf ("In IntraProcAliasAnalysis::buildCFG(): call for loop \n");
#endif
            for(unsigned int i=0; i< outs.size(); i++) 
                if(find(visited.begin(), visited.end(), outs[i]->get_to()) == visited.end() ) {
                       workQ.push(outs[i]->get_to());
                       visited.push_back(outs[i]->get_to());
                }
        }

#if 0
     printf ("Leaving IntraProcAliasAnalysis::buildCFG() \n");
#endif
}


IntraProcAliasAnalysis::IntraProcAliasAnalysis(SgNode *head, ClassHierarchyWrapper *_classHierarchy, CallGraphBuilder *_cgBuilder,
                    boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> &_mapping,
                    boost::unordered_map<SgExpression*, std::vector<SgFunctionDeclaration*> > &_resolver) :
           IntraProcDataFlowAnalysis<SgGraphNode, CompReprPtr>(head), classHierarchy(_classHierarchy), cfg(0), cgBuilder(_cgBuilder), mapping(_mapping), resolver(_resolver) {

        ROSE_ASSERT(isSgFunctionDeclaration(head));    

#if 0
        printf ("In IntraProcAliasAnalysis constructor: calling buildCFG() \n");
#endif
        buildCFG();
#if 0
        printf ("In IntraProcAliasAnalysis constructor: done calling buildCFG() \n");
#endif
        checkPointHash = 0;
        gen = new AliasInfoGenerator;
#if 0
        printf ("In IntraProcAliasAnalysis constructor: done calling CollectAliasRelations constructor \n");
#endif
        CollectAliasRelations *car = new CollectAliasRelations(cfg, gen);
#if 0
        printf ("In IntraProcAliasAnalysis constructor: done calling buildCFG() \n");
#endif
        car->run();
#if 0
        printf ("Leaving IntraProcAliasAnalysis constructor \n");
#endif
};  

void IntraProcAliasAnalysis:: run() {
    
    std::cout<< "IntraProcedural Analysis for : " << std::endl;

 // DQ (12/12/2018): This might be an issue for my latest testing.
    ROSE_ASSERT(head != NULL);

 // printf ("Calling unparseToCompleteString() for head = %p = %s = %s \n",head,head->class_name().c_str(),SageInterface::get_name(head).c_str());
 // head->get_file_info()->display("Calling unparseToCompleteString(): debug");

    std::cout<< head->unparseToCompleteString() << std::endl;

 // printf ("DONE: Calling unparseToCompleteString() for head = %p = %s \n",head,head->class_name().c_str());

    if(gen->getEntryData(cfg->getEntry()).get()  == NULL) {
        entry = CompReprPtr(new CompactRepresentation);
        gen->setEntryData(cfg->getEntry(), entry);
    }
    std::cout << cfg->getEntry() << std::endl;
    
    //std::ofstream graph("cfg.dot");
    //cfgToDotForDebugging(graph, "dotGraph", stmt->cfgForBeginning());
#if 0    
    SgFunctionDeclaration *decl = isSgFunctionDeclaration(head);
    if(decl) {
        decl = isSgFunctionDeclaration(decl->get_definingDeclaration());
        if(decl) {
            SgFunctionDefinition *defn = decl->get_definition();
            if(defn) {
                cfg->cfgToDot(defn, "cfg.dot");
            }
        }
    
    }
    
#endif    
    
    
    
    IntraProcDataFlowAnalysis<SgGraphNode, CompReprPtr>::run();
    

    for(unsigned int i=0; i< cfgNodes.size(); i++) {
        std::stringstream ss, ss2;
        ss << isSgFunctionDeclaration(head)->get_mangled_name().getString()<< "in_" << i << ".dot";
        std::string name, name2;
        ss >> name;

        ss2 << isSgFunctionDeclaration(head)->get_mangled_name().getString() << "out_" << i << ".dot";
        ss2>> name2;
        
        //getCFGInData(cfgNodes[i]).get()->toDot(name);
        //getCFGOutData(cfgNodes[i]).get()->toDot(name2);
        
       // std::cout << i<< " in:" << getCFGInData((cfgNodes[i])).get()->getHash() << std::endl;
       // std::cout << i<< " out:" << getCFGOutData((cfgNodes[i])).get()->getHash() << std::endl;
      
    }
    
  //  gen->getEntryData(nodes[nodes.size()-1]).get()->toDot(def->get_mangled_name()+"_cr.dot");
    
}
 
bool IntraProcAliasAnalysis::runCheck() {
        
        run();
        //Now the inter procedural part 
        // First add the Virtual Functions based on object resolved so far
        // Compute the IN of each function called from this one based on
        //    EntryS = InC U EntryS
        // Also add the parameter aliases  
        vector<SgFunctionCallExp*> functionCalls = SageInterface::querySubTree<SgFunctionCallExp>(head, V_SgFunctionCallExp);
        CFGNode cfgn;
        SgGraphNode *graphNode;
        bool change = false;
        // For each Function Call Site
        for(unsigned index=0; index< functionCalls.size(); index++) {
            SgFunctionCallExp *funcCall = functionCalls[index];    
            cfgn = funcCall->cfgForBeginning();
            graphNode = cfg->toGraphNode(cfgn);
            if(graphNode == NULL) {
                // example: function call inside catch block is not added on controlflow graph
                continue;
            }
            CompReprPtr callSiteIN =  getCFGInData(graphNode);
            ROSE_ASSERT(resolver.count(funcCall) != 0);
            //std::cout << funcCall->unp
            // Now add the virtual functions based on the resolved objects
            // check if it's a virtual function call site?
            if(resolver.at(funcCall).size() == 0 ||
                  resolver.at(funcCall)[0]->get_functionModifier().isVirtual() == true) {
               change |= updateVirtualFunctionInCallGraph(funcCall, callSiteIN);
            }
            
            // For each referenced functions
            for(unsigned j=0; j < resolver.at(funcCall).size(); j++) {
                SgFunctionDeclaration *declaration = isSgFunctionDeclaration(resolver.at(funcCall)[j]);
                if(declaration == NULL) {
                    std::cout << "Defining declaration not found for " <<funcCall->unparseToCompleteString()<<  std::endl;
                    continue;
                }
                if(mapping.count(declaration) == 0) {
                    // Compiler Generated functions;
                    continue;
                }
                
                 //std::cout << funcCall->unparseToCompleteString() << std::endl;
                 //std::cout << resolver.at(funcCall).size() << std::endl;
                 
                
                mapping.at(declaration)->setFunctionEntry(callSiteIN);
                
                std::vector <std::pair<AliasRelationNode, AliasRelationNode> > arg_relations, return_relations;
                
                
                getFunctionParametersAliasRelations(funcCall, resolver.at(funcCall)[j], arg_relations, return_relations);
                // add the parameters to the alias relations to the function entry
                for(unsigned int i=0; i< arg_relations.size(); i++) 
                   mapping.at(declaration)->getFunctionEntry().get()->addMustAliasRelation(arg_relations[i].first, arg_relations[i].second);

                for(unsigned int i=0; i< return_relations.size(); i++) 
                    gen->addNewAliasRelation(graphNode, return_relations[i]);
            }
        }
        
        
        // handle the constructors
        vector<SgConstructorInitializer*> constrs = SageInterface::querySubTree<SgConstructorInitializer>(head, V_SgConstructorInitializer);
        for(unsigned index=0; index< constrs.size(); index++) {
            SgConstructorInitializer *con = constrs[index];    
            cfgn = con->cfgForBeginning();
            graphNode = cfg->toGraphNode(cfgn);
            CompReprPtr callSiteIN =  getCFGInData(graphNode);
            
            ROSE_ASSERT(resolver.count(con) != 0);
            
            // For each referenced functions
            for(unsigned j=0; j < resolver.at(con).size(); j++) {
                SgFunctionDeclaration *declaration = isSgFunctionDeclaration(resolver.at(con)[j]);
                
                if(declaration == NULL) {
                    std::cout << "Defining declaration not found for " << con->unparseToCompleteString()<<  std::endl;
                    continue;
                }

                if(mapping.count(declaration) == 0) {
                    // Compiler Generated functions;
                    continue;
                }
                
                mapping.at(declaration)->setFunctionEntry(callSiteIN);
                
                std::vector <std::pair<AliasRelationNode, AliasRelationNode> > arg_relations, return_relations;
                
                
                getConstructorParametersAliasRelations(con, resolver.at(con)[j], arg_relations);
                
                // add the parameters to the alias relations to the function entry
                for(unsigned int i=0; i< arg_relations.size(); i++) 
                   mapping.at(declaration)->getFunctionEntry().get()->addMustAliasRelation(arg_relations[i].first, arg_relations[i].second);
            }
            

        }

            CompReprPtr c_out = getCFGOutData(cfgNodes[cfgNodes.size()-1]);
            setFunctionExit(c_out);
            
            //c_out.get()->toDot(isSgFunctionDeclaration(head)->get_mangled_name()+".dot");
            
            unsigned long thisEndHash = getFunctionExit().get()->getHash();
            change  |= (checkPointHash != thisEndHash);
            checkPointHash = thisEndHash;
            return change;
        
 }


void IntraProcAliasAnalysis::getFunctionParametersAliasRelations(SgFunctionCallExp *f_exp, SgFunctionDeclaration *funcDecl, 
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &arg_relations,
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &return_relations){
    
            
            ROSE_ASSERT(funcDecl != NULL);
            
            
            SgExprListExp *args = f_exp->get_args();
            SgExpressionPtrList& actual = args->get_expressions();
            SgInitializedNamePtrList& formal = funcDecl->get_parameterList()->get_args();
            SgInitializedNamePtrList::iterator j = formal.begin();
            SgExpressionPtrList::iterator i = actual.begin();
            AliasRelationNode leftARNode, rightARNode;
            unsigned int index =0;
            
            for (; i != actual.end() && j != formal.end(); ++j, i++) {
                
                ProcessExpression::processLHS(*j, leftARNode);
                
                if((*i)->variantT() == V_SgFunctionCallExp) {
                    SgFunctionCallExp *funcCall = isSgFunctionCallExp(*i);
                    ROSE_ASSERT(funcCall != NULL);
                    CFGNode cfgn = funcCall->cfgForBeginning();
                    SgGraphNode* graphNode = cfg->toGraphNode(cfgn);
                    std::vector <std::pair<AliasRelationNode, AliasRelationNode> > relations = gen->getAliasRelations(graphNode);
                    for(index=0; index < relations.size(); index++) {
                         rightARNode = relations[index].second;
                         if(leftARNode.var != NULL && rightARNode.var != NULL) {
                              arg_relations.push_back(make_pair(leftARNode, rightARNode)); 
                         }
                    }
                    rightARNode.var = NULL;
                } else 
                        ProcessExpression::processRHS(*i, rightARNode);
                
                if(leftARNode.var != NULL && rightARNode.var != NULL) {
                   arg_relations.push_back(make_pair(leftARNode, rightARNode)); 
                }
            }
            SgExpression *lhs, *rhs;
            AliasRelationNode arNode;

            switch(f_exp->get_parent()->variantT()) {
                case V_SgAssignOp:
                    if(SageInterface::isAssignmentStatement(f_exp->get_parent(),&lhs, &rhs)) {
                        ProcessExpression::processLHS(lhs,arNode);
                    }
                    break;
                case V_SgAssignInitializer:
                {
                     SgAssignInitializer *assgn_i = isSgAssignInitializer(f_exp->get_parent());
                     ROSE_ASSERT(assgn_i != NULL);

                    lhs = static_cast<SgExpression *> (assgn_i->get_parent());
                    if(lhs != NULL)
                        ProcessExpression::processLHS(lhs,arNode);
                }   
                break;
                default:
                    arNode.var = NULL;
            }
            
            
            std::vector<AliasRelationNode>returns = mapping.at(funcDecl)->getReturnStmts();
            
             for(unsigned int index = 0; index < returns.size(); index++) {
                 return_relations.push_back(make_pair(arNode, returns[index]));
             }
}
void IntraProcAliasAnalysis::getConstructorParametersAliasRelations(SgConstructorInitializer *f_exp, SgFunctionDeclaration *funcDecl, 
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &arg_relations ) {
    
            
            assert(funcDecl != NULL);
            
            SgExprListExp *args = f_exp->get_args();
            SgExpressionPtrList& actual = args->get_expressions();
            SgInitializedNamePtrList& formal = funcDecl->get_parameterList()->get_args();
            SgInitializedNamePtrList::iterator j = formal.begin();
            SgExpressionPtrList::iterator i = actual.begin();
            AliasRelationNode leftARNode, rightARNode;
            
            unsigned int index;
            
            for (; i != actual.end(); ++j, i++) {
                
                ProcessExpression::processLHS(*j, leftARNode);

                if((*i)->variantT() == V_SgFunctionCallExp) {
                    SgFunctionCallExp *funcCall = isSgFunctionCallExp(*i);
                    ROSE_ASSERT(funcCall != NULL);
                    CFGNode cfgn = funcCall->cfgForBeginning();
                    SgGraphNode* graphNode = cfg->toGraphNode(cfgn);
                    std::vector <std::pair<AliasRelationNode, AliasRelationNode> > relations = gen->getAliasRelations(graphNode);
                    for(index=0; index < relations.size(); index++) {
                         rightARNode = relations[index].second;
                         if(leftARNode.var != NULL && rightARNode.var != NULL) {
                              arg_relations.push_back(make_pair(leftARNode, rightARNode)); 
                         }
                    }
                    rightARNode.var = NULL;
                } else 
                      ProcessExpression::processRHS(*i, rightARNode);
                
                if(leftARNode.var != NULL && rightARNode.var != NULL) {
                   arg_relations.push_back(make_pair(leftARNode, rightARNode)); 
                }
            }
}


/**
 * Adds a resolved virtual function to the callgraph and also update the resolver
 * @param type
 * @param right
 * @return 
 */

bool IntraProcAliasAnalysis::addVirtualFunction(SgType *type, SgFunctionCallExp *funcExp) {
    
    SgExpression *right = isSgBinaryOp(funcExp->get_function())->get_rhs_operand();
    SgMemberFunctionRefExp *func = isSgMemberFunctionRefExp(right);
    SgMemberFunctionDeclaration * memberFunctionDeclaration = 
                isSgMemberFunctionDeclaration(func->get_symbol()->get_declaration());
    
    ROSE_ASSERT(memberFunctionDeclaration);

    
    SgClassType *classType = isSgClassType(type);
    
    ROSE_ASSERT(classType != NULL);

    SgFunctionDeclaration *functionDeclaration = CallTargetSet::getFirstVirtualFunctionDefinitionFromAncestors(classType, memberFunctionDeclaration, classHierarchy);
    
    if(functionDeclaration == NULL) 
        return false;
    
    //std::cout << "Decl::\n"<<functionDeclaration->unparseToCompleteString() << std::endl;
    
    //Now add this function to CallGraph
    
     if (isSgMemberFunctionDeclaration(functionDeclaration))   {
            // always saving the in-class declaration, so we need to find that one
            SgDeclarationStatement *nonDefDeclInClass =
                    isSgMemberFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
            // functionDeclaration is outside the class (so it must have a definition)
            if (nonDefDeclInClass)
                functionDeclaration = isSgMemberFunctionDeclaration(nonDefDeclInClass);
     } else {
            // we need to have only one declaration for regular functions as well
            SgFunctionDeclaration *nonDefDecl = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
            if (nonDefDecl)
                functionDeclaration = nonDefDecl;
     }
    
    
    
    SgIncidenceDirectedGraph *callGraph = cgBuilder->getGraph();

    SgGraphNode* toNode;
    
    if(cgBuilder->getGraphNodesMapping().count(functionDeclaration) ==0) {
        toNode = new SgGraphNode(functionDeclaration->get_qualified_name().getString());;
        toNode->set_SgNode(functionDeclaration);
        callGraph->addNode(toNode);
        (cgBuilder->getGraphNodesMapping())[functionDeclaration] = toNode;
        
    } else {
        toNode = (cgBuilder->getGraphNodesMapping())[functionDeclaration] ;
    }
    
    
    SgFunctionDeclaration *fromDecl = isSgFunctionDeclaration(head);
    
    SgGraphNode *fromNode = cgBuilder->getGraphNodesMapping().at(fromDecl);
    
    assert(fromNode && toNode);
    
    if(callGraph->checkIfDirectedGraphEdgeExists(fromNode, toNode) == false) {
            // add the edge on the call graph
            callGraph->addDirectedEdge(fromNode, toNode);
            
            // update the resolver
            resolver.at(funcExp).push_back(functionDeclaration);
        
            return true;
    }
    
    return false;
}
    
bool IntraProcAliasAnalysis::updateVirtualFunctionInCallGraph (SgFunctionCallExp *funcCall, CompReprPtr &callSiteIN) {
       //get the object which is calling the virtual function
      AliasRelationNode leftNode;
      bool change = false;               
      SgExpression *funcExp = funcCall->get_function();
      std::vector<SgVariableSymbol *>aliases;
      SgExpression *leftSide;

      switch(funcExp->variantT()) {
          case V_SgArrowExp:
          {
              //parse the LHS
              leftSide = isSgBinaryOp(funcExp)->get_lhs_operand();
              ProcessExpression::processLHS(leftSide, leftNode);
              
              // a->  equivalent to (*a).
              leftNode.derefLevel = leftNode.derefLevel + 1;
          }
          break;
          case V_SgDotExp:
          {
              //parse the LHS
              leftSide = isSgBinaryOp(funcExp)->get_lhs_operand();
              ProcessExpression::processLHS(leftSide, leftNode);
          }
          break;
          default:
          {
              return false;
          }
      }
      
      // compute the aliases
      getAliases(callSiteIN, leftNode, aliases);
      for(unsigned int i=0; i < aliases.size(); i++) {
          change |= addVirtualFunction(aliases[i]->get_type(), funcCall);
      }

      return change;
    }
    

 IntraProcAliasAnalysis::~IntraProcAliasAnalysis() {
        
        delete cfg;
        cfg = NULL;
        
        
        delete gen;
        gen = NULL;
        
    }
    
 void IntraProcAliasAnalysis::getAliases(CompReprPtr &ptr, AliasRelationNode &node, std::vector<SgVariableSymbol*>& aliases) {
        
        std::vector<SgGraphNode *> graph_aliases;
        ptr.get()->computeAliases(node.var, node.derefLevel, graph_aliases);
        
        for(unsigned int i=0; i< graph_aliases.size(); i++) {
            aliases.push_back(static_cast<SgVariableSymbol *> (graph_aliases[i]->get_SgNode()));
        }
 }
 void IntraProcAliasAnalysis::setFunctionEntry(CompReprPtr &n) {
        if(entry.get() == NULL) 
            entry = CompReprPtr(new CompactRepresentation());
        
        //entry.get()->merge(*(n.get()));
        entry += n;
 }
    
 void IntraProcAliasAnalysis::setFunctionExit(CompReprPtr &n) {
        if(exit.get() == NULL) 
            exit = CompReprPtr(new CompactRepresentation());
        
        //exit.get()->merge(*(n.get()));
        exit += n;
 }
   

 CompReprPtr IntraProcAliasAnalysis::meet_data( const CompReprPtr& d1, const CompReprPtr& d2) {
        
/*        CompactRepresentation *repr = new CompactRepresentation;

        if(d1.get() != NULL) {
            repr->merge(*(d1.get()));
        }
        if(d2.get() != NULL) {
            repr->merge(*(d2.get()));
        }
 */ 
       CompReprPtr result = CompReprPtr(new CompactRepresentation);
       result += d1;
       result += d2;
       
        return result;
    };

std::vector<SgGraphNode *> IntraProcAliasAnalysis::getPredecessors(SgGraphNode *n) {
      std::vector<SgGraphNode *> nodes;
      cfg->getGraph()->getPredecessors(n, nodes);
      return nodes;
  }


void ProcessExpression::processLHS(SgNode *node, struct AliasRelationNode &arNode) {

    SgVariableSymbol *sym = NULL;
    SgVarRefExp *var_exp;
    int derefLevel = 0;
    switch (node->variantT()) {

        case V_SgInitializedName:
        {
            SgInitializedName *init_exp = isSgInitializedName(node);
            
            ROSE_ASSERT(init_exp != NULL);
            
            sym = static_cast<SgVariableSymbol *>(init_exp->get_symbol_from_symbol_table());
        
        }
        break;   

        case V_SgVarRefExp:
        {    
             var_exp = isSgVarRefExp(node);
             ROSE_ASSERT(var_exp != NULL);
        
            // get the variable symbol
            sym = var_exp->get_symbol();
        }   
        break;
            
        case V_SgPointerDerefExp:
        {   
            SgPointerDerefExp *tmp = isSgPointerDerefExp(node);
            
            SgPointerDerefExp *deref_exp = NULL;
            

            ROSE_ASSERT(tmp != NULL);
            
            while(tmp != NULL) {
                
                deref_exp = tmp;
                
                tmp = isSgPointerDerefExp(deref_exp->get_operand());
                
                derefLevel++;
            }
            
            ProcessExpression::processLHS(deref_exp->get_operand(), arNode);
            arNode.derefLevel += derefLevel;
            
            return;

        }    
        break;

        case V_SgDotExp:
        case V_SgArrowExp:
        {    
            SgBinaryOp *bin_exp = isSgBinaryOp(node);
            if(bin_exp != NULL) {
                ProcessExpression::processLHS(bin_exp->get_rhs_operand(), arNode);
                return;
            }
        }
        break;

        default:
            //std::cout << "Left Node Type : "<< node->variantT() << std::endl;
            sym = NULL;
    }; 

    if(sym != NULL && 
            (SageInterface::isPointerType(sym->get_type()) == false || SageInterface::isReferenceType(sym->get_type())) )
                sym = NULL;
    
    arNode.var = sym;
    arNode.derefLevel = derefLevel;

    
    
}

void ProcessExpression::processRHS(SgNode *node, struct AliasRelationNode &arNode) {
    SgVariableSymbol *sym = NULL;
    SgVarRefExp *var_exp;
    int derefLevel = 0;
    static int new_index;
    static boost::unordered_map<SgExpression*, SgVariableSymbol *> new_variables;
    
    switch (node->variantT()) {
    
        // = a
        case V_SgVarRefExp:
        {    
             var_exp = isSgVarRefExp(node);
            
            ROSE_ASSERT(var_exp != NULL);
        
            // get the variable symbol
            sym = var_exp->get_symbol();
        }   
        break;
        
        // = *a or = **a or = ***a  etc
        case V_SgPointerDerefExp:
        {   
            SgPointerDerefExp *tmp = isSgPointerDerefExp(node);
            
            SgPointerDerefExp *deref_exp = NULL;

            ROSE_ASSERT(tmp != NULL);
            
            while(tmp != NULL) {
                
                deref_exp = tmp;
                
                tmp = isSgPointerDerefExp(deref_exp->get_operand());
                
                derefLevel++;
            }
            ProcessExpression::processRHS(deref_exp->get_operand(), arNode);
            arNode.derefLevel += derefLevel;
            return;
            
        }    
        break;
        
        //  = &b
        case   V_SgAddressOfOp:
        {
            SgAddressOfOp *add_exp = isSgAddressOfOp(node);
 
            assert(add_exp != NULL);
            
            ProcessExpression::processRHS(add_exp->get_operand(), arNode);

            sym = arNode.var;
            derefLevel = arNode.derefLevel-1;
            
        }
       break;
       // a.b or a->b or this->b
        case V_SgDotExp:
        case V_SgArrowExp:
        {    
            SgBinaryOp *bin_exp = isSgBinaryOp(node);
            if(bin_exp != NULL) {
                ProcessExpression::processRHS(bin_exp->get_rhs_operand(), arNode);
                return;
            }
        }
        break;
        // Case where *a = *b = *c ...etc
       case V_SgAssignOp:
       {
            SgExpression *lhs = NULL;
            SgExpression *rhs = NULL;
            if(SageInterface::isAssignmentStatement(node,&lhs, &rhs)){
                
                ROSE_ASSERT(rhs != NULL);
                ProcessExpression::processRHS(rhs, arNode);
                return;
            }
       }
       break;
       
       case V_SgCastExp:
       {
           SgCastExp *cast_exp = isSgCastExp(node);
           ProcessExpression::processRHS(cast_exp->get_operand(), arNode);
           return;
       }
       break;
       
       // C *c = new C or
       // c = new C
       //return new C
       case V_SgNewExp:
       {
        SgNewExp *new_exp = isSgNewExp(node);
        if(new_variables.count(new_exp) == 0){
        
            SgScopeStatement *scope = NULL;
           SgNode *parent=new_exp->get_parent();
           //isSgStatement(parent)->get_scope()
           SgStatement *stmt = NULL;
           
           // find the nearest parent which is a statement
           while(!(stmt = isSgStatement(parent)))
                parent = parent->get_parent();
           
           scope = stmt->get_scope();

           SgType *type = new_exp->get_type()->dereference();
           std::stringstream ss;
           ss << "__tmp_VFA__" << new_index;
           std::string name;
           ss >> name;
           SgName var_name = name;
           
           new_index++;
           
           assert(scope != NULL);
           
           // Create a temporary variable so that every memory location has an assigned name
           SageBuilder::buildVariableDeclaration_nfi(var_name, type, NULL, scope);
           
           //SageInterface::prependStatement(var_decl, scope);
           sym = scope->lookup_variable_symbol(var_name);
           ROSE_ASSERT(sym != NULL);
           new_variables[new_exp] = sym;
        
        }
        else
            sym = new_variables[new_exp];
        derefLevel = derefLevel - 1;
       }
       break;     
       default:
            sym = NULL;
    }
    
    arNode.derefLevel = derefLevel;
    arNode.var = sym;
}


void CollectAliasRelations::processNode(SgGraphNode* g_node){
    
    SgNode *node = g_node->get_SgNode();    
    g->init(g_node);
    

    AliasRelationNode leftARNode, rightARNode;
    SgExpression *lhs = NULL;
    SgExpression *rhs = NULL;
  
    switch(node->variantT()) {
        case V_SgAssignOp: 
        {
            if(!SageInterface::isAssignmentStatement(node,&lhs, &rhs))
                lhs = rhs = NULL;
        }   
        break;
        
        case V_SgAssignInitializer:
        {
              SgAssignInitializer *assgn_i = isSgAssignInitializer(node);
              assert(assgn_i != NULL);
        
            lhs = static_cast<SgExpression *> (assgn_i->get_parent());
            rhs = assgn_i->get_operand();
        
        }
        break;

        case V_SgReturnStmt:
        {
            SgFunctionDeclaration *functionDeclaration =  isSgFunctionDeclaration(isSgFunctionDefinition(
                                                                            cfg->getEntry()->get_SgNode())->get_declaration());

            ROSE_ASSERT(functionDeclaration);
            
            SgFunctionType *functionType = functionDeclaration->get_type();
            SgType *returnType = functionType->get_return_type();
            
            if (SageInterface::isPointerType(returnType) == false && SageInterface::isReferenceType(returnType) == false)  
                return;
            
            SgReturnStmt *returnStmt = isSgReturnStmt(node);
            rhs  = returnStmt ->get_expression();
            
            //std::cout << "Return for processing : " << rhs->unparseToCompleteString() << std::endl;
            ProcessExpression::processRHS(rhs, rightARNode);
            g->addReturnStmt(rightARNode);
            return;
        }
        break;
         
        default:
            lhs = rhs = NULL;
            leftARNode.var = rightARNode.var = NULL;
    }

    if(lhs != NULL)
        ProcessExpression::processLHS(lhs, leftARNode);  
    
    if(rhs != NULL)
        ProcessExpression::processRHS(rhs, rightARNode);

    if(leftARNode.var != NULL && rightARNode.var != NULL) {
            
        g->addNewAliasRelation(g_node, make_pair(leftARNode, rightARNode));
     }

}

void CollectAliasRelations::recursiveCollect(SgGraphNode *node, boost::unordered_map<SgGraphNode*, COLOR> &colors){

    if(node == NULL) return;
    
    colors[node] = GREY;
    
    
    processNode(node);
    
    std::vector<SgDirectedGraphEdge*>outs = StaticCFG::outEdges(node);
    
    for(unsigned int i=0; i< outs.size(); i++ ) {
        
        SgGraphNode *out = outs[i]->get_to();
            if(colors.at(out)  == WHITE) {
        
                recursiveCollect(outs[i]->get_to(), colors);
            }
    
    }
    colors[node] = BLACK;
    
}


void CollectAliasRelations::run() {

    SgIncidenceDirectedGraph *graph = cfg->getGraph();
    
    set<SgGraphNode*> allNodes = graph->computeNodeSet();
    
    boost::unordered_map<SgGraphNode*, COLOR> colors;

    foreach(SgGraphNode *node, allNodes) {
            colors[node] = WHITE;
    }
    
    //repr = new CompactRepresentation();
    
    SgGraphNode* graphNode = cfg->getEntry();
    
    recursiveCollect(graphNode, colors);

#ifndef NDEBUG
    SgFunctionDefinition *defn = isSgFunctionDefinition(cfg->getEntry()->get_SgNode());
#endif
    //repr.toDot(def->get_mangled_name()+"_cr.dot");
    assert(defn != NULL);
    //cfg->cfgToDot(defn, defn->get_mangled_name()+"_cfg.dot");
    

}


void IntraProcAliasAnalysis::applyCFGTransferFunction(SgGraphNode* s) {

    /**
     * If no relation exists then out =in
     * else
     *  out = (in -kill) + gen
     */
    std::vector <std::pair<AliasRelationNode, AliasRelationNode> > relations = gen->getAliasRelations(s);
/*    
      if( relations.size() == 0) {
          gen->setExitData(s,gen->getEntryData(s));
          return;
      }
  */    
      unsigned int i;
      
      CompReprPtr repr(new CompactRepresentation);
        
      CompReprPtr in = gen->getEntryData(s);  // out =in
      
      assert(in.get() != NULL);
      
      repr+= in;
      
      if(s->get_SgNode()->variantT() == V_SgFunctionCallExp || s->get_SgNode()->variantT() == V_SgConstructorInitializer) {
          //CompactRepresentation *repr = new CompactRepresentation();

          //repr->merge(*(out.get()));  // cfg.in
          
          //SgFunctionCallExp *exp = isSgFunctionCallExp(s->get_SgNode());
          SgExpression *exp = isSgExpression(s->get_SgNode());

          ROSE_ASSERT(resolver.count(exp) != 0);
          for(unsigned int index =0; index < resolver.at(exp).size(); index++) {
              
              SgFunctionDeclaration *decl = isSgFunctionDeclaration(resolver.at(exp)[index]);
              
              if(decl == NULL) continue;
              
              //std::cout << "CFG Xfer function for: "<< exp->unparseToCompleteString()<< ":->"<< decl->unparseToCompleteString()<< std::endl;
              if(mapping.count(decl) == 0) {
              
                  // built in function/ Compiler generated function so skip
              } else {
              
                    IntraProcAliasAnalysis *ipa = mapping.at(decl);
                    ROSE_ASSERT(ipa != NULL);
                    CompReprPtr thatExit = ipa->getFunctionExit();
              
                    //if(thatExit.get() != NULL)
                    // repr->merge(*(thatExit.get()));
                    repr += thatExit;
             }
              
              //c_out = c_in U f_exit
          }
      
          for(i=0; i< relations.size(); i++) 
            repr.get()->addMayAliasRelation(relations[i].first, relations[i].second);

          
      } else {
                for(i=0; i< relations.size(); i++) 
                   repr.get()->addMustAliasRelation(relations[i].first, relations[i].second);
                
                
                //gen->setExitData(s, out);

      }
      
          gen->setExitData(s, repr);
      
 }




#define TEMPLATE_ONLY
#include <IntraProcDataFlowAnalysis.C>
template class IntraProcDataFlowAnalysis<SgGraphNode, CompReprPtr >;

#include<customFilteredCFG.C>
template class StaticCFG::CustomFilteredCFG<AliasCfgFilter>;
