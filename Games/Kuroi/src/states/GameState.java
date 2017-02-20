package states;

import java.awt.Graphics2D;

/** Abstract class for 'state' object. **/
public abstract class GameState {
	
	// An object that operate the current state of the game
	protected GameStateManager gsm;
	
	// Constructor
	public GameState(GameStateManager gsm) {
		this.gsm = gsm;
	}
	
	// Methods
	public abstract void init();
	public abstract void update();
	public abstract void render(Graphics2D g);
	public abstract void checkKey();
	
}
