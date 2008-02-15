package util;

import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Toolkit;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Stack;
import java.util.Vector;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Element;
import javax.swing.text.Highlighter;
import javax.swing.text.StyledDocument;


public class Viewer {
	public long waittime = (long) 1.0;

	public boolean debug = false;
	public boolean dollar = false;

	private String debugInfo = "";

	public boolean highlight =false;
	private int filesFailed = 0;

	private WindowListener wl = new WindowAdapter() {
		public void windowClosing(WindowEvent se) {
			System.out.println("closing.");
			System.exit(0);
		}
	};

	public Viewer() {
		debugInfo = "";
		textFrame = new JFrame("tview");
		positionInfo = new JLabel("absPos: 0  line: 0  column: 0");
		positionSearchLabel = new JLabel("Search:");
		positionSearch = new JTextField("");

		textFrame.addWindowListener(wl);
//		positionInfo.setAlignmentX(JLabel.RIGHT);
		positionInfo.setHorizontalTextPosition(JLabel.RIGHT);
		positionInfo.setHorizontalAlignment(JLabel.RIGHT);
		
		positionSearch.getDocument().addDocumentListener(new DocumentListener(){
			public void removeUpdate(DocumentEvent e){
				if (!positionSearch.getText().equals("")) {
					highlightYellow();
				} else {
					hl.clear(field);
				}
			}
			public void insertUpdate(DocumentEvent e)	{
				highlightYellow();
//				updateCaretPosition();
			}
			public void changedUpdate(DocumentEvent e)	{
			}
			}); 
			
		positionSearch.addFocusListener(new MyFocusListener());
	}
	
	public class MyFocusListener extends FocusAdapter {
        public void focusGained(FocusEvent evt) {
        	updateCaretPosition();
        }
        public void focusLost(FocusEvent evt) {
			hl.clear(field);
        	highlight();
        }
    }
	private void updateCaretPosition() {
		if (!positionSearch.getText().equals("")) {
			highlightYellow();
		} else
			hl.clear(field);
		
		if (!positionSearch.getText().equals("")) {
			int lastpos = field.getText().indexOf(positionSearch.getText(), field.getCaretPosition()+1);
			if (lastpos==-1)
				lastpos = field.getText().indexOf(positionSearch.getText(), 0);
			if (lastpos==-1)
				lastpos=0;
			field.setCaretPosition(lastpos);
			int lightpos = field.getCaretPosition()-1;
			if (lightpos!=-1)
			hl.highlight(field,lightpos,1,myHighlightPainterRed);
		}
	}
	
	private void highlight() {
		if (highlight) {
			hl.clear(field);
			hl.highlight(field, "$",myHighlightPainterLightGrey);
			hl.highlight(field, " ",myHighlightPainterGrey);
			hl.highlight(field, "\t",myHighlightPainterDarkGrey);
		}
	}

	private void highlightYellow() {
		hl.clear(field);
		hl.highlight(field, positionSearch.getText(),myHighlightPainterYellow);
	}
	
	private JFrame textFrame = null;
	private JTextField positionSearch = null;
	private JLabel positionInfo =null;
	private JLabel positionSearchLabel =null;

	public JTextPane field = new JTextPane();

	private StyledDocument doc;

	private JScrollPane pane = new JScrollPane(field);

	public String text = "";

	private String newText = "";

	private HashMap<String, PosInfo> rememberColors = new HashMap<String, PosInfo>();

	public boolean screenshot = false;

	public boolean syntax = false;

	private Toolkit tk = Toolkit.getDefaultToolkit();

	private Element paragraph;

	private void runViewer(boolean show, int width, int height, String mytext, String filename) {
		field.setText(mytext);
		adaptString();
		try {
			Dimension size = field.getPreferredSize();
			height = size.height + 50;
		} catch (Exception e) {
			System.err.println(">> PROBLEMS WITH getPreferredSize.");
			filesFailed++;
			return;
			// System.exit(0);
		}
		Toolkit t = Toolkit.getDefaultToolkit();
		Dimension screen = t.getScreenSize();
		if (height > (screen.height-50)) {
			height = screen.height - 75;
		}
		textFrame.setSize(width, height);
		openTextViewer(show, screenshot, filename);
	}

