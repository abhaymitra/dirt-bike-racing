/**
@file objRendererlib.cpp

This library is used to draw obj files and load 
specified textures on them.

*/

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "objRendererlib.h"

using namespace std;
objRenderer::objRenderer() {}

/**
This function takes an image datastream and binds that image
to a GLuint data type, which can be later used for mapping 
texture onto rendered objects.

*/

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
				 0,                            //0 for now
				 GL_RGB,                       //Format OpenGL uses for image
				 image->width, image->height,  //Width and height
				 0,                            //The border of the image
				 GL_RGB, //GL_RGB, because pixels are stored in RGB format
				 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
				                   //as unsigned numbers
				 image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture

}

GLuint _textureId; //The id of the texture

void InitRendering()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}


/**

This function renders the obj file associated with
a particular instance of the class. It uses the
external library GLM to parse and render the 
object and the associated material files.

param[in] filename 		Name of file that will be used for texture mapping over
the object. (Not required if mtl files used)

*/

void objRenderer::drawObj(char* filename){
	glMatrixMode(GL_MODELVIEW);
	Image* image = loadBMP(filename);
	_textureId = loadTexture(image);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glmFacetNormals(myModel);
	glmVertexNormals(myModel,90);
	glmDraw(myModel ,GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	delete image;	    	


}

/**

This function sets the obj file to be used by a particular 
instance of the class.

param[in] filename 	Filename of the obj file to be used to render the object

*/


void objRenderer::readObj(char* filename){
	myModel = glmReadOBJ(filename);
	InitRendering();
}


