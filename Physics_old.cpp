

/**
	@file Physics_old.cpp
	This is the Physics Engine we used in the first phase of the game. We had to change it because of 
	a very simulation like feeling of this Engine.
	
	In this model, the bike's motion is broken down into two independent parts, longitudinal and lateral, where longitudinal 		means in the direction of the bike, and lateral means its perpendicular.
	Longitudinal force can be changed by frictional forces, air drag, engine force and gravity when on a slope.
	Lateral forces can be changed by centripetal forces. 
*/


#include <iostream>
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/type_ptr.hpp>
//#include <glm/transform.hpp>
#include <stdlib.h>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glut.h>
#include <vector>
#include "Physics_old.h"
#include "ter.h"


using namespace std;




Physics_old::Physics_old(){}
float PI = 3.14159;


// objects bonusMark[15];
// objects obstacles[15];
// objects fixedObjects[15];

// class objects{
// public:
// 	bonus bonusMark;
// 	bonus obstacle;
// 	bonus fixedObjects;
// };






/*
****
****
****
****
****					CODE FOR RIGID-BODY DYNAMICS OF A BIKE
****
****
****
****
****
****
****/
glm::vec3 GRAVITY = ( (float) 9.8)*(glm::vec3{0,-1,0});
glm::vec3 ZERO = glm::vec3{0,0,0};
Terrain* t;





float magnitude(glm::vec3 vector){
	return sqrtf(glm::dot(vector,vector));
}

/**
Calculates the friction force exerted by the ground on the bike.

*/

glm::vec3 forceFriction(obj *o){
	glm::vec3 tempDynamic = o->velocity*o->muFrictionDynamic*(o->mass)*o->FRICTION_VELOCITY_CONSTANT;
	return tempDynamic;

}

/**
Calculates the force due to air drag on the bike.
Modelled as proportional to magnitude(velocity)^2 of the bike.

*/

glm::vec3 forceDrag(obj *o,int i){
	if (i==0){
	return o->velocity*magnitude(o->velocity)*o->DRAG_COEFFICIENT;
	}
	else{
		return o->speed*o->projectileVelocity*o->DRAG_COEFFICIENT;
	};
}

/**
Calculates the force exerted by the engine on the bike.
(Technically, force exerted by ground on bike due to torque of engine.)

*/

glm::vec3 forceEngine(obj *o){
	o->bikeVector = glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
	return o->bikeVector*o->engineForce;
}

glm::vec3 forceGravity(obj *o){
		return o->mass*((float) 9.8)*sin(glm::dot(glm::normalize(GRAVITY),glm::normalize(o->bikeVector)))*glm::normalize(o->bikeVector);
}


float bilinearInterpolation(float x, float z, obj *o){
	float tempMaxXY = max(o->currentTerrain->width() - 1, o->currentTerrain->length() - 1);
	x = x/o->terrainScaleFactor + tempMaxXY/2.0;
	z = z/o->terrainScaleFactor + tempMaxXY/2.0;
	 float tempX1 = (ceil(x)-x)*(o->currentTerrain->getHeight(floor(x),floor(z)))+(x-floor(x))*(o->currentTerrain->getHeight(ceil(x),floor(z)));
	 float tempX2 = (ceil(x)-x)*(o->currentTerrain->getHeight(floor(x),ceil(z)))+(x-floor(x))*(o->currentTerrain->getHeight(ceil(x),ceil(z)));
	 float tempZ = (ceil(z)-z)*(tempX2) + (z-floor(z))*(tempX1);

	 return o->terrainScaleFactor*(tempZ);

}


/**

Adjust the angle the length vector of the bike makes with the 
X-Z axis such that the front tire stays in contact with the 
ground if fractional change is less, or goes in the air otherwise.
*/

void adjustPhi(obj *o){
	float maxDifference =0.5;
	int ifFrontTireUp ;
	bool difference;
	o->bikeVector = glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
	// calculates difference between front tire height and ground height at that point. 
	if ((o->rearTire + o->bikeVector*o->length)[1] - bilinearInterpolation((o->rearTire+o->bikeVector*o->length)[0],(o->rearTire+o->bikeVector*o->length)[2],o)<maxDifference){
		difference = true;
	}
	else{
		difference=false;
	}
	// calculates if front tire is below ground or above it.
	if ((o->rearTire + o->bikeVector*o->length)[1] < bilinearInterpolation((o->rearTire+o->bikeVector*o->length)[0],(o->rearTire+o->bikeVector*o->length)[2],o)){
		// tire is below ground
		ifFrontTireUp = 1 ;
	}
	else {
		ifFrontTireUp = -1;
	}
	if ((ifFrontTireUp==1) || difference){
		int temp = ifFrontTireUp;
		while (temp == ifFrontTireUp){
			o->phi = o->phi - ifFrontTireUp*0.05;
			if (o->phi>2*PI){
						o->phi -= 2*PI;
					}
			o->bikeVector = glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
			if ((o->rearTire + o->bikeVector*o->length)[1] < bilinearInterpolation((o->rearTire+o->bikeVector*o->length)[0],(o->rearTire+o->bikeVector*o->length)[2],o)){
				temp = 1 ;
			}
			else {
				temp = -1;
			}
			o->frontTireOnGround=true;
		}
	}
	else {
		o->frontTireOnGround=false;
	}
}

/**

This function is responsible for maintaining contact of bike with the 
ground if difference between bike's position and ground is less,
or setting the bike in projectile motion. 


*/

