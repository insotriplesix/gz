package other;

import java.awt.event.KeyEvent;

/** This class handles all of keys that available in the game. **/
public class Keys {
	
	/* Max game keys */
	public static final int NKEYS = 20;
	
	/* Key storage */
	public static boolean keyState[] = new boolean[NKEYS];
	public static boolean prevKeyState[] = new boolean[NKEYS];
	
	/* Keys constants */
	public static int UP 	= 0;	// up, jump, doublejump
	public static int DOWN 	= 1;	// down
	public static int LEFT 	= 2;	// left
	public static int RIGHT = 3;	// right
	public static int SPACE = 4;	// charge
	public static int SHIFT = 5;	// dash
	public static int ENTER = 6;	// action
	public static int ESC	= 7;	// pause
	
	/* Set the current state of a key (pressed/released) */
	public static void setKey(int k, boolean b) {
		switch (k) {
		case KeyEvent.VK_UP:
			keyState[UP] = b;
			break;
		case KeyEvent.VK_DOWN:
			keyState[DOWN] = b;
			break;
		case KeyEvent.VK_LEFT:
			keyState[LEFT] = b;
			break;
		case KeyEvent.VK_RIGHT:
			keyState[RIGHT] = b;
			break;
		case KeyEvent.VK_SPACE:
			keyState[SPACE] = b;
			break;
		case KeyEvent.VK_SHIFT:
			keyState[SHIFT] = b;
			break;
		case KeyEvent.VK_ENTER:
			keyState[ENTER] = b;
			break;
		case KeyEvent.VK_ESCAPE:
			keyState[ESC] = b;
			break;
		}
	}
	
	/* Update keys */
	public static void update() {
		for (int i = 0; i < NKEYS; i++) {
			prevKeyState[i] = keyState[i];
		}
	}
	
	/* Check if 'k' key pressed */
	public static boolean isPressed(int k) {
		return keyState[k] && !prevKeyState[k];
	}

}
