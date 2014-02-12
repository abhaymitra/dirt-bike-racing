/**
@file Terrainlib.cpp

This library is used to read data from char arrays and 
populate a 2-D array containing float values representing
heights of the corresponding 2-D array. 
*/

#include <math.h>
#include "Terrainlib.h"
using namespace std;

ter::ter() {
}

ter::ter(float x, float y, float z) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

float &ter::operator[](int index) {
	return v[index];
}

float ter::operator[](int index) const {
	return v[index];
}

ter ter::operator*(float scale) const {
	return ter(v[0] * scale, v[1] * scale, v[2] * scale);
}

ter ter::operator/(float scale) const {
	return ter(v[0] / scale, v[1] / scale, v[2] / scale);
}

ter ter::operator+(const ter &other) const {
	return ter(v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}

ter ter::operator-(const ter &other) const {
	return ter(v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}

ter ter::operator-() const {
	return ter(-v[0], -v[1], -v[2]);
}

const ter &ter::operator*=(float scale) {
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
	return *this;
}

const ter &ter::operator/=(float scale) {
	v[0] /= scale;
	v[1] /= scale;
	v[2] /= scale;
	return *this;
}

const ter &ter::operator+=(const ter &other) {
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];
	return *this;
}

const ter &ter::operator-=(const ter &other) {
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];
	return *this;
}

float ter::magnitude() const {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float ter::magnitudeSquared() const {
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

ter ter::normalize() const {
	float m = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return ter(v[0] / m, v[1] / m, v[2] / m);
}

float ter::dot(const ter &other) const {
	return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
}

ter ter::cross(const ter &other) const {
	return ter(v[1] * other.v[2] - v[2] * other.v[1],
				 v[2] * other.v[0] - v[0] * other.v[2],
				 v[0] * other.v[1] - v[1] * other.v[0]);
}

ter operator*(float scale, const ter &v) {
	return v * scale;
}

ostream &operator<<(ostream &output, const ter &v) {
	cout << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
	return output;
}


		Terrain::Terrain(int w2, int l2) {
			w = w2;
			l = l2;

			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}

			normals = new ter*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new ter[w];
			}

			computedNormals = false;
		}

		Terrain::~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;

			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}

		int Terrain::width() {
			return w;
		}

		int Terrain::length() {
			return l;
		}

		/**Sets the height at (x, z) to y*/
		void Terrain::setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}

		/**Returns the height at (x, z)*/
		float Terrain::getHeight(int x, int z) {
			return hs[z][x];
		}

		/**Computes the normals, if they haven't been computed yet.
			Does so by first calculating rough normals, and then 
			smoothing the normals by taking averages over the
			neighbourhood of a point in the array.*/
		void Terrain::computeNormals() {
			if (computedNormals) {
				return;
			}

			//Compute the rough version of the normals
			ter** normals2 = new ter*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new ter[w];
			}

			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					ter sum(0.0f, 0.0f, 0.0f);

					ter out;
					if (z > 0) {
						out = ter(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					ter in;
					if (z < l - 1) {
						in = ter(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					ter left;
					if (x > 0) {
						left = ter(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					ter right;
					if (x < w - 1) {
						right = ter(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}

					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}

					normals2[z][x] = sum;
				}
			}

			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					ter sum = normals2[z][x];

					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}

					if (sum.magnitude() == 0) {
						sum = ter(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}

			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;

			computedNormals = true;
		}

		/**Returns the normal at (x, z)b */
		ter Terrain::getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}

		/**
		This function reads a BMP file, extracts a char array and
		populates the 2-D array of an object of the Terrain class.
		It also sets the normals of each point in the array.
		
		*/
		
		Terrain* loadTerrain(const char* filename, float height) {
					Image* image = loadBMP(filename);
					Terrain* t = new Terrain(image->width, image->height);
					for(int y = 0; y < image->height; y++) {
						for(int x = 0; x < image->width; x++) {
							unsigned char color =
								(unsigned char)image->pixels[3 * (y * image->width + x)];
							float h = height * ((color / 255.0f) - 0.5f);
							t->setHeight(x, y, h);
						}
					}

					delete image;
					t->computeNormals();
					return t;
				}







