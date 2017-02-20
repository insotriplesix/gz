package states;

import java.awt.Color;
import java.awt.Graphics2D;

import audio.AudioPlayer;
import main.GamePanel;

/** Object of this class is destined for operate the game states. **/
public class GameStateManager {
	
	/* Game states storage */
	private GameState[] gameStates;
	
	/* Current state of the game */
	private int currentState;
	
	/* States constants */
	public static final int NSTATES = 30;	// Number of the game states
	
	public static final int MENU 	= 0;	// Menu state
	public static final int PAUSE 	= 1;	// Pause state
	
	public static final int CUT1_1	= 2;	// Cutscene 1-1
	public static final int LVL1_1 	= 3;	// Level 1-1
	public static final int CUT1_2 	= 4;	// Cutscene 1-2
	public static final int LVL1_2 	= 5;	// Level 1-2
	/* . . . */

	/* Constructor */
	public GameStateManager() {
		// Initialize the audio player
		AudioPlayer.init();
		
		gameStates = new GameState[NSTATES];
		currentState = MENU;
		
		// Load the first state of the game
		load(currentState);	
	}
	
	/* Load the state */
	private void load(int s) {
		switch (s) {
		case MENU:
			gameStates[s] = new Menu(this);
			break;
		case CUT1_1:
			gameStates[s] = new CutScene1_1(this);
			break;
		case LVL1_1:
			gameStates[s] = new Level1_1(this);
			break;
		}
	}
	
	/* Unload the state */
	private void unload(int s) {
		gameStates[s] = null;
	}
	
	/* Set the current state of the game */
	public void set(int s) {
		unload(currentState);
		currentState = s;
		load(currentState);
	}
	
	/* Update the current state */
	public void update() {
		if (gameStates[currentState] != null) {
			gameStates[currentState].update();
		}
	}
	
	/* Render graphics of the current state */
	public void render(Graphics2D g) {
		if (gameStates[currentState] != null) {
			gameStates[currentState].render(g);
		} else {
			// If state is unavailable 
			g.setColor(Color.BLACK);
			g.fillRect(0, 0, GamePanel.WIDTH, GamePanel.HEIGHT);
		}
	}
	
}
