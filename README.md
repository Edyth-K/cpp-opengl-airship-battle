# cpp-opengl-airship-battle
 An Airship Battle Simulator in C++ with OpenGL

 ![me](https://github.com/Edyth-K/cpp-opengl-airship-battle/blob/main/airshipbattle.gif)
 
* two zeppelins; user can control one zeppelin with arrow keys
* switch between first-person and top-down view with 'v' key
* enemy zeppelin moves around on its own
* when close to player -> face player, match elevation, fire missiles
* collision detection with missiles for both enemy and player 
	- when enemy gets hit, despawns and respawns randomly after short duration
	- when player gets hit, just falls to the ground
* ground mesh + "buildings" scattered around the environment

## Installation
1. Clone or Download repository
2. Run "/source/Debug/airship-battle.exe"

## Controls
 
| Key    | Action |
| -------- | ------- |
| 'W'  | Move Forward    |
| 'Left / Right Arrow' | Turn Left / Right     |
| 'Up / Down Arrow'    | Change Elevation    |
| 'Space Bar' | Shoot Missile     |
| 'V'    | Toggle First-Person / Top-Down View    |
