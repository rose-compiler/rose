
#include "rose-max-cover/TDBUtraversal.hpp"

int main(int argc, char * argv[]) {
    SgProject* project = frontend(argc,argv);

    PolyhedralModelisation poly_model;

    poly_model.traverse(project);

    std::ofstream report_file;
    report_file.open(((*project)[0]->getFileName() + ".report").c_str());

    if (report_file.is_open()) {
        poly_model.genReport(report_file);
        std::cout << "Analysis report saved in " << (*project)[0]->getFileName() << ".report"  << std::endl;
    }
    else
        std::cerr << "Cannot open " << (*project)[0]->getFileName() << ".report to save the analysis report..."  << std::endl;

    std::ofstream analysis_file;
    analysis_file.open(((*project)[0]->getFileName() + ".analysis").c_str());

    if (analysis_file.is_open()) {
        std::set<PolyhedralElement *> collection;

        analysis_file << "Higher level statements allowing non_linear_data_accesses and data_dependant_conditionals:" << std::endl << std::endl;

        poly_model.collectPolyhedralElement(collection, true, false, true, true, false); // default arguments...

        std::set<PolyhedralElement *>::iterator it;
        for (it = collection.begin(); it != collection.end(); it++) {
            const PolyhedralControl * control = dynamic_cast<const PolyhedralControl *>(*it);
            const PolyhedralStatement * statement = dynamic_cast<const PolyhedralStatement *>(*it);
            SgLocatedNode * node;
            if (control != NULL)
                node = isSgLocatedNode(control->associated_node);
            else if (statement != NULL)
                node = isSgLocatedNode(statement->statement);
            else
                ROSE_ASSERT(false);
            ROSE_ASSERT(node != NULL);
            Sg_File_Info * fi = node->get_startOfConstruct();
            analysis_file << "\tNode " << node << " of type " << node->class_name() << " at " << fi->get_filenameString() << ":" << fi->get_line() << std::endl;
            (*it)->print(analysis_file, "\t\t");
            analysis_file << std::endl;
        }
    }
    else
        std::cerr << "Cannot open " << (*project)[0]->getFileName() << ".analysis to save the analysis..."  << std::endl;

    return 0;
}
