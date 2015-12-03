#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include <windows.h>
#ifdef __APPLE__

#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Game.h"
#include "Obj.h"
#include <math.h>
#include <sstream>

class Draw
{
    public:
        Draw();
        virtual ~Draw();
        void drawGame(HHGame *game=nullptr, loc *selected_coord=nullptr, std::string err_message="");
        loc getBoardCoordOfSelection(int index);
    private:
        Obj hounds;
        Obj hare;
        Obj& initPlayerPiece(player p);
        void drawVertices(board gameBoard, loc vertex_selected=loc(-1, -1));
};
#endif // INIT_H_INCLUDED