	public void loadFile(String name, int width, int height) {
		if (name.equals(""))
			return;
		FileHandler fhandler = new FileHandler(this);
		Colors col = new Colors(this);

		textFrame.setTitle("tview: " + name);
		Container content = textFrame.getContentPane();
        JPanel boxPanel = new JPanel(new GridBagLayout());
        content.add(boxPanel);
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.fill = GridBagConstraints.BOTH;

        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.weightx = 1000;
        gbc.weighty = 1000;
        gbc.gridwidth=5;

        // System.err.println("Detected the node: " + path);
        boxPanel.add(pane,gbc);
        gbc.gridx = 0;
        gbc.weightx = 1;
        gbc.weighty = 1;
        gbc.gridwidth=1;
        gbc.gridy = 1;
        boxPanel.add(positionSearchLabel,gbc);
        gbc.weightx = 10;
        gbc.gridx = 1;
        gbc.gridwidth=2;
        boxPanel.add(positionSearch,gbc);
        gbc.weightx = 10;
        gbc.gridx =3;
        gbc.gridwidth=2;
        boxPanel.add(positionInfo,gbc);
		textFrame.setSize(tk.getScreenSize().width / 4, (tk.getScreenSize().height / 2) - 24);
		field.setBackground(Color.WHITE);
		doc = field.getStyledDocument();
		col.prepare();
		// Get paragraph element
		paragraph = field.getDocument().getDefaultRootElement();

		boolean isDir = fhandler.readFile(name);
		if (!isDir) {
			fhandler.readHFile(name);
			runViewer(true, width, height, text, name);
		} else {
			int filesHandled = 0;
			for (int i = 0; i < fhandler.textDir.length; i++) {
				if (fhandler.isUnwantedFile(fhandler.testFiles[i].toString())) {
					// System.out.println("- Skipping
					// "+fhandler.testFiles[i].toString()+".");
					continue;
				}
				if (fhandler.pngFileExists(fhandler.testFiles[i])) {
					// System.out.println("- Skipping
					// "+fhandler.testFiles[i].toString()+". PNG File already
					// exists.");
					continue;
				}
				filesHandled++;
				fhandler.readHFile(fhandler.testFiles[i].toString());
				textFrame.setTitle("tview: " + fhandler.testFiles[i]);
				System.out.println("Checking file (" + filesHandled + "/" + fhandler.totalnroffiles + "): "
						+ fhandler.testFiles[i]);
				text = fhandler.textDir[i];
				runViewer(false, width, height, fhandler.textDir[i], fhandler.testFiles[i].toString());
			}
			System.out.println("Total # of files handled: " + filesHandled);
			System.out.println("Total # of files failed: " + filesFailed + ".");
		}
	}
	Highlight hl = new Highlight();
	private static Color lightgrey = new Color(0.82f, 0.82f, 0.82f);

	private Highlighter.HighlightPainter myHighlightPainterLightGrey =        new MyHighlightPainter(Color.LIGHT_GRAY);
	private Highlighter.HighlightPainter myHighlightPainterGrey =        new MyHighlightPainter(lightgrey);
	private Highlighter.HighlightPainter myHighlightPainterDarkGrey =        new MyHighlightPainter(Color.DARK_GRAY);
	private Highlighter.HighlightPainter myHighlightPainterYellow =        new MyHighlightPainter(Color.yellow);
	private Highlighter.HighlightPainter myHighlightPainterRed=        new MyHighlightPainter(Color.red);

