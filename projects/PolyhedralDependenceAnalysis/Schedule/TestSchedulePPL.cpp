
#include "RoseToFadaAST.hpp"
#include "FadaToPPL.hpp"
#include "Schedule.hpp"

void print(FadaToPPL::PolyhedricContext * ppl_ctx, std::vector<PolyhedralSchedule::ProgramSchedule *> * schedule_set) {
	int statement_cnt = 0, cnt = 0;
	
	std::vector<PolyhedralSchedule::ProgramSchedule *>::iterator schedule_it;
	PolyhedralSchedule::StatementSchedule * statement_schedule;
	
	std::vector<SgStatement *> * stmts = ppl_ctx->getStatements();
	std::vector<SgStatement *>::iterator stmts_it;
	
	std::vector<std::string> * var_list;
	std::vector<std::string>::iterator var_it;

	for (stmts_it = stmts->begin(); stmts_it != stmts->end(); stmts_it++) {
	
		var_list = ppl_ctx->getDomain(*stmts_it)->getIteratorsList();
		for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) {
			std::cout << "| A_" << statement_cnt << "_" << *var_it << " ";
		}
		
		var_list = ppl_ctx->getGlobals();
		for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) {
			std::cout << "| B_" << statement_cnt << "_" << *var_it << " ";
		}
		std::cout << "|  K_" << statement_cnt << "  ";
		statement_cnt++;
	}
	std::cout << "|" << std::endl;
	
	for (stmts_it = stmts->begin(); stmts_it != stmts->end(); stmts_it++) {
		var_list = ppl_ctx->getDomain(*stmts_it)->getIteratorsList();
		for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) std::cout << "|-------";
		var_list = ppl_ctx->getGlobals();
		for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) std::cout << "|-------";
		std::cout << "|-------";
	}
	std::cout << "|" << std::endl;
	
	for (schedule_it = schedule_set->begin(); schedule_it != schedule_set->end(); schedule_it++) {
		for (stmts_it = stmts->begin(); stmts_it != stmts->end(); stmts_it++) {
			statement_schedule = (*schedule_it)->getSchedule(*stmts_it);
			
			cnt = 0;
			var_list = ppl_ctx->getDomain(*stmts_it)->getIteratorsList();
			for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) {
				std::cout << "| " << std::setw(5) << statement_schedule->getIteratorCoef(cnt++) << " ";
			}
			
			cnt = 0;
			var_list = ppl_ctx->getGlobals();
			for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) {
				std::cout << "| " << std::setw(5) << statement_schedule->getGlobalCoef(cnt++) << " ";
			}
			std::cout << "| " << std::setw(5) << statement_schedule->getConstant() << " ";
		}
		std::cout << "|" << std::endl;
	}
	
	for (stmts_it = stmts->begin(); stmts_it != stmts->end(); stmts_it++) {
		var_list = ppl_ctx->getDomain(*stmts_it)->getIteratorsList();
		for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) std::cout << "|-------";
		var_list = ppl_ctx->getGlobals();
		for (var_it = var_list->begin(); var_it != var_list->end(); var_it++) std::cout << "|-------";
		std::cout << "|-------";
	}
	std::cout << "|" << std::endl;
	std::cout << std::endl;
	
	delete stmts;
}

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
			
			print(ppl_ctx, all_valid_schedule);
		}
	}
	
	return 0;
}
