package util;

import java.awt.Color;
import java.awt.Container;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.HashMap;
import java.util.Iterator;

import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
	
	public class ColorDialog extends JDialog {
		private	JPanel		topPanel;
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		
		private WindowListener wl = new WindowAdapter() {
			public void windowClosing(WindowEvent se) {
				System.out.println("closing.");
				System.exit(0);
			}
		};
		
		HashMap<String, Color> colorTable = new HashMap<String, Color>();
		
		public void addColor(String name, Color col) {
			colorTable.put(name,col);
		}
		
		public ColorDialog() {
			setTitle( "ColorDialog" );
			setBackground( Color.gray );
			Container content = this.getContentPane();
			
			topPanel = new JPanel();
			content.add(topPanel);
			
		}
		
		private void createTable() {
			Iterator it = colorTable.keySet().iterator();
			while (it.hasNext() ) {
				String name = (String) it.next();
				Color col = (Color) colorTable.get(name);
				JLabel text = new JLabel(name);
				text.setForeground(col);
				topPanel.add(text);
			}
		}
		
		public void showDiag() {
			createTable();
			this.addWindowListener(wl);
			this.setSize(200,400);
			this.setVisible(true);
			long time1 = System.currentTimeMillis();
			long time2 = System.currentTimeMillis();
			long diff = (time2-time1);
			while (diff<60000) {
				time2 = System.currentTimeMillis();
				diff = (time2-time1);
			}
		}
	}