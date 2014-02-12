/**

@file gameSource.cpp
This file contains the main file of the game.

Using libraries developed for  Terrain generation, Texture Mapping
and Physics Engine, as well as external OpenGL libraries, this file
is responsible for running the game and saving the score and time
if it is a new record, once the game exits.

For testing purposes, the executable of the file can be given two
arguments, one from 1 to 10 and another either 0 or 1.

The first argument is the number of track, while the second number 
is the difficulty level selected for that track.

For example, if the executable file name is game, we can start track
4 with hard difficulty level (in bash) as :

			game 4 1

@author Abhay Mitra

*/





#include <math.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "Terrainlib.h"
#include "objRendererlib.h"
#include <vector>
#include <fstream>
#include <string.h>
//#include <stdlib.h>
#include <string>
#include <sstream>
#include "physics.h"
#include "gameSource.h"
//#include <stdio.h>
//#include <cstdlib>
//#include "Physics_new.h"
using namespace std;


Physics *bike = new Physics();


int num=0;				
char quote[6][8000];		/*!<  Stores characters used for drawing HUD	*/
int numberOfQuotes=0,i;

float totalTime = 0;	/*!<  Stores total time since starting the game	*/
int bonusT = 0;			/*!<  Stores numbers of bonus taken	*/
float timeFrame;		/*!<  This number is equal to the number of times the  Physics Engine reloads in a second	*/

string objadd;			/*!<  Address of the file containing location of objects on track	*/
string hscoreadd;		/*!<  Address of the file containing highest scores on track	*/
string htimeadd;		/*!<  Address of the file containing lowest times on track	*/

std::vector<float> bonus(0);					/*!< Stores location of bonus objects. 4i, 4i+1, 4i+2 , 4i+3hold x cord, y cord, z cord and radius respectively */
std::vector<bool> bonusFlag(0);				/*!< whether ith bonus has been taken */
std::vector<float> obstacles(0);  			/*!< Stores location of  obstacles. 4i, 4i+1, 4i+2 , 4i+3hold x cord, y cord, z cord and radius respectively */
std::vector<bool> obstaclesFlag(0);			/*!< whether ith obstacle has been taken */
std::vector<float> checkpoints;				/*!< Stores location of  checkpoints. 4i, 4i+1, 4i+2 , 4i+3hold x cord, y cord, z cord and radius respectively */
std::vector<bool> checkpointsFlag(0);		/*!< whether ith obstacle has been taken */
vector<float> fixedObj;						/*!< Stores location of  fixed obects. 4i, 4i+1, 4i+2 , 4i+3hold x cord, y cord, z cord and radius respectively */

bool ifPractice;							/*!< Boolean variable stores if game was started in practice mode. No HUD and objectives in this mode.*/

float aftertime = 1;						/*!< Time to wait after game is finished.*/
bool finished=false;						/*!< If game is finished yet,i.e., the payer has taken half plus the last checkpoint.*/


float checkpointRadius=200;					/*!< Radius of checkpoint.  */

float rotateObjects=0;


float phiExtra;								/*!< Variable used to adjust camera angle according to height.*/


bool oneLeft=false;							/*!< Boolean variable true if only the last checkpoint is left.*/

float PI = 3.14159;
float length = 0.1;							/*!< Length taken for approximating tangent at a point. */
float a,b;
float _angle = 60.0f;
//float DeltaAnglex = 0.0f;
//float DeltaAngley = 0.0f;
int xOrigin = -1;
int yOrigin = -1;

float red=1.0f, blue=1.0f, green=1.0f;
float phi = 0.3;
float angle = 1.5;

float cameraRadius=20;					/*!< Distance of camera from bike. */
bool* keyStates = new bool[256];		/*!< bool array stores which keys are pressed down at any moment.*/
Terrain* terrain;
GLuint tex;								/*!< GL Reference int used to store the currently loaded texture. */

float scale ;							/*!< Stores the factor by which the terrain was scaled in all three directions.*/


float radius = 2.50;					/*!< Radius of bike for collission.*/



bool turnLeft;							/*!< True only if bike is turning left.*/
bool turnRight;							/*!< True only if bike is turning right.*/
bool inAir;								/*!< If bike is in Projectile motion. */
bool boost;			