void terrainFollower(int n,obj *o){
	switch(n){
		case 1:
			if (o->rearTire[1]-bilinearInterpolation(o->rearTire[0],o->rearTire[2],o)<02.00){
				o->rearTire[1] = bilinearInterpolation(o->rearTire[0],o->rearTire[2],o);
				o->rearTireOnGround = true;
				adjustPhi(o);
			}
			else{
				o->rearTireOnGround=false;
				adjustPhi(o);
			};
			break;
		case 2:
			o->phi = o->phi +(10.0/360.0)*2*PI*o->timeFrame;
			terrainFollower(1,o);
			break;
		case 3:
			cout << glm::to_string(o->rearTire) << endl ;
			o->rearTire = o->rearTire-(o->timeFrame*GRAVITY);
			//cout << " it was me" << endl;
			terrainFollower(1,o);
			break;
		case 4:
			if (o->rearTire[1]-bilinearInterpolation(o->rearTire[0],o->rearTire[2],o)<02.0){
				o->rearTire[1] = bilinearInterpolation(o->rearTire[0],o->rearTire[2],o);
				o->rearTireOnGround = true;
			}
			adjustPhi(o);
	}
}

/**
This function checks collision bike and other objects assuming 
both the objects to be spheres encompassing the bodies 
themselves.

*/

bool checkCollision(obj *o,objects a){
	
	return (magnitude(a.pos-(o->rearTire+o->length*o->bikeVector)) < (a.radius+(o->length/2)));

}

bool collisionloop(obj *o,objects a[]){
	int i;
	for (i=0;i<15;i++){
		if (checkCollision(o,a[i])){
			a[i].state==true;
		}

	}

}
void velocitybonus(obj *o){
	if (o->velocity==ZERO){
		o->projectileVelocity += ((float) 10.0)*glm::normalize(o->projectileVelocity);
	}
	else {
		o->velocity += ((float) 10.0)*glm::normalize(o->velocity);
	}
}
void velocitypenality(obj *o){
	if (o->velocity==ZERO){
		o->projectileVelocity = o->projectileVelocity/((float)2.0);
	}
	else{
		o->velocity -= o->velocity/((float)2.0);
	}
}

/**

This loop is responsible for updating the position of the 
bike after every dt time interval. 

It calculates the netforce on the bike at every iteration 
of the loop, and displaces the bike accordingly. It then
calls the terrain follower module to adjust the bike to
stay on the ground or loose contact.

*/


void Physics_old::updateLoop(obj* o){
	o->bikeVector = glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
	if ( o->frontTireOnGround && o->rearTireOnGround){
		o->theta = o->theta + o->turnRate*o->timeFrame;
		if (o->theta>2*PI){
			o->theta -= 2*PI;
		}
		else{};

		o->velocity = o->speed*glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
		glm::vec3 netForce;
		netForce = forceEngine(o) + forceDrag(o,0) + forceGravity(o) + forceFriction(o);
		o->velocity += netForce*(o->invMass)*o->timeFrame ;
		if (magnitude(netForce)<100 && abs(o->speed)<0.20){
			o->velocity = ZERO;
		}
		else{};
		if (glm::dot(o->bikeVector,o->velocity)<0){
			o->speed = (-1.0)*magnitude(o->velocity);
			}
		else{
			o->speed = magnitude(o->velocity);
			};

		glm::vec3 displacement = o->velocity*o->timeFrame ;
		o->rearTire = o->rearTire + displacement;
		cout << glm::to_string(o->rearTire) <<endl;
		cout << glm::to_string(o->velocity) <<endl;
		terrainFollower(1,o);
		o->projectileVelocity = ZERO;
	}
	else if (o->rearTireOnGround){
		o->velocity = o->speed*glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
		glm::vec3 netForce;
		netForce = forceEngine(o) + forceDrag(o,0) + forceGravity(o) + forceFriction(o);
		o->velocity += netForce*(o->invMass)*o->timeFrame ;
		glm::vec3 displacement = o->velocity*o->timeFrame ;
		if (glm::dot(o->bikeVector,o->velocity)<0){
			o->speed = (-1.0)*magnitude(o->velocity);
			}
		else{
			o->speed = magnitude(o->velocity);
			};
		o->rearTire = o->rearTire + displacement;
		terrainFollower(2,o);
		cout << glm::to_string(o->rearTire) <<endl;
		o->projectileVelocity = ZERO;
	}
	else if (o->frontTireOnGround){

			o->velocity = o->speed*glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));

		glm::vec3 netForce;
		netForce = forceEngine(o) + forceDrag(o,0) + forceGravity(o) + forceFriction(o);

		o->velocity += netForce*(o->invMass)*o->timeFrame ;
		glm::vec3 displacement = o->velocity*o->timeFrame ;
		if (glm::dot(o->bikeVector,o->velocity)<0){
			o->speed = (-1.0)*magnitude(o->velocity);
			}
		else{
			o->speed = magnitude(o->velocity);
			};
		
		o->rearTire = o->rearTire + displacement;

		terrainFollower(3,o);
		
		o->projectileVelocity = ZERO;
	}
	else{
		if (magnitude(o->projectileVelocity)==0){
			o->projectileVelocity = o->speed*glm::vec3(sin(o->phi)*sin(o->theta),cos(o->phi),-sin(o->phi)*cos(o->theta));
		}
		terrainFollower(4,o);
		glm::vec3 netForce = o->mass*GRAVITY + forceDrag(o,1);
		o->projectileVelocity = o->projectileVelocity + netForce*o->invMass*o->timeFrame;
		glm::vec3 displacement = o->projectileVelocity*o->timeFrame;
		o->rearTire = o->rearTire + displacement;
		if (glm::dot(o->bikeVector,o->projectileVelocity)<0){
			o->speed = (-1.0)*magnitude(o->projectileVelocity);
		}
		else{
		o->speed = magnitude(o->projectileVelocity);
		};
		cout << glm::to_string(o->rearTire) <<endl;

	}
	