/**
@file physics.cpp
This file contains the source code for the Physics Library
of the Game.

For the purpose of the game, we model the bike as one point,
which is the rear point of contact, and and two angles, the
angle Phi the length vector of the vector makes with the X-Z
axis and an angle Delta the width vector of the bike makes 
with the X-Z axis.

The velocity of the bike has little dependence on the tangent 
of the terrain when acceleration is on, but has considerable 
effect in case of no acceleration or deceleration.

This was a conscious decision on the programmer's part to 
exude the characteristics of an arcade racing game and make
gameplay difficult and challenging, rather than a pure Physics Simulation.
 (Two copies of Physics Engine were
made during the course of Game Developement, one as close to 
a simulation as possible, and another was this version.
The code for the simulation Physics Engine is also included 
for reference. )

For collision detection, we consider the bike as well as all
other objects to be spheres. Collision is detected whenever
two spheres intersect each other, i.e., 

			|C1C2| < r1 + r2

where C1C2 is the vector joining the two spheres, and r1
and r2 are the radii of the first and second sphere 
respectively. 

@author Abhay Mitra

*/



#include <iostream>
#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include "physics.h"
#include "gameSource.h"

using namespace std;



float PII = 3.14159;



Physics::Physics(){};


/**

This function takes as input the position of the bike and calculates whether the
bike has collided with any bonus, obstacle or fixed objects. An integer value 
between 0-2 is returned, which is used by the changeVelocity() function
to increase of decrease speed according to the collsion.

@param[in] x 	X coordinate of the bike
@param[in] y 	Y coordinate of the bike
@param[in] z 	Z coordinate of the bike

@return 	Integer representing type of object the bike collided with, if it collided
*/


int Physics::resolveCollision(float x,float y,float z){
//bool c = 0;
for (int i=0;i < Pbonus->size();i=i+4){
	if ((vecDifference(x,y,z,Pbonus->at(i),Pbonus->at(i+1),Pbonus->at(i+2))<radius + Pbonus->at(i+3))&& !((*PbonusFlag)[i])){

		(*PbonusFlag)[i]=true;
		bonusT ++;
		score += 1000;
		return 1;
	}
}

for (int i=0;i < Pobstacles->size();i=i+4){
	if ((vecDifference(x,y,z,Pobstacles->at(i),Pobstacles->at(i+1),Pobstacles->at(i+2))<radius + Pobstacles->at(i+3))&&!((*PobstaclesFlag)[i])){
		(*PobstaclesFlag)[i]=true;
		if(score>0){
			score = score/2;
		}
		else{
			score = 2*score;
		}
		return 2;
	}
}
for (int i=0;i < PfixedObj->size();i=i+4){
	if (vecDifference(x,y,z,PfixedObj->at(i),PfixedObj->at(i+1),PfixedObj->at(i+2))<radius + PfixedObj->at(i+3)){
		score = score - 50;
		if(!inAir){
			speed = -speed;
		}
		else{
			vecX = -vecX;
			vecY = -vecY;
		};
			while (!(vecDifference(x,y,z,PfixedObj->at(i),PfixedObj->at(i+1),PfixedObj->at(i+2))<0.1+radius + PfixedObj->at(i+3))){
				
				objX = objX + 0.001*sin(rotateangle/360.0*(2*PII));
				objZ = objZ - 0.001*cos(rotateangle/360.0*(2*PII));
				}

			}
		}
	
return 0;
}

/**

Function to check whether the bike has entered projectile motion. 
Sets the value of the boolean flag inAir 1 when bike enters 
projectile motion.

Two criteria are used to check the projectile condition,
difference between current height of the bike(on ground) and 
a point some distance ahead in the direction of the bike, 
and the change in the tangent at the current location and the 
next.   

@param[in] 	n 	0 means bike is accelerating, in which case projectile motion
is easier to enter

*/