float bikePhi =0;						/*!< Angle length vector of bike makes with ground.*/
float bikeApparentPhi=0;				/*!< angle to avoid jitters and sudden changes to bikePhi.*/
float bikeRadian =0;					/*!< Angle width vector of bike makes with ground.*/

float bikeDelta;


bool phiFlag=false;

float rotateangle=0;//rotation of cube

objRenderer aa;
objRenderer bb;
objRenderer cc;
objRenderer dd;
float lx=0.0,lz=-1,ly=-0.783327;
float x=0.0,y=4.69996,z=14.7297;
//float x,y,z;
float cameraHeight = 3.0 ;



//test variables

float testaa=90;
float testbb=2220;
float testcc=0;
float testscale ;


int score=5000;

float objX;				/*!< Current x coordinate of bike.*/
float objZ;				/*!< Current z coordinate of bike.*/
float tempObjY;			/*!< Holds the Y coordinate of bike when in air.*/
float speed;			/*!< Speed in direction of length vector of the bike.*/
bool accelerate;		/*!< If player is acclelerating */
bool decelerate;		/*!< If player is decelerating */



/**
This function is used to bind an image for later use in texture 
mapping on terrain and obj files.

@param[in]	img 	Input of Image type defined in imageloader.cpp.

*/


GLuint Texture(Image* img) {
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->width, img->height,0,GL_RGB,GL_UNSIGNED_BYTE,img->pixels);               //The actual pixel data
	return tex;
}


/**

updateHighScore
This function is called on completion of the game. 
If the score is a new high score
for the track, it saves it in the corresponding file.

@param[in] filename 	Filename of the highscore file.

*/


void updatehscore(const char *filename){
  string line2;
  ifstream myfile3 ("name.txt");
  if (myfile3.is_open())
  {
      getline (myfile3,line2);
      cout << line2;	
    myfile3.close();
  }
  std::vector<string> hscore(0);
  string line;
	int temp;
	std::string sscore;
  ifstream myfile (filename);
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      temp = atof(line.c_str());
      if(score > temp){
        stringstream ss;
        ss << score;
      	string stt = ss.str();
        string stt1 = stt + "\t"+line2;
        //cout <<"line added is"<<stt1<<"\n";
      	hscore.push_back(stt1);
        score = 0;
      }
        hscore.push_back(line);
    }
    myfile.close();
  }
  ofstream myfileo (filename);
  string line3;
  if (myfileo.is_open())
  {
    for(int i=0; (i<4) && (i<hscore.size()) ;i++){
      line3 = hscore[i];
      cout <<line3;
    //sscore = static_cast<ostringstream*>(&(ostringstream() << hscore[i]))->str();
    myfileo << hscore.at(i)+"\n";

	}
    myfileo.close();

  }
  else cout << "Unable to open file";

}

/**
updateTimeScore
This function is called on completion of the game. 
If the time is a new high score
for the track, it saves it in the corresponding file.

@param[in] filename 	Filename of the highscore file.

*/


void updatetscore(const char *filename){
  string line2;
  ifstream myfile3 ("name.txt");
  if (myfile3.is_open())
  {
      getline (myfile3,line2);	
    myfile3.close();
}
  std::vector<string> tscore(0);
  string line;
	int temp;
	std::string sscore;
  ifstream myfile (filename);
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      temp = atoi(line.c_str());
      if(totalTime < temp){
	int tt = floor(totalTime);
        stringstream ss;
        ss << tt;
        string stt = ss.str();
            // stt.append(line2.begin(),line2.end());
        string stt1 = stt + "\t"+line2;
        //cout <<"line added is"<<stt1;
        tscore.push_back(stt1);
        totalTime = 600;
      }
        tscore.push_back(line);
    }
    myfile.close();
  }
  ofstream myfileo (filename);
  if (myfileo.is_open())
  {
    for(int i=0; (i<4) && (i<tscore.size()) ;i++){
    //sscore = static_cast<ostringstream*>(&(ostringstream() << tscore[i]))->str();
    myfileo << tscore.at(i)+'\n';
	}
    myfileo.close();

  }
  else cout << "Unable to open file";

}

