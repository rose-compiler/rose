package util;

import java.awt.Color;

import javax.swing.text.DefaultHighlighter;

// A private subclass of the default highlight painter
class MyHighlightPainter extends DefaultHighlighter.DefaultHighlightPainter {
    public MyHighlightPainter(Color color) {
        super(color);
    }
}