void Physics::checkProjectile(int n){
float difference = 0.10;
if(n==1){

	difference= 8;
}
		float tangent = calculatePhi(length,objX,objZ);
		float nextTangent = calculatePhi(length,objX+16.0*sin(rotateangle/360.0*(2*PII)),objZ - 16.0*cos(rotateangle/360.0*(2*PII)));
		//if(speed>80 && ( -bilinearInterpolation(objX,objZ) + bilinearInterpolation(objX+6.0*sin(rotateangle/360.0*(2*PII)),objZ - 6.0*cos(rotateangle/360.0*(2*PII))) > difference)|| (abs(tangent- nextTangent)>20 && !decelerate && speed>0 &&tangent>30)){
		//cout <<  -bilinearInterpolation(objX,objZ) + bilinearInterpolation(objX+0.6*sin(rotateangle/360.0*(2*PII)),objZ - 0.6*cos(rotateangle/360.0*(2*PII))) << endl;
		//cout << nextTangent << endl;
		if(( -bilinearInterpolation(objX,objZ) + bilinearInterpolation(objX+0.6*sin(rotateangle/360.0*(2*PII)),objZ - 0.6*cos(rotateangle/360.0*(2*PII))) > difference) && nextTangent<10){
		vecX = speed*cos((tangent/180.0)*PII)*sin(rotateangle/360.0*(2*PII));
		vecZ = -speed*cos((tangent/180.0)*PII)*cos(rotateangle/360.0*(2*PII));
		vecY = speed*sin((tangent/180.0)*PII);
		inAir = true;
		cout << inAir <<endl;
		}
	else{};

};

/**

Checks whether the bike has reached the boundaries of the terrain.

*/

bool Physics::atBorder(){
return ((abs(objX) > 2450)||(abs(objZ)>2450));
}


/**

The main update Loop of the Physics Engine. 
This is the function called by the renderer 
to update the current location of the bike.

It first runs the boolean function onBorder() to ascertain
whether the bike has reached the boundaries of the terrain.
If yes, it pulls the bike back by a certain ammount to avoid
the bike from leaving the playing area.

Next, the collision checker is called to resolve the type
of collision the bike was in, if any.

It then checks if the bike is in the air. If not, it 
updates the velocity based on parameters such as acceleration,
current velocity, tangent at the point, collision and then calls
the projectile function to check if the bike has reached the
projectile condition.

If in air, the bike's velocity is changed by gravity , and a check is
run to ensure that the bike hasn't yet gone below the terrain.
If yes, it is lifted to touch the ground and the boolean flag inAir
is set as False.


Also, this loop maintains the score of the player. Flying increases
the score, taking a bonus gives a one time increase of 1000 points, 
hitting an obstacle halves the velocity as well as the score and
hitting a fixed object decreases the score by 500 points.
*/

