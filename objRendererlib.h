/**
@file objRendererlib.h
*/

#include <stdio.h>

#include <math.h>
#include "imageloader.h"
#include <iostream>
#include "glm.h"
#include <vector>

/**

This class handles the rendering and texture mapping 
of object files. 

*/

class objRenderer{
public:
	objRenderer();
	GLMmodel* myModel;
	void readObj(char* filename);
	void drawObj(char* filename);

};

