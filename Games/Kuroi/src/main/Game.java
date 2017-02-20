package main;

import javax.swing.JFrame;

/** Here where we get started. **/
public class Game {
	
	public static void main(String[] args) {
		// Create new game window 
		JFrame frame = new JFrame("~ KUROI ~");
		frame.add(new GamePanel());
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Autosize the window
		frame.pack();
		
		// Put the window in the centerpoint of the screen
		frame.setLocationRelativeTo(null);
		
		// Other
		frame.setResizable(false);
		frame.setVisible(true);
	}
	
}
