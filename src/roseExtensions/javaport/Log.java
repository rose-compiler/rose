package gov.llnl.casc.rose;

import java.io.FileNotFoundException;
import java.io.PrintStream;

class Log
{
    static PrintStream log;

    static {
        try {
	    log = new PrintStream("/tmp/nodes.log");
        }
        catch(FileNotFoundException filenotfoundexception) {
            filenotfoundexception.printStackTrace();
        }
    }
}
