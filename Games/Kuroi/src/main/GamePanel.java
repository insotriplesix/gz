package main;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

import states.GameStateManager;
import other.Keys;

@SuppressWarnings("serial")

/** This subclass of the JPanel that handles the game routine. **/
public class GamePanel extends JPanel implements Runnable, KeyListener {
	
	/* Panel dimensions */
	public static final int WIDTH = 320;
	public static final int HEIGHT = 240;
	public static final int SCALE = 4;
	
	/* Flags */
	private boolean isRunning;
	//public boolean isGameOver;
	
	/* Graphics */
	private BufferedImage image;
	private Graphics2D graphics;
	
	/* Other */
	private final int FPS = 60;
	private GameStateManager gsm;
	private Thread thread;
	
	/* Constructor */
	public GamePanel() {
		super();
		
		// Set the size of the panel
		setPreferredSize(new Dimension(WIDTH * SCALE, HEIGHT * SCALE));
		
		// Now it can handle key events
		setFocusable(true);
		requestFocus();
	}
	
	/* Peer interaction */
	public void addNotify() {
		// Create the peer
		super.addNotify();
		
		// Initialize and start the thread
		if (thread == null) {
			thread = new Thread(this);
			addKeyListener(this);
			thread.start();
		}
	}
	
	/* Initialize stuff */
	private void init() {
		isRunning = true;
		
		image = new BufferedImage(WIDTH, HEIGHT, BufferedImage.TYPE_INT_RGB);
		graphics = (Graphics2D) image.getGraphics();
		
		gsm = new GameStateManager();
	}
	
	/* Update stuff */
	private void update() {
		gsm.update();
		Keys.update();
	}
	
	/* Render the game elements and place them to the image buffer */
	private void render() {
		gsm.draw(graphics);
	}
	
	/* Draw stuff upon the buffer and paint the screen */
	private void paint() {
		Graphics g;	
		try {
			g = getGraphics();
			g.drawImage(image, 0, 0, WIDTH * SCALE, HEIGHT * SCALE, null);
			g.dispose();
		} catch (Exception ex) {
			ex.printStackTrace();
		}	
	}
	
	/* thread.start() run this code */
	public void run() {
		init();
		
		long startTime, elapsedTime, sleepTime;
		
		// Game routine
		while (isRunning) {
			startTime = System.nanoTime();
			
			// Make one frame
			update();
			render();
			paint();

			elapsedTime = System.nanoTime() - startTime;
			
			// Desired iteration time - real iteration time
			sleepTime = 1000L / FPS - elapsedTime / 1000000L;
			
			// If we had negative value let the thread sleep anyway
			if (sleepTime <= 0) sleepTime = 5;
			
			try {
				Thread.sleep(sleepTime);
			} catch (Exception ex) {
				ex.printStackTrace();
			}	
		}	
	}
	
	/* KeyListener stuff */
	public void keyTyped(KeyEvent key) {}
	public void keyPressed(KeyEvent key) { Keys.setKey(key.getKeyCode(), true); }
	public void keyReleased(KeyEvent key) { Keys.setKey(key.getKeyCode(), false); }
	
}