	public void openTextViewer(boolean show, boolean screenshot, String filename) {
		// Get number of content elements
		highlight();
		int contentCount = paragraph.getElementCount();
		try {
			textFrame.setVisible(true);
		} catch (Exception ee) {
			System.err.println("Not able to show Frame!");
			filesFailed++;
		}

		// Get index ranges for each content element.
		// Each content element represents one line.
		// Each line includes the terminating newline.
		for (int i = 0; i < contentCount; i++) {
			// one paragraph seems to be one line of code
			Element e = paragraph.getElement(i);
			int rangeStart = e.getStartOffset();
			int rangeEnd = e.getEndOffset();
			try {
				String line = field.getText(rangeStart, rangeEnd - rangeStart);
				// System.err.println("**** linenr: " +line);
//				doc.setCharacterAttributes(rangeStart, line.length(), field.getStyle("11pts"), true);

				if (syntax) {
					colorCodeSyntax(line, rangeStart);
					colorCodeFromTo(commentPatterns);
				}
				colorCodeFromTo(highlightPatterns);
				// colorcode exceptions
				for (int pos = 0; pos < line.length(); pos++) {
					for (int j = 0; j < Colors.colorCodes.length; j++) {
						int absolutPos = rangeStart + pos;
						String namePos = Colors.colorCodes[j] + ":" + absolutPos;
						// System.err.println("-- curpos: " +absolutPos);
						PosInfo posinfo = (PosInfo) rememberColors.get(namePos);
						// System.err.println("checking "+namePos);
						if (posinfo == null)
							continue;
						int s = posinfo.getStartPos();
						int end = posinfo.getEndPos();
						if (debug) {
							System.err.println("******* getting: " + namePos + " with name:" + posinfo.getColorType()
									+ " start:" + s + " end:" + end + " length: " + posinfo.getLength());
						}
						String colorCode = Colors.getColor(posinfo.getColorType());
						int length = end-s;
						doc.setCharacterAttributes(s, length, field.getStyle(colorCode), true);

					}
				}

			} catch (BadLocationException ex) {
			}

		}

		if (show) {
			final String f = filename;
			KeyListener listener = new KeyListener() {
				public void keyPressed(KeyEvent e) {
					int code = e.getKeyCode();
					if (KeyEvent.getKeyText(code) == "F10") {
						try {
							ScreenShot.createScreenShot(0, f, textFrame);
						} catch (Exception e1) {
							e1.printStackTrace();
						}
					}
					if (code == KeyEvent.VK_F && e.isControlDown()) {
				    	positionSearch.requestFocus();
						updateCaretPosition();
					}
				}

				public void keyReleased(KeyEvent e) {
					int carpos = field.getCaretPosition();
					getRelativePos(carpos);
				}

				public void keyTyped(KeyEvent e) {
				}
			};
			MouseListener mouselistener = new MouseListener() {
				public void mousePressed(MouseEvent e) {
				}

				public void mouseClicked(MouseEvent e) {
					int carpos = field.getCaretPosition();
					getRelativePos(carpos);
				}
				public void mouseExited(MouseEvent e) {
				}
				public void mouseEntered(MouseEvent e) {
				}
				public void mouseReleased(MouseEvent e) {
				}
			};
			field.addMouseListener(mouselistener);
			field.addKeyListener(listener);
			positionSearch.addKeyListener(listener);
//			textFrame.setVisible(true);
		}

		if (screenshot) {
			try {
				ScreenShot.createScreenShot(waittime, filename, textFrame);
				textFrame.setVisible(false);
			} catch (Exception e1) {
				System.err.println("Not able to create screenshot!");
				filesFailed++;
				// e1.printStackTrace();
			}
		}
	}

	private int getRelativePos(int carpos) {
		char[] textField = field.getText().toCharArray();
		int line =1;
		int col=1;
		for (int i=0; i<carpos; i++) {
			col++;
			if (textField[i]=='\n') {
				line++;
				col=1;
			}
		}
		positionInfo.setText("absPos:"+carpos+"  line:"+line+"  column:"+col);
		return col;
	}
	
	

	private void print(HashMap rememberColors) {
		Iterator it = rememberColors.keySet().iterator();
		while (it.hasNext()) {
			String name = (String) it.next();
			System.err.println("found color: " + name);
		}
	}

	Stack<String> stackFoundName = new Stack<String>();

	Stack<Integer> stackFoundPos = new Stack<Integer>();

