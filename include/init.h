#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include <windows.h>
#ifdef __APPLE__

#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <sstream>
void init(void);

void drawBoard(void);

float *getSpacesWidthLengths(float width, int num_circles, float frac);

void setOrthographicProjection(int w, int h);

void restorePerspectiveProjection();

void renderString(int x, int y, void *font, const char *str, int len);

void windowCoordToSpaceCoord(int x, int y);

#endif // INIT_H_INCLUDED
