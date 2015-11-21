#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include <windows.h>
#ifdef __APPLE__

#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Game.h"
#include <math.h>
#include <sstream>
// HHGame *game
void drawGame(loc *selected_coord=nullptr, std::string err_message="you good");

loc getBoardCoordOfSelection(int index);
#endif // INIT_H_INCLUDED
