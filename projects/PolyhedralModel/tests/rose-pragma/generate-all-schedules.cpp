
#include "rose-pragma/PolyhedricPragma.hpp"

int main ( int argc, char* argv[] ) {

	if (argc != 3) {
		std::cerr << "Usage: generate-all-schedule file.c directory" << std::endl;
		ROSE_ASSERT(false);
	}

	SgSourceFile * source_file = isSgSourceFile(SageBuilder::buildFile(argv[1], argv[1]));
	
	Rose_STL_Container<SgNode*> pragma_decls = NodeQuery::querySubTree(source_file, V_SgPragmaDeclaration);
	
	if (pragma_decls.size() == 1) {
		SgPragmaDeclaration * pragma_decl = isSgPragmaDeclaration(pragma_decls[0]);

		try {
		
		 // Generate polyhedric modelisation 
			PolyhedricAnnotation::parse(pragma_decl);

			PolyhedricAnnotation::PragmaPolyhedralProgram & polyhedral_program =
					PolyhedricAnnotation::getPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(pragma_decl);

		 // Compute dependency from the generated modelisation
			std::vector<PolyhedricDependency::PragmaDependency *> * deps =
				PolyhedricDependency::ComputeRaW<SgPragmaDeclaration, SgExprStatement, RoseVariable>(polyhedral_program);

		 // Generate the space containing all valid schedules for the previously compute dependencies	
			Scheduling::PragmaScheduleSpace schedule_space(polyhedral_program, *deps, Scheduling::PBCC);

		 // Traverse the schedule's space to list the schedules
			std::vector<Scheduling::PragmaSchedule> * schedules = schedule_space.generateScheduleList(-1, 1, -1, 1, -1, 1);

		 // **********

			SgStatement * next_statement = SageInterface::getNextStatement(pragma_decl);
			SageInterface::removeStatement(next_statement);
			
			SgStatement * statement_to_replace = pragma_decl;
			size_t cnt = 0;
			std::vector<Scheduling::PragmaSchedule>::iterator it;
			for (it = schedules->begin(); it != schedules->end(); it++) {
			
				std::ostringstream oss;
				oss << cnt;
				
				std::string file_name(argv[1]);
			
				size_t point = file_name.find_last_of(".");
			
				file_name.insert(point, oss.str());
				
				size_t slash = file_name.find_last_of("/");
				
				source_file->set_unparse_output_filename(std::string(argv[2]) + std::string("/") + file_name.substr(slash+1));
				
				SgBasicBlock * generated_block = CodeGeneration::generateBasicBlockFor(*it);
				SageInterface::replaceStatement(statement_to_replace, generated_block);
				statement_to_replace = generated_block;
				
				source_file->unparse();
				
				cnt++;
			}

		}
		catch (Exception::ExceptionBase & e) {
			e.print(std::cerr);
		}
	}
	else {
		std::cerr << "Need to have one and only one pragma in the program for this." << std::endl;
	}

	return 0;
}
