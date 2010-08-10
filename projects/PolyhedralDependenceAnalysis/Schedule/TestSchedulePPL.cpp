
#include "RoseToFadaAST.hpp"
#include "FadaToPPL.hpp"
#include "Schedule.hpp"

int main ( int argc, char* argv[] ) {
	SgProject * project = frontend ( argc , argv ) ;
	
	SgFilePtrList & file_list = project->get_fileList();
	SgFilePtrList::iterator it_file_list;
   	for (it_file_list = file_list.begin(); it_file_list != file_list.end(); it_file_list++) {
   	
		SgSourceFile * source_file = isSgSourceFile(*it_file_list);
		ROSE_ASSERT(source_file);
		
		SgDeclarationStatementPtrList & decl_list = source_file->get_globalScope()->get_declarations();
		SgDeclarationStatementPtrList::iterator it_decl_list;
		for (it_decl_list = decl_list.begin(); it_decl_list != decl_list.end(); it_decl_list++) {
			
			std::vector<FadaToPPL::PolyhedricDependence *> deps;
			
			SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_decl_list);
			if (!func_decl || func_decl->get_name().getString()[0] == '_') continue;
					
			fada::Program * program = new fada::Program();
			
			program->SetSyntaxTree(RoseToFada::parseSgFunctionDeclarationToFadaAST(func_decl));
			
			program->ComputeSourcesForAllReadVariables();
			
			RoseToFada::FadaRoseCrossContext * ctx = new RoseToFada::FadaRoseCrossContext(program, func_decl);
			
//			std::cout << "FADA analysis done ! Construction of Polyhedric Depences..." << std::endl;
	
			// Traverse Quast for statement where a variable is write (that can have a dependence)
			std::vector<fada::References*>::iterator it0;
			for(it0 = program->GetNormalizedStmts()->begin(); it0 != program->GetNormalizedStmts()->end(); it0++) {
				if (!(*it0)->GetWV()->empty()) {
//					std::cout << "\tA writer statement found: " << (*it0)->GetStmtID() << std::endl;
					std::vector<fada::Read_Reference*>::iterator it1;
					for(it1 = (*it0)->GetRV()->begin(); it1 != (*it0)->GetRV()->end(); it1++) {
//						std::cout << "\t\tTraverse Quast of Read Variables..." << std::endl;
						std::vector<FadaToPPL::PolyhedricDependence *> * vect = FadaToPPL::traverseQuast(
							ctx,
							ctx->getSgStatementByID((*it0)->GetStmtID()),
							(*it1)->GetDefinition()
						);
						std::vector<FadaToPPL::PolyhedricDependence *>::iterator it2;
						for (it2 = vect->begin(); it2 != vect->end(); it2++)
							deps.push_back(*it2);
						delete vect;
					}
				}
			}
			
/*			std::cout << "Dependences in " << func_decl->get_name().getString() << ":" << std::endl;
			std::vector<FadaToPPL::PolyhedricDependence *>::iterator it;
			for (it = deps.begin(); it != deps.end(); it++) {
				(*it)->print(std::cout);
				std::cout << std::endl;
				(*it)->printMinimized(std::cout);
				std::cout << std::endl;
			}
*/			
			PolyhedralSchedule::ValidScheduleSpacePPL * vss = new PolyhedralSchedule::ValidScheduleSpacePPL(ctx, deps);
			
			vss->print(std::cout);
		}
	}
	
	return 0;
}
