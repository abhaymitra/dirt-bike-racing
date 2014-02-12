/**
@file imageloader.h
*/

#ifndef IMAGE_LOADER_H_INCLUDED
#define IMAGE_LOADER_H_INCLUDED

/**

Objects of this class store image in a char array,
along with the width and height of the image.

*/

class Image {
	public:
		Image(char* ps, int w, int h);
		~Image();
		char* pixels;
		int width;
		int height;
};
Image* loadBMP(const char* filename);
#endif
