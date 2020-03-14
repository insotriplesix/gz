:crown: :hocho: :video_game:

#### Version 1.5 (stable) supplied on May 11th, 2016.

**1.5 (stable) update log:**
- Multiple simultaneous games support enabled.

**1.4 (stable) update log:**
- Basically an optimized version of 1.4 beta release with a stable logic of version 1.3.
- Stable server mechanics inherited from version 1.3. We are still using server relays, but now they work fine and seamlessly reconnect even if some middle node was shut down.
- Added some informative outputs for a client and codenames for every server.
- The launcher now tries to enable your audio devices automatically.
- The code is cleaner and shinier than ever!

**1.0 to 1.3 contributions:**
- Fixed at least one critical error!
- Cleaned up the code.
- Added a new melody to the main menu.
- If there are no pending games, *join game* won't just crash everything. A server sends a *refused* byte, which causes a client to return to the main menu. Nice and clear!
- Added the AI functions (can be toggled on and off during the game by pressing 'N').
- Added some useful outputs to the game screen.
- More adequate server outputs.
- Exiting the game by pressing ESC now works! Any player can quit during the game, the server alerts their opponent and safely deletes the current game while both players return to the start screen.
- After successfully finishing the game both players also return to the start menu, from which they can start a new game or quit.
- While waiting for their turn, a player now can toggle AI, music or exit the game.
- The cursor is now strolling in cycles while choosing a card (and in the main menu too!).
- Optimized random function calls (*rand_r* instead of *rand*).
- The passive server overtakes the active role a bit faster now.

**Usage hints:**
- Use `make` to compile, then `./launch` to start the needed amount of servers and two clients.
- Edit *server.conf* to set a desired quantity of servers and clients and a starting server port number.
- If there is no sound, try `sudo modprobe pcspkr` (the current user must have an access to the *audio* group), also the package *beep* is needed. **Update:** Since v1.4 the launcher tries to do it automatically. 

**Warning:** use 'proper' server shutdown key ('q') only when you are done! Otherwise it will cause a *weird* behaviour.

**In-game controls:**
- Arrows (left and right) to navigate through cards.
- SPACE to choose a card.
- 'N' to toggle the AI mode.
- 'M' to mute or unmute the sound.
- ESC to exit the game.
