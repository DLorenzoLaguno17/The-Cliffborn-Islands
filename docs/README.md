# The Cliffborn Islands

The Cliffborn Islands is a platformer game developed by two students of the Videogame Design and
Development degree at the UPC, a university of Barcelona. You can go to our GitHub by clicking [here](https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands).

## Controls

* A - Move left.
* D - Move right.
* O - Throw the hook.
* P - Attack with the sword.
* Spacebar - Jump.

<iframe width="850" height="480" src="https://www.youtube.com/embed/_q9eXDu2KOA" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

Download the latest version [here](https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands/releases/tag/v0.4)!

## Main Core Subsystems

* Map: maps are loaded from Tiled reading an XML file, which loads all the properties and also places all the colliders, enemies and coins in each level.
* Pathfinding: we use the A* algorithm so the enemies can go towards the player with the most efficient path.
* Entity subsystem manages the player, the hook, both type of enemies (harpy and skeleton) and the coins.
* UI subsystem is organized as a tree that has three different elements: Buttons, Labels and Boxes and all of them inherit from a general class, called UserInterfaceElement. Buttons trigger actions, labels are only to show text and boxes are elements than can be dragged through the screen.

## Innovation

We have implemented a hook which you need to use to overcome the level. If it collides with a wall, it moves the player towards it.

## Authors of the game

![Òscar Faura https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands/blob/master/docs/oscar.jpg?raw=true](https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands/blob/master/docs/oscar.jpg?raw=true)

Òscar Faura: 
* Creation of level 1.
* Load entities from Tiled.
* Creation and implementation of the entity system.
* Implementation of the pathfinding and enemies' movement.
* Implementation of the in-game HUD.
* Programmed coins, lives, score and timer.
* Save & Load implementation.

![Dani Lorenzo https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands/blob/master/docs/dani.JPG?raw=true](https://github.com/DLorenzoLaguno17/The-Cliffborn-Islands/blob/master/docs/dani.JPG?raw=true)

Daniel Lorenzo: 
* Creation of level 2.
* Controls and mechanics of the player and the hook.
* FPS and normalization of the movement using delta time.
* Creation and implementation of the user interface system.
* Creation of the logotype.

## Credits
### Art

Sprites and tilesets of character and maps: "Foggy Cliffs - Fantasy Tileset"
[Nauris Amatnieks](https://twitter.com/Namatnieks).

### Music
Music of the menu scene: "Getting it Done"
[Kevin MacLeod](https://incompetech.com/).
Licensed under [Creative Commons](http://creativecommons.org/licenses/by/3.0/): By Attribution 3.0.

Music of the menu scene: "River Fire"
[Kevin MacLeod](https://incompetech.com/).
Licensed under [Creative Commons](http://creativecommons.org/licenses/by/3.0/): By Attribution 3.0.

Music of the first and second level: "Airship Serenity"
[Kevin MacLeod](https://incompetech.com/).
Licensed under [Creative Commons](http://creativecommons.org/licenses/by/3.0/): By Attribution 3.0.

### Sound effects
* Death sound: Minecraft.
* Death music: Uncharted 2.
* Player attack and jump: Link from Super Smash Bros.

This game has been created by Òscar Faura and Daniel Lorenzo.
Licensed under the [MIT License](https://goo.gl/c19PjY).