void saveScoreExit(){
	updatetscore(htimeadd.c_str());
	updatehscore(hscoreadd.c_str());	
	exit(0);
}


/**
	Calculates |v1-v2|^2 
 where v1 = (x1,y1,z1) and v2 = (x2,y2,z2) are both vectors.

*/

float vecDifference(float x1, float y1, float z1,float x2,float y2, float z2){
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2 -z1)*(z2-z1));
	}


/**
Calculates height of a given point (x,z) by using bilinear
Interpolationon the closest four points in the Terrain 2-D
Array.

@param x 	X coordinate of the point after scaling
@param y 	Z coorinates of the point after scaling

@return 	Calculated average height of point (x,z)
*/

float bilinearInterpolation(float x, float y){
	float tempMaxXY = max(terrain->width() - 1, terrain->length() - 1);
	float X = x/(scale) + tempMaxXY/2.0;
	float Y  = y/(scale) + tempMaxXY/2.0;
	float temp =  terrain->getHeight(floor(X),floor(Y))*(ceil(X)-X)*(ceil(Y)-Y) + terrain->getHeight(ceil(X),floor(Y))*(X-floor(X))*(ceil(Y)-Y) + terrain->getHeight(floor(X),ceil(Y))*(ceil(X)-X)*(Y-floor(Y)) + terrain->getHeight(ceil(X),ceil(Y))*(X-floor(X))*(Y-floor(Y));
	//return scale*terrain->getHeight(floor(X),floor(Y));
	return temp*scale;
}


/**

Checks if the player has succesfully completed the game.
The game is considered complete when the player has attained
atleast half of the checkpoints distributed evenly on the 
track, and taken the last checkpoint.


@return Returns the number of checkpoints attained
*/



int ifComplete(){

	
	for (int i=0;i < checkpoints.size();i=i+3){
		int temp=0;

		if ((vecDifference(objX,bilinearInterpolation(objX,objZ),objZ,checkpoints.at(i),checkpoints.at(i+1),checkpoints.at(i+2))<radius + checkpointRadius) && !checkpointsFlag.at(i/3)){
			checkpointsFlag[(i/3)]=true;
			num++;
		}					
	}


	return num;
}


/**

Reads the track configuration file corresponding to the
selected track, and loads the vectors with the appropriate
values according to the selected difficulty level.

@param[in] 	difficulty 	Alters the number of obstacles based on selected difficulty.

*/



void loadVectorFromFile(bool difficulty){
		vector<float> tempObjects;
		int i=0;
		std::string line;

		ifstream myfile(objadd.c_str());
		//fixedObj.resize(24);
  		if (myfile.is_open())
  		{
    	while ( myfile.good() )
    	{
      	getline (myfile,line);
      	if (i<80){
      	tempObjects.push_back(atof(line.c_str()));
  }

      	i++;
   		}

    myfile.close();
   
    i--;



    for (int i=0;i<tempObjects.size();i=i+4){
    	tempObjects[i] = (1024.0 -tempObjects.at(i));
    	tempObjects[i] = (512.0-tempObjects.at(i) )*scale ;
    	
    	tempObjects[i+2] = (512.0-tempObjects.at(i+2) )*scale ;
    	tempObjects[i+1] = bilinearInterpolation(tempObjects.at(i),tempObjects.at(i+2));
    	tempObjects[i+3] = 8.0;

    	if (i%8==0){
    		checkpoints.push_back(tempObjects.at(i));
    		checkpoints.push_back(tempObjects.at(i+1));
    		checkpoints.push_back(tempObjects.at(i+2));
    		checkpointsFlag.push_back(false);
    		//checkpoints.push_back(tempObjects.at(i));
    	}

    }
	 

    int c ;
    if (difficulty){
    	for (int i=0;i<tempObjects.size();i=i+12){
    		
    	
    		for (int j=0;j<8;j++){
    			if(i+j<80){
    				if ((j==3)||(j==7)){
    					obstacles.push_back(30);
    				}
    				else{
    		obstacles.push_back(tempObjects.at(i+j));
    		}
    	}

    		obstaclesFlag.push_back(false);
    		obstaclesFlag.push_back(false);
    	}

    			// if(i+7<80){
    			// 	obstacles[i+3] = 30.0;
    			// 	obstacles[i+7] = 30.0;
    			// }
    		for (int j=8;j<12;j++){
    			if(i+j<80){
    		bonus.push_back(tempObjects.at(i+j));
    	}
    		
    	}
    	bonusFlag.push_back(false);
    

}
}

    else{
    	for (int i=0;i<tempObjects.size();i=i+12){
    		for (int j=0;j<8;j++){
    			if(i+j<80){
    		bonus.push_back(tempObjects.at(i+j));
    	}
    	}
    	bonusFlag.push_back(false);
    	bonusFlag.push_back(false);
    		for (int j=8;j<11;j++){
    			if(i+j<80){
    		obstacles.push_back(tempObjects.at(i+j));
    	}
    	}
    	if(i+11<80){
    	obstacles.push_back(30);
    }
    	obstaclesFlag.push_back(false);
    }
    };

   	float randomAngle = 0;
   	for (int i=0;i<tempObjects.size();i=i+8){
   			

   		fixedObj.push_back(tempObjects.at(i)+60.0*cos(randomAngle));
   		fixedObj.push_back(bilinearInterpolation(tempObjects.at(i)+60.0*cos(randomAngle),tempObjects.at(i+2)+60.0*sin(randomAngle)));
   		fixedObj.push_back(tempObjects.at(i+2)+60.0*sin(randomAngle));
   		fixedObj.push_back(5.0);
   		randomAngle += (30.0/360.0)*(2*PI);
}

}}


