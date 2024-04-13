# cpp-opengl-airship-battle
 An Airship Battle Simulator in C++ with OpenGL
 
* two zeppelins; user can control one zeppelin as per A1 functionality
* switch between FPV and normal world view (bird's eye view) with 'v' key
* enemy zeppelin moves around on its own
* when close to player -> face player, match elevation, fire missiles
* collision detection with missiles for both enemy and player 
	- when enemy gets hit, despawns and respawns randomly after short duration
	- when player gets hit, just falls to the ground
* ground mesh
* "buildings" scattered around the environment