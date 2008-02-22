import gov.llnl.casc.rose.sage3;
import gov.llnl.casc.rose.SgProject;

public class test {
    static {
	System.loadLibrary("sage3_wrap");
    }

    static public void main(String[] args) {
        String[] myargs = new String[args.length+1];
	myargs[0] = "test";
        for (int i = 0; i < args.length; ++i) {
	    myargs[i+1] = args[i];
        }
	SgProject p = sage3.frontend(myargs.length, myargs);
	if (p instanceof SgProject) {
	    System.exit(sage3.backend(p));
	}
	System.exit(-1);
    }
}
