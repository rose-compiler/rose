package util;

public class PosInfo {
	
	private String color;
	int posE, posS;
	int line;
	int relativeStart;
	int lengthWord;
	
	public PosInfo(String colorType, int startpos, int endpos, int lineNr, int relStartPos, int length) {
		color=colorType;
		posE=endpos;
		posS=startpos;
		line=lineNr;
		relativeStart = relStartPos;
		lengthWord=length;
	}
	public String getColorType() {
		return color;
	}
	public int getEndPos() {
		return posE;
	}
	public int getStartPos() {
		return posS;
	}
	public int getLinePos() {
		return line;
	}
	public int getRelativeStart() {
		return relativeStart;
	}
	public int getLength() {
		return lengthWord;
	}
}