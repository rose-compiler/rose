package gov.llnl.casc.rose;

import java.io.FileOutputStream;
import java.io.PrintStream;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;
import java.util.Stack;
import java.util.Iterator;

public class Ptr
{
    private Ptr(long l)
    {
        cptr = l;
        links = new HashSet<Ptr>();
        sources = new HashSet<Object>();
    }
    
    public static Ptr createPtr(long l)
    {
        Ptr ptr = null;
        WeakReference<Ptr> weakreference = table.get(new Long(l));
        if(weakreference != null)
            ptr = weakreference.get();
        if(ptr == null) {
	    ptr = new Ptr(l);
	    table.put(new Long(l), new WeakReference<Ptr>(ptr));
        }
        return ptr;
    }
    
    public int hashCode()
    {
        return (int)cptr;
    }

    public boolean equals(Object obj)
    {
        if(obj instanceof Ptr)
            return ((Ptr)obj).cptr == cptr;
        else
            return false;
    }
    
    public void addSource(Object obj)
    {
        sources.add(obj);
    }
    
    public void removeSource(Object obj)
    {
        sources.remove(obj);
    }
    
    public static void setLinksFor(Object obj, Ptr ptr)
    {
        if(obj instanceof SgNode)
        {
            HashSet<Ptr> hashset = new HashSet<Ptr>();
            SgNode sgnode = (SgNode)obj;
            unsignedVector unsignedvector = sgnode.get_graph();
	    //SWIGTYPE_p_std__vectorTunsigned_long_t unsignedvector = sgnode.get_graph();
            long l = unsignedvector.size();
            for(int j = 0; (long)j < l; j++)
                hashset.add(createPtr(unsignedvector.get(j)));

            ptr.links = hashset;
        } else {
	    if(obj instanceof GenericVector) {
		HashSet<Ptr> hashset1 = new HashSet<Ptr>();
		GenericVector genericvector = (GenericVector)obj;
		for(int i = 0; (long)i < genericvector.size(); i++) {
		    Object obj1 = genericvector.get(i);
		    if(obj1 instanceof SgNode)
			hashset1.add(createPtr(SgNode.getCPtr((SgNode)obj1)));
		}
		ptr.links = hashset1;
	    }
	}
    }

    public static void debugGraph(Ptr ptr)
    {
        HashSet<Ptr> hashset = new HashSet<Ptr>();
        Stack<Ptr> stack = new Stack<Ptr>();
        PrintStream printstream;
        try {
            printstream = new PrintStream(new FileOutputStream("/tmp/g.dot"));
	}
        catch(Exception exception) {
            return;
        }
        stack.push(ptr);
        printstream.println("digraph mygraph {");
        do {
            if(stack.empty())
                break;
            Ptr ptr1 = stack.pop();
            if(!hashset.contains(ptr1)) {
		Iterator iterator = ptr1.sources.iterator();
		Object obj = null;
                do {
                    if(!iterator.hasNext())
                        break;
                    obj = iterator.next();
                } while(obj == null);
                if(obj == null)
                    printstream.println((new StringBuilder())
					.append("Ptr_")
					.append(ptr1.cptr)
					.append(" [label=\"")
					.append(ptr1.cptr)
					.append("\"];").toString());
                else
                    printstream.println((new StringBuilder())
					.append("Ptr_")
					.append(ptr1.cptr)
					.append(" [label=\"")
					.append(obj)
					.append("\"];").toString());
                hashset.add(ptr1);
                Iterator<Ptr> iterator1 = ptr1.links.iterator();
                while(iterator1.hasNext()) {
		    Ptr ptr2 = iterator1.next();
                    stack.push(ptr2);
                    printstream.println((new StringBuilder())
					.append("Ptr_")
					.append(ptr1.cptr)
					.append(" -> Ptr_")
					.append(ptr2.cptr)
					.append(";").toString());
                }
            }
        } while(true);
        printstream.println("}");
        printstream.close();
        try
        {
            Runtime.getRuntime().exec(new String[] {
		    "dotty", "/tmp/g.dot"
		});
        }
        catch(Exception exception1) { }
    }
    
    private long cptr;
    private static HashMap<Long, WeakReference<Ptr>> table =
	new HashMap<Long, WeakReference<Ptr>>();
    private Set<Ptr> links;
    private Set<Object> sources;
}