/**

Calcuates the tangent at point (x,y,z) in direction of 
the bike, when the bike is on the ground.

Effectively, calculates the angle the length vector of the bike should make
with the X-Z plane .

@param[in] 	length 	The differential length selected for calculating
the tangent.
@param[in] 	x 	X coordinate of the bike
@param[out] y 	Z coordinate of the bike

@return 	Angle tangent makes with X-Z plane, in degrees.
*/


float calculatePhi(float length,float x,float y){
	float T = (rotateangle/360.0)*2*PI;

	float x_dash = (x+length*sin(T));
	float y_dash = (y-length*cos(T));
	float height = bilinearInterpolation(x,y);
	float height_dash = bilinearInterpolation(x_dash,y_dash);
	float tempPhiRadian = atan((height_dash-height)/length);
	return (180.0/PI)*tempPhiRadian;
}

/**

Calcuates the tangent at point (x,y,z) in the direction perpendicular 
to that of the bike, when the bike is on the ground.

Effectively, calculates the angle the width vector of the bike 
should make with the X-Z plane.

@param[in] 	length 	The differential length selected for calculating
the tangent.
@param[in] 	x 	X coordinate of the bike
@param[out] y 	Z coordinate of the bike

@return 	Angle tangent makes with X-Z plane, in degrees.
*/


float calculateDelta(float length){
	float T = (rotateangle/360.0)*2*PI;
	float x_dash = (objX-length*cos(T));
	float y_dash = (objZ-length*sin(T));
	float height = bilinearInterpolation(objX,objZ);
	float height_dash = bilinearInterpolation(x_dash,y_dash);
	float tempPhiRadian = atan((height_dash-height)/length);
	return (180.0/PI)*tempPhiRadian;

}




GLuint list;
GLuint listobj;
GLuint listtree;
GLuint listbonus;
GLuint listobst;

/**

Function called before running the game to preprocess information
required for successful initialization of the game.

It loads the terrain, maps the texture onto the terrain, populates
the object vectors and the various display lists for the obj files
used in the game.

It also initializes the bike, passing the pointers to the vectors
used for collision detection and setting the position of the bike
at the starting point of the track.

@param[in] 	filename 	Filename of the terrain to be loaded
@param[in]	filename1 	Filename of the texture to be mapped 
@param[in]	height 		Scaling factor of the terrain along Y(height) axis.
@param[in]	diff 		Difficulty level of the selected track

*/



