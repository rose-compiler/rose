package util;

import java.awt.Color;

import javax.swing.text.Style;
import javax.swing.text.StyleConstants;



public class Colors {
	
	Viewer view;
	
	public Colors(Viewer v) {
		view=v;
	}
	
	private static Color grey = new Color(0.5f, 0.5f, 0.5f);

	private static Color darkgrey = new Color(0.3f, 0.3f, 0.3f);

	private static Color lightblue = new Color(0.25f, 1.0f, 1.0f);

	private static Color lightred = new Color(1.0f, 0.5f, 0.5f);

	private static Color green = new Color(0.0f, 0.6f, 0.0f);

	private static Color orange = new Color(1.0f, 0.5f, 0.0f);

	private static Color violet = new Color(0.5f, 0.0f, 1.0f);

	private static Color brown = new Color(0.4f, 0.0f, 0.2f);

	private static Color purple = new Color(0.96f, 0.3f, 0.44f);

	private static Color black = new Color(0.0f, 0.0f, 0.0f);

	private static Color red = new Color(1.0f, 0.0f, 0.0f);

	private static Color yellow = new Color(1.0f, 0.75f, 0.15f);

	private static Color blue = new Color(0.0f, 0.0f, 1.0f);

	private static Color lightgreen = new Color(0.0f, 1.0f, 0.0f);

	private static Color darkblue = new Color(0.14f, 0.14f, 0.56f);

	private static Color olivegreen = new Color(0.60f, 0.60f, 0.28f);

	private static Color darkgreen = new Color(0.23f, 0.40f, 0.16f);

	public static Color[] colors = { darkgrey, grey, lightblue, lightred, lightgreen, orange, violet, brown, purple, black, red, yellow, blue, 
		green, darkblue ,olivegreen, darkgreen};

	public static Color fetchColor(String col) {
		if (col.toLowerCase().equals("grey"))
			return grey;
		if (col.toLowerCase().equals("olivegreen"))
			return olivegreen;
		if (col.toLowerCase().equals("darkgreen"))
			return darkgreen;
		if (col.toLowerCase().equals("darkgrey"))
			return darkgrey;
		if (col.toLowerCase().equals("lightblue"))
			return lightblue;
		if (col.toLowerCase().equals("lightred"))
			return lightred;
		if (col.toLowerCase().equals("green"))
			return green;
		if (col.toLowerCase().equals("orange"))
			return orange;
		if (col.toLowerCase().equals("violet"))
			return violet;
		if (col.toLowerCase().equals("brown"))
			return brown;
		if (col.toLowerCase().equals("black"))
			return black;
		if (col.toLowerCase().equals("purple"))
			return purple;
		if (col.toLowerCase().equals("red"))
			return red;
		if (col.toLowerCase().equals("yellow"))
			return yellow;
		if (col.toLowerCase().equals("blue"))
			return blue;
		if (col.toLowerCase().equals("lightgreen"))
			return lightgreen;
		if (col.toLowerCase().equals("darkblue"))
			return darkblue;
		
		return black;
	}
	

	static String redOn = "/* colorCode:red:on */";

	static String redOff = "/* colorCode:red:off */";

	static String orangeOn = "/* colorCode:orange:on */";

	static String orangeOff = "/* colorCode:orange:off */";

	static String yeallowOn = "/* colorCode:yellow:on */";

	static String yellowOff = "/* colorCode:yellow:off */";

	static String blueOn = "/* colorCode:blue:on */";

	static String blueOff = "/* colorCode:blue:off */";

	static String greenOn = "/* colorCode:green:on */";

	static String greenOff = "/* colorCode:green:off */";

	static String violetOn = "/* colorCode:violet:on */";

	static String violetOff = "/* colorCode:violet:off */";

	static String brownOn = "/* colorCode:brown:on */";

	static String brownOff = "/* colorCode:brown:off */";

	static String purpleOn = "/* colorCode:purple:on */";

	static String purpleOff = "/* colorCode:purple:off */";

	static String lightblueOn = "/* colorCode:lightblue:on */";

	static String lightblueOff = "/* colorCode:lightblue:off */";

	static String lightgreenOn = "/* colorCode:lightgreen:on */";

	static String lightgreenOff = "/* colorCode:lightgreen:off */";

	static String lightredOn = "/* colorCode:lightred:on */";

	static String lightredOff = "/* colorCode:lightred:off */";

	static String blackOn = "/* colorCode:black:on */";

	static String blackOff = "/* colorCode:black:off */";

	static String darkblueOn = "/* colorCode:darkblue:on */";

	static String darkblueOff = "/* colorCode:darkblue:off */";

	static String greyOn = "/* colorCode:grey:on */";

	static String greyOff = "/* colorCode:grey:off */";

	static String darkgreyOn = "/* colorCode:darkgrey:on */";

	static String darkgreyOff = "/* colorCode:darkgrey:off */";

	static String olivegreenOn = "/* colorCode:olivegreen:on */";

	static String olivegreenOff = "/* colorCode:olivegreen:off */";

	static String darkgreenOn = "/* colorCode:darkgreen:on */";

	static String darkgreenOff = "/* colorCode:darkgreen:off */";

	public static String[] colorCodes = { redOn, redOff, orangeOn, orangeOff, yeallowOn, yellowOff, blueOn, blueOff, greenOn,
			greenOff, violetOn, violetOff, brownOn, brownOff, purpleOn, purpleOff, lightblueOn, lightblueOff, lightgreenOn, lightgreenOff,
			lightredOn, lightredOff, blackOn, blackOff, darkblueOn, darkblueOff, greyOn, greyOff, darkgreyOn, darkgreyOff,
			olivegreenOn, olivegreenOff, darkgreenOn, darkgreenOff};

	public static String getColor(String type) {
		String colorStr = type.substring(13,type.lastIndexOf(":"));
		return colorStr.toLowerCase();
	}
	
	private void createStyle(String color) {
		Style style = view.field.addStyle(color, null);
		StyleConstants.setForeground(style, fetchColor(color));
		StyleConstants.setBold(style, true);
		StyleConstants.setFontSize(style, fontsize);
//		StyleConstants.setFontFamily(style, "SansSerif");
		StyleConstants.setFontFamily(style, "Monospaced");
	}
	
	public void prepare() {
		// Makes text red
		createStyle("grey");
		createStyle("darkgrey");
		createStyle("lightblue");
		createStyle("lightred");
		createStyle("lightgreen");
		createStyle("orange");
		createStyle("violet");
		createStyle("brown");
		createStyle("purple");
		createStyle("black");
		createStyle("red");
		createStyle("yellow");
		createStyle("blue");
		createStyle("green");
		createStyle("darkblue");
		createStyle("olivegreen");
		createStyle("darkgreen");
		


		Style style = view.field.addStyle("12pts", null);
//		StyleConstants.setBold(style, true);
		StyleConstants.setFontSize(style, fontsize);
		StyleConstants.setFontFamily(style, "Monospaced");

	}
	public static int fontsize=12;


}