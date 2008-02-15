import java.awt.Color;

import util.ColorDialog;
import util.Colors;
import util.Viewer;



public class tview {


	  
	public static void main(String[] args) {
		ColorDialog colDiag = new ColorDialog();
		Viewer viewer = new Viewer();
		String filename = "";
		int width=1000;
		int height=800;
		System.out.println("tview version 0.11  (11-06-2006)");
		if (args.length < 1) {
			System.out.println("Usage: java tview [file] [-w width] [-h height] [-screenshot] [-syntax] [-t time] [-f fontsize] [-light] [-eol]");
			System.out.println("Tip: [F10] = make a screenshot.");
			String[] colors = Colors.colorCodes;
			for (int i=0;i<colors.length;i++) {
				String color = colors[i];
				String colorStr = color.substring(13,color.lastIndexOf(":"));
				Color col = Colors.fetchColor(colorStr);
				if (color.contains("on")) {
					System.out.println("     color: "+color);
					colDiag.addColor(color, col);
				}
			}
			colDiag.showDiag();
			System.exit(0);
		} else {
			filename = args[0];
		}
		if (args.length>=2)
			for (int i=1; i<args.length;i++) {
				String arg = args[i];
				if (arg.startsWith("-w")) {
					width=Integer.parseInt(args[i+1]);
					System.out.print(" w="+width+" ");
				}
				if (arg.startsWith("-h")) {
					height=Integer.parseInt(args[i+1]);
					System.out.print(" h="+height+" ");
				}
				if (arg.startsWith("-screenshot")) {
					viewer.screenshot=true;
					System.out.print(" screenshot=true ");
				}
				if (arg.startsWith("-syntax")) {
					viewer.syntax=true;
					System.out.print(" syntax=true ");
				}
				if (arg.startsWith("-eol")) {
					viewer.dollar=true;
					System.out.print(" showEndofLine=true ");
				}
				if (arg.startsWith("-t")) {
					double wt=Double.parseDouble(args[i+1]);
					viewer.waittime=(long) wt;
					System.out.print(" waittime="+wt+" ");
				}
				if (arg.startsWith("-f")) {
					int fontsize=Integer.parseInt(args[i+1]);
					System.out.print(" fontsize="+fontsize+" ");
					Colors.fontsize=fontsize;
				}
				if (arg.startsWith("-light")) {
					viewer.highlight=true;
					System.out.print(" highlight=true ");
				}
			}
		System.out.println("\nLoading filename: " + filename);
		viewer.loadFile(filename, width, height);
		System.out.print("done. ");

	}
	
      
}
