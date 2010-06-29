
#include "rose.h"
#include "Schedule.hpp"
#include "FadaToPPL.hpp"

#ifndef __CODE_GENERATOR_PPL_
#define __CODE_GENERATOR_PPL_

/*namespace Parma_Polyhedra_Library {
	class Variable;
	class C_Polyhedron;
	class Constraint;
	class Constraint_System;
}*/

namespace CodeGenerator {
	
	void generate(PolyhedralSchedule::ProgramSchedule * schedule, FadaToPPL::PolyhedricContext * ctx);
	
	
}

#endif