	private void adaptString() {
		newText = "";
		// Get number of content elements
		int contentCount = paragraph.getElementCount();

		rememberColors.clear();
		removePatterns.clear();
		commentPatterns.clear();
		stackFoundName.clear();
		stackFoundName.clear();

		// Get index ranges for each content element.
		// Each content element represents one line.
		// Each line includes the terminating newline.
		String rem_Name = "";
		int rem_StartPos = 0;
		boolean justOnePatternOnMode = false;
		int depth = 0;
		debugInfo = "";
		String lastLine = "";
		int lastPos = 0;
		int removeLength = 0;
		int endPos = 0;
		boolean foundAtLeastOne = false;
		boolean rerun = true;
		while (rerun) {
			boolean foundOff = false;
			for (int i = 0; i < contentCount; i++) {
				// one paragraph seems to be one line of code
				Element e = paragraph.getElement(i);
				int rangeStart = e.getStartOffset();
				int rangeEnd = e.getEndOffset();
				try {
					String line = field.getText(rangeStart, rangeEnd - rangeStart);
				
					doc.setCharacterAttributes(rangeStart, line.length(), field.getStyle("12pts"), true);
					// for (int j=0; j<Colors.colorCodes.length;j++) {
					for (int pos = 0; pos < line.length(); pos++) {
						int absolutPos = rangeStart + pos;
						for (int j = 0; j < Colors.colorCodes.length; j++) {
							// if a pattern exits
							if (line.startsWith(Colors.colorCodes[j], pos)) {
								String namePos = Colors.colorCodes[j] + ":" + absolutPos;
								if (namePos.contains(":on")) {
									if (justOnePatternOnMode == false) {
										rem_Name = Colors.colorCodes[j];
										rem_StartPos = absolutPos;
										justOnePatternOnMode = true;
										removeLength = Colors.colorCodes[j].length();
										debugInfo += "\n---------- foundOn : " + namePos + " rem_StartPos:"
												+ rem_StartPos + "\n";
										if (debug)
											System.err.print("\n---------- foundOn : " + namePos + " rem_StartPos:"
													+ rem_StartPos + "\n");
										removePattern(rem_StartPos, rem_StartPos + Colors.colorCodes[j].length(),
												namePos);
									} else {
										depth++;
										int startPos = absolutPos - removeLength;
										if (debug) {
											System.err.print("Inside On: " + depth + "  ");
											System.err
													.println("Found on: " + Colors.colorCodes[j] + " pos:" + startPos);
										}
										stackFoundName.push(Colors.colorCodes[j]);
										stackFoundPos.push(startPos);

										namePos = Colors.colorCodes[j] + ":" + startPos;
										removePattern(startPos, startPos + Colors.colorCodes[j].length(), namePos);
										removeLength += Colors.colorCodes[j].length();
										debugInfo += "*********** found ON offest ..: " + namePos + " : "
												+ Colors.colorCodes[j].length() + " depth:" + depth + "\n";
									}
								}
								if (namePos.contains(":off")) {
									String tmpName = rem_Name.substring(0, rem_Name.length() - 5);
									if (namePos.contains(tmpName) && justOnePatternOnMode && depth == 0) {
										justOnePatternOnMode = false;
										endPos = absolutPos - removeLength;
										String name = rem_Name + ":" + rem_StartPos;
										removePattern(endPos, endPos + Colors.colorCodes[j].length(), name);
										debugInfo += "*** foundOFF : " + namePos + " s:" + rem_StartPos + " e:"
												+ endPos + "\n";
										PosInfo posInfo = new PosInfo(rem_Name, rem_StartPos, endPos, 0, 0,
												(absolutPos - rem_StartPos));
										rememberColors.put(name, posInfo);
										removeLength = 0;
										if (debug)
											System.err.print("*** foundOFF : " + rem_Name + " s:" + rem_StartPos
													+ " e:" + endPos + "\n");
										foundOff = true;
										break;
									} else {
										depth--;
										int startPos = absolutPos - removeLength;
										namePos = Colors.colorCodes[j] + ":" + startPos;
										if (debug)
											System.err.println("Found off: " + Colors.colorCodes[j] + " pos:"
													+ startPos);

										String colorCodeC = stackFoundName.pop();
										int startPositionIB = stackFoundPos.pop();
										String colorCodeName = colorCodeC + ":" + startPositionIB;
										PosInfo posInfo = new PosInfo(colorCodeC, startPositionIB, startPos, 0, 0,
												startPos - startPositionIB);
										rememberColors.put(colorCodeName, posInfo);

										if (debug)
											System.err.print("Inside Off: " + depth + "  ");
										removePattern(startPos, startPos + Colors.colorCodes[j].length(), namePos);
										removeLength += Colors.colorCodes[j].length();
										debugInfo += "*********** found OFF offest ..: " + namePos + " : "
												+ Colors.colorCodes[j].length() + " depth:" + depth + "\n";
									}
								}
							}
						} // for colors
						if (foundOff)
							break;
					} // for pos
					if (foundOff)
						break;
				} catch (BadLocationException ex) {
				}
				if (foundOff)
					break;
			} // for line
			if (foundOff) {
				newText = getNewText(contentCount);
				// System.err.println("*************************************");
				// System.err.println(newText);
				rerun = true;
				field.setText(newText);
				foundAtLeastOne = true;
			} else
				rerun = false;
		} // rerun
		if (depth != 0 || justOnePatternOnMode) {
			System.err.println("Error: Color-Tags are not matching!");
			System.err.println("(" + (lastPos + 1) + ")" + " >> " + lastLine);
			System.err.println(debugInfo);
			System.exit(0);
		}
		if (!foundAtLeastOne)
			newText = text;
		if (syntax)
			checkComments(contentCount);
	}

