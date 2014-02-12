/**
@file Physics_old.h
*/

#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glut.h>
#include <vector>
#include "ter.h"


/**
Struct to define an object which can interact with the world.
*/
typedef struct{
public:
	glm::vec3 pos;
	int radius;						
	bool state;
}objects;

/**
This struct defines an object which can interact with 
other object and follows rigid body dynamics.
*/

typedef struct  {
public:
	glm::vec3 velocity;
	glm::vec3 projectileVelocity ;		// Initiated and maintained by physics Engine
	glm::vec3 ACCELERATION;				// Not required as input
	glm::vec3 bikeVector;

	glm::vec3 rearTire;
	float phi;						// Variables to store the position of the bike. 
	float theta;					// phi is elevation angle from ground

	float length;
	float mass;
	float invMass;
	float muFrictionStatic;
	float muFrictionDynamic;
	float DRAG_COEFFICIENT;
	float engineForce ;
	float speed;
	float turnRate;					// In radians per second
	bool rearTireOnGround;			
	bool frontTireOnGround;

	float timeFrame;							// Time after which updateLoop is called
	float FRICTION_VELOCITY_CONSTANT;			// For modelling rolling friction
	
	Terrain* currentTerrain;						//	Takes input of class Terrain
	objects bonusMark[15];
	objects obstacles[15];
	objects fixedObjects[15];

	float terrainScaleFactor;


} obj;

class Physics_old{
public:
	Physics_old();
	void updateLoop( obj* o);
};