#include <genCfg.h>

#include <mops/Cfg.h>
#include <mops/MetaFsa.h>
#include <mops/Pda.h>
#include <mops/Fsa.h>

#include <java/lang/Throwable.h>
#include <java/util/Iterator.h>
#include <java/util/Vector.h>

#include <iostream>
using namespace std;

int main(int argc, char **argv) {
	using namespace mops;
	using java::lang::String;
	using java::lang::Throwable;
	using java::util::Iterator;
	using java::util::Vector;

	JvCreateJavaVM(0);
	JvAttachCurrentThread(0, 0);

	SgProject *prj = frontend(argc-1, argv+1);

	try {
		Cfg *cfg = MopsBuildProject(prj);

		jstring strMain = JvNewStringLatin1("main");
		MetaFsa *mfsa = new MetaFsa;
		mfsa->read(JvNewStringLatin1(argv[1])); // in the future we may use ROSE to build the mfsa ourselves using a more natural syntax
		cfg->compact(mfsa, strMain);

		Pda *pda = new Pda;
		pda->read(cfg, strMain);

		mfsa->resolveVariable(cfg);
		Iterator *i = mfsa->iterator();

		while (i->hasNext()) {
		     mfsa = (MetaFsa *)i->next();
		     Pda *pda2 = pda->compose(mfsa);
		     Fsa *fsa2 = pda2->post();

		     Vector *initialTransitions = fsa2->findInitialTransitions();

		     if (initialTransitions->size() == 0) {
			  cout << "The program satisfies the property." << endl;
		     } else {
			  cout << "The program does not satisfy the property." << endl;
		     }
		}
	} catch (Throwable *e) {
		e->printStackTrace();
	}
}

