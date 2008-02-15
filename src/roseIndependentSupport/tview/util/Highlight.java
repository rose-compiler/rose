package util;

import java.awt.Color;

import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultHighlighter;
import javax.swing.text.Document;
import javax.swing.text.Highlighter;
import javax.swing.text.JTextComponent;

public class Highlight {



	public void clear(JTextComponent textComp) {
        removeHighlights(textComp);
	}
	   // Creates highlights around all occurrences of pattern in textComp
    public void highlight(JTextComponent textComp, String pattern, Highlighter.HighlightPainter  painter) {
        // First remove all old highlights
        // An instance of the private subclass of the default highlight painter
        try {
            Highlighter hilite = textComp.getHighlighter();
            Document doc = textComp.getDocument();
            String text = doc.getText(0, doc.getLength());
            int pos = 0;
    
            // Search for pattern
            while ((pos = text.indexOf(pattern, pos)) >= 0) {
                // Create highlighter using private painter and apply around pattern
                hilite.addHighlight(pos, pos+pattern.length(), painter);
                pos += pattern.length();
            }
        } catch (BadLocationException e) {
        }
    }
    
	   // Creates highlights around all occurrences of pattern in textComp
    public void highlight(JTextComponent textComp, int pos, int length, Highlighter.HighlightPainter  painter) {
        // First remove all old highlights
        // An instance of the private subclass of the default highlight painter
        try {
            Highlighter hilite = textComp.getHighlighter();
            hilite.addHighlight(pos,pos+ length, painter);
        } catch (BadLocationException e) {
        }
    }
    
    // Removes only our private highlights
    public void removeHighlights(JTextComponent textComp) {
        Highlighter hilite = textComp.getHighlighter();
        Highlighter.Highlight[] hilites = hilite.getHighlights();
    
        for (int i=0; i<hilites.length; i++) {
            if (hilites[i].getPainter() instanceof MyHighlightPainter) {
                hilite.removeHighlight(hilites[i]);
            }
        }
    }
    
    

}
