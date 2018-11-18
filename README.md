# The Cliffborn Islands

Go to our GitHub: https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands 

## Controls

### Gameplay
* A - Move left
* D - Move right
* H - Throw the hook
* O - Attack with the sword
* Spacebar - Jump.

### Special keys
* F1 - To start from the very first level
* F2 - To start from the beginning of the current level
* F3 - To start from the beggining of the second level
* F5 - Save the current state
* F6 - Load the previous state
* F9 - To view colliders and logic of the game
* F10 - God mode
* F11 - Enable / Disable the FPS cap to 30

## Innovation

We have implemented a hook which you need to use to overcome the level. If it collides with a wall, it moves the player towards it.

Furthermore, we have added the possibility of attacking and killing the enemies.

## Special mechanics

You can push yourself into the air once for each time you leave the ground. That means double jump when you leave the gound with a hop or one jump if you are already in a free fall.

The hook will only work if it collides perpendicularly with a wall, and it cannot be used in GodMode.

## Credits

### Art
	Sprites and tilesets of the character and the maps: "Foggy Cliffs - Fantasy Tileset"
	Nauris Amatnieks (https://twitter.com/Namatnieks)

### Music
	Music of the first and second level: "Airship Serenity"
	Kevin MacLeod (incompetech.com)
	Licensed under Creative Commons: By Attribution 3.0
	http://creativecommons.org/licenses/by/3.0/

### Sound effects
	Death sound: Minecraft.		
	Death music: Uncharted 2.
	Player attack and jump: Link from Super Smash Bros.

## Task List
	- Òscar Faura: 
		* Implemented pathfinding and enemiess movement
		* Create and implement the enemies
		* Created and Implemented entity system
		* Implemented Save & Load
	- Dani Lorenzo: 
		* Capped FPS and normalizd movement using dt
		* Implemented the hook
		* Created player death and hook animations
		* Implemented the attack of the player
		* Created the UML

This game has been created by Òscar Faura and Daniel Lorenzo.
Licensed under the [MIT License](LICENSE).