void initRendering(const char* filename,const char* filename1, float height,bool diff) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
    	glShadeModel(GL_SMOOTH);
		Image* i = loadBMP(filename);
		tex = Texture(i);
		a=i->width;
		b=i->height;
		delete i;
		terrain = loadTerrain(filename1,height);

	
	scale = 5000.0f / max(terrain->width() - 1, terrain->length() - 1);
	list = glGenLists(1); //set the list to Generate a List
	glNewList(list,GL_COMPILE); //compile the new list
	glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, tex);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


for(int z = 0; z < terrain->length() - 1; z++) {
			//MAKES TERRAIN USING TRIANGLE STRIPS
			glColor3f(1.0,1.0,0);
	        glBegin(GL_TRIANGLE_STRIP);
			for(int x = 0; x < terrain->width(); x++) {
				ter normal = terrain->getNormal(x, z);
				glNormal3f(normal[0], normal[1], normal[2]);
				glTexCoord2f((((float)x)*(a/terrain->width()))/a,(((float)z)*(a/terrain->width()))/a);
				glVertex3f(x, terrain->getHeight(x, z), z);
				normal = terrain->getNormal(x, z + 1);
				glNormal3f(normal[0], normal[1], normal[2]);
				glTexCoord2f((((float)x)*(a/terrain->width()))/a,(((float)z+1)*(a/terrain->width()))/a);
				glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);

			}



			glEnd();
		}
		 glEndList();

	aa = objRenderer();
	aa.readObj("lowpolytree5.obj");
	bb = objRenderer();
	bb.readObj("quad.obj");
	cc = objRenderer();
	cc.readObj("Coin 1.obj");
	dd = objRenderer();
	dd.readObj("cube_texture.obj");

	listobj = glGenLists(1); //set the list to Generate a List
	glNewList(listobj,GL_COMPILE); //compile the new list
	bb.drawObj("black.bmp");
	glEndList();

	listbonus = glGenLists(1); //set the list to Generate a List
	glNewList(listbonus,GL_COMPILE); //compile the new list
	cc.drawObj("bonus.bmp");
	glEndList();

	
	listobst = glGenLists(1); //set the list to Generate a List
	glNewList(listobst,GL_COMPILE); //compile the new list
	glScalef(50,50,50);
	dd.drawObj("red.bmp");
	glEndList();



	listtree = glGenLists(1); //set the list to Generate a List
	glNewList(listtree,GL_COMPILE); //compile the new list
	glScalef(5,5,5);
	aa.drawObj("shu1.bmp");
	glEndList();



	glDisable(GL_TEXTURE_2D);

	if(!ifPractice){
	loadVectorFromFile(1);
	bike->Pbonus = &bonus;
	bike->Pobstacles = &obstacles;
	bike->PbonusFlag = &bonusFlag;
	bike->PobstaclesFlag = &obstaclesFlag;
	bike->PfixedObj = &fixedObj;
	bike->objX = obstacles.at(0)-70;
	bike->objZ = obstacles.at(2)-70;
	}	
	else{
		objX = 200;
		objZ = 200;
	}
	bike->topSpeed = 200;
//				INITIALIZE BIKE HERE, AFTER CALLING LOADVECTORFROMFILE
	bike->minSpeed = -30;
	bike->speed = 0;
	bike->ifPractice = ifPractice;
	
	bike->tempObjY = bilinearInterpolation(objX,objZ);
	bike->inAir = false;
	
	timeFrame = 1/60.0;
	bike->timeFrame = timeFrame;
	strcpy(quote[0],"score:");
	strcpy(quote[1],"time:");
	strcpy(quote[2],"bonus:");
	strcpy(quote[3],"Speed");
	numberOfQuotes=6;


}

/**

This function is called whenever the size of the Game Window
is changed. It changes the Projection Matrix of OpenGL accordingly.

@param[in] 	w 	Width of the new Window
@param[in]	h 	Height of the new Window

*/


