
#include "rose-max-cover/TDBUtraversal.hpp"

int main(int argc, char * argv[]) {
    SgProject* project = frontend(argc,argv);

    std::ofstream report_file;
    report_file.open(((*project)[0]->getFileName() + ".report").c_str());
    
    if (report_file.is_open()) {
        PolyhedralModelisation poly_model;

        poly_model.traverse(project);

        std::set<PolyhedralElement *> collection;

        poly_model.collectPolyhedralElement(collection, true, false, true, true, false); // default arguments...

        std::set<PolyhedralElement *>::iterator it;
        for (it = collection.begin(); it != collection.end(); it++) {
            const PolyhedralControl * control = dynamic_cast<const PolyhedralControl *>(*it);
            if (control == NULL) continue;
            SgFunctionDefinition * func_def = isSgFunctionDefinition(control->associated_node);
            if (func_def == NULL) continue;
            SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(func_def->get_declaration());
            if (func_decl == NULL) continue;
            if (!func_decl->get_functionModifier().isCudaKernel()) continue;
            Sg_File_Info * fi = func_def->get_startOfConstruct();
            report_file << "Found: " << func_decl->get_name() << " at " << fi->get_filenameString() << ":" << fi->get_line() << std::endl;
            report_file << "\tStructure:" << std::endl;
            (*it)->print(report_file, "\t\t");
            report_file << std::endl;
            report_file << "\tAccesses:" << std::endl;
            std::set<Access *> accesses;
            (*it)->collectAccess(accesses);
            std::set<Access *>::iterator it_access;
            for (it_access = accesses.begin(); it_access != accesses.end(); it_access++) {
                (*it_access)->print(report_file, "\t\t");
            }
            report_file << std::endl;
            report_file << std::endl;
        }
    }   
    else
        std::cerr << "Cannot open " << (*project)[0]->getFileName() << ".report to save the analysis report..."  << std::endl;
    return 0;
}
