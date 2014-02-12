/**
@file physics.h
*/

#include <math.h>
#include <vector>

//float PI = 3.1459;


/**
This class contains all the data required for calculation 
and updation of the position and velocity of our bike.

Data about objects on a terrain is passed to this class
through vector pointers, and about the height of the 
terrain at any point through the public bilinear
interpolation function of the game class.
*/

class Physics{

private:
	int resolveCollision(float,float,float);
	void checkProjectile(int);
	bool atBorder();


public:

	std::vector<float> *Pbonus;					// 4i, 4i+1, 4i+2 , 4i+3hold x cord, y cord, z cord and radius respectively
	std::vector<bool> *PbonusFlag;				// whether ith bonus has been taken
	std::vector<float> *Pobstacles;
	std::vector<bool> *PobstaclesFlag;
	//std::vector<float> *Pcheckpoints;
	//std::vector<bool> *PcheckpointsFlag;
	std::vector<float> *PfixedObj;


	float PII = 3.14159;

	bool ifPractice;
	bool inAir ;
	float vecX;
	float vecY;
	float vecZ;
	float speed;
	bool accelerate=false;
	bool boost=false;
	bool decelerate=false;
	float topSpeed;
	float topBoostSpeed;		/*!< Top Speed of the bike*/
	float timeFrame;			
	float minSpeed;				/*!< Min Speed of the bike */


	int score=5000;
	float totalTime = 0;
	int bonusT = 0;
	float checkpointRadius=125;

	float phiExtra;			
	float length = 0.1;
	float scale ;

	float radius = 1.50;
	float objX;//cordinates of object
	float objZ ;
	float tempObjY ;	

	//bool turnLeft;
	//bool turnRight;


	float bikePhi =0;
	float bikeApparentPhi=0;
	//float bikeRadian =0;
	//float bikeDelta;

	bool phiFlag=false;

	float rotateangle=0;



	Physics();
	void changeVelocity(int);
	
};