void changeSize(int w, int h){
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(60.0f, ratio, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

/**

This function reads the input from the keyboard and 
changes values of variables like acceleration and rotation
accordingly. 

*/



void changeKey(void){
	float fraction = 2.0;
	if (keyStates[27]) {
	       exit(0);
	}
	if ((keyStates['a'])&& !inAir){
		if((decelerate)&&(speed<0)){
			rotateangle += 1.6;
		}
		else{
		rotateangle -= 1.6;
	}
		turnLeft = true;
		if(bikeDelta>-30){
			bikeDelta-=1;
		}
	}
	if(!(keyStates['a'])){
		turnLeft = false;
	}
	if((keyStates['m'])){
		testscale -=  0.01;
	}
	if((keyStates['n'])){
		testscale += 0.1;
	}
	if (keyStates['s']) {
		decelerate = true;
	}
	if ((keyStates['d'])&& !inAir){
		if((decelerate) &&(speed<0)){
			rotateangle -= 1.6;
		}
		else{
		rotateangle += 1.6;
	};
		turnRight= true;
		if(bikeDelta<30){
			bikeDelta+=1;
		}
		if ( rotateangle > 360.0f){
			rotateangle -= 360;
			
		}

		else {};
	}
	if(!(keyStates['d'])){
		turnRight=false;
	}
	if (keyStates['w'] && !keyStates[' ']) {
		accelerate = true;
		boost = false;
	}
	if (!keyStates['w']){
		accelerate = false;
	}
	if (!keyStates['s']){
		decelerate = false;
	}
	if (keyStates['w'] && keyStates[' ']){
		accelerate = true;
		boost = true;
	}

}


/**

This is the update function of the game and it acts like
an intermediate between the Physics Engine and the User.

It runs 60 times every second and updates the values of the 
co-ordinates and the various angles the bike makes with 
the three axes. These values are used by the renderer
to draw the scene.

*/

void updatePhysics(int a){
	changeKey();
	bike->changeVelocity(0);
	objX = bike->objX;
	objZ = bike->objZ;
	tempObjY = bike->tempObjY;
	inAir = bike->inAir;
	bike->rotateangle = rotateangle;
	bikePhi = bike->bikePhi;
	speed = bike->speed;
	score = bike->score;
	bonusT = bike->bonusT;
	bike->accelerate = accelerate;
	bike->decelerate = decelerate;
	totalTime = bike->totalTime;
	glutTimerFunc(100.0/6.0,updatePhysics,0);


}

/**

This function draws the bonus objects every frame 
using the display lists prepared in initRendering() 
in the scene if they haven't already been taken.

*/



void drawBonus(){
	for (int i=0;i < bonus.size();i=i+4){

		if (!bonusFlag[i]){
			glPushMatrix();
			glTranslatef(bonus.at(i),bonus.at(i+1)+22.0,bonus.at(i+2));
			glRotatef(rotateObjects*5,0,1,0);
			glRotatef(90,1,0,0);
			glColor3f(0,0,1);
			glScalef(2,2,2);
			glCallList(listbonus);
			//glutSolidCube(5) ;
			glPopMatrix() ;
		}
	}
}

/**

This function draws obstacles every frame using the
display lists prepared in initRendering() in the 
scene if they haven't already been taken.

*/


void drawObstacles(){
	for (int i=0;i < obstacles.size();i=i+4){
		
		if (!obstaclesFlag[i]){
			glPushMatrix();
			glTranslatef(obstacles.at(i),obstacles.at(i+1)+20.0,obstacles.at(i+2));
			glRotatef(-rotateObjects,0,1,0);
			glutSolidSphere(50,20,20);
			//glCallList(listobst);
			glPopMatrix();
		}
	}
}

/**

This function draws all the fixed objects in the scene
every frame using the display lists and position array 
populated in initRendering(). 

*/

void drawFixed(){
	for (int i=0;i < fixedObj.size();i=i+4){
			glPushMatrix();
			glTranslatef(fixedObj.at(i),fixedObj.at(i+1),fixedObj.at(i+2));
			glScalef(2,2,2);
			glCallList(listtree);
			glPopMatrix() ;
		
		}
}

/**

This function draws the bike in every frame using 
display lists and the updated values of relevant 
parameters from the Physics Engine.

It also does calculations to minimize the jerking
of the bike due to large changes in the angle the 
bike makes with the axes(can't be avoided because 
the terrain is inherently discontinous and bilinear
interpolation only serves as an approximation. )

*/


void drawObject(){
	glPushMatrix();

	glTranslatef(0,1.5,0);
	
	float radian = calculateDelta(0.05);

	if(!turnLeft && !turnRight){
		if(abs(bikeDelta)<0.01){

		}	
		else if (bikeDelta>0){
			bikeDelta -=1;
		}
		else{
			bikeDelta += 1;
		};
	}

	else{};
	if((abs(radian)<5) && !inAir){
		bikeRadian = 0;
		radian = 0;

	}



if(!inAir){
	if((radian-bikeRadian< 3) && (radian>0)){
		bikeRadian += min(0.1f,radian-bikeRadian);
	}
	else if((radian-bikeRadian< -3) && (radian<0)){
		bikeRadian -= min(0.1f,-radian+bikeRadian);
	}
	else if(radian-bikeRadian>30){
		bikeRadian += 0.1;
	}
	else if(radian-bikeRadian<-30){
		bikeRadian -= 0.1;
	}
	else{
		bikeRadian=radian;
	};
}





	glRotatef(-bikeRadian,0,0,1);

	if((bikePhi-bikeApparentPhi< 3) && (bikePhi>0)){
		bikeApparentPhi += min(1.0f,bikePhi-bikeApparentPhi);
	}
	else if((bikePhi-bikeApparentPhi< -3) && (bikePhi<0)){
		bikeApparentPhi -= min(1.0f,-bikePhi+bikeApparentPhi);
	}
	else if(bikePhi-bikeApparentPhi>30){
		bikeApparentPhi += 5;
	}
	else if(bikePhi-bikeApparentPhi<-30){
		bikeApparentPhi -= 5;
	}
	else{
		bikeApparentPhi=bikePhi;
	};

	glRotatef(bikeApparentPhi,1,0,0);
	glRotatef(180,0,1,0);
	glCallList(listobj);
	glPopMatrix();



}



/**

This function uses global vaiables like Score, Time,
CheckPoints, Bonus taken and Speed and displays it on
top of the scene rendered by the renderer.

*/

void drawHUD(){
	    int tt = 2.0*floor(totalTime);
	string ttt = static_cast<ostringstream*>(&(ostringstream()<<tt))->str();
	string tttt = "Time  :	" + ttt;
	//string tt = " Time : " + std::to_string(totalTime);
	strcpy(quote[0],tttt.c_str());
	string s_score = static_cast<ostringstream*>(&(ostringstream()<<score))->str();
	string ss_score = "Score  : 	";
	string sss_score = ss_score+s_score;
	string s_bonus = static_cast<ostringstream*>(&(ostringstream()<<bonusT))->str();
	string ss_bonus = "Bonus: 	";
	string sss_bonus = ss_bonus + s_bonus;
	strcpy(quote[1],sss_score.c_str());
	strcpy(quote[2],sss_bonus.c_str());
	string s_speed = static_cast<ostringstream*>(&(ostringstream()<<abs(floor(speed))))->str();
	string ss_speed = "Speed  : 	";
	string sss_speed = ss_speed+s_speed;
	strcpy(quote[3],sss_speed.c_str());
	int nnum = num;
	string s_cp = static_cast<ostringstream*>(&(ostringstream()<<nnum))->str();
	string ss_cp= "Checkpoints : 	";
	string sss_cp = ss_cp+s_cp;
	strcpy(quote[4],sss_cp.c_str());
	if (oneLeft){
		if(finished){
			string message = "Race Finished";
		strcpy(quote[5],message.c_str());
		}
		else{
		string message = "Last Checkpoint Left";
		strcpy(quote[5],message.c_str());
	}
	}
	int lenghOfQuote;
	for( int l=0;l<numberOfQuotes;l++)
	    {

	        lenghOfQuote = (int)strlen(quote[l]);
	        glPushMatrix();
	        glLoadIdentity();
	        glMatrixMode(GL_PROJECTION);
	        	glPushMatrix();
				glLoadIdentity();
				gluOrtho2D(0, 800, 0, 600);
				glScalef(0.25,0.25,0.25);
	        //
	        if(l==3){
				glTranslatef(testaa,testbb,testcc);

	        }
	        else if(l==4)
	        	glTranslatef(testaa,testbb-120,testcc);
	        else if(l==5)
	        	glTranslatef(800,testbb-500,testcc);
	        else
	        glTranslatef(1980,testbb-120*l,testcc);
	        for (i = 0; i < lenghOfQuote; i++)
	        {
	          glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[l][i]);
	        }
	        glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
	        glPopMatrix();
	    }
}

/**

This is the function responsible for drawing all the scenes
of the game. 

We use a Double Buffer system where we replace two buffers after
some ammount of time, using this time to draw the other buffer 
while the first one is being drawn.

*/



void render() {
	if(!ifPractice){
	if (finished){
		aftertime -= timeFrame;
		if(aftertime<0){
		saveScoreExit();
		exit(0);
	}
	}
	}
	rotateObjects  += 1;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3,0.6,1.0,0);
	glLoadIdentity();
	float temp = max(terrain->width() - 1, terrain->length() - 1);
	if (!ifPractice){
	if((checkpointsFlag.at(checkpoints.size()/3-1))&&(num>=checkpointsFlag.size()/2)){
		finished = true;
	}
	else if((num>=checkpointsFlag.size()/2)){
		oneLeft = true;		
	}

	 ifComplete();
	}
	 if((bikePhi<-20) && !phiFlag && !inAir){
	 	phiFlag= true;

	 }
	 if(phiFlag){
	 	if(bikePhi>-10){
	 		phiFlag=false;
	 	}
	 	else{
	 		if(phiExtra < 0.90){
	 			phiExtra +=0.06;
	 		}
	 	}
	 }
	 if((!phiFlag)&&(phiExtra>0.02)){
	 	phiExtra -= 0.1;
	 }

	gluLookAt(cos(phi+phiExtra)*cos(angle)*cameraRadius,1.5+sin(phi+phiExtra)*cameraRadius,cos(phi+phiExtra)*sin(angle)*cameraRadius,0,1.5,0,0,1,0);
	drawObject();
	glRotatef(rotateangle,0,1,0);
	if (!inAir){
		glTranslatef(-1.0*objX,-bilinearInterpolation(objX,objZ),-1.0*objZ);
	}
	else{
		glTranslatef(-1.0*objX,-tempObjY,-1.0*objZ);
	};
	GLfloat ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	GLfloat COLOR[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat POS[] = {-1.0f, 0.6f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, COLOR);
	glLightfv(GL_LIGHT0, GL_POSITION, POS);
	glPushMatrix();
	if(!ifPractice){
	drawFixed();
	drawBonus();
	drawObstacles();
}
	glPopMatrix();
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(terrain->width() - 1) / 2,0.0f,-(float)(terrain->length() - 1) / 2);
    glCallList(list);
    if(!ifPractice){
    drawHUD();
}
	glFlush();
	glutSwapBuffers();
}


