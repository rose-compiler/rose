#include <genCfg.h>
#include <java/lang/Throwable.h>

int main(int argc, char **argv) {
	using namespace mops;
	using java::lang::String;
	using java::lang::Throwable;
	using java::util::Iterator;
	using java::util::Vector;

	JvCreateJavaVM(0);
	JvAttachCurrentThread(0, 0);

	SgProject *prj = frontend(argc, argv);

	try {
		Cfg *cfg = MopsBuildProject(prj);
		cfg->write(JvNewStringLatin1(prj->get_outputFileName().c_str()));
	} catch (Throwable *e) {
		e->printStackTrace();
	}
}