	private void checkComments(int contentCount) {
		int commentStart = 0;
		for (int i = 0; i < contentCount; i++) {
			// one paragraph seems to be one line of code
			Element e = paragraph.getElement(i);
			int rangeStart = e.getStartOffset();
			int rangeEnd = e.getEndOffset();
			try {
				String line = field.getText(rangeStart, rangeEnd - rangeStart);
				doc.setCharacterAttributes(rangeStart, line.length(), field.getStyle("12pts"), true);
				// for (int j=0; j<Colors.colorCodes.length;j++) {
				for (int pos = 0; pos < line.length(); pos++) {
					int absolutPos = rangeStart + pos;
					if (line.startsWith("/*", pos)) {
						// System.err.println("found open: "+absolutPos);
						commentStart = absolutPos;
					}
					if (line.startsWith("*/", pos)) {
						// System.err.println("found close: "+absolutPos);
						addComment(commentStart, absolutPos + 2);
					}
				}
			} catch (BadLocationException ex) {
			}
		}
	}

	private String getNewText(int contentCount) {
		String newText = "";
		int startPos = 0;
		for (int i = 0; i < contentCount; i++) {
			// one paragraph seems to be one line of code
			Element e = paragraph.getElement(i);
			int rangeStart = e.getStartOffset();
			int rangeEnd = e.getEndOffset();
			try {
				int lengthDeleted = 0;
				String line = field.getText(rangeStart, rangeEnd - rangeStart);
				// System.err.println(" *************** "+line);
				for (int pos = 0; pos < line.length(); pos++) {
					int absolutPos = startPos + pos;
					Iterator it = removePatterns.iterator();
					while (it.hasNext()) {
						String delLine = (String) it.next();
						int poscolon = delLine.indexOf(":");
						int start = Integer.parseInt(delLine.substring(0, poscolon));
						// System.err.println(" ----"+absolutPos+"/"+start+"--
						// found pattern: "+delLine+" rangeStart:"+rangeStart+"
						// pos:"+pos);
						if (absolutPos == start) {
							int end = Integer.parseInt(delLine.substring(poscolon + 1, delLine.length()));
							int length = end - start;
							int newStart = pos;// -lengthDeleted;
							lengthDeleted += length;
							if (debug)
								System.err.print("removing pattern (" + start + "/" + end + "): 0-:" + newStart + " "
										+ (newStart + length) + "-" + (line.length()) + "  line: " + line);
							line = line.substring(0, newStart) + line.substring(newStart + length, line.length());
							// System.err.print(" ** removing pattern:
							// length:"+length+" line: "+line);
						}
					}
				}
				startPos += line.length();
				newText += line;
			} catch (BadLocationException ex) {
			}
		}
		removePatterns.clear();
		return newText;
	}

