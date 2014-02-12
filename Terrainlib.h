/**
@file Terrainlib.h
*/

#ifndef ter_H_INCLUDED
#define ter_H_INCLUDED
#include <iostream>
#include "imageloader.h"
/**
Object of this class represents a three dimensional vector.
This class implements functions for vector operations such 
as dot product, cross product, vector addiction, scalar
multiplication and such.

*/

class ter {
	private:
		float v[3];
	public:
		ter();
		ter(float x, float y, float z);

		float &operator[](int index);
		float operator[](int index) const;

		ter operator*(float scale) const;
		ter operator/(float scale) const;
		ter operator+(const ter &other) const;
		ter operator-(const ter &other) const;
		ter operator-() const;

		const ter &operator*=(float scale);
		const ter &operator/=(float scale);
		const ter &operator+=(const ter &other);
		const ter &operator-=(const ter &other);

		float magnitude() const;
		float magnitudeSquared() const;
		ter normalize() const;
		float dot(const ter &other) const;
		ter cross(const ter &other) const;
};

ter operator*(float scale, const ter &v);
std::ostream &operator<<(std::ostream &output, const ter &v);

/**
Objects of this class define a 2-D array heightmap generated from a 24-bit
BMP file. 
This class implements functions for scaling the terrain by some factor, 
calculating smooth normals and public functions for getting height 
at a point in the 2D array.

*/


class Terrain {
	private:
		int w;
		int l;
		float** hs;
		ter** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2);
		~Terrain();
		int width();
		int length();
        //Sets the height at (x, z) to y
		void setHeight(int x, int z, float y);
		//Returns the height at (x, z)
		float getHeight(int x, int z);
		//Computes the normals, if they haven't been computed yet
		void computeNormals();
		//Returns the normal at (x, z)b
		ter getNormal(int x, int z);
};
Terrain* loadTerrain(const char* filename, float height);

#endif
