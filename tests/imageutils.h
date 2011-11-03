#ifndef IMAGEUTILS_H_
#define IMAGEUTILS_H_

#include <stdlib.h>

bool write_png(const char *filename, unsigned char *pixels, int width, int height);
void flip_image(const unsigned char *src, unsigned char *dst, size_t pixelsize, size_t width, size_t height);

#endif