void Physics::changeVelocity(int a){

totalTime = totalTime + timeFrame;
bool onEdge = atBorder();
if (onEdge){
	if(objX>2400){
		objX = objX - 20;
	}
	if (objZ > 2400){
		objZ = objZ -20;
	}
	if (objX< -2400){
		objX += 20;
	}
	if(objZ < -2400){
		objZ += 20;
	}
	speed = speed/2;
	}
int collide;
if(!ifPractice){
collide = resolveCollision(objX,tempObjY,objZ);
}
else{
	collide = 0;
}

if (!inAir){					// The bike is on the ground
	
	if (accelerate){			// The player is accelerating the bike
		switch(collide){
			case 0:
				break;
			case 1:
				speed = speed + topSpeed/3.0;
				break;
			case 2:
				speed = speed/4.0;
				break;

		}
		float tangent = calculatePhi(length,objX,objZ);
		bikePhi = tangent;
		float temp = ((3+cos((tangent/(2.0*PII))*360.0))/4.0);
		float fraction = speed*timeFrame*temp;
		tempObjY = bilinearInterpolation(objX,objZ) + 4.0*fraction*sin((tangent/180.0)*PII);
		if (boost){
			temp = temp/2;
		};
		objX = objX + 4.0*fraction*sin(rotateangle/360.0*(2*PII));
		objZ = objZ - 4.0*fraction*cos(rotateangle/360.0*(2*PII));

		//speed = speed + (topSpeed - speed)*((2-cos((tangent/(2*PII))*360.0))/2.0)*(timeFrame);
		if (speed<0){
			speed = speed + timeFrame*100;
		}
		else if(speed<topSpeed/3.0){
			speed = speed + timeFrame*30;
		}
		else if(speed < topSpeed/2.0){
			speed = speed + timeFrame*20;

		} 
		else if(speed >topSpeed){
			speed = speed-timeFrame*5;
		}
		else{
			speed = speed+timeFrame*10;
		}
		score -= 5*(speed*speed)/(topSpeed*topSpeed);
		checkProjectile(0);


	}
	else{			// Bike is not accelerating
		

		switch(collide){
			case 0:
				break;
			case 1:
				speed = speed + topSpeed/3.0;
				break;
			case 2:
				speed = speed/2.0;
		}
		float reverse;
		if (decelerate){		// Bike is decelerating
			reverse = 20.0;
		}
		else{					// Bike is neither accelerating not decelerating
			reverse = 0.0;
		};

		float tangent = calculatePhi(length,objX,objZ);
		bikePhi = tangent;
		float temp =((11+cos((tangent/(2.0*PII))*360.0))/12.0);
		float fraction = speed*timeFrame*temp;
		tempObjY = bilinearInterpolation(objX,objZ) + fraction*sin((tangent/180.0)*PII);
		objX = objX + 3*fraction*sin(rotateangle/360.0*(2*PII));
		objZ = objZ - 3*fraction*cos(rotateangle/360.0*(2*PII));
		if(speed>minSpeed){
			//*((0.42+cos((tangent/(2*PII))*360.0))/2.0)
		speed = speed - (speed - minSpeed )*((1+reverse)/21.0)*(timeFrame)*5;
		
	}
		else{
			speed = minSpeed;

		};
		if ((speed < 0.1) && !decelerate && ((tangent>-30)||(tangent<30))){

			speed = 0.0;
		}
		if (speed<5 && tangent >30){

			speed = speed - 0.5;
		}	
		else if(speed<2 && tangent<-30){
			speed = speed +0.5;
		}
		score -= 5.0*((topSpeed+10-speed)*(topSpeed+10-speed))/(topSpeed*topSpeed);
		checkProjectile(1);
	};
}
else {						// Bike is in the air
	score += 10;
	bikePhi -= 0.5;
	if(-bilinearInterpolation(objX,objZ)+tempObjY<-0.05){
		inAir = false;
		tempObjY = bilinearInterpolation(objX,objZ);
	}
	else{

	switch(collide){
			case 0:
				break;
			case 1:
				speed = speed + topSpeed/3.0;
				vecX = 1.5*vecX;
				vecZ = 1.5*vecY;
				break;
			case 2:
				speed = speed/2.0;
				vecX = 0.5*vecX;
				vecY = 0.5*vecY;
				break;

		}
	vecY = vecY - 9.8*10*timeFrame;
	float TobjX = objX + vecX*timeFrame*5;
	float TobjZ = objZ + vecZ*timeFrame*5;
	float TtempObjY = tempObjY + vecY*timeFrame*5;
	speed = sqrt(vecX*vecX + vecY*vecY + vecZ*vecZ);

	if(-bilinearInterpolation(TobjX,TobjZ)+TtempObjY<-0.01){
		objX = TobjX;
		objZ = TobjZ;
		inAir = false;
		tempObjY = bilinearInterpolation(objX,objZ);
	}
	else{
		objX = TobjX;
		objZ=TobjZ;
		tempObjY = TtempObjY;
	};
	};
};
}







