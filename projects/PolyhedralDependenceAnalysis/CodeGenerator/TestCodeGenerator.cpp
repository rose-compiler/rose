
#include "RoseToFadaAST.hpp"
#include "FadaToPPL.hpp"
#include "Schedule.hpp"
#include "CodeGenerator.hpp"

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
			
			SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_decl_list);
			if (!func_decl || func_decl->get_name().getString()[0] == '_') continue;
					
			fada::Program * program = new fada::Program();
			
			program->SetSyntaxTree(RoseToFada::parseSgFunctionDeclarationToFadaAST(func_decl));
			
			program->ComputeSourcesForAllReadVariables();
			
			RoseToFada::FadaRoseCrossContext * fada_ctx = new RoseToFada::FadaRoseCrossContext(program, func_decl);
			std::vector<FadaToPPL::PolyhedricDependence *> * deps = FadaToPPL::constructAllPolyhedricDependences(fada_ctx, program);
			
			FadaToPPL::PolyhedricContext * ppl_ctx = new FadaToPPL::PolyhedricContext(fada_ctx);
	
			PolyhedralSchedule::ValidScheduleSpacePPL * vss = new PolyhedralSchedule::ValidScheduleSpacePPL(ppl_ctx, deps);
		
			vss->bounding(-2, 2, 1);
			
			std::vector<PolyhedralSchedule::ProgramSchedule *> * all_valid_schedule = vss->generateAllValidSchedules();
			
			int cnt = 0;
			std::cout << "*-*-*-*-*-*-*-*-*" << std::endl;
			std::vector<PolyhedralSchedule::ProgramSchedule *>::iterator schedule_it;
			for (schedule_it = all_valid_schedule->begin(); schedule_it != all_valid_schedule->end(); schedule_it++) {
				std::cout << std::endl << cnt++ << std::endl;
				(*schedule_it)->print(ppl_ctx, std::cout);
				std::cout << std::endl;
				CodeGenerator::generate(*schedule_it, ppl_ctx);
				std::cout << std::endl;
				std::cout << "*-*-*-*-*-*-*-*-*" << std::endl;
			}
			
/*			PolyhedralSchedule::ProgramSchedule * sched;
			
			std::cout << "*-*-*-*-*-*-*-*-*" << std::endl << std::endl;
			
			sched = all_valid_schedule->at(1);
			sched->print(ppl_ctx, std::cout);
			std::cout << std::endl;
			CodeGenerator::generate(sched, ppl_ctx);
			std::cout << std::endl;
			
			std::cout << "*-*-*-*-*-*-*-*-*" << std::endl << std::endl;
			
			sched = all_valid_schedule->at(6);
			sched->print(ppl_ctx, std::cout);
			std::cout << std::endl;
			CodeGenerator::generate(sched, ppl_ctx);
			std::cout << std::endl;
			
			std::cout << "*-*-*-*-*-*-*-*-*" << std::endl;*/
		}
	}
	
	return 0;
}
