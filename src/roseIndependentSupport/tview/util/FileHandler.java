package util;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;



public class FileHandler {

	private boolean debug=false;
	public String textDir[];

	public File[] testFiles;

	Viewer view;
	
	public FileHandler(Viewer v) {
		view=v;
	}
	
	public int totalnroffiles=0;
	
	public boolean readFile(String path) {
		String result = "";
		File file = new File(path);
		boolean isDir = file.isDirectory();
		if (isDir) {
			testFiles = file.listFiles();
			textDir = new String[testFiles.length];
			System.out.println("#files in directory: " + testFiles.length);
			for (int i = 0; i < testFiles.length; i++) {
				if (testFiles[i].isFile()) {
					String result2 = "";
					file = testFiles[i];
					if (isUnwantedFile(file.toString())) {
						continue;
					}
					BufferedReader is = null;
					try {
						is= new BufferedReader(new FileReader(file));

					} catch (FileNotFoundException e) {
						System.err.println("File in Dir not found: " + file);
						System.exit(0);
					}
					totalnroffiles++;
					if (is!=null) {
					try {
						String texttmp = "";
						while ((texttmp = is.readLine()) != null) { // while
																	// loop
							if (view.dollar)
							result2 += (texttmp + "$\n");
							else
							result2 += (texttmp + "\n");
						}
					} catch (IOException e) {
						e.printStackTrace();
					} // end while
					}
					textDir[i] = result2;
					if (i % 50 == 0)
							System.err.println("checked #files: " + totalnroffiles);
				}
			}

			return true;
		} else {
			BufferedReader is=null;
			try {
				is = new BufferedReader(new FileReader(file));
			} catch (FileNotFoundException e) {
				System.err.println("File not found: " + file);
				System.exit(0);
			}
			if (is!=null) {
				String tmp="";
				try {
					while ((tmp = is.readLine()) != null) { // while loop
						// begins
						// if (text.trim().length() > 0)
						if (view.dollar)
						result += (tmp + "$\n");
						else
						result += (tmp + "\n");
					}
				} catch (IOException e) {
					e.printStackTrace();
				} // end while
			}
			view.text = result;
			
		}
		return false;
	}
	public boolean isUnwantedFile(String file) {
		if(file.endsWith(".png") || file.endsWith(".o"))
			return true;
		return false;
	}
	
	public boolean pngFileExists(File file) {
		boolean exists=false;
		BufferedReader is2=null;
		File filepng = null;
		try {
			filepng = new File(file+".png");
			is2 = new BufferedReader(new FileReader(filepng));
			exists=true;
		} catch (FileNotFoundException e) {
		}
		return exists;
	}
	
	
	public void readHFile(String file) {
		BufferedReader is2=null;
		File fileh = null;
		try {
			fileh = new File(file+".highlight");
			is2 = new BufferedReader(new FileReader(fileh));
		} catch (FileNotFoundException e) {
			return;
		}
		if (is2==null) return;
		System.out.println("Highlight file found: " + fileh);
		view.highlightPatterns.clear();
		view.colorPatterns.clear();
		String tmp="";
		if (is2!=null) {
			try {
				while ((tmp = is2.readLine()) != null) { // while loop
					// begins
					StringTokenizer t=new StringTokenizer(tmp,",");
					int startLine = Integer.parseInt(t.nextToken());
					int startCol = Integer.parseInt(t.nextToken());
					String color = t.nextToken();
					int endLine = Integer.parseInt(t.nextToken());
					int endCol = Integer.parseInt(t.nextToken());
					int start = getAbsolutePos(startLine, startCol);
					int end = getAbsolutePos(endLine, endCol+1);
					// if (text.trim().length() > 0)
					String textPat=start+":"+end;
					if ((end-start)<=0) {
						System.err.println("-> Warning in .highlight file: Length <= 0 in: \""+tmp+"\"");
					} else {
					view.highlightPatterns.add(textPat);
					if (view.debug)
						System.err.println("  adding color: "+color+" to pat:"+textPat);
					view.colorPatterns.put(textPat, color);
					}
				}
			} catch (IOException e) {
				System.err.println("Your .h file is incorrect!");
				e.printStackTrace();
			} // end while
		}
	}

	private int getAbsolutePos(int line, int col) {
		int abspos=0;
		char[] txt = view.text.toCharArray();
		int l=1;
		int c=0;
		if (debug)
			System.err.println("*** *** line:"+line+" col:"+col);
		for (int i=0; i < txt.length; i++) {
			c++;
			if (debug)
					System.err.println("l: "+l+" c:"+c+"  abs:"+abspos+" char:"+txt[i]);

//			if (l==3)
//				System.exit(0);
			if (l==line && c==col) {
				if (debug)
					System.err.println("converting line:"+line+" col:"+col+"  to abs:"+abspos);
				return abspos;
			}
			if (txt[i]=='\n') {
				if (l==line && c!=col) {
					if (debug)
						System.err.println("converting line:"+line+" col:"+col+"  to abs:"+abspos);
					return abspos;
				}
				l++;
				c=0;
			} 
			abspos++;
		}
		if (debug)
			System.err.println("converting line:"+line+" col:"+col+"  to abs:"+abspos);
		return abspos;
	}



}