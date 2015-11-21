#include "include/draw.h"
#include <iostream>
using namespace std;

const double z_coord = -6.0;
const float radius = 0.25f;

const int vertices_length = 11;
struct Coord {
    loc objCoord, boardCoord;
    Coord ();
    Coord (loc objXY, loc boardXY) : objCoord(objXY), boardCoord(boardXY) {};
    string print() {
        stringstream ss;
        ss << "Coord(" << objCoord.print() << ", " << boardCoord.print() << ")";
        return ss.str();
    };
};
Coord vertices[11] = {
  Coord(loc(-1, 1), loc(0, 0)), Coord(loc(0, 1), loc(0, 1)), Coord(loc(1, 1), loc(0, 2)),
  Coord(loc(-2, 0), loc(1, 0)), Coord(loc(-1, 0), loc(1, 1)), Coord(loc(0, 0), loc(1, 2)), Coord(loc(1, 0), loc(1, 3)), Coord(loc(2, 0), loc(0, 3)),
  Coord(loc(-1, -1), loc(2, 0)), Coord(loc(0, -1), loc(2, 1)), Coord(loc(1, -1), loc(2, 2)) };

loc getBoardCoordOfSelection(int index)
{
    return vertices[index - 1].boardCoord;
}

void drawSquare(float x, float y, float width)
{
    float len = width/2;
    float bottom_left_coord[2] = { x-len, y-len };
    float top_right_coord[2] = { x+len, y+len };
    // first the outline's drawn, then the gray inside
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int j = 0; j < 2; j++) {
        glRectfv(bottom_left_coord, top_right_coord);

        len *= .85;
        bottom_left_coord[0] = x-len; bottom_left_coord[1] = y-len;
        top_right_coord[0] = x+len; top_right_coord[1] = y+len;
        glColor3f(211/255.0, 211/255.0, 211/255.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glColor3d(1, 1, 1);
}
void drawCircle(float cx, float cy, float radius)
{
    int num_of_seg = 6;
    float x, y;
    x = radius + 0.06;
    y = 0;
    float tangent_vector_x, tangent_vector_y;
    float theta = 2 * 3.14159265359 / num_of_seg;
    float tan = tanf(theta);
    float cos = cosf(theta);
    glFrontFace(GL_CW);
    // first the outline's drawn, then the gray inside
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int j = 0; j < 2; j++) {
        glBegin(GL_POLYGON);
            for (int i = 0; i < num_of_seg; i++) {
                glVertex2f(cx + x, cy + y);

                tangent_vector_x = y;
                tangent_vector_y = -1 * x;

                x += tangent_vector_x * tan;
                y += tangent_vector_y * tan;

                x *= cos;
                y *= cos;
            }
        glEnd();
        x *= .85;
        y = 0;
        glColor3f(211/255.0, 211/255.0, 211/255.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glColor3d(1, 1, 1);
    glFrontFace(GL_CCW);
}
void drawVertices(loc vertex_selected=loc(-1, -1))
{
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    loc o_coord;
    loc b_coord;
    for (int i = 0; i < vertices_length; i++) {
        Coord c = vertices[i];
        o_coord = c.objCoord;
        b_coord = c.boardCoord;
        glStencilFunc(GL_ALWAYS, i + 1, -1);
        // outline is drawn red
        if (b_coord.equals(vertex_selected)) {

                glColor3d(1, 0, 0);
        }
        if (b_coord.y % 2 == 0) {
            drawCircle(o_coord.x, o_coord.y, radius);
        } else {
            drawSquare(o_coord.x, o_coord.y, radius*2);
        }
    }
}

void drawDiamond(void)
{
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2d(-1, 1);
    glVertex2d(-2, 0);
    glVertex2d(-1, 0);
    glVertex2d(-1, -1);
    glVertex2d(0, 0);
    glEnd();

    glBegin(GL_LINES);
    glVertex2d(-1, 1);
    glVertex2d(0, 0);
    glEnd();
}
void drawBoard(void)
{
    drawDiamond();
    glPushMatrix();
        glTranslated(2, 0, 0);
        drawDiamond();
    glPopMatrix();
    glBegin(GL_LINES);
        glVertex2d(-1, 1);
        glVertex2d(1, 1); // top of board
        glVertex2d(-1, -1);
        glVertex2d(1, -1); // bottom of board
        glVertex2d(0, 1);
        glVertex2d(0, -1); // middle of board
    glEnd();
}

void updateText(string message);

void drawGame(loc *selected_coord, string err_message)
{
    glColor3d(1, 1, 1);
    glTranslated(0, 0, z_coord);
    glScaled(1.5, 1.5, 1.0);

    glLineWidth(3.0f);
    glEnable(GL_LINE_SMOOTH);
    // draw place mats at vertices on board

    if (selected_coord) {
        drawVertices(*selected_coord);
    } else {
        drawVertices();
    }


    glLineWidth(0.5);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawBoard();

    updateText(err_message);
}

void *font = GLUT_BITMAP_9_BY_15;
void setOrthographicProjection(int w, int h);
void restorePerspectiveProjection(void);
void renderString(int x, int y, void *font, const char *str, int len);
void updateText(string message)
{
    // dimensions of current window
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    setOrthographicProjection(window_width, window_height);
    renderString(window_width/5, window_height*6.5/7, font, "ROUND: 1", 8);
    renderString(window_width/5, window_height/7, font, "YOU: 1", 6);
    renderString(window_width*3.5/5, window_height/7, font, "OPP: 2", 6);
    renderString(window_width*2.25/5, window_height/7, font, message.c_str(), 9);
    restorePerspectiveProjection();
}
void setOrthographicProjection(int w, int h)
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    glLoadIdentity(); // reset matrix
    // set a 2D orthographic projection
    gluOrtho2D(0, w, 0, h);

    // Note: origin is at bottom left corner

    // switch back to modelview mode
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}
void restorePerspectiveProjection(void)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // restore previous projection matrix
    // get back to modelview mode
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void renderString(int x, int y, void *font, const char *str, int len)
{
    glRasterPos2i(x, y);
    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}