	List<String> removePatterns = new Vector<String>();

	List<String> commentPatterns = new Vector<String>();

	List<String> highlightPatterns = new Vector<String>();

	private void removePattern(int s, int e, String namePos) {
		String textPat = s + ":" + e;
		if (debug)
			System.err.println("add pattern: " + textPat + "  " + namePos);
		removePatterns.add(textPat);
	}

	private void addComment(int s, int e) {
		String textPat = s + ":" + e;
		if (debug)
			System.err.println("  add comment: " + textPat);
		commentPatterns.add(textPat);
		colorPatterns.put(textPat, "dgr");
	}

	HashMap<String, String> colorPatterns = new HashMap<String, String>();

	private void colorCodeFromTo(List<String> patterns) {
		Iterator it = patterns.iterator();
		while (it.hasNext()) {
			String delLine = (String) it.next();
			int poscolon = delLine.indexOf(":");
			int start = Integer.parseInt(delLine.substring(0, poscolon));
			int end = Integer.parseInt(delLine.substring(poscolon + 1, delLine.length()));
			// if (debug)
//			System.err.println(" ---- colorCoding -- : start:"+start+" end:"+end+" line: "+delLine);
			int length = end - start;
			String col = colorPatterns.get(delLine);
//			String colorCode = Colors.getColor(col);
			doc.setCharacterAttributes(start, length, field.getStyle(col), true);
		}
	}

	private void colorCodeSyntax(String line, int rangeStart) {
		String[] find = { "#include", "#endif", "#ifndef", "#define", "#if", "#else", "#ifdef", "#undef" };
		String[] find7 = { "@return", "@param", "@see", "$Revision" };
		String[] find2 = { "int", "float", "double", "char", "string", "for", "namespace", "std", "void", "{", "}",
				"while", "if", "switch", "class", "public:", "private:", "return", "NULL", "case", "break" };
		String find4 = "//";
		for (int f1 = 0; f1 < find.length; f1++) {
			if (line.contains(find[f1])) {
				int pos = line.indexOf(find[f1]);
				doc.setCharacterAttributes(rangeStart + pos, find[f1].length(), field.getStyle("LRed"), true);
			}
		}
		for (int f2 = 0; f2 < find2.length; f2++) {
			int pos = 0;

			// String tmp = line;
			while (line.indexOf(find2[f2], pos) != -1) {
				pos = line.indexOf(find2[f2], pos);
				// tmp = line.substring(pos);
				if (pos == 0)
					doc.setCharacterAttributes(rangeStart + pos, find2[f2].length(), field.getStyle("LightBlue"), true);
				else {
					String tokenBefore = line.substring(pos - 1, pos);
					String tokenAfter = " ";
					if ((pos + find2[f2].length() + 1) < line.length())
						tokenAfter = line.substring(pos + find2[f2].length(), pos + find2[f2].length() + 1);

					// System.err.println("before:"+tokenBefore+"
					// after: "+tokenAfter+" line:"+line);
					if (tokenBefore.equals("(") || tokenBefore.equals(";") || tokenBefore.equals(" ")
							|| tokenBefore.equals("\t") || tokenBefore.equals("{"))
						if (tokenAfter.equals(" ") || tokenAfter.equals("\n") || tokenAfter.equals("\t")
								|| tokenAfter.equals(",") || tokenAfter.equals("(") || tokenAfter.equals(";")
								|| tokenAfter.equals("*"))
							doc.setCharacterAttributes(rangeStart + pos, find2[f2].length(), field
									.getStyle("LightBlue"), true);
				}
				pos++;
			}
		}
		if (line.trim().contains(find4)) {
			int pos = line.indexOf(find4);
			doc.setCharacterAttributes(rangeStart + pos, line.length(), field.getStyle("Green"), true);
		}
		for (int f7 = 0; f7 < find7.length; f7++) {
			if (line.contains(find7[f7])) {
				int pos = line.indexOf(find7[f7]);
				doc.setCharacterAttributes(rangeStart + pos, find7[f7].length(), field.getStyle("LRed"), true);
			}
		}
	}


    
}