/**

This function changes the values of the KeyStates array 
based on the keys pressed by the user.

*/

void keyPressed (unsigned char key, int x, int y) {
keyStates[key] = true; // Set the state of the current key to pressed
}

/**

This function changes the values of the KeyStates array 
based on the keys just unpressed by the user.

*/

void keyUp (unsigned char key, int x, int y) {
keyStates[key] = false; // Set the state of the current key to not pressed
}

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1366,768);
	glutCreateWindow("Ultimate Dirt Bike Racing");
	 bool difficulty;
    char* difficult=argv[2];
    if(difficult[0]=='1')
    	difficulty=1;
    else
    	difficulty=0;
    string n=argv[1];
    if (n=="0"){
    	ifPractice = true;
    	cout << ifPractice << endl;
    }
    string texadd = "tex"+n+".bmp";
    string teradd = "ter"+n+".bmp";
    objadd = "object"+n+".txt";
    hscoreadd = "hscore"+n+".txt";
    htimeadd = "tscore"+n+".txt";
	initRendering(texadd.c_str(),teradd.c_str(),30,difficulty);
	glutTimerFunc(100.0/6.0,updatePhysics,0);
    glutReshapeFunc(changeSize);
	glutIdleFunc(render);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutDisplayFunc(render);
    glutMainLoop();
	return 1;
}

