
#ifndef PACECC_PACECCMODULE_HPP
#define PACECC_PACECCMODULE_HPP

#include <rose.h>
#include <string>

using namespace std;

class PaceccModule {

private:
	bool enable;
	string modulePrefix;

public:
	PaceccModule(string module_prefix) : enable(true), modulePrefix(module_prefix) {

	}

	virtual int visit(SgProject * project) = 0;

	virtual void handleModuleOptions(Rose_STL_Container<string> &args) = 0;

	virtual bool isEnable() {
		return enable;
	}

	virtual bool isDisable() {
		return !isEnable();
	}

	virtual bool setEnable() {
		enable = true;
	}

	virtual bool setDisable() {
		enable = false;
	}

	virtual string getModulePrefix() {
		return modulePrefix;
	}

	virtual void cleanCommandLine(Rose_STL_Container<string> &args) {
		CommandlineProcessing::removeArgs (args, getModulePrefix());
	}

	virtual void handleOptions(Rose_STL_Container<string> &args) {
	    if (CommandlineProcessing::isOption(args, getModulePrefix(), "disable", true)) {
	    	setDisable();
	    }

	    if (isEnable()) {
	    	handleModuleOptions(args);
		}

	   	// cleaning remaining module options
	   	cleanCommandLine(args);
	}
};


#endif
